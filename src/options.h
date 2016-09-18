#undef  DEBUG

#define TIMEOUT_SEC 30
#define TIMEOUT_USEC 30000000
/* Where do we log */
#define LOGFILE "trivia.log"
/* What is the max data we can receive from the IRC server? */
#define RECVSIZE 1024
#define BASETIME 1
/* Size of a nick from IRC */
#define NICKMAX 32
/* Max size of a question line*/
#define QUESTIONMAX 1024
/* Size of ghost password*/
#define PASSMAX 4
/* Max size of team name */
#define TEAMMAX 32
/* Define if compiling under win32 */
#ifndef WIN32
#define WIN32
#endif

