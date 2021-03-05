#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

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
struct phar_comp
{  int batchrem;
   int id;
   pthread_t thrd;
   pthread_mutex_t mutex;
   pthread_cond_t cond;   
   int wait_time;
   int capacity;
   float p;
};


struct vacci_zone
{   int id;
    int slots;
    int rem_slots;
    float p;
   pthread_t thrd;
   pthread_mutex_t mutex;
   pthread_cond_t cond; 

};

struct student
{  int id;
   int round_num;
   bool test;
   pthread_t thrd;


};

struct phar_comp comparr[100];
struct vacci_zone vacciarr[100];
struct student stdarr[100];

int totcomp;
int totzone;
int totstud;
int remstud;
pthread_mutex_t stud_mutex;

void comp_status(struct phar_comp * curr_phar);
void *phar_comp(void * a)
{  struct phar_comp * curr_phar=(struct phar_comp *)a;
    while(1)
    {
     int no_of_batches = 1+rand()%5;
       printf(GREEN "comp %d is preparing  %d batches with probability of %0.2f \n" RESET,curr_phar->id,no_of_batches,curr_phar->p);

     int capacity = 10 + rand()%11;
     
     curr_phar->wait_time =2+rand()%4;;
    
      sleep(curr_phar->wait_time);
     //locking 
     
      pthread_mutex_lock(&curr_phar->mutex);
     

      curr_phar->batchrem=no_of_batches;
      curr_phar->wait_time=curr_phar->wait_time;
      curr_phar->capacity=capacity;
      
      printf(GREEN "comp %d has prepared %d batches each with cap of %d  and probability of %0.2f\n" RESET,curr_phar->id,curr_phar->batchrem,curr_phar->capacity,curr_phar->p);

      comp_status(curr_phar);
      pthread_mutex_unlock(&curr_phar->mutex);  

    }
    return NULL;
}

void comp_status(struct phar_comp * curr_phar)
{ bool emptied= false;
while(!emptied)
{  if(curr_phar->batchrem<=0)
   {printf(CYAN "All vaccines prepared by company %d are emptied resuming production now\n" RESET,curr_phar->id);
    emptied=true;
   }  
   else
   pthread_cond_wait(&curr_phar->cond,&curr_phar->mutex);
   
}
pthread_mutex_unlock(&curr_phar->mutex);
}






void zone_status(struct vacci_zone * cur_zone);

void * vacci_zones(void *a)
{  struct vacci_zone * curr_zone = (struct vacci_zone *)a;
   int i=0;
while(1)
{
while(1)
{
    i++;
    i=i%totcomp;
    struct phar_comp *curr_comp= &comparr[i];
    pthread_mutex_lock(&curr_comp->mutex);
    if(curr_comp->batchrem>0)
    {  curr_zone->slots=curr_comp->capacity;
       curr_zone->p=curr_comp->p;
       curr_comp->batchrem--;

       printf(BLUE"Company %d is delivering a vaccine batch to Vaccination Zone %d which has success probability  %.2f \n" RESET,curr_comp->id,curr_zone->id,curr_zone->p);


       pthread_cond_signal(&curr_comp->cond);
       pthread_mutex_unlock(&curr_comp->mutex); 
       break; 
    }
    else
    pthread_mutex_unlock(&curr_comp->mutex);
}
while(curr_zone->slots>0)
{  printf(MAGENTA"Vaccination zone %d entering vaccinatin phase\n",curr_zone->id); 
  sleep(1.5);
   pthread_mutex_lock(&curr_zone->mutex);
    pthread_mutex_lock(&stud_mutex);
    int k = 1+ rand()%8;
    if(k>curr_zone->slots)
    k=curr_zone->slots;
    if(k>remstud)
    k=remstud;
    
    if(remstud==0)
    { pthread_mutex_unlock(&stud_mutex);
      pthread_mutex_unlock(&curr_zone->mutex);
      break;
    }


    pthread_mutex_unlock(&stud_mutex);
    curr_zone->rem_slots=k;
    curr_zone->slots -= k;
    printf(BCYAN"Vaccination zone %d ready to vaccinate with %d slots \n" RESET,curr_zone->id,curr_zone->rem_slots);
    
    zone_status(curr_zone); 
    pthread_mutex_unlock(&curr_zone->mutex);

    
}
if(remstud==0)
break;

printf(MAGENTA "Vaccination Zone %d has ran outof vaccines \n" RESET,curr_zone->id);

}
return NULL;

}


