/*
 * main.cpp
 *
 * Copyright (C) 2001  Erik Fears
 *
 * This is a fork of the original project
 * (http://harlequin.sourceforge.net/)
 *
 * Copyright (C) 2016  Andy Alt (andyqwerty@users.sourceforge.net)
 * This file is part of Blitzed IRC Trivia
 * (https://git.io/vicjS)
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

void do_alarm(int);


Client client;
Game game;
Parse parse;
Log log;
Player player;
Config config;
Question question;

config_hash table[] = {
      { "IRC_Server"              ,  TYPE_STRING  ,0,   &(config.IRC_Server)               },
      { "IRC_Port"                ,  TYPE_INT     ,0,   &(config.IRC_Port)                 },
      { "IRC_Channel"             ,  TYPE_STRING  ,0,   &(config.IRC_Channel)              },
      { "CLIENT_Nick"             ,  TYPE_STRING  ,0,   &(config.CLIENT_Nick)              },
      { "CLIENT_Username"         ,  TYPE_STRING  ,0,   &(config.CLIENT_Username)          },
      { "CLIENT_TIME_Timeout"     ,  TYPE_INT     ,0,   &(config.CLIENT_TIME_Timeout)      },
      { "CLIENT_AdminPass"        ,  TYPE_STRING  ,0,   &(config.CLIENT_AdminPass)         },
      { "GAME_DB"                 ,  TYPE_STRING  ,0,   &(config.GAME_DB)                  },
      { "GAME_Points"             ,  TYPE_INT     ,0,   &(config.GAME_Points)              },
      { "GAME_BasePoints"         ,  TYPE_INT     ,0,   &(config.GAME_BasePoints)          },
      { "GAME_TimeWise"           ,  TYPE_BOOL    ,0,   &(config.GAME_TimeWise)            },
      { "GAME_UseTeams"           ,  TYPE_BOOL    ,0,   &(config.GAME_UseTeams)            },
      { "GAME_NumTeams"           ,  TYPE_INT     ,0,   &(config.GAME_NumTeams)            },
      { "GAME_ACTIVITY_Enabled"   ,  TYPE_BOOL    ,0,   &(config.GAME_ACTIVITY_Enabled)    },
      { "GAME_ACTIVITY_Count"     ,  TYPE_INT     ,0,   &(config.GAME_ACTIVITY_Count)      },
      { "GAME_ACTIVITY_Time"      ,  TYPE_INT     ,0,   &(config.GAME_ACTIVITY_Time)       },
      { "GAME_HintRatio"          ,  TYPE_INT     ,0,   &(config.GAME_HintRatio)           },
      { "GAME_UseHint"            ,  TYPE_BOOL    ,0,   &(config.GAME_UseHint)             },
      { "GAME_MinHint"            ,  TYPE_INT     ,0,   &(config.GAME_MinHint)             },
      { "GAME_AutoRem"            ,  TYPE_BOOL    ,0,   &(config.GAME_AutoRem)             },
      { "GAME_ShowAnswer"         ,  TYPE_BOOL    ,0,   &(config.GAME_ShowAnswer)          },
      { "GAME_TIME_Timeout"       ,  TYPE_INT     ,0,   &(config.GAME_TIME_Timeout)        },
      { "GAME_TIME_StartDelay"    ,  TYPE_INT     ,0,   &(config.GAME_TIME_StartDelay)     },
      { "GAME_TIME_QuestionDelay" ,  TYPE_INT     ,0,   &(config.GAME_TIME_QuestionDelay)  },
      { "GAME_TIME_GiveHint"      ,  TYPE_INT     ,0,   &(config.GAME_TIME_GiveHint)       },
      { "GAME_SaveState"          ,  TYPE_INT     ,0,   &(config.GAME_SaveState)           },
      { "LOG_FromServer"          ,  TYPE_BOOL    ,0,   &(config.LOG_FromServer)           },
      { "TEXT_GAME_Newplayer"     ,  TYPE_STRING  ,0,   &(config.TEXT_GAME_Newplayer)      },
      { "TEXT_GAME_Notplayer"     ,  TYPE_STRING  ,0,   &(config.TEXT_GAME_Notplayer)      },
      { "TEXT_GAME_Removed"       ,  TYPE_STRING  ,0,   &(config.TEXT_GAME_Removed)        },
      { "TEXT_GAME_HasLeft"       ,  TYPE_STRING  ,0,   &(config.TEXT_GAME_HasLeft)        },
      { "TEXT_GAME_Listhead"      ,  TYPE_STRING  ,0,   &(config.TEXT_GAME_Listhead)       },
      { "TEXT_GAME_Listtail"      ,  TYPE_STRING  ,0,   &(config.TEXT_GAME_Listtail)       },
      { "TEXT_GAME_Correct_Answer",  TYPE_STRING  ,0,   &(config.TEXT_GAME_Correct_Answer) },
      { "TEXT_GAME_Winmsg"        ,  TYPE_STRING  ,0,   &(config.TEXT_GAME_Winmsg)         },
      { "TEXT_GAME_Question"      ,  TYPE_STRING  ,0,   &(config.TEXT_GAME_Question)       },
      { "TEXT_GAME_Timeout"       ,  TYPE_STRING  ,0,   &(config.TEXT_GAME_Timeout)        },
      { "TEXT_GAME_Start"         ,  TYPE_STRING  ,0,   &(config.TEXT_GAME_Start)          },
      { "TEXT_GAME_Settings"      ,  TYPE_STRING  ,0,   &(config.TEXT_GAME_Settings)       },
      { "TEXT_GAME_Toosmall"      ,  TYPE_STRING  ,0,   &(config.TEXT_GAME_Toosmall)       },
      { "TEXT_GAME_Hint"          ,  TYPE_STRING  ,0,   &(config.TEXT_GAME_Hint)           },
      { "TEXT_GAME_Password"      ,  TYPE_STRING  ,0,   &(config.TEXT_GAME_Password)       },
      { "TEXT_GAME_Timeout_Show"  ,  TYPE_STRING  ,0,   &(config.TEXT_GAME_Timeout_Show)   },
};


int main(int argv, char *args[])
{


   ofstream pidout;
   int pid;

/* Interclass access */

   client.game   = &game;
   client.parse  = &parse;
   client.log    = &log;
   client.config = &config;

   game.parse    = &parse;
   game.client   = &client;
   game.log      = &log;
   game.player   = &player;
   game.config   = &config;
   game.question = &question;

   player.config = &config;

   question.config = &config;
   question.parse  = &parse;

   config.log    = &log;

/* Check command line args */

  if(argv != 2)
   {
      printf("Syntax: trivia config_file\n");
      return 0;
   }



/* Background */
#ifndef WIN32
   if(pid = fork())
    {
       pidout.open("trivia.pid", ios::out);
       if(!pidout.fail())
            pidout << pid;
       pidout.close();
       exit(0);
    }
#endif /* ifndef WIN32 */

/* Load config via table */
   config.m_table = table;
   config.m_size = (sizeof(table) / sizeof(config_hash));
   if(!config.load(args[1]))
      exit(1);

/* Prepare Alarm SIGNAL for timer */

   signal(SIGALRM,do_alarm);
   alarm(BASETIME);

/*Pass control to Client class*/
   client.get_localhost();
   client.init_socket();
   client.connect_to(config.IRC_Server , config.IRC_Port);
   client.ignore_load();
   client.begin();

   return 0;
}


void do_alarm(int notused)
{
   client.m_alarmed = 1;
   alarm(BASETIME);
}
