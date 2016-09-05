#ifndef PARSE_H
#define PARSE_H


struct timestruct
{

   int weeks;
   int days;
   int hours;
   int minutes;
   int seconds;

};

class Parse {

   public:
    char  *strip_leading(char *line);
    char  *get_nick(char *line);
    void  soundex(char *str, char *strex);    
    void  clean(char *str);
    void  timeparse(timestruct &output, int value);


    source_struct *get_source(char *source);
    Parse();
    ~Parse();

  
};

#endif /* PARSE_H */
