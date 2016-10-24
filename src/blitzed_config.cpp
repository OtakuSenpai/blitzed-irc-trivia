/*
 * blitzed_config.cpp
 *
 * Copyright (C) 2001  Erik Fears
 *
 * Copyright (C) 2016  Andy Alt (andy400-dev@yahoo.com)
 *
 *  This file is part of Blitzed IRC Trivia
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

Config::Config ()
{
}

Config::~Config ()
{
}

int
Config::load (char *file)
{
  ifstream in;
  char line[4096];
  int error = 0;

  in.open (file, ios::in);

  if (in.fail ())
  {
    log->logtofile ("Error opening %s\n", file);
    in.close ();
    return 0;
  }

  //Parse config file
  do
  {
    in.getline (line, 4096, '\n');

    if (line[0] == '#')
      continue;

    parse_entry (line);
  }
  while (!in.eof ());

  int i;
  for (i = 0; i < m_size; i++)
  {
    if (!(m_table[i].flag))
    {
      error = 1;
      log->logtofile ("Error reading '%s' in config file.\n",
                      m_table[i].name);
    }
  }

  if (error)
  {
    log->
      logtofile ("Terminating due to unrecoverable errors in config file\n");
    exit (1);
  }

  in.close ();
  return 1;
}

int
Config::parse_entry (char *entry)
{
  int num;
  char *variable, *value, *s = NULL;

  if (!entry)
    return 0;

  variable = strtok (entry, " ");

  if (!entry || !variable)
    return 0;

  value = strtok (NULL, "");

  if (!value)
    return 0;

  int i = 0;
  for (i = 0; i < m_size; i++)
  {
    if (!strcasecmp (m_table[i].name, variable))
    {
      switch (m_table[i].type)
      {
      case TYPE_INT:
        *(int *) m_table[i].value = strtol (value, &s, 0);
        if (m_table[i].value)
          m_table[i].flag = 1;
        return 1;

      case TYPE_STRING:
        if (m_table[i].flag)
          delete[] * (char **) m_table[i].value;

        *(char **) m_table[i].value = strdup (value);
        if (!m_table[i].value)
        {
          log->logtofile ("Error: Out of memory!\n");
          exit (1);
        }
        m_table[i].flag = 1;
        return 1;

      case TYPE_BOOL:
        num = strtol (value, &s, 0);

        if (num != 0 && num != 1)       //Is it 1/0 ? If not dont accept it (and dont flag)
          return 0;

        *(int *) m_table[i].value = num;
        m_table[i].flag = 1;
        return 1;

      }
    }
  }

  return 0;                     //No match
}
