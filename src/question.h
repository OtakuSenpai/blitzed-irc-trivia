#ifndef QUESTION_H
#define QUESTION_H

struct history_struct
 {
    history_struct *next;
    int value;
 };


class Question
{

public:	

      struct q_struct
       {
          char *nick;
          int type;   
          int answers;
          int *history;
          int  historysize;
          char *question;
          char mainanswer[QUESTIONMAX + 1];
	  char hintanswer[QUESTIONMAX + 1];
          char raw[QUESTIONMAX + 1];
          char **answer;
       } m_question;
	
   Question();
   ~Question();

   Parse *parse;
   Config *config;
      
   int load_question();
   int questions();

   /* State Saving */
   void savestate(ofstream &out);
   void loadstate(ifstream &in);


private:
   
   int  in_history(int value);
   void add_history(int value);
   void resize_history();
   
   int m_lines;               //Lines in question file
   time_t m_lastaccess;          //When question file was last accessed
   int m_historysize;         //Current size of history
   history_struct *m_history; //History head


};
#endif /* QUESTION_H */
