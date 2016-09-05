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

#ifndef DATA_H
#define DATA_H

struct source_struct
{
    char *nick; 
    char *user;
    char *host;
};


#define TYPE_INT    1
#define TYPE_STRING 2
#define TYPE_BOOL   3

struct config_hash {
        char *name;
        int type;
        int flag;
        void *value;
};

#endif /* DATA_H */