void zone_status(struct vacci_zone * cur_zone)
{  
while(1)
{
if(cur_zone->rem_slots<=0)
   break;
else
pthread_cond_wait(&cur_zone->cond,&cur_zone->mutex);
}
pthread_mutex_unlock(&cur_zone->mutex);

}

void wait_for_vaccine(struct student * curr_std)
{  bool slot_found =false;
   int zone_no =0 ;
  printf(BBLUE "Student %d has arrived for %d round of vaccination\n" RESET,curr_std->id,curr_std->round_num+1);
  printf( YELLOW"Student %d is waiting to be alloted a slot on a vaccination Zone\n"RESET,curr_std->id); 
  while(!slot_found)
  {  zone_no = (zone_no+1)%totzone;
     struct vacci_zone * cur_zone= &(vacciarr[zone_no]);
    pthread_mutex_lock(&cur_zone->mutex);
    if(cur_zone->rem_slots>0)
    {  printf(GREEN"Student %d assigned a slot on the %d zone and waiting to be vaccinated \n" RESET,curr_std->id,cur_zone->id);
        cur_zone->rem_slots--;
      
       slot_found = true;
       float tmp = cur_zone->p;
       pthread_cond_signal(&cur_zone->cond);
       
       printf(BRED "Student %d on Vaccination Zone %d has been vaccinated which has success probability %.2f\n" RESET,curr_std->id,cur_zone->id,cur_zone->p);
       curr_std->round_num++;
       int a = rand()%101;
       int p = (cur_zone->p)*100;
       if(a<p)
       {curr_std->test =true;
         pthread_mutex_lock(&stud_mutex);
         remstud--;
         pthread_mutex_unlock(&stud_mutex); 
       }
       else
       curr_std->test = false;
       printf(BYELLOW"Student %d has tested %s for antibodies\n" RESET,curr_std->id,curr_std->test==true ? "pos":"neg");


    }
    pthread_mutex_unlock(&cur_zone->mutex);
  


  }
 if(curr_std->test== false  && curr_std->round_num<3)
   wait_for_vaccine(curr_std);
 else if(curr_std->test==false && curr_std->round_num ==3)
 {  printf(BGREEN"Student %d has been vaccinated 3 times but failed \n" RESET,curr_std->id);

 }

return;

}




void  * student(void *a)
{ struct student * curr_std =(struct student *)a;
  wait_for_vaccine(curr_std);

    return NULL; 
}




int main()
{  scanf("%d %d %d",&totcomp,&totzone,&totstud);
   if(totcomp==0 | totzone==0 | totstud==0)
    { printf("Enter Valid input\n");
      return 0;
    }
    pthread_mutex_lock(&stud_mutex);
     remstud=totstud;
    pthread_mutex_unlock(&stud_mutex);
   
    float arr[totcomp];
    for(int i=0;i<totcomp;i++)
    scanf("%f",&arr[i]);

    


    for(int i=0;i<totcomp;i++)
    { comparr[i].id=i;
      comparr[i].p=arr[i];
      pthread_mutex_init(&comparr[i].mutex,NULL);
      pthread_cond_init(&(comparr[i].cond),NULL);
      pthread_create(&comparr[i].thrd,NULL,phar_comp,(void *)(&comparr[i]));

    }
    sleep(1);
    for(int i=0;i<totzone;i++)
    { vacciarr[i].id=i;
      pthread_mutex_init(&vacciarr[i].mutex,NULL);
      pthread_cond_init(&(vacciarr[i].cond),NULL);
      pthread_create(&vacciarr[i].thrd,NULL,vacci_zones,(void *)(&vacciarr[i]));

    }
    sleep(1);
    for(int i=0;i<totstud;i++)
    { stdarr[i].id=i;
      stdarr[i].test=false;
      stdarr[i].round_num=0;
      pthread_create(&stdarr[i].thrd,NULL,student,(void *)(&stdarr[i]));

    }
    for(int i =0;i<totstud;i++)
    { pthread_join(stdarr[i].thrd,NULL);

    }

   /* for(int i=0;i<totcomp;i++)
    { pthread_join(comparr[i].thrd,NULL);

    }*/
    
    
 printf(BMAGENTA"Simulation Over\n" RESET);
 return 0;


}