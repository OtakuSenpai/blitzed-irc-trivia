/*
 * game.cpp
 *
 * Copyright (C) 2001  Erik Fears
 *
 * Copyright (C) 2016  Andy Alt (andy400-dev@yahoo.com)
 *
 * This file is part of Blitzed IRC Trivia
 * (https://github.com/andy5995/blitzed-irc-trivia/wiki)
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
 */

#include "h.h"

Game::Game ()
{
  m_status.question_active = 0; // initialise status
  m_status.active = 0;
  m_status.hinted = 0;
  m_status.lastlist = 0;

  time (&m_start);              //Store time of creation (of object) for uptime

}

Game::~Game ()
{
}

void
Game::do_status (source_struct * source, const char *args)
{


  client->notice (source->nick,
                  "Questions in %s: %d - UseHint: %d - HintRatio: %d - MinHint: %d - Give Hint after %d seconds - Question timeout after %d seconds - Delay between questions %d seconds - Points needed for win %d -=- Teams : %s - Number of Teams: %d",
                  config->GAME_DB, question->questions (),
                  config->GAME_UseHint, config->GAME_HintRatio,
                  config->GAME_MinHint, config->GAME_TIME_GiveHint,
                  config->GAME_TIME_Timeout, config->GAME_TIME_QuestionDelay,
                  config->GAME_Points, (config->GAME_UseTeams) ? "yes" : "no",
                  config->GAME_NumTeams);
}

struct cmd channel_commands[CMD_ITEMS] = {
  {".start", CMD_START,
   "start a game round if there isn't one already running"},
  {".list", CMD_LIST, "list players"},
  {".status", CMD_STATUS, "display the game configuration"},
  {".uptime", CMD_UPTIME, "show uptime"},
  {".version", CMD_VERSION, "list players"},
  {".help", CMD_HELP, "show help"},
};

//Very sloppy/quick, needs rewritten!
void
Game::do_uptime (source_struct * source, const char *args)
{

  char out[512];
  char tmp[512];

  time_t present;
  time_t diff;
  timestruct uptime;

  *out = 0;                     // NULL Term

  time (&present);              //Get current time
  diff = present - m_start;     //Calculate time in seconds between now and start

  parse->timeparse (uptime, diff);


  if (uptime.weeks)
  {
    snprintf (tmp, 512, "%dw ", uptime.weeks);
    strcat (out, tmp);
  }
  if (uptime.days)
  {
    snprintf (tmp, 512, "%dd ", uptime.days);
    strcat (out, tmp);
  }
  if (uptime.hours)
  {
    snprintf (tmp, 512, "%dh ", uptime.hours);
    strcat (out, tmp);
  }
  if (uptime.minutes)
  {
    snprintf (tmp, 512, "%dm ", uptime.minutes);
    strcat (out, tmp);
  }
  if (uptime.seconds)
  {
    snprintf (tmp, 512, "%ds", uptime.seconds);
    strcat (out, tmp);
  }

  client->privmsg (config->IRC_Channel, "Uptime: %s", out);

}

void
Game::do_start (source_struct * source, char *args)
{
  start ();
}

void
Game::do_stop (source_struct * source, char *args)
{
  stop ();
}

void
Game::do_newplayer (source_struct * source)
{
  pi *p;
  ti *t;

  player->add_player (source->nick);    //Add player
  p = player->find_pi (source->nick);   //Get newly created player struct

  client->notice (source->nick, config->TEXT_GAME_Newplayer);

  if (config->GAME_UseTeams)
  {
    t = player->find_ti (p->team);
    if (!t)
      return;
    client->notice (source->nick, "You have been placed on '%s'.", t->name);
    client->privmsg (config->IRC_Channel,
                     "\002%s\002 has been placed on '\002%s\002'",
                     source->nick, t->name);
  }
}

void
Game::reset_scores ()
{
  pi *p = player->head;

  while (p)
  {
    p->points = 0;
    p = p->next;
  }

  for (ti * t = player->teams; t; t = t->next)
  {
    t->points = 0;
  }

}

