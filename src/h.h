/*
 * h.h
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

#ifndef H_H
#define H_H

using namespace std;

#include <sys/types.h>



#ifndef WIN32
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <string.h>
#endif

#ifdef WIN32
#include <winsock2.h>
#endif

/* regex.h: On Windows, using MinGW32, I installed msys-libregex-dev
 * and dll package. I then had to use -I/MinGW32/msys/1.0/include to find
 * regex.h
 */
#include <regex.h>

#include <sys/time.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

#include <signal.h>
#include <stdarg.h>


#include <fstream>
#include <iostream>
#include <ctype.h>

#include "datatypes.h"

#include "activity.h"
#include "log.h"
#include "options.h"
#include "parse.h"
#include "game.h"
#include "blitzed_config.h"
#include "client.h"
#include "question.h"
#include "player.h"
#include "config.h"

#ifndef VERSION
#define VERSION "0.8.21"
#endif

#ifndef PACKAGE_STRING
#define PACKAGE_STRING "Blitzed IRC Trivia "VERSION
#endif

#endif /* #ifndef H_H */
