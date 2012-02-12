/*Sabino San Juan, IV
 * getPic
 * this file hold the getPicture function from the program*/
#include "head.h"

void getExe(list *o_O, char *dir){/*getpicture begin*/
        long long i=o_O->name;
        char *boom  =malloc(MAXLEN);
        sprintf(boom,"%sexe/test%lld.jpg",dir,i);/*the file name*/
        FILE *fp;/*file to be written to*/
        FILE *stream;/*file reading from*/
        stream=fopen(o_O->file,"r");
        fseek(stream,o_O->address,SEEK_SET);
        /*clanup the object being passed*/
        int c;
        int j=0;
        fp = fopen(boom,"w");
        putc(0xff,fp);
        putc(0xd8,fp);
        putc(0xff,fp);
        while(1){/*loop to read through the section*/
           if((c=fgetc(stream))==0xff){
             if((c=fgetc(stream))== 0xd9){
               putc(0xff,fp);
               putc(0xd9,fp);
               if(j>9000){
                 break;
               }
             }
             putc(0xff,fp);
           }
           if(c == EOF){
             break;
           }
           putc(c,fp);
           j=j+1;
           if(j==2){/*check to see if the fifth bute is 0x00*/
             if(c!=0x00){
               break;
             }
           }
        }/*end while loop*/
        fclose(fp);/*close file*/
        fp=NULL;
        if(j <=9000){
        /*file is very tiny and probably garbage*/
           remove(boom);
        }
        fclose(stream);/*close the file*/
        stream=NULL;
        free(boom);
        boom=NULL;
}/*end function*/

void checkExe(global *box,unsigned char arry[]){
  /*check to see if the function above should be activated*/
  long long i = 3;/*counter*/
  long long p=0;
  p=box->cur;
  if((box->flags & Ao)==Ao ||(box->flags & Po)==Po){
    /*we may execute*/
	for(i=3;i<SCN;i++,p++){
      if (p >= box-> length){
        break;
      }
      if((arry[i-3]==0xff)&&(arry[i-2]==0xd8)&&(arry[i-1]==0xff)){
        box->picCount=box->picCount+1;
        p--;
        start(box->picCount, p, &box, Po,0x0000);
      }/*end if this is a valid picture*/  
    }/*end looper*/
  }/*end if we can even execute*/
}/*end the check program*/






