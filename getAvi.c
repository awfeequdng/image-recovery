/*Sabino San Juan, IV
 * getAvi
 * this file hold the getAvi function from the program*/
#include "head.h"
static int start_typeI(unsigned char arry[], long long i){
  /*type 1 .mov*/
  if((arry[i-3]==0x20)&&(arry[i-2]==0x66)&&(arry[i-1]==0x74)&&(arry[i]==0x79)){/*missing 00 00 00 from begining*/
    return True;
  }
  return False;
}
static int start_typeII(unsigned char arry[], long long i){
  /*type 2 .mov*/
  if((arry[i-3]==0x01)&&(arry[i-2]==0x18)&&(arry[i-1]==0x73)&&(arry[i]==0x6b)){/*missing 00 00 from begining*/
    return True;
  }
  return False;
}

static int end_type(unsigned char arry[], long long i, unsigned int type){/*type of file, returns False if not done*/
   if(type == MOV1){/*mov1*/
      if((arry[i-3]==0x75)&&(arry[i-2]==0x64)&&(arry[i-1]==0x74)&&(arry[i]==0x61)){/*this type is over!*/
	     return True;
	  }
   }/*mov1*/
   if(type == MOV2){/*mov2*/
      if((arry[i-3]==0x75)&&(arry[i-2]==0x64)&&(arry[i-1]==0x74)&&(arry[i]==0x61)){/*this type is over!*/
	     return True;
	  }
   }/*mov2*/   
   return False;/*sorry, not done*/

}
void getAvi(list *o_O, char *dir){/*getAvi begin*/
	long long i=o_O->name;
	char *boom  =malloc(MAXLEN);
	long long j=3;/*counter*/
	long long notDone = True;/*is load done?*/
	int first=True;/*first iteration*/
    arry *cached = malloc(sizeof(arry));
	/*setup array*/
	unsigned char data[SCN] = {0};
    memmove(data,cached->arr, SCN*sizeof(char));/*copy array*/
	cached ->processed =0;
	cached ->cur = 0;
	cached ->closed = False;
	cached ->isReady = False;
	cached ->trans = False;
	/*end setup of array*/
	sprintf(boom,"%smov/mov%lld.mov",dir,i);/*the file name*/
	FILE *fp;/*file to be written to*/
	FILE *stream;/*file reading from*/
	stream=fopen(o_O->file,"rb");
	fseek(stream,o_O->address,SEEK_SET);
	/*clanup the object being passed*/
	fp = fopen(boom,"w");
	notDone=load(&cached, stream, True);
	if(o_O->misc ==MOV1){
	   putc(0x00,fp);
	   putc(0x00,fp);
	   putc(0x00,fp);
	}
	if(o_O->misc ==MOV2){
	   putc(0x00,fp);
	   putc(0x00,fp);
	}
	if(notDone != True ){ /*spit out*/
	   fwrite(cached ->arr,sizeof(unsigned char),notDone,fp);/*dump data into HDD from array A.S.A.P*/
	}
	while(notDone == True ){/*loop to read through the section*/
	   if(end_type(cached -> arr, j, o_O ->misc)==True){
	      fwrite(cached ->arr,sizeof(unsigned char),j+1,fp);/*dump data into HDD from array A.S.A.P*/
		  break;/*get OUT of here*/
	   }/*end case to end */
	   if(j==SCN-1){/*at the end of the array. Reload time! :D*/
	      if(first ==True){/*iteration 1*/
		    fwrite(cached ->arr,sizeof(unsigned char),j,fp);/*dump data into HDD from array A.S.A.P*/
			first =False;
		  }
		  else if (first == False){
		    fseek(fp,-2,SEEK_CUR);
			fwrite(cached ->arr,sizeof(unsigned char),j,fp);/*dump data into HDD from array A.S.A.P*/
		  }
		  j=2;/*reset array check*/
	      notDone=load(&cached, stream, False);/*get data from HDD QUICKLY*/
		  if (notDone != True ) {/*done and get out of here!*/
		    if (first == False){
		       fseek(fp,-2,SEEK_CUR);
			}
		    fwrite(cached ->arr,sizeof(unsigned char),notDone,fp);/*dump data into HDD from array A.S.A.P*/
			break;
		  }/*end get OUT*/
	   }/*end reload*/
	   j++;
	}/*end while loop*/
	fclose(fp);/*close file*/
	fp=NULL;
	fclose(stream);/*close the file*/
	stream=NULL;
	free(boom);
	boom=NULL;
	free(cached);
	cached = NULL;
}/*end function*/

void checkAvi(global *box,unsigned char arry[]){
  long long i = 3;/*counter*/
  long long p=0;
  p=box->cur;
  /*check to see if the function above should be activated*/
  if((box->flags & Ao)==Ao ||(box->flags & Vo)==Vo){
    /*we may execute*/
    for(i=3;i<SCN;i++,p++){
      if (p >= box-> length){
        break;
      }
      if(start_typeI(arry,i)==True ){
	     p=p-4;
	     box->aviCount=box->aviCount+1;
         start(box->aviCount, p, &box, Vo,MOV1);
      }/*end if this is a valid video*/  
      else if(start_typeII(arry,i)==True){
	     p=p-4;
	     box->aviCount=box->aviCount+1;
         start(box->aviCount, p, &box, Vo,MOV2);
      }/*end if this is a valid video*/
    }/*end looper*/ 	
  }/*end if we can even execute*/
}/*end the check program*/
