//Header files included
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <string.h>
#include <csignal>
#include <mqueue.h>

#include <ncurses.h>
#include <panel.h>
#include <cstdlib>
#include <cstring>
#include <utility> //for std::pair
#include <stdexcept>

#include "goldchase.h"
#include "Map.h"
#include "Screen.h"


//Macros
#define MAPSIZE (sizeof(int))


//Standard namespace convention
using namespace std;


//Function Prototypes
void termHandler(pid_t);	//Signal Handler
void syncUp(pid_t);	//SIGUSR1 Handler [for map sync]
void sigWinner(pid_t);	//Signal
void bcastHandler(pid_t);

void sync();
void clearGold(int);
void postWinner(pid_t);
void broadcastMsg(pid_t);


//Global Variables
sem_t* p_sem;	//semaphore for player/process count
int sem_val;	//store semaphore value
int p_shm;	//shared memory
int pl=0;	//Player number [Player #1 by default]
char map[2080];	//map grid
int mdump[11];	//local map placeholder
int *p_map;	//towards mmap usage
char m[2080];
Map goldMine(map,26,80);


//Reused code [hackish]
std::pair<int,int> _getScreenSize()
{
  int x,y;
  getmaxyx(stdscr,y,x);
  return std::pair<int,int>(y,x);
}


