/* Considered Tests:
1. Player move into wall
+ Invalid move doesn't change the map (shared memory)

2. Player move onto other player
+ Invalid move doesn't change the map (shared memory)

3. Player move off edge of map (without gold)
+ Invalid move doesn't change the map (shared memory)

4. Valid move
+ change map.

5. Move on gold 
+ changes map
+ notice is posted

6. move off edge of map with gold
+ change map
+ notice is posted
*/


#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define READ_END 0
#define WRITE_END 1

using namespace std;


int main(int argc, const char **argv)
{
	pid_t c1PID=-1,c2PID=-1;	//error case
	int stat=0;
	char testC='l';	//test case command(s) -- can be changed
	int fnum1[2],fnum2[2];
	char mymap[]="**1G2*";	//testmap -- can be modified
	
	const char* c1mem;
	const char* c2mem;
		
	int rows,cols;
	int numCallsToPostNotice,numCallsToDrawMap;
	
	const char* gcfifo="gcfifo";
	mkfifo(gcfifo,0666);
	int fd=open(gcfifo,O_RDWR);
	write(fd,&mymap,sizeof(mymap));
	close(fd);
	
	pipe(fnum1);
	pipe(fnum2);
	
	if((c2PID=fork())==-1)	return 1;
	if(c1PID==0)
	{
		execlp("./p_goldchase", "./p_goldchase", NULL);
		
		int i=0;

		read(21, &rows, sizeof(rows));
		read(21, &cols, sizeof(cols));
		
		char m[rows*cols];
		char *mem=m;
		c1mem=mem;
		read(21, mem, rows*cols);
		
		dup2(fnum1[READ_END],21);

		//getKey
		do
		{
			read(21, &numCallsToPostNotice, sizeof(rows));
			read(21, &numCallsToDrawMap, sizeof(cols));
			read(21, mem, rows*cols);
			
			write(20, &testC, sizeof(testC));
			
			if(i==1 && memcmp(c1mem, mem, rows*cols)==0)
				cerr<<"Test 5 successful";
			else
				cerr<"Test 5 failed";
			
			if(i==2 && memcmp(c1mem, mem, rows*cols)==0)
				cerr<<"Test 2 successful";
			else
				cerr<"Test 2 failed";
			
			i++;
		}while(testC!='\0');
		
		close(fnum1[READ_END]);
	}
	
	else
	{
		if((c2PID=fork())==-1)	return 1;
		if(c2PID==0)
		{
			execlp("./p_goldchase", "./p_goldchase", NULL);

			read(21, &rows, sizeof(rows));
			read(21, &cols, sizeof(cols));
					
			char m[rows*cols];
			char *mem=m;
			c2mem=mem;
			read(21, mem, rows*cols);
			
			dup2(fnum2[READ_END],21);
		
			read(21, &numCallsToPostNotice, sizeof(rows));
			read(21, &numCallsToDrawMap, sizeof(cols));
			read(21, mem, rows*cols);

			write(20, &testC, sizeof(testC));
			
			if(memcmp(c1mem, mem, rows*cols)==0)
				cerr<<"Test 1 successful";
			else
				cerr<<"Test 1 failed";
		
			close(fnum2[READ_END]);
		}
		
		else
		{
			dup2(fnum1[WRITE_END],20);
			dup2(fnum2[WRITE_END],20);
			
			close(fnum1[WRITE_END]);
			close(fnum2[WRITE_END]);
			
			if((c1PID=waitpid(-1,&stat,WNOHANG|WUNTRACED))==-1||(c2PID=waitpid(-1,&stat,WNOHANG|WUNTRACED))==-1)
				wait(0);
			
			close(20);
			close(21);
			
			unlink(gcfifo);
		}
	}
	
	return 0;
}
