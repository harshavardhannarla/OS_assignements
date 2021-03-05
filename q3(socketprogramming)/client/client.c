#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include<stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 8000

#define RED  "\x1b[0;31m"
#define BRED "\x1b[1;31m"
#define GREEN  "\x1b[0;32m"
#define BGREEN "\x1b[1;32m"
#define YELLOW  "\x1b[0;33m"
#define BYELLOW "\x1b[1;33m"
#define BLUE "\x1b[0;34m"
#define BBLUE "\x1b[1;34m"
#define MAGENTA  "\x1b[0;35m"
#define BMAGENTA "\x1b[1;35m"
#define CYAN  "\x1b[0;36m"
#define BCYAN "\x1b[1;36m"
#define RESET  "\x1b[0m"






void separating_buff(char * buff, char *parts[],int *num)
{   
   int c=0;
   char * token = strtok(buff," \n");

   while( token!=NULL )
   {
       parts[c] = strdup(token);
       c++;
       token = strtok(NULL," \n");
   
   }
   parts[c]=NULL;   
   *num = c;
}



int main()
{
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
     
    bool flag = true;
    size_t sz =1024;
    size_t ssz = 100;
    char * inpbuff = (char *)calloc(ssz,sizeof(char));
    char *inpbuff_dup = (char *)calloc(ssz,sizeof(char));
    char *parts[10];
    int n;
    char *buffer = (char *)calloc(sz,sizeof(char));
    char  filename[100] = {0};
    char *parts2[4];
    int k=0;

    while(flag)
    {  //free(buffer);free(inpbuff);free(inpbuff_dup);
        memset(buffer,0,sz);
        memset(inpbuff,0,ssz);
        memset(inpbuff_dup,0,ssz);
        printf("\nclient>");
       getline(&inpbuff,&sz,stdin);
       strcpy(inpbuff_dup,inpbuff);
      
       separating_buff(inpbuff_dup,parts,&n);

        if(strcmp(parts[0],"exit")==0)
        { strcpy(filename,"exit");
         
         send(sock,filename,100,0);
          flag= false;
          continue;
        }       

       if(strcmp(parts[0],"get")!=0)
       { printf("Invalid input format\n");
         continue;
       }
       n-=1;
       int cur =0,tot=0; 

       for(int i=1;i<=n;i++)
       {  memset(filename,0,ssz);
          strncpy(filename,parts[i],strlen(parts[i]));
         
         if(send(sock,filename,100,0) == -1)
         {perror("Error in send");exit(1);}
         
         recv(sock,filename,100,0);
         separating_buff(filename,parts2,&k);
            
         if(strcmp(parts2[0],"no")==0)
         { printf("\n");
           printf(BRED "File-%s doesnot exists/not readable on server side" RESET,parts[i]);
         printf("\n");
         continue;
         }
          
         else if(strcmp(parts2[0],"yes")==0)
         {  printf("\n");
            printf(BBLUE "File-%s exists on server side " RESET,parts[i]);
            printf("\n");
             cur=0; tot= atoi(parts2[1]);
           FILE * to = fopen(parts[i],"w");
           int p=0;
           while((p= recv(sock,buffer,sz,0))>0)
           {    
               if(fwrite(buffer,sizeof(char),p,to)!=p)
               {perror("write file error");exit(1);}
               
               cur+=p;
               memset(filename,0,ssz);
               
               sprintf(filename, "\rDownload percent is  %.2Lf", ((long double)(cur) / (long double) tot) * 100);
               write(1, filename, strlen(filename));

               
               if(p!=sz)
               break;
           }
           printf("\n");
          fclose(to);       
         }

         else
         { printf("\n");
            printf(BGREEN "Error in checking of file -%s" RESET,parts[i]);
           printf("\n");
            continue;
         }
       
         
       }
      


    }

    return 0;
}
