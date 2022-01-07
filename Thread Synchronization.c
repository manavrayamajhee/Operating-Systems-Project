#include <sched.h>                                                            
#include <stdio.h>    
#include <stdlib.h>                                                           
#include <unistd.h>     
#include <pthread.h> 
#include <fcntl.h>    

#define BUFFER_SIZEONE_TWO 10
#define BUFFER_SIZETWO_THREE 20

struct Buffer {
   char line[256];
};

struct Buffer first_buffer[BUFFER_SIZEONE_TWO];
struct Buffer second_buffer[BUFFER_SIZETWO_THREE];

int in=0, out=0, in_TwoThree=0, out_TwoThree=0, num_lines=0,readsize=1,word_count=0,total_word=0,inputFile;
char ary, numeral[2],line_numeral[2];         

//convert integer to string numeral
char* line_Numeral(int num){
  int temp, temp2, digit_size=0,startpos=0;
  temp = num;
  temp2 = num;
  static char string_numeral[5];
  while ( temp > 0 ){
      temp = temp / 10;
      digit_size++;
    }
    startpos= digit_size-1;
  while ( startpos >= 0 ){
      temp = temp2 % 10;
      string_numeral[startpos] = '0' + temp;
      temp2 = temp2 / 10;
      startpos--;
      }
    string_numeral[digit_size]='\0';
    return string_numeral;
}

void *thread_one(void *arg) {  
  while(1){
    int i = 0, temp;
    while((readsize =read( inputFile, &ary, 1))>0){
      if(ary=='\n'){
        temp = i-1; 
        //removing whitespaces from last to first 
        while(temp > 0){
            if(first_buffer[in].line[temp]== '\n' || first_buffer[in].line[temp] == '\t' || first_buffer[in].line[temp]==' '){
                temp--;
            }else break;
        }     
        //adding \n right after the last character other than whitespace
        first_buffer[in].line[temp]='\n';
        break;}
        else{
        first_buffer[in].line[i]=ary;
        i++;    
        }
    }
    //break when end of file is reached.
    if (readsize == 0){
      break;           
    }
    while(((in+1)%BUFFER_SIZEONE_TWO)==out){
     sched_yield();
    }
    in = (in+1)%BUFFER_SIZEONE_TWO;  
  }
                                                         
}   
void *thread_two(void *arg) {  
  while(1){
    while(in==out){
      sched_yield();
    }
    int pos=0,j=0;
    // Reading from the first buffer one line at a time.
    while (first_buffer[out].line[pos] != '\0') {
      if(first_buffer[out].line[pos]==' ' || first_buffer[out].line[pos]=='\n'){
        // creates a c-string appened with '\0' when space is encountered
        second_buffer[in_TwoThree].line[j]='\0'; 
        in_TwoThree = (in_TwoThree+1)%BUFFER_SIZETWO_THREE;
        j=0;  
        while(((in_TwoThree+1)%BUFFER_SIZETWO_THREE)==out_TwoThree){
              sched_yield();
        } 
      }else{
        // Read from the first buffer and assigns the word into second buffer.
        second_buffer[in_TwoThree].line[j] = first_buffer[out].line[pos];
        j++;
      }
      pos++; 
    }
    for (int k=0;k<256;k++){
              first_buffer[out].line[k]='\0';
            }
    num_lines++;  
    sleep(1);
    write(1,"*",2);
    write(1,"word count:",12);
    write(1, line_Numeral(word_count), 4);
    write(1,"*",2);
    write(1,"\n*line count:",14);
    write(1, line_Numeral(num_lines), 4);
    write(1,"*",2);
    write(1,"\n\n",3);
    word_count=0;
    out = ( out + 1 ) % BUFFER_SIZEONE_TWO;  
  }                                                                                                                        
}  
void *thread_three(void *arg) {  
  while(1){
            while(in_TwoThree==out_TwoThree){
              sched_yield();
            }
            // Ignore empty strings
            if( second_buffer[out_TwoThree].line[0]=='\0'){
            }else{
              // Prints words from second buffer.
              write(1,"*",2);
              write(1,&second_buffer[out_TwoThree].line,sizeof(second_buffer[out_TwoThree].line));
              write(1,"*\n",3);
              word_count++;
              total_word++;
            }
            //Emptying the buffer after each read
            for (int k=0;k<256;k++){
              second_buffer[out_TwoThree].line[k]='\0';
            }
            out_TwoThree = (out_TwoThree+1)%BUFFER_SIZETWO_THREE; 
          }                                                                                                                        
}  
                                                                         
int main(int argc, const char *argv[]) {                                                                        
  pthread_t t1, t2, t3;     
  if ((inputFile = open(argv[1], O_RDONLY, 0)) == -1){
		write(1,"Could not open file", 16);
		return 2;
	}                                                                         
  if (pthread_create(&t1, NULL, thread_one,"thread 1") != 0) {                     
    perror("pthread_create() error");                                           
    exit(1);                                                                    
  }                                                                                                                                                          
  if (pthread_create(&t2, NULL, thread_two, "thread 2") != 0) {                     
    perror("pthread_create() error");                                           
    exit(2);                                                                    
  }                                                                                                                                                           
  if (pthread_create(&t3, NULL, thread_three, "thread 3") != 0) {                     
    perror("pthread_create() error");                                           
    exit(3);                                                                    
  }               
  if(pthread_join(t1,NULL)!=0){
            perror("failed to join");
        }           
  if(pthread_join(t2,NULL)!=0){
            perror("failed to join");
        }     
  if(pthread_join(t3,NULL)!=0){
            perror("failed to join");
        }                                                                                                                                                                                                        
  close(inputFile);                                                                           
  exit(0); /* this will tear all threads down */                                
}                                                                               