#include "head.h"
long long lof(FILE *str){
  /*read how long RIFF  file is */
  unsigned char a,b,c,d;
  int er;
  long long ret=0;
  a=fgetc(str);
  b=fgetc(str);
  c=fgetc(str);
  d=fgetc(str);
  er=fseek(str,-4,SEEK_CUR);
  if (er==-1){
    perror("fseek in wav");
    exit(EXIT_FAILURE);
  }
  /*file size little endian*/ 
  ret=ret|d<<24;
  ret=ret|c<<16;
  ret=ret|b<<8;
  ret=ret|a;
  return ret;
}

int riff_type(unsigned char arry[],long long i, global *box){/*check type of riff and handles end of array problems*/
/*returns the following:
** 0 if not valid
** 1 if avi
** 2 if wav*/
   int j=0;
   unsigned char arr[11];
   if((arry[i-3]==0x52)&&(arry[i-2]==0x49)&&(arry[i-1]==0x46)&&(arry[i]==0x46)){/*RIFF in ascii, means RIFF format*/
     if (i+7 >=SCN){/*array border case*/
	    FILE *fp;
	    long long cur =0;/*local current*/
	    cur=box->cur;/*current pointer*/
		cur=cur-3;/*set to beginning of sequence*/
		fp=fopen(box->name,"r");
        fseek(fp,cur,SEEK_SET);
		j=fread(arr,sizeof(unsigned char),(11),fp);/*dump data into array from HDD A.S.A.P*/
        if(feof(fp) || ferror(fp)){/*end of file*/
		    fclose(fp);
			fp=NULL;
            return 0;
        }/*end eof*/	
        fclose(fp);
        fp=NULL;
        if(arr[0] != arry[i-3]){/*fail*/
            fprintf(stderr,"***ERROR***\nriff_type failed to copy correctly.\n");
			exit(EXIT_FAILURE);
        }
        if(arr[8]==0x57 && arr[9]==0x41 && arr[10]==0x56){/*wav*/
            return 2;
        }
        if(arr[8]==0x41 && arr[9]==0x56 && arr[10]==0x49){/*avi*/
            return 1;
        }
	 }/*end case of needing space*/
	 /*check array instead(normal case)*/
     if(arry[i+5]==0x41 && arry[i+6]==0x56 && arry[i+7]==0x49){/*avi*/
         return 1;
     }
	 /*if(arry[i+5]==0x57 && arry[i+6]==0x41 && arry[i+7]==0x56){
        */ return 2;/*
     }*/
   }
   /*not a RIFF so return 0*/
   return 0;
}

int get_line(char *line, FILE *stream){/*get line from file, useful for import of stack*/
  /*get a new line from file
 *  * return length of line or -1 if error
 *  line MUST BE DECLARED ALREADY BY MALLOC!*/
  int ch=-1;/*char from fgetc*/
  int oo=0;/*length of line*/
  int curSize=MAXLEN;
  char *ttt;/*placeholder*/
  while(1){
    ch=fgetc(stream);/*get char from file*/
    if(oo >=curSize){/*need more space*/
      while(curSize <=oo){/*guarantee I get something big enough*/
        curSize=curSize*2;
      }/*end size loop*/
      if(line == NULL){/*this should NEVER happen*/
        fprintf(stderr,"UHOH\n");
        exit(EXIT_FAILURE);
      }
      ttt=realloc(line,curSize);/*I would like more space please*/
      if(ttt !=NULL){
        line = ttt;
      }
    }/*end case of too little space*/
    /*checking the char value*/
    if(ch == EOF || ch=='\0'){/*we are at end of the file*/
      if(oo==0){/*still at beginning*/
        return -1;
      }
      else{/*not at beginning*/
        line[oo]='\0';
        return oo;/*return length*/
      }/*end case of eof, and in middle of line*/
    }/*end case of reaching end of file*/
    if((ch=='\0') ||(ch=='\n') ||(ch=='\f')){
      /*if here, any of these cases result in returning
 *  *     to the function that called get_line*/
      line[oo]='\0';
      return oo;/*return length*/
    }/*end newline,endline,or page case*/
    /*if here means that we are not in any case to terminate loop*/
    line[oo]=ch;/*append character*/
    oo=oo+1;/*add tally*/
  }/*end while loop*/
  fprintf(stderr,"if we are here, uhoh\n");
  return -1;
}/*end get_line*/