void
Game::do_channel (source_struct * source, char *target, char *msg)
{
  //This code needs redone with a hash


  time_t present;
  time (&present);

  //If activity monitor enabled and it's not me who is speaking...
  //Log it on the monitor
  if (config->GAME_ACTIVITY_Enabled
      && strcmp (source->nick, config->CLIENT_Nick))
    activity.update (source->nick);

  if (!source->nick || !msg)
    return;

  short check_for_cmd;
  bool cmd_found = 0;

  for (check_for_cmd = 0; check_for_cmd < CMD_ITEMS; check_for_cmd++)
  {
    if (strcasecmp (msg, channel_commands[check_for_cmd].str) == 0)
    {
      cmd_found = 1;

      switch (check_for_cmd)
      {
      case CMD_START:
        if (!m_status.active)
          start ();
        break;

      case CMD_LIST:
        if ((present - m_status.lastlist) > 30)
        {
          if (!config->GAME_UseTeams)
            list_players (config->IRC_Channel);
          else
            list_teams (config->IRC_Channel);
          time (&(m_status.lastlist));
        }
        break;

      case CMD_STATUS:
        do_status (source, NULL);
        break;

      case CMD_UPTIME:
        do_uptime (source, NULL);
        break;

      case CMD_VERSION:
        client->privmsg (config->IRC_Channel, "%s", PACKAGE_STRING);
        client->privmsg (config->IRC_Channel,
"home page: https://github.com/andy5995/blitzed-irc-trivia/wiki");
        break;

      case CMD_HELP:
        short help_item;
        for (help_item = 0; help_item < CMD_ITEMS; help_item++)
        {
          client->privmsg (source->nick, "%s\t\t%s",
                           channel_commands[help_item].str,
                           channel_commands[help_item].desc);
        }
        break;

      default:
        break;
      }

    }

    if (cmd_found)
      break;
  }

  if (m_status.question_active && m_status.active)
    check_answer (source, msg);

}

void
Game::do_msg (source_struct * source, char *target, char *cmd, char *rest)
{
  //Rewrite this code!

  char *passwd = strtok (rest, " ");
  char *args = strtok (NULL, "");

  if (!cmd || !passwd)
  {
    client->notice (source->nick, "Password expected for that command");
    return;
  }

  if (config->GAME_UseTeams)
    if (!strcasecmp (cmd, "name") && !strcasecmp (passwd, "team"))
      do_nameteam (source, passwd, args);

  if (strcmp (passwd, config->CLIENT_AdminPass))        //Incorrect admin password
    return;

  //Administrator Commands
  if (!strcasecmp (cmd, "stop"))
    do_stop (source, args);
  if (!strcasecmp (cmd, "list"))
    do_admin_list (source, args);
  if (!strcasecmp (cmd, "start"))
    do_start (source, args);
  if (!strcasecmp (cmd, "set"))
    do_set (source, args);
  if (!strcasecmp (cmd, "force"))
    do_force (source, args);
  if (!strcasecmp (cmd, "die"))
    do_die (source, args);
  if (!strcasecmp (cmd, "config"))
    do_config (source, args);
  if (!strcasecmp (cmd, "load"))
    do_rehash (source, args);
}

void
Game::do_nameteam (source_struct * source, char *passwd, char *args)
{

  pi *p;
  ti *t;
  time_t present;

  time (&present);

  if (!(p = player->find_pi (source->nick)))
    return;
  if (!(t = player->find_ti (p->team)))
    return;


  if (player->is_lead (source->nick))
  {
    if ((present - t->lastname) < 180)
    {
      client->privmsg (source->nick,
                       "You must wait \002%d\002 seconds before changing the team name",
                       180 - (present - t->lastname));
      return;
    }

    player->set_name (p->team, args);
    client->privmsg (config->IRC_Channel,
                     "%s has set his Team's name to '\002%s\002'",
                     source->nick, t->name);
  }
  else
    client->privmsg (source->nick,
                     "Only leading team members may change team name.");

}

void
Game::do_admin_list (source_struct * source, char *args)
{

  //Can be flooding! Needs rewritten with queue or something
  for (pi * p = player->head; p; p = p->next)
    client->notice (source->nick, "NICK: %s POINTS: %d TEAM: %d",
                    p->nick, p->points, p->team);



}

void
Game::do_force (source_struct * source, char *args)
{

  if (m_status.question_active) //If question active, ignore
    return;

  m_status.timer = 0;           //Set timer to 0, forcing question on next update

}

void
Game::do_die (source_struct * source, char *args)
{
  client->s (0, "QUIT :DIE (%s)", source->nick);
  exit (0);
}

