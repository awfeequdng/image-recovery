/*Sabino San Juan, IV
 * getDoc
 * this file hold the getDocument function from the program*/
#include "head.h"
/*begin type of text document*/
/*Note all are returning ints*/

static unsigned int isLegit(unsigned char arry[], long long i){/*This a file I can use?*/
     if(arry[i-3]==0xd0 && arry[i-2]==0xcf && arry[i-1]==0x11 && arry[i]==0xe0){/*DOC*/
        return DOC;
     }
     else if(arry[i-3]==0x50 && arry[i-2]==0x4b && arry[i-1]==0x03 && arry[i]==0x04){/*DOCX*/
        return DOX;
     }
     else if(arry[i-3]==0x25 && arry[i-2]==0x50 && arry[i-1]==0x44 && arry[i]==0x46){/*PDF*/
        return PDF;
     }
     else if(arry[i-3]==0x7b && arry[i-2]==0x5c && arry[i-1]==0x72 && arry[i]==0x74){/*RTF*/
        return RTF;
     }
     else if(arry[i-3]==0x3c && arry[i-2]==0x3f && arry[i-1]==0x78 && arry[i]==0x6d){/*FDX*/
        return FDX;
     }
	 return 0x0000;/*returns 0x0000 if no match. anything else means it found something*/
}
/*end type of text document*/
/*isComplete*/
static int isComplete(unsigned char ar[],unsigned int type){
    /*checks if we are done*/
	if(type == (type & DOC)){/*.doc*/
	    if((ar[0]==0x74)&& (ar[1]==0x2e)&& (ar[2]==0x38)){
		   return True;
		}
	}/*.doc*/
	if(type == (type & DOX)){/*.docx*/
	    if((ar[0]==0x4b)&& (ar[1]==0x05)&& (ar[2]==0x06)){ /*50 4b 05 06*/
		   return True;
		}
	}/*.docx*/
	if(type == (type & PDF)){/*.pdf*/
	    if((ar[0]==0x45)&& (ar[1]==0x4f)&& (ar[2]==0x46)){/*45 4f 46 0d 0a OR 45 4f 46 0a*/
		   return True;
		}
	}/*.pdf*/
	if(type == (type & RTF)){/*.rtf*/
	    if((ar[0]==0x0d)&& (ar[1]==0x0a)&& (ar[2]==0x00)){/*VERIFY! 20 7d 7d */
		   return True;
		}	
	}/*.rtf*/
	if(type == (type & FDX)){/*.fdx*/
	    if((ar[0]==0x74)&& (ar[1]==0x3e)&& (ar[2]==0x0a)){/*VERIFY! 6e 61 6c 44 72 61 66 74 3e 0a */
		   return True;
		}	
	}/*.rtf*/
	/*um we are not done*/
	return False;
}
/*isComplete*/
void getDoc(list *o_O, char *dir){/*getDocbegin*/
	long long i=o_O->name;
	unsigned int misc = 0x0000;/*what is this thing?*/
	/*0 is doc, 1 is docx, 2 is pdf, 3 is rtf*/
	char *boom  =malloc(MAXLEN);
	unsigned char ar[4] = {0};/*this will be the array to use to check type of file*/
	FILE *fp;/*file to be written to*/
	FILE *stream;/*file reading from*/
	stream=fopen(o_O->file,"r");
	fseek(stream,o_O->address,SEEK_SET);
	/*clanup the object being passed*/
    misc =o_O->misc;
	/*getName*/
	if((misc & DOC)==DOC){/*.doc*/
	   sprintf(boom,"%sdoc/doc%lld.doc",dir,i);/*the file name*/
	}
	if((misc & DOX)==DOX){/*.docx*/
	   sprintf(boom,"%sdoc/docx%lld.docx",dir,i);/*the file name*/
	}
	if((misc & PDF)==PDF){/*.pdf*/
	   sprintf(boom,"%sdoc/pdf%lld.pdf",dir,i);/*the file name*/
	}
	if((misc & RTF)==RTF){/*.rtf*/
	   sprintf(boom,"%sdoc/rtf%lld.rtf",dir,i);/*the file name*/
	}
	if((misc & FDX)==FDX){/*.fdx*/
	   sprintf(boom,"%sdoc/fdx%lld.fdx",dir,i);/*the file name*/
	}
	/*end getname*/
	if(misc != 0x0000){/*should be an actual word file, if not it is not getDoc's fault!*/
  	   fp = fopen(boom,"w");
	   if(!fp){
   	      perror(boom);
	      exit(EXIT_FAILURE);
	   }
	   putc(ar[0],fp);
	   putc(ar[1],fp);
	   putc(ar[2],fp);
	   putc(ar[3],fp);
	   ar[0] = fgetc(stream);/*start throwing data on the pile :3*/
	   putc(ar[0],fp);
	   ar[1] = fgetc(stream);
	   putc(ar[1],fp);
	   int c = fgetc(stream);
	   ar[2] = c;
       while(c!=EOF){/*loop to read through the section*/
	     /*check for end of loop and EOF*/
         if((c==EOF)||isComplete(ar,misc)==True){/*are we done with this text file yet?*/
	        break;
	     }
	     putc(ar[2],fp);/*store actual data*/
	     ar[0]=ar[1];/*move forward*/
	     ar[1]=ar[2];
		 c=fgetc(stream);
	     ar[2]=c;
	   }/*end while loop*/
	   fclose(fp);/*close exported file*/
	   fp=NULL;
	   free(boom);
	   boom=NULL;
	}/*not an invalid file*/
	fclose(stream);/*close the masterfile*/
	stream=NULL;
}/*end function*/

void checkDoc(global *box,unsigned char arry[]){
  long long i = 3;/*counter*/
  long long p=0;
  p=box->cur;
  /*check to see if the function above should be activated*/
  unsigned int good=0;/*to tell us what we got on our paws o.o*/
  if((box->flags & Ao)==Ao ||(box->flags & To)==To){
    /*we may execute*/
    for(i=3;i<SCN;i++,p++){
      if (p >= box-> length){
        break;
      }
      good = isLegit(arry,i);
	  if(good !=0x0000){/*go for it, and we have type too <3 :D*/
	    box->docCount=box->docCount+1;/*name at end of file*/
	    start(box->docCount, p, &box, To,good);
      }/*end if this is a valid text*/ 
    }/*end looper*/    
  }/*end if we can even execute*/
}/*end the check program*/
