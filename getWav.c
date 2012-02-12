/*Sabino San Juan, IV
 * getWav
 * this file hold the getWav function from the program*/
#include "head.h"

void getWav(list *o_O, char *dir){/*getWav begin*/
        long long i=o_O->name;
        long long j = 0;
        unsigned char arr[SCN]={0};
        char *boom  =malloc(MAXLEN);
        long long fileSize=0;/*how long is file*/
        long long limit=0;/*so we do not exceed the size of the file*/
        if(o_O->misc ==AVI){
          sprintf(boom,"%smov/avi%lld.avi",dir,i);/*AVI*/
        }
        else{
          sprintf(boom,"%swav/wav%lld.wav",dir,i);/*WAV*/
        }
        FILE *fp;/*file to be written to*/
        FILE *stream;/*file reading from*/
        stream=fopen(o_O->file,"r");
        fseek(stream,o_O->address,SEEK_SET);
		int skip=False;/*if we need to skip outta here*/
        fileSize=lof(stream);/*size of the file*/
        if((fileSize<1000000)/*||(fileSize>3000000000)*/){
          /*do not proceed if < 900 kb or > 3 gb*/
          fclose(stream);
          stream=NULL;
          free(boom);
          boom=NULL;
		  skip = True;
        }
		if(skip !=True){
          fp = fopen(boom,"w");
          putc(0x52,fp);
          putc(0x49,fp);
          putc(0x46,fp);
          putc(0x46,fp);
		}
        while(skip == False && limit <=fileSize){/*loop to read through the section*/
           j=fread(arr,sizeof(unsigned char),(SCN),stream);/*dump data into array from HDD A.S.A.P*/
           if(feof(fp) || ferror(fp)){/*end of file*/
              break;
           }
           limit=limit+j;
           j=fwrite(arr,sizeof(unsigned char),j,fp);/*dump data into HDD from array A.S.A.P*/
        }/*end while loop*/
		if(skip !=True){
          fclose(fp);/*close file*/
          fp=NULL;
          fclose(stream);/*close the file*/
          stream=NULL;
          free(boom);
          boom=NULL;/*done in here*/
		}
}/*end function*/


void checkWav(global *box,unsigned char arry[]){
  long long i = 3;/*counter*/
  long long p=0;
  p=box->cur;
  /*check to see if the function above should be activated*/
  if((box->flags & Ao)==Ao ||(box->flags & Wo)==Wo){
    /*we may execute*/
    for(i=3;i<SCN;i++,p++){
      if (p >= box-> length){
        break;
      }
	  int typ=riff_type(arry,i,box);
      if(typ!=0){/*valid*/
	    if(typ == 1){/*avi*/
           box->aviCount=box->aviCount+1;/*name at end of file*/
	       start(box->aviCount, p, &box, Vo,AVI);	     
	    }/*end avi*/
        if(typ==2){/*wav*/
           box->wavCount=box->wavCount+1;/*name at end of file*/
	       start(box->wavCount, p, &box, Wo,0x0000);
	    }/*end wav*/
      }/*end if this is a valid audio file*/  
    }/*end looper*/
  }/*end if we can even execute*/
}/*end the check program*/
