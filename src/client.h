/*
 * game.h
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


#ifndef CLIENT_H
#define CLIENT_H

#include "h.h"

#include "log.h"
#include "options.h"
#include "parse.h"
#include "game.h"
#include "blitzed_config.h"
#include "client.h"
#include "question.h"



struct ignore_struct
{
  ignore_struct *next;
  char *mask;

};

class Game;

class Client
{

public:

  Game * game;
  Log *log;
  Parse *parse;
  Config *config;


  ignore_struct *ignore;

    Client ();
   ~Client ();

  int m_alarmed;

  void begin ();

/* Parsing */
  void parse_line (char *line);
  void parse_privmsg (source_struct * source, char *line);
  void parse_part (source_struct * source, char *line);
  void parse_newnick (source_struct * source, char *line);
  void parse_join (source_struct * source, char *line);
  void parse_kick (source_struct * source, char *line);

/* Ignores */
  void do_ignore (source_struct * source, char *args);
  int ignore_check (char *mask);

  void ignore_list (source_struct * source);
  void ignore_add (source_struct * source, char *mask);
  void ignore_del (source_struct * source, char *mask);
  void ignore_load ();
  void ignore_save ();


/* Client Actions */
  void s (int priority, const char *data, ...);
  void sendq ();
  void connect_to (const char *host, const char *port);
  void close_socket ();
  void privmsg (char *target, const char *msg, ...);
  void notice (char *target, const char *msg, ...);
  void do_register ();
  void do_ctcp (source_struct * source, char *target, char *msg);
  void get_from_server ();
  void reconnect ();
  void get_localhost ();
  int get_line ();
  void alarm ();



private:

  struct client_time
  {
    timeval last_line;
    timeval last_checked_join;
  } m_time;

  struct status_struct
  {
    int registered;
    int joined;
  } m_status;


  struct send_struct
  {
    send_struct *next;
    char *data;
    int priority;
  } *m_sendq;

  int m_sockfd;
  char m_tmpline[RECVSIZE];

  struct hostent *m_server_hostname;
  struct sockaddr_in m_server_addr;
  struct hostent *m_localhost;

  char m_localhostname[256];
  int m_locallongip;

  fd_set m_fdset;
  struct timeval m_timeout;
};

#endif
