#include <stdio.h>
#include <sys/socket.h>
#include<sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include<stdbool.h>
#define PORT 8000

int main()
{   int sockfd,skt;
    size_t sz =1024;
    struct sockaddr_in serveaddr;
    int opt=1;
    if((sockfd= socket(AF_INET,SOCK_STREAM,0))==0)
    { perror("Error reg sock creation");
       return 0;
    }

    
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    serveaddr.sin_addr.s_addr=INADDR_ANY;
    serveaddr.sin_family=AF_INET;
    serveaddr.sin_port=htons(PORT);
    
    if (bind(sockfd, (struct sockaddr *)&serveaddr,
                                 sizeof(serveaddr))<0)
   {  perror("Error reg binding");
       return 0;
    }
    if (listen(sockfd, 3) < 0) 
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    int siz= sizeof(serveaddr);
   if ((skt = accept(sockfd, (struct sockaddr *)&serveaddr,
                       (socklen_t*)&siz))<0)
    {  perror("Error reg accepting");
       return 0;
    }
   char filename[100]= {0};

  /* char *t =(char *)malloc(10*sizeof(char));
   recv(skt,t,sizeof(t),0);
   int n = atoi(t);*/
  // printf("The value of n is %d\n",n);
   char *buffer = (char *)malloc(sz*sizeof(char));
   int a;
   bool flag= true;
   while(flag)
   { 
       a=recv(skt,filename,100,0);
       
       if(a==-1)
       {
        perror("error in recv");
        exit(1);
       }
      // printf("%s e adf\n",filename);
      // printf("value of a is %d and %s\n",a,filename);
        if(!strcmp(filename,"exit"))
       {  
           flag=false;
          continue;

       }

       FILE *frm = fopen(filename,"r"); 
       int p=0;
       if(frm == NULL)
       {   memset(filename,0,100);
           sprintf(filename,"no");
           send(skt,filename,100,0);
           memset(filename,0,100);
           continue;
       }
       else
        {   if(access(filename,R_OK)==-1)
            {   memset(filename,0,100);
                sprintf(filename,"no");
                send(skt,filename,100,0);
                memset(filename,0,100);
                continue;

            }
            
             memset(filename,0,100);
             fseek(frm,0,SEEK_END);
             long int fsz = ftell(frm);
             rewind(frm);

           sprintf(filename,"yes %ld",fsz);
           send(skt,filename,100,0);
           memset(filename,0,100);
          
         while( (p=fread(buffer,sizeof(char),sz,frm))  > 0)
          { if(send(skt,buffer,p,0) ==-1)
             {perror("Error reg sending");
             exit(1);}

            memset(buffer,0,sz);
                        
          }       
         fclose(frm);
       }
       

     
       memset(filename,0,100);

    
   }
   close(skt);

return 0;
}