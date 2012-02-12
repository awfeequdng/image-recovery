/*Sabino San Juan IV
1/3/2011
fifo.c
checks all formats.
*/
#include "head.h"

/*begin getOptions*/
static void getOptions(char *container,global **bo){
   global *box = *bo;
  /*this is to find the options*/
   int i;
   box->flags=0;
   for(i=0;i<strlen(container);i++){
     if(container[i]=='a'){
       box->flags=box->flags|=Ao;
       break;
     }
     if(container[i]=='t'){
       box->flags=box->flags|=To;
     }
     if(container[i]=='v'){
       box->flags=box->flags|=Vo;
     }
     if(container[i]=='w'){
       box->flags=box->flags|=Wo;
     }
     if(container[i]=='e'){
       box->flags=box->flags|=Eo;
     }
     if(container[i]=='p'){
       box->flags=box->flags|=Po;
     }
   }/*end for statement*/
   *bo=box;
}/*end the get options*/
/*end getOptions*/

/*start function*/
void start(long long i, long long pos, global **box, unsigned int type, unsigned int misc){/*unsigned int type*/
  int t = (*box)->total;/*total nodes in use*/
  t++;
  (*box)->total = t;
  list *head = (*box) ->head;
  list *tail = (*box) ->tail;
  pushNode(createNode(i, (*box) ->name, type, misc, pos), &head,&tail);
  (*box) ->head = head;
  (*box) ->tail = tail;
}
/*start function*/
/*a-e functions to launch*/
static void a(list *popped, global *box){/*pic*/
  if((box->flags & Ao)==Ao || (box->flags & Po)==Po){/*user wants this type*/
    if((popped->type & Ao)==Ao ||(popped->type & Po)==Po){/*list has this type*/
      getPic(popped,box->saveDir);
    }/*end type in list*/
  }/*end type user wants*/
}
static void b(list *popped, global *box){/*exe*/
}
static void c(list *popped, global *box){/*wav*/
  if((box->flags & Ao)==Ao || (box->flags & Wo)==Wo){/*user wants this type*/
    if((popped->type & Ao)==Ao ||(popped->type & Wo)==Wo){/*list has this type*/
      getWav(popped,box->saveDir);
    }/*end type in list*/
  }/*end type user wants*/
}
static void d(list *popped, global *box){/*video*/
  if((box->flags & Ao)==Ao || (box->flags & Vo)==Vo){/*user wants this type*/
    if((popped->type & Ao)==Ao ||(popped->type & Vo)==Vo){/*list has this type*/
	  if((popped ->misc ==MOV1)||(popped ->misc ==MOV2)){/*.mov file*/
            getAvi(popped,box->saveDir);
	  }
	  if(popped ->misc ==AVI){/*avi*/
	    getWav(popped,box->saveDir);
	  }
    }/*end type in list*/
  }/*end type user wants*/
}
static void e(list *popped, global *box){/*doc*/
  if((box->flags & Ao)==Ao || (box->flags & To)==To){/*user wants this type*/
    if((popped->type & Ao)==Ao ||(popped->type & To)==To){/*list has this type*/
      getDoc(popped,box->saveDir);
    }/*end type in list*/
  }/*end type user wants*/
}
/*a-e functions to launch*/
/*function to clear house*/
static void *scan(void *param){/*Finds files*/
  o_o *o_O=((o_o *)param);/*this mess is to allocate the proper pointer*/
  global *container = NULL;
  container = o_O ->oo;
  free(o_O);
  o_O=NULL;
  /*begin work*/
  while(container ->total >= container->turre){/*Make this threaded*/
   list *popped = popNode(&(container->head), &(container->tail));
   container ->turre = (container->turre)+1;
   if(popped == NULL){/*nobody home o.o*/
     break;
   }
   a(popped, container);
   /*b(popped, container);*/
   c(popped, container);
   d(popped, container);
   e(popped, container);
   /*cleanup of node*/
   free(popped->file);
   popped ->file = NULL;
   free(popped);
   popped = NULL;
   /*end cleanup*/
  }
  container ->searchActive=False;
  /*We're done :D*/
  pthread_exit(NULL);  
}
void end(global **box){
  global *container = *box;
  long long perc = 0;
  char *options;/*options*/
  char *location;/*location*/
  pthread_t threadID;
  o_o *ooo=NULL;/*To pass to thread.*/
  container ->searchActive=True; 
  /*Folder to store files in.*/
  printf("\nSelect output location:\n");
  location = getInput();/*Get main folder location.*/
  printf("\nSelect retrieval options:\n");
  printf("P: Picture files\n");
  printf("E: Windows executables\n");
  printf("W: Wave audio files\n");
  printf("V: Video files\n");
  printf("T: Text files\n");
  printf("A: All types\n");
  printf(">");
  options = getInput();/*Get options.*/
  to_lower(options);
  getOptions(options,&container);
  free(options);
  options=NULL;
  
  /*Folder to store files in.*/
  char *tr1=calloc(sizeof(int),1024);
  char *tr2=calloc(sizeof(int),1024);
  realpath(".",tr1);
  /*Build initial directory.*/
  strcat(tr1,"/");/*add the slash to end of current directory.*/
  strcat(tr1,location);/*add name of new directory.*/
  free(location);/*free the location.*/
  location=NULL;
  mkdir(tr1,0777);/*make new directory.*/
  /*now build internal directories.*/
  strcat(tr2,tr1);
  strcat(tr2,"/");
  strcpy(container->saveDir,tr2);/*Save this location to memory*/
  free(tr2);
  free(tr1);
  tr1=calloc(sizeof(int),1024);
  strcat(tr1,container->saveDir);
  strcat(tr1,"mov");
  mkdir(tr1,0777);
  free(tr1);
  tr1=calloc(sizeof(int),1024);
  strcat(tr1,container->saveDir);
  strcat(tr1,"doc");
  mkdir(tr1,0777);
  free(tr1);
  tr1=calloc(sizeof(int),1024);
  strcat(tr1,container->saveDir);
  strcat(tr1,"wav");
  mkdir(tr1,0777);
  free(tr1);
  tr1=calloc(sizeof(int),1024);
  strcat(tr1,container->saveDir);
  strcat(tr1,"exe");
  mkdir(tr1,0777);
  free(tr1);
  tr1=calloc(sizeof(int),1024);
  strcat(tr1,container->saveDir);
  strcat(tr1,"pic");
  mkdir(tr1,0777);
  free(tr1);
  /*Setup thread.*/
  ooo=malloc(sizeof(o_o));
  ooo->oo = container;
  printf("\n");
  printf("Running actual recovery\n");
  pthread_create(&threadID,NULL,scan,(void *)ooo);/*thread of course :D*/
  while(container ->searchActive ==True){
	  perc = (container->turre*100)/container->total;
	  fprintf(stderr,"%lld %% complete; %lld out of %lld results saved\r",perc,container->turre, container->total);
  }
  printf("\n");
  *box = container;
}
/*end the end function o.o*/

