/*Sabino San Juan
This file handles storing the FIFO onto hard disk*/
#include "head.h"

static int checkForm(char *line){
   int count = 0;
   int i=0;
   for(i=0;line[i] != '\0';i++){
      if(line[i]=='|'){
	     count=count+1;
	  }
   }
   if(count <= 3){/*old version*/
     return False;
   }
   return True;/*new version*/

}
static int answer1(char *inp){/*returns the selection*/
   int i;
   for(i=0;i<strlen(inp);i++){
	if(inp[i]=='1'){
		return 1;
	}
	if(inp[i]=='2'){
		return 2;
	}
	if(inp[i]=='3'){
		return 3;
	}
	if(inp[i]=='4'){
		return 4;
	}
	if((inp[i]=='5')||(inp[i]=='q')||(inp[i]=='Q')){
		return 5;
	}
   }
   return -1;
}
/*Begin OPTIONS
All options are int based and will return False if failure present*/
static int one(global *container){
   if (container->head == NULL || container ->tail ==NULL){
	 return False;
   }
   char *inp;
   /*if here we are saving file.*/
   printf("Name of results document\n>");
   inp = getInput();
   if (strcmp(container->name,inp)==0){/*trying to overwrite*/
	 printf("Trying to overwrite source!\n");
	 free(inp);
	 inp=NULL;
	 return False;
   }
   FILE *stream;/*file writing to*/
   stream=fopen(inp,"w");
   free(inp);
   inp=NULL;
   if(!stream){/*cannot write to*/
	  return False;
   }
   /*begin process*/
   list *ptr = container ->head->next;/*head has NO information*/
   long long counter = 0;
   long long perc = 0;
   /*put header*/
   putc(0x5A,stream);/*SA*/
   putc(0xB1,stream);/*BI*/
   putc(0xF0,stream);/*NO*/
   putc(0x0A,stream);/* \n */
   /*begin output*/
   for (counter = 0; ptr ->next !=NULL; ptr=ptr ->next, counter++) {
	  fprintf(stream,"%lld|%d|%d|%s|%lld\n",ptr->name, ptr->type, ptr->misc, ptr->file, ptr->address);
	  perc = (counter*100)/container->total;
	  fprintf(stderr,"%lld %% complete; %lld out of %lld results saved\r",perc,counter, container->total);
   }
   fprintf(stream,"%lld|%d|%d|%s|%lld\n",ptr->name, ptr->type, ptr->misc, ptr->file, ptr->address);/*for the last one*/
   /*end process*/
   putc('\0',stream);/*end of file*/
   fclose(stream);
   stream = NULL;
   return True;   
}/*end one*/

