/*
 * game.h
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

#ifndef GAME_H
#define GAME_H

#include "log.h"
#include "options.h"
#include "parse.h"
#include "game.h"
#include "config.h"
#include "client.h"
#include "question.h"
#include "activity.h"
#include "player.h"

class Client;

class Game
{

public:
  Client * client;
  Player *player;
  Parse *parse;
  Log *log;
  Config *config;
  Question *question;


  Activity activity;

  struct status_struct
  {
    int active;
    int question_active;
    int timer;
    int hinted;
    time_t lastlist;
  } m_status;

  time_t m_start;               //for uptime

/* MSG commands */
  void do_stop (source_struct * source, char *args);
  void do_start (source_struct * source, char *args);
  void do_add (source_struct * source, char *args);
  void do_set (source_struct * source, char *args);
  void do_admin_list (source_struct * source, char *args);
  void do_force (source_struct * source, char *args);
  void do_die (source_struct * source, char *args);
  void do_config (source_struct * source, char *args);
  void do_rehash (source_struct * source, char *args);

  void do_nameteam (source_struct * source, char *passwd, char *args);
  void do_identify (source_struct * source, char *passwd, char *args);
  void do_regain (source_struct * source, char *passwd, char *args);

/* Channel Commands */
  void do_rem (source_struct * source, char *args);
  void do_status (source_struct * source, const char *args);
  void do_uptime (source_struct * source, const char *args);

/* Handlers for IRC stuff */
  void do_channel (source_struct * source, char *target, char *msg);
  void do_part (source_struct * source, char *target);
  void do_join (source_struct * source, char *target);
  void do_newnick (source_struct * source, char *target);
  void do_msg (source_struct * source, char *target, char *cmd, char *rest);

/* Game Stuff */
  void do_newplayer (source_struct * source);
  void check_answer (source_struct * source, char *msg);
  void check_winner ();
  void clean (char *str);
  void alarm ();
  void list_players (char *target);
  void list_teams (char *target);
  void reset_scores ();
  void showhint ();
  int string_match (int type, char *str1, char *str2);
  int check_adminpass (char *password);
  void start ();
  void stop ();
  void disconnected ();

/* State Saving */
  void savestate ();
  void loadstate ();

    Game ();
   ~Game ();

};

#endif