void
Game::do_config (source_struct * source, char *args)
{
  if (config->parse_entry (args))
  {
    client->notice (source->nick, "Set.");
    client->privmsg (config->IRC_Channel, "%s -> Config %s", source->nick,
                     strtok (args, " "));
  }
  else
    client->notice (source->nick, "Unable to set (Error).");
}

void
Game::do_rehash (source_struct * source, char *args)
{
  if (config->load (args))
    client->privmsg (config->IRC_Channel, "%s -> Load %s", source->nick,
                     args);
  else
    client->notice (source->nick, "Error loading %s", args);
}

void
Game::do_set (source_struct * source, char *args)
{
  char *nick, *value;
  int nvalue;

  pi *p;

  nick = value = 0;
  nvalue = 0;

  nick = strtok (args, " ");
  value = strtok (NULL, "");


  if (!nick || !value || !m_status.active)
    return;

  nvalue = atoi (value);

  if (nvalue < 1)
  {

    if (player->is_player (nick))
    {
      player->del_player (nick);
      client->privmsg (config->IRC_Channel,
                       "\002%s\002 has been removed from the game by \002%s\002",
                       nick, source->nick);
    }
    return;
  }
  //Get player or create if they dont already exist
  if (player->is_player (nick))
    p = player->find_pi (nick);
  else
  {
    p = player->find_pi (nick);
  }

  player->set_point (nick, nvalue);     //Set their points
  client->privmsg (source->nick, "%s set to %d points.", nick, nvalue);
  client->privmsg (config->IRC_Channel,
                   "\002%s\002 has set \002%s\002 to \002%d\002 points",
                   source->nick, nick, nvalue);
}

void
Game::do_newnick (source_struct * source, char *target)
{
  char *newnick = parse->strip_leading (target);
  pi *p;
  pi *newp;

  if (!source->nick || !newnick)
    return;

  p = player->find_pi (source->nick);
  newp = player->find_pi (newnick);

  if (config->GAME_ACTIVITY_Enabled)    //Change activity key of nick
    activity.keychange (source->nick, newnick);

  if (!p && !newp)
    return;

  if (p)
    if (!newp)
      return;

  if (p && !newp)
  {
    strcpy (p->nick, newnick);
    return;
  }
}

void
Game::list_teams (char *target)
{
  player->sort ();

  if (!player->teams)
    return;

  char out[512];
  char temp[512];
  int tcount = 0;

  for (ti * t = player->teams; t; t = t->next)
  {
    tcount++;
    out[0] = 0;                 //Null term string for strcat

    if (t->members)
    {
      client->privmsg (target, "[\002%s\002] - \002%d\002 points",
                       t->name, t->points);

      for (pi * p = player->head; p; p = p->next)       //Rewrite for efficiency!
      {

        if (p->team == tcount)
        {
          strcat (out, p->nick);
          if (p->next)
            strcat (out, ", ");
        }
      }

      client->privmsg (target, out);
    }
  }
}

void
Game::list_players (char *target)
{
  //Sort player list
  player->sort ();

  pi *p = player->head;

  if (!p)
    return;

  int counter = 0;

  char out[512];
  char tmp[512];

  out[0] = 0;

  client->privmsg (target, config->TEXT_GAME_Listhead);

  do
  {

    if (p->next)
      sprintf (tmp, "%s[%d], ", p->nick, p->points);
    else
      sprintf (tmp, "%s[%d]", p->nick, p->points);
    strcat (out, tmp);

    counter++;

    if (strlen (out) >= 75)
    {
      client->privmsg (target, out);
      out[0] = 0;
    }

    p = p->next;

  }
  while (p);

  client->privmsg (target, out);
  client->privmsg (target, config->TEXT_GAME_Listtail, counter);
}

void
Game::check_winner ()
{
  int tcount = 0;

  if (config->GAME_UseTeams)
    for (ti * t = player->teams; t; t = t->next)
    {
      tcount++;
      if (t->points >= config->GAME_Points)
      {
        client->privmsg (config->IRC_Channel,
                         "'\002%s\002' has won the game!", t->name);
        stop ();
        return;
      }
    }

  else
    for (pi * p = player->head; p; p = p->next)
    {
      if (p->points >= config->GAME_Points)
      {
        client->privmsg (config->IRC_Channel, config->TEXT_GAME_Winmsg,
                         p->nick);
        stop ();
        return;
      }
    }
}