//Main
int main(int argc, char** argv)
{
	pid_t plid;

	//Signals
	signal(SIGTERM,termHandler);
	signal(SIGINT,termHandler);
	signal(SIGHUP,termHandler);
	signal(SIGTSTP,termHandler);

	
	//Semaphore
	p_sem=sem_open("/gc_sem",O_RDWR,5);
	if(p_sem==SEM_FAILED)
	{
		p_sem=sem_open("/gc_sem",O_RDWR|O_CREAT|O_EXCL,S_IRUSR|S_IWUSR|S_IWGRP|S_IRGRP|S_IROTH|S_IWOTH,5);
		if(p_sem==SEM_FAILED)
		{
			cerr << "Critical fault!" << endl;
			exit(1);
		}
	}
	else
		p_sem=sem_open("/gc_sem",O_RDWR|O_CREAT,S_IRUSR|S_IWUSR,5);
		
	sem_getvalue(p_sem,&sem_val);
	if(sem_val>0)
		sem_wait(p_sem);
	else
	{
		cerr<<"Too many players! Exiting.."<<endl;
		exit(1);
	}
	

	//Reading map file
	ifstream in("mymap.txt");
	string cont((istreambuf_iterator<char>(in)),istreambuf_iterator<char>());
	in.close();
    
    //Reading number of gold drops on map
    char fc=cont.c_str()[0];
    int gc=fc-'0';
    int tot=5+gc;
    int maptot=tot+6;
    
    
    int c=0,sc=0,ccount=0;
    
    const char* p=cont.c_str();
    
    //Saving map without first line and line breaks
    while(*p!='\0')
    {
		if(c>1 && *p!='\n')
		{
			m[ccount]=*p;
			ccount++;
		}
		else
			c++;
		
		if(*p==' ')
			sc++;
		
		++p;
	}
	
	const char* px=m;

	//Shared Memory

	/* p_map reference
		0-4 players 1-5
		5-9 fool's gold
		10 real gold
		11-15 process IDs
		16 winner PID
	*/

	int result;
	p_shm=shm_open("/gc_shm", O_RDWR,S_IRUSR|S_IWUSR);
	if(p_shm==-1)
	{
		p_shm=shm_open("/gc_shm", O_RDWR|O_CREAT|O_EXCL, S_IRUSR|S_IWUSR|S_IWGRP|S_IRGRP|S_IROTH|S_IWOTH);
		if(p_shm==-1)
		{
			cerr << "Critical fault!" << endl;
			exit(1);
		}
		
		//Initializing shared memory
		result = lseek(p_shm, (maptot*MAPSIZE)-1, SEEK_SET);
		if (result == -1) {
			close(p_shm);
			cerr << "Critical fault!" << endl;
			exit(1);
		}
		result = write(p_shm, "", 1);
		if (result != 1) {
			close(p_shm);
			cerr << "Critical fault!" << endl;
			exit(1);
		}
		p_map = (int*)mmap(0,(maptot*MAPSIZE),PROT_READ|PROT_WRITE,MAP_SHARED,p_shm,0);
		if (p_map == MAP_FAILED) {
			close(p_shm);
			cerr << "Critical fault!" << endl;
			exit(1);
		}
		
		//Random number generator
		srand(time(NULL));
		int tmpA[tot];
		for(int i=0;i<tot;i++)
		{
			//For available number of spaces
			tmpA[i]=rand()%sc;
		}
		
		int ncntr=0,scntr=0,flag=0,dc=1;
		
		//Random drop placement generator
		while(*px!='\0')
		{
			if(*px==' ')
			{
				for(int i=0;i<tot;i++)
				{
					if(tmpA[i]==scntr)
						flag=1;
				}
			
				if(flag==1)
				{
					if(dc==1)
						p_map[0]=ncntr;

					else if(dc==2)
						p_map[1]=ncntr;

					else if(dc==3)
						p_map[2]=ncntr;
						
					else if(dc==4)
						p_map[3]=ncntr;
						
					else if(dc==5)
						p_map[4]=ncntr;
						
					else if(dc>5 && dc<tot)
						p_map[dc-1]=ncntr;
						
					else if(dc==tot)
						p_map[dc-1]=ncntr;
						
					dc++;
					flag=0;
				}
			
				++scntr;
			}
		
			++ncntr;
			++px;
		}

		for(int i=11;i<=15;i++)
		{
			p_map[i]=0;
		}

	}

	else
	{
		p_shm=shm_open("/gc_shm",O_RDWR|O_CREAT,S_IRUSR|S_IWUSR);
		p_map = (int*)mmap(0,(maptot*MAPSIZE),PROT_READ|PROT_WRITE,MAP_SHARED,p_shm,0);
		if (map == MAP_FAILED)
		{
			close(p_shm);
			cerr << "Critical fault!" << endl;
			exit(1);
		}
	}

	//Initializing player number
	if(argv[1])
	{
		pl=atoi(argv[1])-1;

		if(pl>=5)
		{
			cerr<<"Maximum Player #: 5\nPlease try again!..."<<endl;
			sem_post(p_sem);
			exit(1);
		}
	}

	if(p_map[11+pl]>0)
	{
		cerr<<"Player already exists!\nPlease try again!..."<<endl;
		sem_post(p_sem);
		exit(1);
	}

	
	//Storing PID in SHM
	plid=(int)getpid();
	p_map[11+pl]=plid;
	

	const char* px1=m;
	int ncntr=0,flag=0,dc=1;
	
	//Random drop placement generator
	for(int i=0;i<tot;i++)
	{
		if(i==0 && pl==0)
		{
			m[p_map[i]]='1';
		}
		else if(i==1 && pl==1)
		{
			m[p_map[i]]='2';
		}
		else if(i==2 && pl==2)
		{
			m[p_map[i]]='3';
		}
		else if(i==3 && pl==3)
		{
			m[p_map[i]]='4';
		}
		else if(i==4 && pl==4)
		{
			m[p_map[i]]='5';
		}
		else if(i>4 && i<(tot-1) && p_map[i]!=0)
		{
			m[p_map[i]]='F';
		}
		else if(i==(tot-1) && p_map[i]!=0)
		{
			m[p_map[i]]='G';
		}
	}

	
	const char* ptr=m;
	char* mp=map;

	//Convert the ASCII bytes into bit fields drawn from goldchase.h
	while(*ptr!='\0')
	{
		switch(*ptr)
		{
			case ' ':	*mp=0;
						break;
			case '*':	*mp=G_WALL;
						break;
			case '1':	*mp=G_PLR0;
						break;
			case '2':	*mp=G_PLR1;
						break;
			case '3':	*mp=G_PLR2;
						break;
			case '4':	*mp=G_PLR3;
						break;
			case '5':	*mp=G_PLR4;
						break;
			case 'G':	*mp=G_GOLD;
						break;
			case 'F':	*mp=G_FOOL;
						break;
		}
	
		++ptr;
		++mp;
	}
	

	//p_map to local cache
	for(int i=0;i<=10;i++)
	{
		mdump[i]=p_map[i];
	}


	int a=0;	//input character
	int win=0;

	//Load map
	sync();
	//goldMine.postNotice("Game Start");
	do
	{
		sync();

		a=goldMine.getKey();
		
		if(a=='h' && map[p_map[pl]-1]!=G_WALL)
		{
			if(map[p_map[pl]-1] & G_FOOL)
				goldMine.postNotice("Fool's Gold Found!");
			else if(map[p_map[pl]-1] & G_GOLD)
			{
				win=1;
				goldMine.postNotice("You Won! Exit map to finish..");
			}
			else if(map[p_map[pl]-1] & G_ANYP)
				continue;
			
			map[p_map[pl]]=0x00;
			mdump[pl]=0x00;
			clearGold(p_map[pl]);
			--p_map[pl];
			
			switch(pl)
			{
				case 0:	map[p_map[pl]]=G_PLR0;
						break;
				case 1:	map[p_map[pl]]=G_PLR1;
						break;
				case 2:	map[p_map[pl]]=G_PLR2;
						break;
				case 3:	map[p_map[pl]]=G_PLR3;
						break;
				case 4:	map[p_map[pl]]=G_PLR4;
						break;
			}
		}
		
		else if(a=='j' && map[p_map[pl]+80]!=G_WALL)
		{
			if(map[p_map[pl]+80] & G_FOOL)
				goldMine.postNotice("Fool's Gold Found!");
			else if(map[p_map[pl]+80] & G_GOLD)
			{
				win=1;
				goldMine.postNotice("You Won! Exit map to finish..");

			}
			else if(map[p_map[pl]+80] & G_ANYP)
				continue;
			
			map[p_map[pl]]=0x00;
			mdump[pl]=0x00;
			clearGold(p_map[pl]);
			p_map[pl]+=80;

			switch(pl)
			{
				case 0:	map[p_map[pl]]=G_PLR0;
						break;
				case 1:	map[p_map[pl]]=G_PLR1;
						break;
				case 2:	map[p_map[pl]]=G_PLR2;
						break;
				case 3:	map[p_map[pl]]=G_PLR3;
						break;
				case 4:	map[p_map[pl]]=G_PLR4;
						break;
			}
		}
		
		else if(a=='k' && map[p_map[pl]-80]!=G_WALL)
		{
			if(map[p_map[pl]-80] & G_FOOL)
				goldMine.postNotice("Fool's Gold Found!");
			else if(map[p_map[pl]-80] & G_GOLD)
			{
				win=1;
				goldMine.postNotice("You Won! Exit map to finish..");
			}
			else if(map[p_map[pl]-80] & G_ANYP)
				continue;
				
			map[p_map[pl]]=0x00;
			mdump[pl]=0x00;
			clearGold(p_map[pl]);
			p_map[pl]-=80;

			switch(pl)
			{
				case 0:	map[p_map[pl]]=G_PLR0;
						break;
				case 1:	map[p_map[pl]]=G_PLR1;
						break;
				case 2:	map[p_map[pl]]=G_PLR2;
						break;
				case 3:	map[p_map[pl]]=G_PLR3;
						break;
				case 4:	map[p_map[pl]]=G_PLR4;
						break;
			}
		}
		
		else if(a=='l' && map[p_map[pl]+1]!=G_WALL)
		{
			if(map[p_map[pl]+1] & G_FOOL)
				goldMine.postNotice("Fool's Gold Found!");
			else if(map[p_map[pl]+1] & G_GOLD)
			{
				win=1;
				goldMine.postNotice("You Won! Exit map to finish..");
			}
			else if(map[p_map[pl]+1] & G_ANYP)
				continue;
		
			map[p_map[pl]]=0x00;
			mdump[pl]=0x00;
			clearGold(p_map[pl]);
			++p_map[pl];

			switch(pl)
			{
				case 0:	map[p_map[pl]]=G_PLR0;
						break;
				case 1:	map[p_map[pl]]=G_PLR1;
						break;
				case 2:	map[p_map[pl]]=G_PLR2;
						break;
				case 3:	map[p_map[pl]]=G_PLR3;
						break;
				case 4:	map[p_map[pl]]=G_PLR4;
						break;
			}
		}

		else if(a=='b')
		{
			int screenHeight,screenWidth;
			std::pair<int,int> maxes=_getScreenSize();
			screenHeight=maxes.first;
			screenWidth=maxes.second;

			char *myNote;
			const char* inst="Enter message:";
			const char* dismiss="Press enter to send";
			int greater=strlen(inst)>strlen(dismiss) ? strlen(inst) : strlen(dismiss);
			int ycoord= screenHeight>1 ? screenHeight/2-2 : 0;
			int xcoord=screenWidth>greater ? screenWidth/2-greater/2-1 : 0;
			WINDOW* dialog=newwin(5,greater+4,ycoord,xcoord);
			PANEL* dialog_panel=new_panel(dialog);
			box(dialog,0,0);

			mvwprintw(dialog,1,1+(greater-strlen(inst))/2,inst);
			wscanw(dialog,myNote);
			mvwprintw(dialog,3,1+(greater-strlen(dismiss))/2,dismiss);
			goldMine.drawMap();
			do;
			while(getch()!='\n');
			del_panel(dialog_panel);
			delwin(dialog);
			goldMine.drawMap();

			goldMine.postNotice(myNote);
			goldMine.drawMap();
		}

		if(win==1)
		{
			p_map[16]=plid;
			postWinner(plid);
			win=0;
		}

		sync();

	}while(a!='Q');
	
	
	p_map[11+pl]=0;
	sem_post(p_sem);
	sem_getvalue(p_sem,&sem_val);
	
	//Last user out
	if(sem_val>=5)
	{
		sem_close(p_sem);
		sem_unlink("/gc_sem");
		
		close(p_shm);
		shm_unlink("/gc_shm");
	}
	
	return 0;

}


