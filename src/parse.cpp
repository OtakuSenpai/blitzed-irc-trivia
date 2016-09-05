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

int exkey[] = {'0','1','2','3','0','1','2','0','0','2','2',
               '4','5','5','0','1','2','6','2','3','0','1',
	       '0','2','0','2'};

Parse::Parse()
{
}

Parse::~Parse()
{
}
  


char *Parse::strip_leading(char *line)
{
   *line++;
   return line;
}

char *Parse::get_nick(char *line)
{
   char *nick;
   nick = strtok(line, "!");
   return nick;
}

source_struct *Parse::get_source(char *source)              
{
   source_struct *retsource = new source_struct;
   retsource->nick = strip_leading(strtok(source, "!"));
   retsource->user = strtok(NULL, "@");
   retsource->host = strtok(NULL, "");

   if(!retsource->nick || !retsource->user || !retsource->host)
        return NULL;

   return retsource;
}

void Parse::soundex(char *str, char *strex)
{

  int pos = 0;
  int num = 1;

  if((tolower(str[0]) == 'p') && (tolower(str[1]) == 'h'))
    {
      strex[0] = 'f';
      pos = 2;
    }
  else
    {
      strex[0] = str[0];
      pos = 1;
    }
  while(str[pos])
   {
      if((str[pos] == 'p') && (str[pos + 1] == 'h'))
       {
         strex[num++] = '1';
         pos += 2;
         continue;
       }

      if((str[pos] >= 97) && (str[pos] <= 122))
        if((exkey[ (97 - (int) str[pos]) * - 1 ] != '0') && ((exkey[(97 - (int) str[pos]) * -1]) != strex[num - 1]))
          strex[num++] = exkey[ (97 - (int) str[pos]) * -1 ];
      pos++;
   }

   strex[num] = '\0';
}


void Parse::clean(char *str)
{
   char *cleaned, *in;

   for(in=cleaned=str;*in;in++) 
     if(isalnum(*in)) 
        *cleaned++ = tolower(*in);
   *cleaned = 0;
}


//Sloppy!
// Parse seconds into weeks/days/hours/minutes/seconds and store in output
void Parse::timeparse(timestruct &output, int value)
{
   output.weeks = value / 604800;             //Calculate weeks
   value -= output.weeks * 604800;

   output.days = value / 86400;               //Calculate days
   value -= output.days * 86400;

   output.hours = value / 3600;               //Calculate hours
   value -= output.hours * 3600;

   output.minutes = value / 60;
   value -= output.minutes * 60;

   output.seconds = value;                    //Remaining is seconds
}
