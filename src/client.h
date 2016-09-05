#ifndef CLIENT_H
#define CLIENT_H

#include "h.h"

#include "log.h"
#include "options.h"
#include "parse.h"
#include "game.h"
#include "config.h"
#include "client.h"
#include "question.h"



struct ignore_struct 
{
   ignore_struct *next;
   char *mask;

};

class Game;

class Client {

   public:
  
      Game *game;
      Log *log;
      Parse *parse;
      Config *config;     


      ignore_struct *ignore;

      Client();
      ~Client();

      int m_alarmed;

      void begin();

/* Parsing */
      void parse_line(char *line);
      void parse_privmsg(source_struct *source, char *line);
      void parse_part(source_struct *source, char *line);
      void parse_newnick(source_struct *source, char *line);
      void parse_join(source_struct *source, char *line);
      void parse_kick(source_struct *source, char *line);

/* Ignores */
      void do_ignore(source_struct *source, char *args);
      int ignore_check(char *mask);

      void ignore_list(source_struct *source);
      void ignore_add(source_struct *source, char *mask);
      void ignore_del(source_struct *source, char *mask);
      void ignore_load();
      void ignore_save();


/* Client Actions */
      void s(int priority,char *data,...);
      void sendq();
      void init_socket();
      void connect_to(char *host, int port);
      void close_socket();      
      void privmsg(char *target, char *msg,...);
      void notice(char *target, char *msg,...);
      void do_register();
      void do_ctcp(source_struct *source, char *target, char *msg);
      void get_from_server();
      void reconnect();
      void get_localhost();
      int get_line();
      void alarm();



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
      } * m_sendq;
      
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
