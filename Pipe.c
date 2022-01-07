#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#define Size 128

int main(int argc, const char *argv[])
{
	int inputFile, outputFile;
	if (argc != 3){
		write(1,"Please provide appropriate number of command line arguments\n", 40);
		exit(1);
	}
	if ((inputFile = open(argv[1], O_RDONLY, 0)) == -1){
		write(1,"Could not open file", 16);
		return 2;
	}
	if ((outputFile = creat(argv[2], 0644)) == -1){
		write(1,"Can't create file", 15);
		return 3;
	}
	int datapipe[2],id;

	if (pipe(datapipe)==-1){
		write(1,"Creation of Pipe Failed", 13);
		return 1;
	}
	id = fork();

	if (id < 0)
	{
		write(1,"fork Failed", 12);
		return 4;
	}
	else if (id > 0)
	{
		//Start of parent process
		close(datapipe[0]);
		char buf[Size];
        int readsize;
        while ((readsize = read( inputFile, buf, Size))>0){
            write(datapipe[1], buf,readsize);
        }
		close(datapipe[1]);
        close(inputFile);
        close(outputFile);
	}
	else
	{
								//Start of child process
		close(datapipe[1]);
        int readsize;			//size of chunk that is actually read
		char suf[Size];
        while ((readsize = read(datapipe[0],suf,Size)) > 0){
            write(1,suf, readsize); 
			write(1,"\n", 1);
            if (write(outputFile, suf,readsize) != readsize)
                {
                write(1,"Can't write to file", 17);
                close(inputFile);
                close(outputFile);
                return 4;
                }
			write(outputFile,"\n",1);	
        }
		close(datapipe[0]);
		exit(0);
	} 
}
