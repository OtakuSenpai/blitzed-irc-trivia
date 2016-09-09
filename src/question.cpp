/*
 * question.cpp
 *
 * Copyright (C) 2001  Erik Fears
 *
 * This is a fork of the original project
 * (http://harlequin.sourceforge.net/)
 *
 * Copyright (C) 2016  Andy Alt (andyqwerty@users.sourceforge.net)
 * This file is part of Blitzed IRC Trivia
 * https://git.io/vicjS
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
#include "log.h"
#include "options.h"
#include "parse.h"
#include "game.h"
#include "blitzed_config.h"
#include "client.h"
#include "question.h"

Question::Question ()
{
  m_question.answer = 0;
  m_history = 0;
  m_lastaccess = 0;
}

Question::~Question ()
{
}


int
Question::load_question ()
{

  srand ((unsigned) time (NULL));

  ifstream in;

  char out[1024];
  int rand_line = 0;

  questions ();                 //Recount questions if needed
  resize_history ();            //Resize history if needed (to avoid infinite loop below)

  while (!rand_line)
  {
    rand_line =
      1 + (int) ((double) rand () * (m_lines - 1 + 1.0) / (RAND_MAX + 1.0));
    log->logtofile ("Debug: Random Line # %d\n", rand_line);

    /**
     * FIXME:
     *
     * Even with resize_history (), there is still the possibility of an
     * infinite loop
     */

    if (in_history (rand_line))
      rand_line = 0;

    else
      add_history (rand_line);

  }

  in.open (config->GAME_DB, ios::in);

  if (in.fail ())
    return 0;

  for (int i = 0; i < rand_line; i++)
  {
    in.getline (m_question.raw, QUESTIONMAX, '\n');

    /** FIXME:
     *
     * print a message somewhere that the line is too long
     */
    if (in.fail ())
      return 0;
  }

  m_question.answers = 0;

  for (int i = 0; i < (int) strlen (m_question.raw); i++)
    if (m_question.raw[i] == ';')
      m_question.answers++;

  /** This is the number of fields in the questions database
   * before the answer.
   */
  m_question.answers -= 2;

  m_question.type = atoi (strtok (m_question.raw, ";"));
  m_question.question = strtok (NULL, ";");

  if (m_question.answer)
    delete m_question.answer;

  m_question.answer = new char *[m_question.answers];

  if (!m_question.question)
    return 0;

  for (int i = 0; i < m_question.answers; i++)
  {
    m_question.answer[i] = strtok (NULL, ";");

    if (m_question.answer[i] == 0)
      break;

    if (i == 0)
      strcpy (m_question.mainanswer, m_question.answer[0]);
    if (i == 1)
      strcpy (m_question.hintanswer, m_question.answer[1]);
  }

  if (m_question.answers < 1)
    return 0;

  //Clean answers (strip non alphanumerics)
  for (int i = 0; i < m_question.answers; i++)
    parse->clean (m_question.answer[i]);

  in.close ();
  return 1;                     //No errors
}

int
Question::questions ()
{
  ifstream in;
  struct stat filestats;

  int num_lines = 0;
  char temp[1024];

  //Check to see if we already have current information on the DB
  stat (config->GAME_DB, &filestats);

  if (!(filestats.st_mtime > m_lastaccess))     //If so, return
  {
    return m_lines;
  }
  //Else count the lines again
  m_lines = 0;

  in.open (config->GAME_DB, ios::in);

  if (in.fail ())
    return 0;

  while (in.getline (temp, 1024, '\n') && !in.eof ())
    m_lines++;

  in.close ();

  time (&m_lastaccess);         //Update last access

  return m_lines;
}

/*
 * Keep history size within 50%
 */



void
Question::resize_history ()
{
  history_struct *p;
  int diff;

  diff = m_historysize - (m_lines / 2); //Find how much over 50% we are

  for (int i = 0; i < diff; i++)        //Delete that much
  {
    p = m_history->next;
    delete m_history;
    m_historysize--;
    m_history = p;
  }


}


/*
 *  Check if value is contained in the list
 */

int
Question::in_history (int value)
{

  for (history_struct * p = m_history; p; p = p->next)  //Traverse history sequentially
    if (value == p->value)      //Return 1 if found
      return 1;
  return 0;
}

/*
 *  Add to list
 * List is not sorted for efficiency because we want to know what
 * questions were asked in what order!
 */

void
Question::add_history (int value)
{
  history_struct *newlink;
  history_struct *p1, *p2;

  newlink = new history_struct;

  if (!m_history)               //Add it as first element
  {
    m_history = newlink;
    newlink->value = value;
    newlink->next = 0;
    m_historysize++;
  }
  else                          //Else add to tail
  {
    for (p1 = m_history; p1->next; p1 = p1->next);      //Find tail

    p1->next = newlink;
    newlink->value = value;
    newlink->next = 0;
    m_historysize++;
  }

  //Once it is added, remove head if we're at the history limit (50%)
  if ((m_lines / 2) < m_historysize)
  {
    p2 = m_history->next;       //Keep temp pointer to the second element

    m_historysize--;            //Delete head
    delete m_history;

    m_history = p2;             //Make second element, the new head
  }

}


void
Question::savestate (ofstream & out)
{

  //Write out history size
  out.write ((char *) &m_historysize, sizeof (int));
  //Write out history
  for (history_struct * h = m_history; h; h = h->next)
    out.write ((char *) &(h->value), sizeof (int));

}

void
Question::loadstate (ifstream & in)
{

  int length;
  history_struct *th;           // tmp ptr

  //Clear history
  for (history_struct * h = m_history; h;)
  {
    if (!h->next)
    {
      delete h;
      break;
    }
    th = h->next;
    delete h;
    h = th;
  }

  m_history = 0;

  in.read ((char *) &length, sizeof (int));

  for (int i = 0; i < length; i++)
  {
    th = new history_struct;
    in.read ((char *) &(th->value), sizeof (int));

    if (!m_history)
    {
      m_history = th;
      th->next = 0;
    }
    else
    {

      for (history_struct * h = m_history; h; h = h->next)
      {
        if (!h->next)
        {
          h->next = th;
          th->next = 0;
          break;
        }

      }


    }

  }
}