void destroyContainer(global **contain){/*cleanup*/
   global *container = *contain;   
   free(container -> name);
   container -> name = NULL;   
   free(container -> saveDir);
   container -> saveDir = NULL;
   container -> picCount=0;/*count of pictures found*/
   container -> exeCount=0;
   container -> wavCount=0;
   container -> aviCount=0;
   container -> docCount=0;
   container -> flags = 0;/*flags in use*/
   container -> length=0;/*length of primary file, part one only*/
   container -> processed=0;/*how much has been processed(Where we are in the file)*/
   container -> cur=0;/*where pointer is in array for caching*/
   container -> searchActive=False;/*is the search thread active?*/
  /*for retrieving*/
   container -> turre=0;/*where we are now*/
   container -> total=0;/*total amount of files located*/
   wipeStack(&container -> head, &container -> tail);
   container -> head = NULL;
   container -> tail = NULL;
   /*container -> start_time=0;
   container -> finis_time=0;*/
   container -> time_total=0;



   *contain = container;
}/*end function*/

void resetContainer(global **contain){/*Sets container from new*/
   global *container = *contain;
  /*this function resets the container assuming its global here*/
   container -> name=calloc(sizeof(int),MAXLEN);
   container -> saveDir=calloc(sizeof(int),MAXLEN);   
   container -> picCount=0;/*count of pictures found*/
   container -> exeCount=0;
   container -> wavCount=0;
   container -> aviCount=0;
   container -> docCount=0;
   container -> flags = 0;/*flags in use*/
   container -> length=0;/*length of primary file, part one only*/
   container -> processed=0;/*how much has been processed(Where we are in the file)*/
   container -> cur=0;/*where pointer is in array for caching*/
   container -> searchActive=False;/*is the search thread active?*/
  /*for retrieving*/
   container -> turre=0;/*where we are now*/
   container -> total=0;/*total amount of files located*/
   container -> head = NULL;
   container -> tail = NULL;
   /*container -> start_time=0;
   container -> finis_time=0;*/
   container -> time_total=0;



   *contain = container;
}/*end function*/

int yesNo(){ /*reads stdin returns True if yes, False if NO, -1 on failure*/
  char *oo;
  int ret=10;
  oo=getInput();
  if(oo[0]=='y' || oo[0]=='Y'){
	ret = True;
  }
  else if(oo[0]=='n' || oo[0]=='N'){
	ret = False;
  }
  else{
	ret = -1;/*none of the above*/
  }
  free(oo);
  return ret;
}

char * getInput(){/*read input from stdin*/
   int c=0;
   long i=0;
   long m = MAXLEN;
   char *input = calloc(sizeof(int),m);
   while ((c=getc(stdin)) != '\n'){
      if(i >=m ){/*need more space*/
	    int j=i+1;/*incase i is zero*/
		j=j*4;/*quadrupling the space*/
		m=j;
		char *ttt;/*placeholder*/
		ttt=realloc(input,j);
		if(ttt !=NULL){
          input = ttt;
		}
	  }/*we now have space <3*/
	  /*actually add to array.*/ 
      input[i]=c;
      i++;
   }/*end of input*/
   input[i] ='\0';
   return input;
}

void to_lower(char *blank){/*sets letter options to lower case*/
   /*convert to lower case*/
	int i = 0;
	for(i = 0; blank[i] != '\0' ;i++){
	   blank[i] = tolower((int)blank[i]);
	}
}/*end to lower case*/

char *strTok(char *line,int *po,char delim){/*returns MALLOCED line*/
   int len = strlen(line);/*length of line*/
   int pos = (*po);
   len++;/*include null char*/
   if(pos >= len){/*reading past line*/
      fprintf(stderr,"\nstrTok: Buffer overrun detected. positon %d, line length %d\n",pos,len);
      return NULL;
   }
   char *ret=calloc(sizeof(int),len);/*this will be RETURNED*/
   int i=0;
   for(i=0;pos <len && line[pos] != delim;pos++,i++){/*cycle through line*/
     ret[i]=line[pos];
   }
   ret[i]='\0';
   pos++;/*to be past the delimiter*/
   *po=pos;
   return ret;
}


