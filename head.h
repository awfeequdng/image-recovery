/*this file is designed to read a binary file and extract jpg files from it.
 *  * this is not 100% efficient but will get the job done
 *   * no matter what >:3 */
#define _POSIX_C_SOURCE 200809L
#define _ISO99_SOURCE
#define __EXTENSIONS_
#define _XOPEN_SOURCE 600
#define _FILE_OFFSET_BITS 64
#define __USE_LARGEFILE
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <dirent.h>

/*#include <xlocale.h>*/
#include <limits.h>
/*SCN is the size of cached array. current size is 100 KB*/
#define SCN  102403/*1048579*/  /*4098 2051 1027*/ /*do NOT set below 4, original was 19*/
#define MAXLEN 1024 /*max size of input to stdin*/
/*to create booleans in C*/
#define True  0 
#define False 1
/*flag options*/
#define Po 0x0001/*Picture files*/
#define Eo 0x0002/*Windows executable files*/
#define Wo 0x0004/*Wave audio*/
#define Vo 0x0008/*video files*/
#define To 0x0010/*Text files*/
#define Ao 0x0020/*all the above*/
#define HEAD 0x0100/*HEAD OBJECT*/
#define TAIL 0x0200/*TAIL OBJECT*/
/*flag options*/

/*document defines*/
#define DOC 0x0001/*.doc*/
#define DOX 0x0002/*.docx*/
#define PDF 0x0004/*.pdf*/
#define RTF 0x0008/*.rtf*/
#define FDX 0x0010/*.fdx*/
/*document defines*/

/*video defines*/
#define AVI 0x0001/*.avi*/
#define MOV1 0x0002/*.mov1*/
#define MOV2 0x0004/*.mov2*/

/*video defines*/

/*audio defines*/
/*audio defines*/
/*BEGIN FOR STATISTICS*/
#if defined(_POSIX_TIMERS) && _POSIX_TIMERS != -1

        /* use the newer Posix high-resolution time facilities */
        /* must be linked on all machines with -lrt */

        #include <time.h>

        #define PER_SECOND      1000000000
        #define time_type       struct timespec
        #define fraction_field  tv_nsec
        #define get_time(where) clock_gettime(CLOCK_REALTIME, where)

#else

        /* use the older (BSD) high-resolution time facilities */

        #include <sys/time.h>

        #define PER_SECOND      1000000
        #define time_type       struct timeval
        #define fraction_field  tv_usec
        #define get_time(where) gettimeofday(where, NULL)

#endif
/*END FOR STATISTICS*/
struct pot{
  struct sockaddr client;
  int client_fd;
};
typedef struct pot pot;

struct list{
    long long name;/*name of file*/
    unsigned int type; /*type of file*/
	unsigned int misc;/*extra data for each check*/
    char *file; /*name of master file*/
    long long address; /*where in master file to begin*/
	/*below is NOT stored in list file*/
    struct list *next;
    struct list *prev;
};
typedef struct list list;

struct global{/*goal is to reduce the size of this document*/
  long long picCount;/*Count of pictures found.*/
  long long exeCount;/*Count of executables found.*/
  long long wavCount;/*Count of audio found.*/
  long long aviCount;/*Count of videos found.*/
  long long docCount;/*Count of text documents found.*/
  long long total;/*total amount of files located*/
  char *name; /*Name of master file.*/
  char *saveDir;/*Directory to save to for part 4*/
  unsigned int flags;/*flags in use*/
  long long length;/*length of primary file, part one only*/
  long long processed;/*how much has been processed(Where we are in the file)*/
  long long cur;/*where pointer is in array for caching*/
  short searchActive;/*is the search thread active?*/
  /*for retrieving*/
  long long turre;/*where we are now*/
  struct list *head;/*head argument*/
  struct list *tail;/*tail argument*/
  time_type start_time;/*When did we start part 1.*/
  time_type finis_time;/*When did we finish part 1.*/
  double time_total;/*Time total for part 1.*/
};
typedef struct global global;

struct arry{
  long long processed;/*processed*/
  long long cur;/*current*/
  unsigned char *arr;/*array to work on*/
  /*unsigned char arr[SCN];*//*array to work on*/
  short closed;/*did the thread close on me? If so we're done.*/
  short isReady;/*is this thread ready?*/
  short trans;/*ready for transmission*/
};
typedef struct arry arry;
struct o_o{
  struct global *oo;/*box*/
  struct arry *hi;/*array*/
};
typedef struct o_o o_o;
/*Misc. Functions*/
void end(global **box);
char * getInput();
void start(long long i, long long pos, global **box, unsigned int type, unsigned int misc);
int partOne(global **contain);
int yesNo();
void resetContainer(global **contain);
void destroyContainer(global **contain);
int get_line(char *line, FILE *stream);
char *strTok(char *line,int *po,char delim);
void to_lower(char *blank);
long long atoll(const char *nptr);
long long load(arry **bo, FILE *fp, int isInit);
double retTime(time_type *start,time_type *finish);
char *form(long long value);
char *HuRe(long long number);
void destroy_mutex();
long long lof(FILE *str);
int riff_type(unsigned char arry[],long long i, global *box);
/*Misc. Functions*/

/*SCANNING FUNCTIONS*/
void getPic(list *o_O, char *dir);
void getWav(list *o_O, char *dir);
void getExe(list *o_O, char *dir);
void getAvi(list *o_O, char *dir);
void getDoc(list *o_O, char *dir);
void checkPic(global *box,unsigned char arry[]);
void checkExe(global *box,unsigned char arry[]);
void checkWav(global *box,unsigned char arry[]);
void checkAvi(global *box,unsigned char arry[]);
void checkDoc(global *box,unsigned char arry[]);

void isSame(global *oh, unsigned char array[], long long i, int isIn);
/*SCANNING FUNCTIONS*/
/*LIST FUNCTIONS*/
long long peek(list *head);
void wipeStack(list **head, list **tail);
void pushNode(list *ne, list **head, list **tail);
list *popNode(list **head, list **tail);
list *createNode(long long name, char *file,unsigned int type,unsigned int misc, long long address);
/*LIST FUNCTIONS*/
