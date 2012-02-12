#include "head.h" 
/*esto
 * this is the interface file
 * for the program to work
 * and thrive >:3*/

/*static global *container;*//*GLOBAL VARIABLE!*/
/*this holds the valuable information*/


static pthread_cond_t paus=PTHREAD_COND_INITIALIZER;/*pausing thread*/
static pthread_mutex_t rea=PTHREAD_MUTEX_INITIALIZER;

void destroy_mutex(){
   pthread_cond_destroy(&paus);
   pthread_mutex_destroy(&rea);
}

/*cache*/
static void *cach(void *param){/*scanner of HDD. DOES NOT LAUNCH IF C option used*/
  o_o *o_O=((o_o *)param);/*this mess is to allocate the proper pointer*/
  global *container = NULL;
  arry *array = NULL;
  array = o_O ->hi;
  container = o_O ->oo;
  pthread_mutex_lock(&rea);/*lock it down*/
  array->closed = False;/*not done*/
  array ->isReady = True;/*fire the sea thread*/
  array->trans=False;/*not ready for transmission*/
  /*unsigned char data[SCN]={0};*//*scanning array*/
  /*memmove(array->arr,data,SCN*sizeof(char));*//*scanning array set to 0*/
  int j = True;/*notify we are done*/
  FILE *mai;/*main file to look at*/  
  mai=fopen(container->name,"rb");/*open the master file*/
  fseek(mai,container ->processed,SEEK_SET);/*additional functionality to resume*/
  if(mai==NULL){/*error in file, kill program*/
    perror("");
    fprintf(stderr,"Error opening file\n");
    array->closed = True;/*done*/
	array->trans=True;
    pthread_mutex_unlock(&rea);/*free this mutex*/
    pthread_exit(NULL);
  }
  j=load(&array, mai,True);/*scanning array*/
  if(j!=True){/*this is what I call a BIG problem*/
     fprintf(stderr,"Error, cache size too big!, please recompile with smaller size\n");
  }
  array->trans=True;/*ready to fire. this is reset by the sea thread*/
  pthread_cond_wait(&paus,&rea);/*block the thread and wait for offload*/
  while (j == True){/*read until EOF*/
     /*pthread_mutex_lock(&rea);*//*block*/
     j=load(&array, mai,False);/*This function actually reads the HDD. Notifies if we hit EOF*/
	 array->trans=True;/*ready to fire. this is reset by the sea thread*/
     pthread_cond_wait(&paus,&rea);/*wait for sea thread to copy array*/
  }	 /*read and wait*/
  /*we're done*/
  fclose(mai);
  mai=NULL;
  array->trans=True;
  array->closed = True;
  pthread_mutex_unlock(&rea);/*free this mutex*/
  pthread_exit(NULL);
 }
/*cache*/

/*begin search*/
static void *sea(void *param){/*scanner of HDD. DOES NOT LAUNCH IF C option used*/
  o_o *o_O=((o_o *)param);/*this mess is to allocate the proper pointer*/
  global *container = NULL;
  arry *cached= NULL;
  cached = o_O ->hi;
  container = o_O ->oo;
  free(o_O);
  o_O=NULL;
  /*unsigned char data[SCN]={0};*//*array or data*/
  unsigned char *data = calloc(SCN,sizeof(char));
  /*CRITICAL SECTION BEGIN*/
  pthread_mutex_lock(&rea);/*wait for cached to be ready for us*/
  for(;;){/*trap code just incase*/
    if(cached->trans==True){
	   break;
	}
  }
  /*memmove(data,cached->arr, SCN*sizeof(char));*//*copy array*/
  memmove(data,cached->arr, SCN);/*copy array*/
  container ->cur = cached ->cur;
  container ->processed = cached ->processed;
  cached->trans=False;
  pthread_mutex_unlock(&rea);
  pthread_cond_signal(&paus);/*reactivate cached thread*/
  /*CRITICAL SECTION END*/
  while(1){/*this actually scans the array for uniques*/
    /******run the file checkers******/
    checkPic(container,data);
    /*checkExe(container,data);*/
    checkWav(container,data);
    checkAvi(container,data);
    checkDoc(container,data);
    /******end file checkers******/
    /*at the end of the array. Reload time! :D*/
    if (cached->closed==True){/*Done with the scanning so exit!*/
       break;/*leave while loop*/
    }
	for(;;){/*trap code just incase*/
       if(cached->trans==True){
	     break;
	   }
    }
	/*CRITICAL SECTION BEGIN*/
    pthread_mutex_lock(&rea);/*wait for cached to be ready for us. cannot proceed until mutex given*/
    /*memmove(data,cached->arr, SCN*sizeof(char));*//*copy array*/
    memmove(data,cached->arr, SCN);/*copy array*/
    container ->cur = cached ->cur; /*reassign current*/
    container ->processed = cached ->processed; /*reassign processed*/
	cached->trans=False;/*ready to fire. this is reset by the sea thread*/
    pthread_mutex_unlock(&rea);/*release mutex to cache*/
    pthread_cond_signal(&paus);/*reactivate blocked thread*/
	/*CRITICAL SECTION END*/
  }/*end while loop*/
  free(data);
  data = NULL;
  container->searchActive=False;/*We have finished scanning, move on to part two!*/
  pthread_exit(NULL);
}
/*end search*/