long long load(arry **bo, FILE *fp, int isInit){/*This will load data from HDD into memory*/
   /*requires a minimum array length of FOUR*/
   /*returns True if ok, any other number indicates how many bytes successfully transferred*/
   arry *box = *bo;
   long long j=1;
   int ret = True;/*Placeholder*/
   if(isInit == False){/*this is NOT the initial load*/
      /*move last three characters over!*/
	  box->cur =(box->processed)+1;/*save this location to scan from*/
	  box->processed = box->processed - 3;/*get start array location*/
      fseek(fp,box->processed,SEEK_SET);/*seeh hdd to this location o.o*/
      /*reassign current pointer*/
   }
   else{/*initial load*/
      box->processed = ftell(fp);/*where I am*/
      box->cur = box->processed;/*initial*/
      box->cur = box->cur +4;/*where to begin search*/
   }/*note fread uses FULL SCN size!*/
   j=fread(box->arr,sizeof(unsigned char),(SCN),fp);/*dump data into array from HDD A.S.A.P*/
   if(!feof(fp)){/*end of file*/
       ret = True;
   }
   if(!ferror(fp)){/*end of file*/
       ret = True;
   }
   if (j < SCN){
       ret = j;
   }
   /*cleanup*/
   box->processed = ftell(fp);/**/
   *bo = box;
   return ret;
}


void isSame(global *oh, unsigned char array[], long long i, int isIn){/*ONLY use this to axamine array mismatch issue*/
   /*This will locate any array mismatch issue. This uses BRUTE FORCE and is very very SLOW! Do not use unless mismatch suspected*/
   int aa,bb,cc,dd;
   FILE *fp;
   fp=fopen(oh->name,"rb");
   fseek(fp,(oh->cur)-4,SEEK_SET);
   aa=fgetc(fp);
   bb=fgetc(fp);
   cc=fgetc(fp);
   dd=fgetc(fp);
   fclose(fp);
   fp=NULL;
   if((array[i-3]!=aa )||(array[i-2]!=bb)||(array[i-1]!=cc)||(array[i]!=dd)){
      fprintf(stderr,"\n*****!!!!ERROR!!!!*****\n");
	  if(isIn==True){/*in interface*/
	     fprintf(stderr,"IN INTERFACE\n");
	  }
	  else{
	     fprintf(stderr,"IN CHECK\n");
	  }
	  fprintf(stderr, "array to file mismatch at index %lld position %lld\n",i,oh->cur);
	  fprintf(stderr,"had %02x %02x %02x %02x\n",array[i-3],array[i-2],array[i-1],array[i]);
	  fprintf(stderr,"got %02x %02x %02x %02x\n",aa,bb,cc,dd);
	  exit(EXIT_FAILURE); 
   }
}
/*Time taken*/
double retTime(time_type *start,time_type *finish){
   int sec,fra;
   double send;
   sec = finish ->tv_sec;
   fra = finish ->fraction_field;
   if(start !=NULL){
     sec -= start ->tv_sec;
     fra -= start->fraction_field;
     if(fra <0){
       sec--;
       fra +=PER_SECOND;
     }
   }
   send = ((double)sec)+((double)fra)/((double)PER_SECOND);
   return send;
}
/*format functions*/
char *form(long long value){/*this returns KB, MB form type position `1*/
  if (value==0)/*-.- tiny*/
    return "B";
  if (value==1)/*-.o eh barely picture sized, tiny text file to big text file*/
    return "KB";
  if (value==2)/*o.o okay maybe a few pictures or audio, really really massive text file*/
    return "MB";
  if (value==3)/*^.^ okay slightly bigger than a CD or an average HDD so a lot of goodies*/
    return "GB";
  if (value==4)/*O.o You lost more than Terabyte of information? ouch!*/
    return "TB";
  if (value==5)/*O_O holy shit! I need to get faster!*/
    return "PB";
  return NULL;
}/*end form*/
char *HuRe(long long number){/*Person Readable*/
/*this will determine how large a file is and convert to person readable format
 * EX: 1024 ->1KB
 * 0=byte
 * 1=kilobyte
 * 2=megabyte etc.*/
   float pp=0;
   int i=0;/*type to return*/
   char *retur=calloc(sizeof(int),MAXLEN);/*FREE THIS AFTER FUNCTION*/
   pp = number;
   for (i=0;pp>=1024;i++){
     /*pp=pp%1024;*/
     pp=pp/1024;
   }
   sprintf(retur,"%4.03f %s",pp,form(i));/*number in size*/
   return retur;
}/*end HuRe*/
/*format functions*/