//Signal Handler
void termHandler(pid_t signum)
{
	endwin();	//Destroy the map

	p_map[11+pl]=0;

	sem_post(p_sem);
	sem_getvalue(p_sem,&sem_val);

	//Last user out
	if(sem_val>=5)
	{
		fflush(stdout);

		sem_close(p_sem);
		sem_unlink("/gc_sem");
		
		close(p_shm);
		shm_unlink("/gc_shm");
	}

	exit(0);
}


//Function to start sync
void sync()
{	
	signal(SIGUSR1,syncUp);	//signal to sync up

	for(int i=11;i<=15;i++)
	{
		if(p_map[i]>0)	//signal all processes
			kill(p_map[i],SIGUSR1);
	}
}


//Sync signal (SIGUSR1) handler
void syncUp(pid_t signum)
{
	for(int i=0;i<=10;i++)
	{
		map[mdump[i]]=0x00;

		if(i==0 && p_map[11]>0)
			map[p_map[i]]=G_PLR0;
		else if(i==1 && p_map[12]>0)
			map[p_map[i]]=G_PLR1;
		else if(i==2 && p_map[13]>0)
			map[p_map[i]]=G_PLR2;
		else if(i==3 && p_map[14]>0)
			map[p_map[i]]=G_PLR3;
		else if(i==4 && p_map[15]>0)
			map[p_map[i]]=G_PLR4;
		else if(i>=5 && i<=9 && p_map[i]!=0x00)
			map[p_map[i]]=G_FOOL;
		else if(i==10 && p_map[i]!=0x00)
			map[p_map[i]]=G_GOLD;

		mdump[i]=p_map[i];
	}

	goldMine.drawMap();
}


