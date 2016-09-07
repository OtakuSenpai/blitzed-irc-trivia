/*
 * question.h
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

#ifndef QUESTION_H
#define QUESTION_H

struct history_struct
{
  history_struct *next;
  int value;
};


class Question
{

public:

  struct q_struct
  {
    int type;
    int answers;
    int *history;
    int historysize;
    char *question;
    char mainanswer[QUESTIONMAX + 1];
    char hintanswer[QUESTIONMAX + 1];
    char raw[QUESTIONMAX + 1];
    char **answer;
  } m_question;

    Question ();
   ~Question ();
  Log *log;

  Parse *parse;
  Config *config;

  int load_question ();
  int questions ();

  /* State Saving */
  void savestate (ofstream & out);
  void loadstate (ifstream & in);


private:

  int in_history (int value);
  void add_history (int value);
  void resize_history ();

  int m_lines;                  //Lines in question file
  time_t m_lastaccess;          //When question file was last accessed
  int m_historysize;            //Current size of history
  history_struct *m_history;    //History head


};
#endif /* QUESTION_H */
