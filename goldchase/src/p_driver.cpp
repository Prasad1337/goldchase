#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ncurses.h>

using namespace std;


int main(int argc, char *argv[])
{
	int pid=-1;
	int stat=-1;
	
	pid=fork();
	
	if(pid==0)
	{
		execv("p_goldchase",argv);
		
	}
	
	else
		waitpid(-1,&stat,WNOHANG|WUNTRACED);
	
	return 0;
}
