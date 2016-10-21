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

#include <fstream>
#include <iostream>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <regex.h>
#include <time.h>
#include <unistd.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>

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
#define VERSION "0.8.22-dev"
#endif

#ifndef PACKAGE_NAME
#define PACKAGE_NAME "Blitzed IRC Trivia"
#endif

#ifndef PACKAGE_STRING
#define PACKAGE_STRING PACKAGE_NAME" "VERSION
#endif

#endif /* #ifndef H_H */
