#include <sched.h>                                                            
#include <stdio.h>    
#include <stdlib.h>                                                           
#include <unistd.h>     
#include <pthread.h> 
#include <fcntl.h>   


#define STACK_SIZE 20
int stack[STACK_SIZE];
int top = 0;
pthread_mutex_t myLock;
//local struct variables inside thread two for transferring  data to main thread after it finishes
struct data {
        int _popSum ;
        int _popOrder[120];
    };

void push(int item) {
    if (top < STACK_SIZE) {
        stack[top] = item;
        top++;     
    }
    else
        {
        sched_yield();}
}
int is_empty() {
    if (top == 0)
        return 1;
    else
        return 0;
}

int pop() { 
    if (top != 0) {
        top--;
        return stack[top];
    }
    else
        {
        sched_yield();}  
}

void *thread_one(void *arg) {  
    int pushSum = 0, count =0, data=1;
    int *value = malloc(sizeof(int));
    for(int i=0;i<120;i++){        
        //triggers sched_yield after every 10 continious invocation of push.
        while (count < 10 && data<=120){
          //triggers sched_yield when the stack is full
          if(top == STACK_SIZE){ 
              break;
            } 
            pthread_mutex_lock (&myLock);
                push(data);
                pushSum += data;
                count++;
                data++;    
            pthread_mutex_unlock (&myLock);
        }
      count=0;
      sched_yield();
}
  *value = pushSum;  
  return (void *)value;                                                       
} 
  
void *thread_two(void *arg) {  
    struct data *d1 = malloc(sizeof d1);
    int popSum=0, popOrder[120];
    int count=0;
    // check if popOrder is full.
    while(count<120){
      pthread_mutex_lock(&myLock);
          if (top == STACK_SIZE || top < 5 ) { // Empty the stack when full or has less than 5 elements
            while (!is_empty()){
                popOrder[count] = pop();
                popSum = popSum + popOrder[count];
                count++;
              }
          }   
          else{//When stack is not full pop five elements from the stack
          for(int i=0;i<5;i++){
              popOrder[count] = pop();
              popSum = popSum + popOrder[count];
              count++;
          }     
      } 
      pthread_mutex_unlock (&myLock);
      sched_yield();
      } 
    // saving contents of popSum and popOrder to struct d1 to finally return to main thread
    d1->_popSum = popSum;
    for(int i = 0; i < 120; i++) {
      d1->_popOrder[i] = popOrder[i];
    } 
   pthread_exit(d1);                                                                                                             
}

                                                                         
int main(int argc, const char *argv[]) {  
  FILE *outprint;
  outprint = fopen(argv[1],"w");
  if(outprint == NULL)
   {
      printf("Error!");   
      exit(1);             
   }                                                                 
  pthread_t t1, t2;
  int *pushSum;  
  struct data *d1;   
  
  pthread_mutex_init (&myLock, NULL);                                                              
  if (pthread_create(&t1, NULL, thread_one,"thread 1") != 0) {                     
    perror("pthread_create() error");                                           
    exit(1);                                                                    
  }                                                                                                                                                          
  if (pthread_create(&t2, NULL, thread_two, "thread 2") != 0) {                     
    perror("pthread_create() error");                                           
    exit(2);                                                                    
  }                                                                                                                                                                      
  if(pthread_join(t1,(void *)&pushSum)!=0){
            perror("failed to join");
        }           
  if(pthread_join(t2,(void *)&d1)!=0){
            perror("failed to join");
        }  
  fprintf(outprint,"All 120 elements of Pthread 2 popOrder array\n");   
    for(int i = 0; i < 120; i++) {
      fprintf(outprint,"%d ",d1->_popOrder[i]);   
   }
   fprintf(outprint,"\nthe value of Pthread1 pushSum %d",*pushSum);
   fprintf(outprint,"\nthe value of Pthread2 popSum %d",d1->_popSum);
    free(d1); 
    free(pushSum);                                                                 
    exit(0);                             
}                                                                               