/*void isIt();*/
int partOne(global **contain){/*returns True if success, False if failure*/
   global *container = *contain;
   /*isIt();*/
   arry *arrr;/*array*/
   char *type;/*integer to holdfile size*/
   long long proc=0;
   long long leng=0;
   long long prev=0;/*bytes previously read*/
   long long time_left=0;/*time left*/
   long long time_show=0;/*time left*/
   int isS=True;/*minute or second*/
   int perc=0;/*percentage completed*/
   container->flags=container->flags|=Ao;/*set this to find ALL*/
   /*define arrr*/
   arrr=malloc(sizeof(arry));/*array caching*/
   arrr->processed = 0;
   arrr->cur=0;
   arrr->closed =0;
   arrr->isReady=0;
   arrr->arr=calloc(SCN,sizeof(char));
   /*begin*/
   fprintf(stderr,"Starting up, please standby\n");
   struct stat statbuf;
   char *aa = calloc(sizeof(int),MAXLEN);
   realpath(container->name,aa);
   stat(aa, &statbuf);
   if((!S_ISREG(statbuf.st_mode))){/*not a valid file*/
     perror(container->name);
     free(aa);
     aa=NULL;
     free(arrr->arr);
     arrr->arr =NULL;
     free(arrr);
     arrr=NULL;
     *contain = container;
     return False;/*failure*/
   }/*end is not regular file*/
   container->processed=0;
   container->length=statbuf.st_size;
   leng=container->length;/*size of file*/
   type=HuRe(leng);/*size of file*/
   strcpy(container->name,aa);
   free(aa);
   aa=NULL;  
   fprintf(stderr,"FileName: %s\n",container ->name);
   fprintf(stderr,"%s in size\n",type);/*print off its size*/
   /*scan entire drive first*/
   container->searchActive=True; 
   pthread_t threadID;
   pthread_t cache;
   o_o *ooo=NULL;
   ooo=malloc(sizeof(o_o));
   ooo->oo = container;
   ooo->hi = arrr;
   ooo->hi->isReady=False;/*ready to fire*/
   pthread_create(&cache,NULL,cach,(void *)ooo);/*launch HDD reader thread*/
   for(;;){/*wait to launch scanner thread*/
      if(arrr->isReady==True){/*launch once thread has blocked mutex!*/
        pthread_create(&threadID,NULL,sea,(void *)ooo);/*launching the scanner thread*/
        break;
      }
   }/*fires*/
   char *tya;
   char *avg;
   get_time(&(container->start_time));
   while(container->searchActive==True){/*Loop in here until complete.*/
     proc=container->cur;/*'container->processed'is where fp points to, not whats actually being looked at*/
     tya=HuRe(proc);
     perc=(proc*100)/leng;/*percentage complete*/
	 if((container->processed == 0) || (container->processed == prev)){
	    time_left=0;
		avg=HuRe((container->processed)-(prev));/*how fast it is going at*/
		prev = container ->processed;/*setup of last position*/
	 }
	 else{ /*time left*/
	    time_left=(((container ->length)-(container->processed))/((container->processed)-(prev)));
		avg=HuRe((container->processed)-(prev));/*how fast it is going at*/
		prev = container ->processed;/*setup of last position*/
		/*minutes or seconds?*/
		if(time_left >60){
		  isS=False;
		  time_show=time_left/60;/*time left in minutes*/
		}
		else{
		  isS=True;
		}/*end minutes or seconds*/
	 }/*end time left*/
     if(isS==True){/*seconds*/
        fprintf(stderr,"  %d %% complete %s out of %s processed.   \n  %lld seconds remaining. Rate: %s \r\b\r",perc,tya,type,time_left,avg);/* %lld proc, leng*/  
     } 
     else{
        fprintf(stderr,"  %d %% complete %s out of %s processed.   \n  %lld minutes remaining. Rate: %s \r\b\r",perc,tya,type,time_show,avg);/* %lld proc, leng*/
     }	
     free(tya);
     tya=NULL;
	 free(avg);
	 avg=NULL;
     sleep(1);
   }/*end the while loop*/
   get_time(&(container->finis_time));
   free(type);
   type = NULL;
   printf("\nScan Complete. One moment please\n");
   container->time_total=retTime(&(container->start_time),&(container->finis_time));
   free(arrr->arr);
   arrr->arr =NULL;  
   free(arrr);/*We don't need this anymore*/
   arrr=NULL;
   *contain = container;
   return True; /*Success!*/  
   /*end here.*/
}/*end scanner*/


