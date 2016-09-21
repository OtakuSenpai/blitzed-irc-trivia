/*
Copyright (C) 2001  Erik Fears

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software

      Foundation, Inc.
      59 Temple Place - Suite 330
      Boston, MA  02111-1307, USA.

*/

#include "h.h"

Player::Player()
{
   head  = 0;   //Null point the linked lists
   tail  = 0;
   teams = 0;
}

Player::~Player()
{
}

void Player::add_player(char *nick)
{
   pi *newp = new pi;
   strncpy(newp->nick, nick, NICKMAX);

   newp->points = 0;            //0 points
   newp->team = select_team();  //Select a team for them (even if UseTeam = 0)

   add_pi(newp);
}


ti *Player::find_ti(int team)
{
   int i = 0;

   for(ti *t = teams;t;t = t->next)
     {
        i++;
	if (team == i)
           return t;
     }
   return 0;
}


int Player::team_points(int team)
{

   int retval = 0;

   for(pi *p = head;p;p = p->next)
     if(p->team == team)
       retval += p->points;
   return retval;


}


int Player::is_lead(char *nick)
{

  int max = 0;
  pi *p = find_pi(nick);

  if(!p)
    return 0;

  for(pi *p2 = head;p2;p2 = p2->next)
    if(p2->team == p->team && p2->points > max)
      max = p2->points;

  if(p->points == max)
    return 1;
  else
    return 0;
}


void Player::set_name(int team, char *name)
{
   ti *t;

   if(!(t = find_ti(team)))
     return;

   strncpy(t->name, name, (TEAMMAX -1));
   time(&(t->lastname));
}


/*
 *  Return a team which is eligible for a new player
 */

int Player::select_team()
{

  int i;
  int tcount = 0;
  int low[2];

  for(ti *t = teams; t; t = t->next)
    tcount++;

  //If there is a team which doesnt exist yet, create it
  if(tcount < config->GAME_NumTeams)
    {
       ti *newteam;
       newteam = new ti;

       newteam->next = 0;
       snprintf(newteam->name, TEAMMAX,"Team %d",tcount+1); //Default team name
       newteam->members = 1;                        //Initialise to 1 member
       newteam->points  = 0;
       newteam->lastname = 0;                       //Allow name change from the start
       if(!teams)                                   //Link to team list
         teams = newteam;
       else
        { for(ti *t = teams;t;t = t->next)           //Else add team to tail
	   {
	    if(!t->next)
	     {
	       t->next = newteam;
	       break;
	     }
	   }
        }
       return (tcount + 1);                           //Add them to this new team

    }

  //Else find team with lowest members and add to it

  low[0] = 1;  //Team that holds lowest so far
  low[1] = teams->members;  //Amount of members
  tcount = 0;

  for(ti *t = teams; t; t = t->next) //Traverse keeping track of lowest count yet
   {
    tcount++;
    if(t->members <= low[1])
      {
        low[0] = tcount;
	low[1] = t->members;
      }
   }

   i = 0;
   for(ti *t = teams;t;t = t->next)
    {
       i++;
       if(low[0] == i)         //Find lowest and increment members by 1
         t->members++;
    }
   return low[0];              //Assign to this team
}

void Player::add_point(char *nick, int value)
{
    pi *p;
    ti *t;

    if(!is_player(nick))
      return;

    p = find_pi(nick);
    p->points += value;

    t = find_ti(p->team);
    t->points += value;

}

void Player::set_point(char *nick, int value)
{
    pi *p;

    if(!is_player(nick))
      return;

    p = find_pi(nick);
    p->points = value;
}




void Player::del_player(char *nick)
{

  pi *p = find_pi(nick);
  ti *t;

  if(p)
   {
     t = find_ti(p->team);
     if(t)
       t->members--;
     remove_pi(p);
   }
}

int Player::is_player(char *nick)
{
  pi *p = find_pi(nick);

  if(p)
    return 1;
  else
    return 0;
}

void Player::add_pi(pi *newp)
{

   if(head == NULL)
     {
         head = tail = newp;
         head->next = NULL;
         head->prev = NULL;
     }
   else
    {
      newp->prev = tail;
      newp->next = NULL;

      tail->next = newp;
      tail = newp;
    }

}

pi *Player::find_pi(char *nick)
{

   pi *p = head;

   while(p && p->nick)
    {

       if(!strcasecmp(p->nick, nick))
           return p;

       p = p->next;

    }

   return NULL;
}

void Player::remove_pi(pi *p)
{

     pi *next, *prev;

     if(!p)
        return;

     if( (p->next != NULL) && (p->prev != NULL))
       {
           next = p->next;
           prev = p->prev;

           prev->next = next;
           next->prev = prev;

           delete p;
       }

    else if ((p->next == NULL) && (p->prev == NULL))
       {
           head = NULL;
           tail = NULL;
           delete p;
       }

    else if (p->next == NULL)
       {
           tail = p->prev;
           prev = p->prev;
           prev->next = NULL;

           delete p;
       }
    else if (p->prev == NULL)
      {
           head = p->next;
           next = p->next;
           next->prev = NULL;
           delete p;
      }

}