static int two(global **contain){
	/*load stack*/
	global *container = *contain;
	FILE *stream;/*file reading from*/
	char *inp;
    long long perc = 0; /*percentage complete*/
	long long length = 0;/*length of import file*/
	int a,b,c,d,t,co;
	char *line = calloc(sizeof(int),MAXLEN);
	int j=0;/*used to check EOF*/
	printf("\nPlease select file\n>");
	co=True;/*repeat version check once*/
	inp = getInput();
	stream=fopen(inp,"r");
	if(!stream){/*cannot read from*/
	   free(inp);
	   inp=NULL;
	   free(line);
	   line = NULL;
	   return False;
	}
	struct stat statbuf;
    stat(inp, &statbuf);
    length=statbuf.st_size;
	free(inp);
	inp=NULL;
	/*check if this is a wellformed file*/
	a=fgetc(stream);
	b=fgetc(stream);
	c=fgetc(stream);
	d=fgetc(stream);
	if(a!=0x5A  || b!=0xB1 || c!=0xF0 || d!=0x0A){/*does it have the header?*/
	   fprintf(stderr,"Error: Invalid header\n");
	   fclose(stream);
	   stream=NULL;
	   return False;/*not a valid file*/
	}
	/*begin import*/
	destroyContainer(&container);/*purge results*/
	resetContainer(&container);/*reset use*/
	/*scan document, and rebuild statistics for NEW file*/
	for(container ->total=0;j!= -1;container->total = container ->total+1){
	   j=get_line(line, stream);/*get line from file, is being given length of line*/
	   if(j==-1){/*breakouttahere!*/
	       break;
	   }
	   if(co==True){/*check this once*/
	      co=False;
		  t = checkForm(line);/*check if older type True if current, False if older*/
	   }
	   int le = 0;/*length of line*/
	   /*there are four items in this line*/
	   /*long name|unsigned int type|char *fil|long long address*/
	   char *tmp=NULL;/*ensure we have space*/
	   tmp=strTok(line,&le,'|');
	   if(tmp ==NULL){
	     break;
	   }
	   long name =atol(tmp);
	   free(tmp);
	   tmp=NULL;
	   
	   tmp=strTok(line,&le,'|');
	   if(tmp ==NULL){
	     break;
	   }
	   unsigned int type=atoi(tmp);
	   free(tmp);
	   tmp=NULL;
	   
       
	   unsigned int misc;
	   if(t==True){
	      tmp=strTok(line,&le,'|');
	      if(tmp ==NULL){
	        break;
	      }
	      misc=atoi(tmp);
	      free(tmp);
	      tmp=NULL;
	   }
	   else{
	      misc = 0x0000;
	   }
	   
	   char *fil =strTok(line,&le,'|');
	   if(fil ==NULL){
	     break;
	   }
	   tmp = strTok(line,&le,'\n');
	   if(tmp ==NULL){
	     break;
	   }
	   
	   long long addr = atoll(tmp);
	   free(tmp);
	   tmp=NULL;
	   /*add this thing to the stack*/
	  pushNode(createNode(name,fil,type, misc, addr),&container->head,&container->tail);
	  /*add type to collection*/
	  if((type & Po)==Po){/*picture*/
         container->picCount=container->picCount+1;
	  }
	  if((type & Eo)==Eo){/*executable*/
         container->exeCount=container->exeCount+1;
	  }
	  if((type & Wo)==Wo){/*audio*/
         container->wavCount=container->wavCount+1;
	  }
	  if((type & Vo)==Vo){/*video*/
         container->aviCount=container->aviCount+1;
	  }
	  if((type & To)==To){/*text*/
         container->docCount=container->docCount+1;
	  }
	  /*end add type to collection*/
	  free(fil);
	  fil=NULL;
	  perc = (ftell(stream)*100)/length;
	  fprintf(stderr,"Import %lld %% complete\r",perc);
	}/*end the importing*/
	fprintf(stderr,"\n");
	free(line);
	line = NULL;
	fclose(stream);
	stream=NULL;
	return True;
}
/*End OPTIONS*/

static void stats(global *container){
   double mi=0;
   int se=0;
   char *rate;
   rate = HuRe(container->length/container->time_total);
   mi=(container->time_total)/60;
   se=(int)(container->time_total)%60;
   /*fprintf(stderr,"Time processing in seconds: %10.6f\n",container->time_total);*/
   fprintf(stderr,"Time processing: %4.0f Minutes %d Seconds\n",mi,se);
   fprintf(stderr,"Average read: %s\n",rate);
   fprintf(stderr,"Files processed: %lld\n",container ->total);
   fprintf(stderr,"Pic: %lld\n",container ->picCount);
   fprintf(stderr,"Exe: %lld\n",container ->exeCount);
   fprintf(stderr,"Wav: %lld\n",container ->wavCount);
   fprintf(stderr,"Avi: %lld\n",container ->aviCount);
   fprintf(stderr,"Txt: %lld\n",container ->docCount);
   free(rate);
   rate = NULL;
}

