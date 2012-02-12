/*Sabino San Juan, IV
 * disk image tool
 * 
 * this file has the FIFO
 structure installed*/
#include"head.h"
/*list struct*/

/*Begin peek*/
long long peek(list *head){/*peek at next address*/
   if(head !=NULL){
      return head->next->name;
   }
   /*nobody home, send 0*/
   return 0;
}
/*End peerk*/

/*Begin createNode*/
list *createNode(long long name, char *file, unsigned int type,unsigned int misc, long long address){
/*create a node for me
 * with name of file, type of file, and adress in old file. will return NULL if failure*/
   list *new = malloc(sizeof(list));
   if(name == 0){/*got nothing so give nothing*/
     free(new);
     return NULL;
   }
   new ->name = name;
   new->file = malloc(MAXLEN);
   strncpy(new->file, file, strlen(file)+2);
   new ->type = type;
   new ->misc = misc;
   new ->address = address;
   new ->next = NULL;
   new ->prev = NULL;
   return new;
}
/*End createNode*/

/*Begin pushNode*/
void  pushNode(list *ne, list **head, list **tail){
/*adds name to the list in FIFO order*/
  
  /*printf("adding %s to list\n",ne ->name);*/
  list *hea = *head;
  list *tai = *tail;
  if(ne !=NULL && ne->name !=0){/*make sure we are NOT getting a null item*/
     /*if(haveName(ne ->name,*head)==True){
       printf("Oops^^ trying to add identical name in DIR\n");
     }*/
     if(hea == NULL){
       /*initial spot*/
       hea = malloc(sizeof(list));
	   tai = malloc(sizeof(list));
	   hea ->file = malloc(MAXLEN);
	   tai ->file = malloc(MAXLEN);
	   strncpy(hea->file,"HEAD", strlen("HEAD"+2));
	   strncpy(tai->file,"TAIL", strlen("TAIL"+2));
	   hea->type = HEAD;
	   tai->type = TAIL;
	   /*Finished setting up the head and tail pointers.*/
	   hea ->next = ne; /*head points to item 1*/
	   hea ->prev = NULL;
	   tai-> prev = ne; /*tail points to only item in list*/
	   tai->next = NULL;
       /*end setup of initial item*/   
     }
     else{
       list *nn = tai;
       nn ->prev->next = ne;/*set last item to point next to new last item*/
       nn ->prev->next->prev= nn->prev;/*have new last item point prev to former last item*/
       nn->prev = nn->prev->next;/*have tail point to new last item*/
       tai = nn;
     }
     *head = hea;
	 *tail = tai;
  }/*end if being given null item*/
}
/*End pushNode*/

/*Begin popNode*/
list *popNode(list **head, list **tail){
/*pop first item on FIFO list. returns NULL if no object present.*/
  list *hea = (*head);
  list *tai = (*tail);
  if (hea ==NULL || tai == NULL){/*no FIFO*/
   return NULL;
  }
  if(hea->next == tai->prev){ /*one item left in list*/
    list *ret=hea ->next;/*save the lone item*/
    /*free stuff*/
    free(hea->file);
    free(tai->file);
	hea->file = NULL;
	tai->file = NULL;
    free(hea);
    free(tai);
    hea = NULL;
    tai=NULL;
    *head = hea;
    *tail = tai;
    ret -> next = NULL;
    ret -> prev = NULL;
    return ret;	
  }/*end lone item case*/
  /*begin generic case*/
  list *ret = hea->next;/*save the top of the head*/
  hea->next ->next ->prev =NULL;/*new next item doesn't point to old object*/
  hea ->next = hea->next->next;/*head points to new top item*/
  ret ->next = NULL; /*object to return does NOT point to anything else!*/
  return ret;/*return this object*/
}
/*End popNode*/

/*begin wipeStack*/
void wipeStack(list **head, list **tail){
   list *h = *head;
   list *t = *tail;
   while (h !=NULL || t !=NULL){
    list *del = popNode(&h, &t);/*popping of node*/
	if(del == NULL){
	   break;
	}
	if(del->file !=NULL){
	   free(del->file);
	   del->file = NULL;
	}
	free(del);
	del = NULL;
   }/*end while loop*/
}
/*end wipeStack*/
/*END LIST FUNCTIONS*/
