#ifndef PLAYER_H
#define PLAYER_H


struct pi
{
   pi *next, *prev;
   char nick[NICKMAX + 1];
   char password[PASSMAX + 1];
   int ghost;
   int points;
   int team;
};

struct ti
{
   ti* next;
   char name[TEAMMAX];
   int members;
   int points;
   time_t lastname;
};

class Player {

   public:
 
     Config *config;
	   
     void remove_all();
     void clear_teams();

     int  is_player(char *nick); 
     int  num_players();
     int  num_teams();
     void add_point(char *nick, int value);
     void set_point(char *nick, int value);
     
     void add_player(char *nick);
     void del_player(char *nick);

     int select_team();
     int team_points(int team);
     int is_lead(char *name);
     void set_name(int team, char *name);
     
     void add_pi(pi *newplayer);
     void remove_pi(pi *player);
     void sort();

     pi *find_pi(char *nick);
     ti *find_ti(int team);

     //State saving
     void savestate(ofstream &out);
     void loadstate(ifstream &in);
     
     Player();
     ~Player();

     pi *head, *tail;
     ti *teams;
          
};

#endif 
