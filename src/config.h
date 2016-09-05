#ifndef CONF_H
#define CONF_H

#include "log.h"

class Config {

public:


   Log *log;
   config_hash *m_table;
   int m_size;


   int IRC_Port;

   char *IRC_Server;
   char *IRC_Channel;
   char *CLIENT_Nick;
   char *CLIENT_Username;
   char *CLIENT_Realname;
   int   CLIENT_TIME_Timeout;
   char *CLIENT_AdminPass;
   char *CTCP_Version;
   char *GAME_DB;   
   int   GAME_Points;
   int   GAME_BasePoints;
   int   GAME_TimeWise;
   int   GAME_UseTeams;
   int   GAME_NumTeams;
   int   GAME_ACTIVITY_Enabled;
   int   GAME_ACTIVITY_Count;
   int   GAME_ACTIVITY_Time;
   int   GAME_HintRatio;
   int   GAME_UseHint;
   int   GAME_MinHint;
   int   GAME_AutoRem;
   int   GAME_ShowAnswer;
   int   GAME_TIME_Timeout;
   int   GAME_TIME_QuestionDelay;
   int   GAME_TIME_StartDelay;
   int   GAME_TIME_GiveHint;   
   int   GAME_SaveState;
   int   LOG_FromServer;

   char *TEXT_GAME_Newplayer;
   char *TEXT_GAME_Notplayer;
   char *TEXT_GAME_Removed;
   char *TEXT_GAME_HasLeft;
   char *TEXT_GAME_Listhead;
   char *TEXT_GAME_Listtail;
   char *TEXT_GAME_Correct_Answer;
   char *TEXT_GAME_Winmsg;
   char *TEXT_GAME_Question;
   char *TEXT_GAME_Timeout_Show;
   char *TEXT_GAME_Timeout;
   char *TEXT_GAME_Start;
   char *TEXT_GAME_Settings;
   char *TEXT_GAME_Toosmall;
   char *TEXT_GAME_Hint;
   char *TEXT_GAME_Password;  
   
   Config();
   ~Config();
   int load(char *file);
   int parse_entry(char *entry);


};





#endif