int main(){
   global *container;/*holder for EVERYTHING!*/
   container=malloc(sizeof(global));
   resetContainer(&container);/*clear out container*/
   int esc = False;/*build for loop to contain prompt*/
   /*system("clear");*/
   printf("Good day, this is the recovery utility\n");
   while(esc !=True){
	   char *inp;
	   printf("\nPlease select ONE of the following:\n");
	   printf("1: Scan drive.\n");/*restarts program from beginning*/
	   printf("2: Save results to file.\n");/*save stack to file if exists*/
	   printf("3: Load results from file.\n");/*load stack from file*/
	   printf("4: Begin recovery.\n");/*start processing if we have full stack. if not open option 2, will prompt to save stack if it preexists*/
	   printf("5: Exit.\n");/*exits program, prompts to save current stack*/
	   printf(">");
	   inp = getInput();
	   int select = answer1(inp);
	   free(inp);
	   inp=NULL; 
	   /*begin task*/
	   if(select == 1){/*Scan drive*/
	      int go=True;
	      if(container ->head != NULL && container ->tail != NULL){
                printf("\nSCANNING DRIVE WILL WIPE CURRENT MEMORY!\n");
                printf("Are you sure you would like to continue?(y or n)\n>");
                go=yesNo();
              }
              if(go==True){
        	printf("\nActivating scanner program, please standby\n");
                destroyContainer(&container);
                resetContainer(&container);
                /*system("clear");*/
                printf("Follow the prompt below\n");
                printf("Name of file: ");
                char *tmp3 = getInput();
                strcpy(container->name,tmp3);
                free(tmp3);
                tmp3=NULL;
                /*container->name =getInput();*/
                go=partOne(&container);/*start program*/
                if(go ==True){
                    /*system("clear");*/
                    stats(container);
                }
              }
	   }
	   else if(select == 2){/*Save results to file*/
               if(one(container) == False){
                   printf("\nInitial scan skipped or invalid name given\ninvalid option selected. Try again\n");
               }
               else{
                   printf("\nSAVE COMPLETE\n");
               }
           }
	   else if(select == 3){/*Load results from file*/
               if(container ->head != NULL && container ->tail != NULL){/*pre existing data in memory*/
                   printf("\nLoading from file will DELETE RESULTS already in memory.\nContinue?\n>");
                   if(yesNo() == True){
		      if(two(&container) == False){
                          printf("\nInvalid file or file does not exist.\ninvalid option selected. Try again\n");
                      }/*error*/
                   }/*yes or no*/
               }/*if head and tail are not nulled*/
               else{/*are nulled so skip prompt*/
                   if(two(&container) == False){
                       printf("\nInvalid file or file does not exist.\ninvalid option selected. Try again\n");
                   }/*error*/
               }
               /*system("clear");*/
               stats(container);
           }/*end processing*/
           else if(select == 4){/*Begin processing*/
               if(container ->head != NULL && container ->tail != NULL){
                   printf("\nAbout to begin processing.\nONCE STARTED ALL RESULTS IN MEMORY WILL BE DELETED\n");
                   printf("Would you like to continue?\n>");
		   if(yesNo() == True){/*do not save before continuing*/
                       /*system("clear");*/
                       stats(container);
                       end(&container);/*make threaded to show progress*/
                       /*system("clear");*//*return to main menu*/
                       printf("Complete!\n");
                   }
               }
               else{/*no stack*/
                   printf("\nInitial scan skipped.\ninvalid option selected. Try again\n");
               }
           }/*end the processing case*/
           else if(select == 5){/*exit*/
               if(container ->head != NULL && container ->tail != NULL){/*pre existing data in memory*/
                   printf("\nEXITING WITHOUT SAVING CURRENT RESULTS WILL WIPE MEMORY!\nAre you sure you would like to exit?(y or n)\n>");
                   if(yesNo()==True){
                       printf("\n");
                       esc = True;
                       break;/*break from this loop*/
                   } /*end the yesno question*/
               }/*end case of data preexisting in memory*/
               else{/*no data in memory so just exit*/
                   esc = True;
                   break;/*break from this loop*/
               }
           }/*end the exit case*/
           /*end task*/
   }/*end while loop*/
   printf("\nShutting down program, Please wait\n");
   destroyContainer(&container);
   free(container);
   container=NULL;
   destroy_mutex();
   fprintf(stderr,"All Done! =^.^=\n");
   pthread_exit(NULL);
}/*end mainfile*/