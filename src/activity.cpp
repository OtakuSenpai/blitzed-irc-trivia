/*
 * activity.cpp
 *
 * Copyright (C) 2001  Erik Fears
 *
 * This is a fork of the original project
 * (http://harlequin.sourceforge.net/)
 *
 * Copyright (C) 2016  Andy Alt (andy400-dev@yahoo.com)
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

Activity::Activity ()
{
  m_head = 0;                   //Init list
}

Activity::~Activity ()
{
}

//Update key's entry to current time
void
Activity::update (char *key)
{

  activity_struct *newap;
  activity_struct *ap;

  if (!m_head)
  {
    m_head = new activity_struct;
    m_head->key = strdup (key);
    gettimeofday (&(m_head->last), NULL);

    m_head->next = 0;
    return;
  }

  ap = m_head;

  while (ap)
  {
    if (!ap->next)
    {
      newap = new activity_struct;
      newap->key = strdup (key);
      gettimeofday (&(newap->last), NULL);
      ap->next = newap;
      newap->next = 0;
      return;
    }

    if (!strcasecmp (key, ap->key))
    {
      gettimeofday (&(ap->last), NULL);
      return;
    }

    ap = ap->next;

  }

}

//Change key to newkey if it exists
void
Activity::keychange (char *key, char *newkey)
{

  for (activity_struct * ap = m_head; ap; ap = ap->next)
  {
    if (!strcasecmp (key, ap->key))
    {
      delete ap->key;
      ap->key = strdup (newkey);
    }

  }


}

//Check if 'keys' amount of keys have been active within time seconds
int
Activity::active (int keys, int time)
{
  int count = 0;

  timeval present;
  gettimeofday (&(present), NULL);

  for (activity_struct * ap = m_head; ap; ap = ap->next)
  {
    if ((present.tv_sec - ap->last.tv_sec) < time)
      count++;
  }

  if (count >= keys)
    return 1;
  else
    return 0;

}
