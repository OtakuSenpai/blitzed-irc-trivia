/*
 * parse.h
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


#ifndef PARSE_H
#define PARSE_H


struct timestruct
{

  int weeks;
  int days;
  int hours;
  int minutes;
  int seconds;

};

class Parse
{

public:
  char *strip_leading (char *line);
  char *get_nick (char *line);
  void soundex (char *str, char *strex);
  void clean (char *str);
  void timeparse (timestruct & output, int value);


  source_struct *get_source (char *source);
    Parse ();
   ~Parse ();


};

#endif /* PARSE_H */
