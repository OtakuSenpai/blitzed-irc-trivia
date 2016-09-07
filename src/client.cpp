/*
 * client.cpp
 *
 * Copyright (C) 2001  Erik Fears
 *
 * This is a fork of the original project
 * (http://harlequin.sourceforge.net/)
 *
 * Copyright (C) 2016  Andy Alt (andyqwerty@users.sourceforge.net)
 * This file is part of Blitzed IRC Trivia
 * (https://git.io/vicjS)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */


#include "h.h"


Client::Client ()
{
  m_sockfd = 0;
  m_alarmed = 0;
  m_status.registered = 0;
  m_status.joined = 0;
  m_time.last_checked_join.tv_sec = 0;
  m_time.last_line.tv_sec = 0;

  ignore = 0;
}

Client::~Client ()
{
}

void
Client::init_socket ()
{

  if (m_sockfd != 0)
    close (m_sockfd);

  m_sockfd = 0;
  m_status.registered = 0;

#ifndef WIN32
  bzero (&(m_server_addr.sin_zero), 8);
#endif

  if ((m_sockfd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
  {
    log->logtofile ("Socket initialisation failed\n");
    exit (1);
  }

  log->logtofile ("Socket initialised%d\n", m_sockfd);
}

void
Client::get_localhost ()
{

  int i;
  int bytes[4];
  char *tmpip;
  char *tmpbyte;



  if (gethostname (m_localhostname, 256) != 0)
  {
    log->logtofile ("Error retrieving local hostname\n");
    exit (1);
  }

  m_localhost = gethostbyname (m_localhostname);
  if (m_localhost == NULL)
  {
    log->logtofile ("Error resolving local hostname\n");
    exit (1);
  }

  tmpip = inet_ntoa (*((struct in_addr *) m_localhost->h_addr));

  tmpbyte = strtok (tmpip, ".");

  for (i = 0; i < 4; i++)
  {
    bytes[i] = atoi (tmpbyte);
    tmpbyte = strtok (NULL, ".");
  }

  m_locallongip =
    (bytes[0] * 256 * 256 * 256) + (bytes[1] * 256 * 256) + (bytes[2] * 256) +
    (bytes[3]);
}


void
Client::connect_to (char *host, int port)
{

  if (!(m_server_hostname = gethostbyname (host)))
  {
    log->logtofile ("Failed resolving hostname\n");
    exit (1);
  }

  m_server_addr.sin_family = AF_INET;
  m_server_addr.sin_port = htons (port);
  m_server_addr.sin_addr = *((struct in_addr *) m_server_hostname->h_addr);

#ifndef WIN32
  bzero (&(m_server_addr.sin_zero), 8);
#endif

  if (connect
      (m_sockfd, (struct sockaddr *) &m_server_addr,
       sizeof (struct sockaddr)) == -1)
  {
    log->logtofile ("Connection failed to %s port %d\n", host, port);
    reconnect ();
    return;
  }

  log->logtofile ("Connected to %s port %d\n", host, port);

  do_register ();
}

void
Client::close_socket ()
{
  log->logtofile ("Socket closed on FD %d\n", m_sockfd);
  close (m_sockfd);
  m_sockfd = 0;
}

void
Client::reconnect ()
{
  m_status.joined = 0;
  log->logtofile ("Reconnecting...\n");
  close_socket ();
  sleep (5);
  init_socket ();
  connect_to (config->IRC_Server, config->IRC_Port);
}

void
Client::begin ()
{

  int retval;

  while (1)
  {


    FD_ZERO (&m_fdset);
    FD_SET (m_sockfd, &m_fdset);

    m_timeout.tv_sec = 30;
    m_timeout.tv_usec = 30000000;

    retval = select ((m_sockfd + 1), &m_fdset, NULL, NULL, NULL);
    switch (retval)
    {
    case 0:
      break;

    case -1:
      break;

    default:
      get_from_server ();
    }

    if (m_alarmed)
    {
      alarm ();                 /* Client::alarm() */
      game->alarm ();
      m_alarmed = 0;
    }

  }


}

void
Client::s (int priority, const char *data, ...)
{

  va_list arglist;
  char data2[1024];
  char tosend[1024];
  send_struct *new_q;

  va_start (arglist, data);
  vsprintf (data2, data, arglist);
  va_end (arglist);

  sprintf (tosend, "%s\r\n", data2);

  new_q = new send_struct;

  //Set data and priority of new queue item
  new_q->data = strdup (tosend);
  new_q->priority = priority;
  new_q->next = 0;

  //Link to end of sendq
  if (!m_sendq)
  {
    m_sendq = new_q;
  }
  else
  {
    for (send_struct * q = m_sendq; q; q = q->next)
    {
      if (!q->next)
      {
        q->next = new_q;
        break;
      }
    }
  }

}

void
Client::sendq ()
{
  send_struct *lastq;
  int priority;

  if (!m_sendq)
    return;

  //Determine highest priority in q
  priority = 0;
  for (send_struct * q = m_sendq; q; q = q->next)
    if (q->priority > priority)
      priority = q->priority;

  //Send out first item in list matching priority
  //and link the list around it

  //Check if the head is the next item
  if (m_sendq->priority == priority)
  {
    log->logtofile ("Sending: %s", m_sendq->data);

    if (send (m_sockfd, m_sendq->data, strlen (m_sendq->data), 0) == -1)
    {
      log->logtofile ("Error on sending %s\n", m_sendq->data);
      exit (1);
    }
    lastq = m_sendq->next;
    delete[]m_sendq->data;
    delete m_sendq;
    m_sendq = lastq;
    return;
  }

  for (send_struct * q = m_sendq; q; q = q->next)
  {
    if (priority == q->priority)
    {
      log->logtofile ("Sending: %s\n", q->data);

      if (send (m_sockfd, q->data, strlen (q->data), 0) == -1)
      {
        log->logtofile ("Error on sending %s\n", q->data);
        exit (1);
      }

      //Link around then free
      lastq->next = q->next;
      delete[]q->data;
      delete q;
      break;
    }
    lastq = q;
  }

}

void
Client::privmsg (char *target, const char *msg, ...)
{
  va_list arglist;
  char msg2[1024];

  va_start (arglist, msg);
  vsprintf (msg2, msg, arglist);
  va_end (arglist);

  s (5, "PRIVMSG %s :%s", target, msg2);
}

void
Client::notice (char *target, const char *msg, ...)
{
  va_list arglist;
  char msg2[1024];

  va_start (arglist, msg);
  vsprintf (msg2, msg, arglist);
  va_end (arglist);

  s (3, "NOTICE %s :%s", target, msg2);
}


void
Client::do_register ()
{
  s (100, "NICK %s", config->CLIENT_Nick);
  s (100, "USER %s \"%s\" %s :%s", config->CLIENT_Username,
     config->CLIENT_Username, config->CLIENT_Username,
     config->CLIENT_Realname);

}

void
Client::get_from_server ()
{
  if (FD_ISSET (m_sockfd, &m_fdset))
  {
    if (get_line ())            /* Get line and store in m_tmpline */
    {
      gettimeofday (&(m_time.last_line), NULL);
      parse_line (m_tmpline);
    }
    else
    {
      log->logtofile ("Error get_line()... reconnecting\n");
      reconnect ();
    }
  }
}

int
Client::get_line ()
{
  char c;
  int i = 0;

  do
  {
    if (read (m_sockfd, &c, 1) < 1)
      return 0;
    if (i >= RECVSIZE)
      return 0;

    if (c != 13 && c != 10)
      m_tmpline[i++] = c;
  }
  while (c != 10);

  m_tmpline[i] = '\0';

  return 1;
}

void
Client::parse_line (char *line)
{

  if (config->LOG_FromServer)
    log->logtofile ("%s\n", line);

  char *first = strtok (line, " ");
  char *second = strtok (NULL, " ");
  source_struct *source;

  line = strtok (NULL, "");

  if (!first || !second)
    return;

  if (first[0] == ':')
  {

    if (ignore_check (first))   //Check against ignore list before parsing anything
      return;                   //Drop data completely

    source = parse->get_source (first);
    if (!source)
      return;



    if (first && second)
    {
      if (!strcmp (second, "PRIVMSG"))
        parse_privmsg (source, line);
      if (!strcmp (second, "PART"))
        parse_part (source, line);
      if (!strcmp (second, "QUIT"))
        parse_part (source, line);
      if (!strcmp (second, "NICK"))
        parse_newnick (source, line);
      if (!strcmp (second, "JOIN"))
        parse_join (source, line);
      if (!strcmp (second, "KICK"))
        parse_kick (source, line);
    }

  }

  if (!second)
    return;

  if (strcmp (first, "PING") == 0)
    s (15, "PONG %s", second);
  if (strcmp (second, "375") == 0)
    s (15, "JOIN %s", config->IRC_Channel);
  if (strcmp (second, "001") == 0)
    m_status.registered = 1;
}

void
Client::parse_join (source_struct * source, char *line)
{
  if (!strcmp (source->nick, config->CLIENT_Nick))
  {
    m_status.joined = 1;
    m_time.last_checked_join.tv_sec;
  }
  game->do_join (source, line);

}


void
Client::parse_kick (source_struct * source, char *line)
{
  strtok (line, " ");
  char *kicked = strtok (NULL, " ");
  cout << source->nick << " kicked " << kicked << endl;
  if (!strcmp (kicked, config->CLIENT_Nick))
    m_status.joined = 0;
}

void
Client::parse_newnick (source_struct * source, char *line)
{
  game->do_newnick (source, line);
}


void
Client::parse_part (source_struct * source, char *line)
{

  char *target = strtok (line, " ");

  if (!target)
    return;

  game->do_part (source, target);


}

void
Client::parse_privmsg (source_struct * source, char *line)
{

  char *target;
  char *msg;

  char *cmd;
  char *rest;

  target = strtok (line, " ");
  msg = parse->strip_leading (strtok (NULL, ""));

  if (!source || !target || !msg)
  {
    log->logtofile ("Error in parse_privmsg().\n");
    return;
  }

  if (msg[0] == '\001')
  {
    do_ctcp (source, target, msg);
    return;
  }


  if (!strcasecmp (target, config->IRC_Channel))
    game->do_channel (source, target, msg);

  //Now we assume its a private command, so we parse it
  //into command and its arguments

  cmd = strtok (msg, " ");      //Command
  rest = strtok (NULL, "");     //Arguments

  //Client needs to catch commands that deal
  //with only the client! Anything else is
  //sent to game class for further processing

  if (target[0] != '#')         //Private Message?
  {
    //Ignore command?
    if (!strcasecmp (cmd, "IGNORE"))
      do_ignore (source, rest);

    game->do_msg (source, target, cmd, rest);   //Pass on to game class
  }
}

//Return 1 if check proves true
int
Client::ignore_check (char *mask)
{

  regex_t re;

  if (!ignore)
    return 0;

  //Match mask against every regular expression in ignore list
  for (ignore_struct * is = ignore; is; is = is->next)
  {
    if (regcomp (&re, is->mask, REG_EXTENDED | REG_NOSUB))
      continue;                 //Error

    if (!regexec (&re, mask, (size_t) 0, NULL, 0))
    {
      regfree (&re);
      return (1);               //Found match
    }

    regfree (&re);

  }

  return (0);

}

void
Client::ignore_save ()
{

  ofstream out;
  int count = 0;
  int length;

  out.open ("ignore.state", ios::out);

  if (out.fail ())
  {
    log->logtofile ("Error opening ignore state file for write.");
    return;
  }

  for (ignore_struct * is = ignore; is; is = is->next)
    count++;

  out.write ((char *) &count, sizeof (int));

  for (ignore_struct * is = ignore; is; is = is->next)
  {
    length = strlen (is->mask) + 1;

    out.write ((char *) &length, sizeof (int));
    out.write ((char *) is->mask, (length * sizeof (char)));
  }

  out.close ();
}

void
Client::ignore_load ()
{
  ifstream in;
  int count = 0;
  int length;
  ignore_struct *link;

  in.open ("ignore.state", ios::in);

  if (in.fail ())
  {
    log->
      logtofile
      ("Error opening ignore state file for read. (Might not exist)");
    return;
  }

  in.read ((char *) &count, sizeof (int));

  for (int i = 0; i < count; i++)
  {
    in.read ((char *) &length, sizeof (int));

    link = new ignore_struct;
    link->mask = new char[length];
    in.read ((char *) link->mask, length);
    link->next = 0;

    if (!ignore)
    {
      ignore = link;
    }
    else
    {
      for (ignore_struct * is = ignore; is; is = is->next)
      {

        if (!is->next)
        {
          is->next = link;
          break;
        }

      }
    }

  }

  in.close ();

}


void
Client::do_ignore (source_struct * source, char *args)
{
  char *password = 0;
  char *rest = 0;

  password = strtok (args, " ");
  rest = strtok (NULL, "");



  if (!password || strcmp (password, config->CLIENT_AdminPass))
  {
    notice (source->nick, "Incorrect Password.");
    return;
  }


  //No arguments, list ignores
  if (!rest || strlen (rest) <= 2)
  {
    ignore_list (source);
    return;
  }

  switch (rest[0])
  {
  case '+':
    ignore_add (source, parse->strip_leading (rest));
    break;
  case '-':
    ignore_del (source, parse->strip_leading (rest));
    break;
  default:
    notice (source->nick, "Hostmasks must be proceeded with a +/-");
    return;
  }

  ignore_save ();
}

void
Client::ignore_list (source_struct * source)
{

  int i = 0;

  if (!ignore)
  {
    notice (source->nick, "Ignore list is empty.");
    return;
  }

  notice (source->nick, "- Ignore List -");
  for (ignore_struct * is = ignore; is; is = is->next)
  {
    i++;
    notice (source->nick, "%d - %s", i, is->mask);
  }
  notice (source->nick, "- End list, %d Ignores", i);
}

void
Client::ignore_add (source_struct * source, char *mask)
{

  ignore_struct *link;
  link = new ignore_struct;
  link->mask = strdup (mask);
  link->next = 0;


  //Link to ignore list
  if (!ignore)
  {
    ignore = link;
  }
  else
  {
    for (ignore_struct * is = ignore; is; is = is->next)
    {

      if (!is->next)
      {
        is->next = link;
        break;
      }

    }
  }

  notice (source->nick, "Added %s to ignore list.", mask);
}

void
Client::ignore_del (source_struct * source, char *mask)
{

  ignore_struct *prev = 0;

  for (ignore_struct * is = ignore; is; is = is->next)
  {
    if (!strcasecmp (mask, is->mask))
    {

      notice (source->nick, "Removed %s from ignore list.", mask);
      if (!prev)
      {
        ignore = is->next;
        delete is->mask;
        delete is;
        return;
      }
      else
      {
        prev->next = is->next;
        delete is->mask;
        delete is;
        return;
      }
    }
    prev = is;
  }
  notice (source->nick, "Entry not found in ignore list.");
}




void
Client::do_ctcp (source_struct * source, char *target, char *msg)
{

  char *msg1 = parse->strip_leading (msg);      /* strip off the first \001 */
  msg1 = strtok (msg1, "\001"); /* take whats before the last \001 */

  char *ctcp = strtok (msg1, " ");      /* the first ctcp argument */
  char *ctcp2 = strtok (NULL, "");      /* Second argument if it exists */


  if (!ctcp)
    return;


  if (!strcasecmp (ctcp, "VERSION"))
    s (5, "NOTICE %s :\001VERSION %s\001", source->nick,
       config->CTCP_Version);

  if (!ctcp2)
    return;

  if (!strcasecmp (ctcp, "PING"))
    s (5, "NOTICE %s :\001PING %s\001", source->nick, ctcp2);


}



void
Client::alarm ()
{
  timeval present;
  int recvdiff;
  int joindiff;

  gettimeofday (&present, NULL);

  recvdiff = present.tv_sec - m_time.last_line.tv_sec;

  if ((recvdiff > config->CLIENT_TIME_Timeout)
      && m_time.last_line.tv_sec != 0)
  {
    log->logtofile ("Connection timed out.\n");
    reconnect ();
  }

  if (!m_status.joined)
  {
    if (m_time.last_checked_join.tv_sec == 0)
    {
      s (15, "JOIN %s", config->IRC_Channel);
      gettimeofday (&(m_time.last_checked_join), NULL);
    }
    else
    {
      joindiff = present.tv_sec - m_time.last_checked_join.tv_sec;
      if (joindiff > 30)
      {
        s (15, "JOIN %s", config->IRC_Channel);
        gettimeofday (&(m_time.last_checked_join), NULL);
      }
    }

  }


  //Send next two items in the queue
  sendq ();
  sendq ();
}
