#ifndef ACTIVITY_H
#define ACTIVITY_H

struct activity_struct
{
  activity_struct *next;
  char *key;
  timeval last;
};


class Activity
{

private:
  activity_struct * m_head;


public:

  void update (char *key);
  void keychange (char *key, char *newkey);
  int active (int keys, int time);

    Activity ();
   ~Activity ();

};

#endif