void
Game::check_answer (source_struct * source, char *msg)
{
  int value;

  pi *p;
  ti *t;

  parse->clean (msg);

  if ((strlen (msg) == 0) || !source->nick)
    return;

  for (int i = 0; i < question->m_question.answers; i++)
  {
    if (string_match
        (question->m_question.type, question->m_question.answer[i], msg))
    {

      if (!player->is_player (source->nick))
        do_newplayer (source);

      p = player->find_pi (source->nick);

      if (!p)
      {
        log->logtofile
          ("Could not create player struct for %s... terminating\n",
           source->nick);
        exit (1);
      }
      if (!(t = player->find_ti (p->team)))
      {
        log->logtofile
          ("Could not get team struct for %s... terminating\n", source->nick);
        exit (1);

      }

      if (config->GAME_TimeWise)
        value =
          (int) (((float) (config->GAME_TIME_Timeout - m_status.timer) /
                  (float) config->GAME_TIME_Timeout) *
                 config->GAME_BasePoints);
      else
        value = config->GAME_BasePoints;

      player->add_point (source->nick, value);  //Add point to player and team


      if (config->GAME_UseTeams)
        client->privmsg (config->IRC_Channel,
                         "That is correct \002%s\002, the answer is %s. %d points scored for '\002%s\002'",
                         source->nick, question->m_question.mainanswer,
                         value, t->name);
      else
        client->privmsg (config->IRC_Channel,
                         config->TEXT_GAME_Correct_Answer, value,
                         source->nick, question->m_question.mainanswer);

      if (config->GAME_SaveState)
        savestate ();

      check_winner ();
      m_status.timer = (0 - config->GAME_TIME_QuestionDelay);
      m_status.question_active = 0;
      break;
    }
  }
}

int
Game::string_match (int type, char *str1, char *str2)
{
  char strex1[512];             //512 will cover max irc string length, should be safe
  char strex2[512];

#ifdef DEBUG
  log->logtofile ("string_match(%d, %s, %s)\n", type, str1, str2);
#endif

  parse->soundex (str1, strex1);
  parse->soundex (str2, strex2);
#ifdef DEBUG
  log->logtofile ("Got back: %s %s\nGot back: %s %s\n", str1, strex1, str2,
                  strex2);
#endif

  if ((strlen (strex1) < 2) || (strlen (strex2) < 2))
    type = 0;                   //If strex is only 1 char, use literal matching

  int rval = 0;

#ifdef DEBUG
  log->logtofile ("soundex1 (%s): %s soundex2 (%s): %s\n", strex1, str1,
                  strex2, str2);
#endif

  if (type == 0)
  {
    if (!strcasecmp (str1, str2))
    {
      rval = 1;
      return rval;
    }
  }
  else if (type == 1)
  {

    if (!strcasecmp (str1, str2) || !strcasecmp (strex1, strex2))
    {
      rval = 1;
      return rval;
    }
  }
  else
  {
    client->privmsg (config->IRC_Channel,
                     "There is an error in the question (type must be 0 or 1)\n");
  }

  return 0;
}

void
Game::alarm ()
{
  char out[QUESTIONMAX + 1];

  if (!m_status.active)
    return;

  if (m_status.timer < 0)       /* In between questions */
  {
    if (!(m_status.timer % -300))
    {
      client->s (1, "AWAY :Next question in %d minutes",
                 ((0 - m_status.timer) / 60));
    }
  }

  if ((m_status.timer > 0) && (!m_status.question_active))
  {

    if (config->GAME_ACTIVITY_Enabled
        && !activity.active (config->GAME_ACTIVITY_Count,
                             config->GAME_ACTIVITY_Time))
    {                           //Activity monitor holds question back...
      m_status.timer = -1;
      return;
    }

    client->s (1, "AWAY");      /* Set us back */

    short ctr = 0;
    bool ok = 0;
    int good = 0;

    while (!ok)
    {
      good = (question->load_question ());

      if (good == 1)
      {
        ok = 1;
        ctr = 0;
      }
      else if (good == 0)
      {
        ctr++;
        /** FIXME: The line number needs to be shown, or logged to a file
         */
        client->privmsg (config->IRC_Channel, "Error in question.db");

      /** If this happens more than 3 times in a row, the database is
        * probably corrupt. Abort instead of possibly sending an infinite
        * amount of errors to the channel
        */
        if (ctr >= 3)
        {
          ctr = 0;
          stop ();
          return;
        }
      }
      else if (good == -1)
      {
        stop();
        return;
      }
    }

    m_status.question_active = 1;
    m_status.hinted = 0;

    sprintf (out, config->TEXT_GAME_Question, question->m_question.question);

    client->privmsg (config->IRC_Channel, out);
  }

  m_status.timer++;

  if (m_status.timer >= config->GAME_TIME_Timeout)
  {
    if (config->GAME_SaveState)
      savestate ();

    m_status.timer = -config->GAME_TIME_QuestionDelay;
    m_status.question_active = 0;
    if (config->GAME_ShowAnswer)
      client->privmsg (config->IRC_Channel, config->TEXT_GAME_Timeout_Show,
                       question->m_question.mainanswer);
    else
      client->privmsg (config->IRC_Channel, config->TEXT_GAME_Timeout);
  }

  if (config->GAME_UseHint)
    if ((m_status.timer > config->GAME_TIME_GiveHint) && !m_status.hinted)
      showhint ();
}

