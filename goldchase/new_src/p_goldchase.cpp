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

#include "goldchase.h"
#include "Map.h"

#define MAPSIZE (sizeof(int))


//Standard namespace convention
using namespace std;


//Main
int main(int argc, char** argv)
{
	sem_t* p_sem;	//semaphore for player/process count
	int sem_val;

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
	
		
	//Initializing player number
	int pl=0;
	if(argv[1])
		pl=atoi(argv[1])-1;
	
	//Reading map file
	ifstream in("mymap.txt");
	string cont((istreambuf_iterator<char>(in)),istreambuf_iterator<char>());
	in.close();
    
    //Reading number of gold drops on map
    char fc=cont.c_str()[0];
    int gc=fc-'0';
    int tot=5+gc;
    
    
    int c=0,sc=0,ccount=0;
    char m[strlen(cont.c_str())];
    
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
	char map[ccount+1];
	
	//Shared Memory
	int p_shm;
	int* p_map;
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
		result = lseek(p_shm, (tot*MAPSIZE)-1, SEEK_SET);
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
		p_map = (int*)mmap(0,(tot*MAPSIZE),PROT_READ|PROT_WRITE,MAP_SHARED,p_shm,0);
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
	}
	else
	{
		p_shm=shm_open("/gc_shm",O_RDWR|O_CREAT,S_IRUSR|S_IWUSR);
		p_map = (int*)mmap(0,(tot*MAPSIZE),PROT_READ|PROT_WRITE,MAP_SHARED,p_shm,0);
		if (map == MAP_FAILED)
		{
			close(p_shm);
			cerr << "Critical fault!" << endl;
			exit(1);
		}
	}
	
		const char* px1=m;
		int ncntr=0,flag=0,dc=1;
		
		//Random drop placement generator
		while(*px1!='\0')
		{
			if(*px1==' ')
			{
				for(int i=0;i<tot;i++)
				{
					if(p_map[i]==ncntr)
						flag=1;
				}
			
				if(flag==1)
				{
					if(dc==1 && pl==0)
							m[ncntr]='1';
							
					else if(dc==2 && pl==1)
							m[ncntr]='2';
							
					else if(dc==3 && pl==2)
							m[ncntr]='3';
							
					else if(dc==4 && pl==3)
							m[ncntr]='4';
							
					else if(dc==5 && pl==4)
							m[ncntr]='5';
							
					else if(dc>5 && dc<tot)
						m[ncntr]='F';
						
					else if(dc==tot)
						m[ncntr]='G';
						
					
					dc++;
					flag=0;
				}
			
			}
		
			++ncntr;
			++px1;
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
	
	
	int a=0;
	//Load map
	Map goldMine(map,26,80);
	//goldMine.postNotice("Game Start");
	do
	{
		a=goldMine.getKey();
		
		if(a=='h' && map[p_map[pl]-1]!=G_WALL)
		{
			if(map[p_map[pl]-1] & G_FOOL)
				goldMine.postNotice("Fool's Gold Found!");
			else if(map[p_map[pl]-1] & G_GOLD)
				goldMine.postNotice("You Won! Exit map to finish..");
			else if(map[p_map[pl]-1] & G_ANYP)
				continue;
			
			map[p_map[pl]]=0;
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
				goldMine.postNotice("You Won! Exit map to finish..");
			else if(map[p_map[pl]+80] & G_ANYP)
				continue;
			
			map[p_map[pl]]=0;
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
				goldMine.postNotice("You Won! Exit map to finish..");
			else if(map[p_map[pl]-80] & G_ANYP)
				continue;
				
			map[p_map[pl]]=0;
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
				goldMine.postNotice("You Won! Exit map to finish..");
			else if(map[p_map[pl]+1] & G_ANYP)
				continue;
			
			map[p_map[pl]]=0;
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

		goldMine.drawMap();
		
	}while(a!='Q');
	
	
	sem_post(p_sem);
	sem_getvalue(p_sem,&sem_val);
	if(sem_val>=5)
	{
		sem_close(p_sem);
		sem_unlink("/gc_sem");
		
		close(p_shm);
		shm_unlink("/gc_shm");
	}
	
	return 0;
}