void Player::remove_all()
{

   pi *p = head;
   pi *prev;

   while(p)
   {
     prev = p;
     p = p->next;
     delete prev;
   }

   head = NULL;
   tail = NULL;

}

void Player::clear_teams()
{

   for(ti *t = teams;t;t = t->next)
    {
        t->members = 0;
        t->points  = 0;
    }
}


int Player::num_players()
{

  pi *p = head;
  int counter = 0;

  if(!p)
    return 0;

  while(p)
  {
    counter++;
    p = p->next;
  }
  return counter;
}

int Player::num_teams()
{
   ti *t = teams;
   int counter = 0;

   if(!t)
     return 0;

   while(t)
    {
       counter++;
       t = t->next;
    }
   return counter;
}


void Player::savestate(ofstream &out)
{

    int players;
    int numteams;

    players = num_players();
    numteams   = num_teams();

    //Write number of players at head of file
    out.write((char *) &players, sizeof(int));
    for(pi *p = head;p;p = p->next)
     {
         out.write((char *) p->nick, (NICKMAX + 1));
         out.write((char *) &(p->points), sizeof(int));
         out.write((char *) &(p->team), sizeof(int));
     }


    //Write team structs out to file
    out.write((char*) &numteams, sizeof(int));

    for(ti *t = teams;t;t = t->next)
     {
         out.write((char *) t->name, TEAMMAX);
         out.write((char *) &(t->members), sizeof(int));
         out.write((char *) &(t->points) , sizeof(int));
         out.write((char *) &(t->lastname), sizeof(time_t));
     }

}

void Player::loadstate(ifstream &in)
{

    int players; // How many players we're loading
    int numteams; // How many teams we're loading
    pi *p;

    //Clear what we have for new data
    remove_all();
    clear_teams();


    //Read players
    //First sizeof(int) is the numbers of following players stored
    in.read((char *) &players, sizeof(int));

    for(int i = 0; i < players; i++)
     {
        p = new pi;

        in.read((char *) &(p->nick), (NICKMAX + 1));


        in.read((char *) &(p->points), sizeof(int));
        in.read((char *) &(p->team), sizeof(int));

        //Add p to list

        p->next = 0;
        p->prev = 0;

        if(!head)
         {
          head = p;
          tail = p;
         }
        else
         {
            for(pi *tmp = head;tmp; tmp = tmp->next)
             {
                 if(!tmp->next)
                  {
                     tmp->next = p;
                     p->prev = tmp;
                     tail = p;
                     break;
                  }
             }

         }

     }



     //Read teams

     in.read((char *) &numteams, sizeof(int));
     for(int i = 0; i < numteams; i++)
      {
         ti *t = new ti;

         in.read((char *) t->name, TEAMMAX);
         in.read((char *) &(t->members), sizeof(int));
         in.read((char *) &(t->points) , sizeof(int));
         in.read((char *) &(t->lastname), sizeof(time_t));

         t->next = 0;

         if(!teams)
            teams = t;

         else
          {
              for(ti *tt = teams; tt; tt = tt->next)
                {
                     if(!tt->next)
                       {
                          tt->next = t;
                          break;
                       }
                }
          }
      }
}


//Sort player list by points (selection sort)
void Player::sort()
{

   int ntarget;
   pi *ptarget;

   for(pi *p = head;p;p = p->next)
    {
        ntarget = p->points;
        ptarget = p;

        if(!p->next)  //If we reached the tail thats it!
             break;

        //Traverse list finding suitable target
        //to take place before p.

        for(pi *pp = p->next;pp;pp = pp->next)
         {
                  //Its the same as current target
               if(pp->points == ntarget)
                {
                           //And its greater alphabetically
                     if(strcmp(pp->nick, ptarget->nick) < 0)
                      {    //Change target
                           ptarget = pp;
                           continue;
                      }

                 }

               if((pp->points > ntarget))
                 {
                    ptarget = pp;
                    ntarget = pp->points;
                 }
         }

        //No suitable selection found
        //meaning current element is in
        //proper place. Go on to next
        //element

        if(ntarget == p->points)
          continue;

         else  //Suitable target was found, lets relink it to the new location.
          {
               //Unlink target
               if(!ptarget->next)    //It was a tail
                 {
                      tail = ptarget->prev;
                      ptarget->prev->next = 0;
                  }
               else
                 {         //Link around target
                    ptarget->next->prev = ptarget->prev;
                    ptarget->prev->next = ptarget->next;
                  } //Target is now free


                //Now lets link it before the current element
                if(!p->prev) //Current element IS the head
                  {
                      p->prev = ptarget;
                      ptarget->prev = 0;
                      ptarget->next = p;
                      head = ptarget;
                   }
                 else
                   {
                      p->prev->next = ptarget;
                      ptarget->prev = p->prev;
                      ptarget->next = p;
                      p->prev = ptarget;
                    }

                p = p->prev;   //Set us back one so we can check again

                continue;
           }
    }
}