void
Game::start ()
{
  if (!m_status.active)
  {
    m_status.active = 1;
    client->privmsg (config->IRC_Channel, config->TEXT_GAME_Start,
                     question->questions (), config->GAME_TIME_StartDelay);
    if (config->GAME_UseHint)
    {
      client->privmsg (config->IRC_Channel, config->TEXT_GAME_Settings,
                       config->GAME_HintRatio);
    }
    m_status.timer = -config->GAME_TIME_StartDelay;
  }
}

void
Game::stop ()
{                               //Remove players, stop timers, set game to 0
  client->privmsg (config->IRC_Channel, "Stopping game...");

  player->remove_all ();
  player->clear_teams ();
  m_status.active = 0;
  m_status.question_active = 0;
  m_status.timer = 0;

  client->privmsg (config->IRC_Channel, "The game has been stopped. Use '.start' to begin a new round");
}

void
Game::disconnected ()
{
  stop ();
}

void
Game::showhint ()
{
  m_status.hinted = 1;

  char *out;
  int random, num;
  char *hintanswer;

  // If there is an additional answer then use that one for the hint
  // otherwise use the main answer.
  // Rationale: main answers are often very long with irrelevant
  // explanations and additonal facts but the clue should be
  // one of the shortest possible answers
  hintanswer = question->m_question.answers > 1 ?
    question->m_question.hintanswer : question->m_question.mainanswer;

  if ((int) strlen (hintanswer) < config->GAME_MinHint)
  {
    client->privmsg (config->IRC_Channel, config->TEXT_GAME_Toosmall);
    return;
  }

  out = strdup (hintanswer);

  num = strlen (hintanswer) / config->GAME_HintRatio;

  for (int i = 0; i < (int) strlen (out); i++)
  {
    if (out[i] != ' ')
      out[i] = '-';
  }

  out[0] = hintanswer[0];       /* Always show first letter */

  for (int i = 0; i < (num - 1); i++)
  {
    do
    {
      random =
        1 +
        (int) ((double) rand () * (strlen (hintanswer) - 1 + 1.0) /
               (RAND_MAX + 1.0));

    }
    while (out[random] == ' ' || out[random] != '-');

    out[random] = hintanswer[random];
  }

  client->privmsg (config->IRC_Channel, config->TEXT_GAME_Hint, out);

  delete[]out;
}

int
Game::check_adminpass (char *password)
{
  if (!strcmp (password, config->CLIENT_AdminPass))
    return 1;
  else
    return 0;
}

void
Game::savestate ()
{
  ofstream out;
  out.open ("game.state", ios::out);

  if (out.fail ())
  {
    stop ();
    return;
  }

  /* Save states and counters */
  out.write ((char *) &m_status.active, sizeof (int));
  out.write ((char *) &m_status.lastlist, sizeof (time_t));
  player->savestate (out);
  question->savestate (out);

  out.close ();
}

void
Game::loadstate ()
{
  ifstream in;
  in.open ("game.state", ios::in);

  if (in.fail ())
  {
    stop ();
    return;
  }

  /* Load states and game counters */
  in.read ((char *) &m_status.active, sizeof (int));
  in.read ((char *) &m_status.lastlist, sizeof (time_t));

  m_status.timer = -10;
  m_status.question_active = 0;
  m_status.hinted = 0;

  player->loadstate (in);       //Load player status
  question->loadstate (in);     //Load question/history status

  in.close ();
}