//Function to post winner notice
void postWinner(pid_t signum)
{
	signal(SIGUSR1,sigWinner);

	for(int i=11;i<=15;i++)
	{
		if(p_map[i]!=signum && p_map[i]>0)	//signal all except calling process
			kill(p_map[i],SIGUSR1);
	}
}


//Winner notice signal handler
void sigWinner(pid_t signum)
{
	if(p_map[16]==p_map[11])
		goldMine.postNotice("Player 1 has won!");
	else if(p_map[16]==p_map[12])
		goldMine.postNotice("Player 2 has won!");
	else if(p_map[16]==p_map[13])
		goldMine.postNotice("Player 3 has won!");
	else if(p_map[16]==p_map[14])
		goldMine.postNotice("Player 4 has won!");
	else if(p_map[16]==p_map[15])
		goldMine.postNotice("Player 5 has won!");
}


//Function to broadcast message to players
void broadcastMsg(pid_t signum)
{
	signal(SIGUSR2,bcastHandler);

	for(int i=11;i<=15;i++)
	{
		if(p_map[i]!=signum && p_map[i]>0)	//signal all except calling process
			kill(p_map[i],SIGUSR2);
	}
}


//Message broadcast signal (SIGUSR2) handler
void bcastHandler(pid_t signum)
{
	
	goldMine.drawMap();
}


//Function to clear gold from SHM on pickup
void clearGold(int loc)
{
	for(int i=5;i<=10;i++)
	{
		if(p_map[i]==loc)
		{
			p_map[i]=0x00;
			break;
		}
	}
}
