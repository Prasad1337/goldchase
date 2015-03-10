#include<ncurses.h>
#include<panel.h>
#include<cstdlib>
#include<cstring>
#include<utility> //for std::pair
#include<iostream>
#include<stdexcept>
#include <unistd.h>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>

#include"goldchase.h"
#include"Map.h"


//Initialize the object and draw the map
Map::Map(const char* mmem, int r, int c) 
  : rows(r), cols(c)
{
	const char* mymap;
	const char* gcfifo="gcfifo";
	int fd=open(gcfifo,O_RDWR);
	read(fd,&mymap,sizeof(mymap));
	close(fd);
	
	mem=mymap;
	
	numCallsToPostNotice=0;
	numCallsToDrawMap=0;
	
	drawMap();
	write(21, &rows, sizeof(rows));
	write(21, &cols, sizeof(cols));
	write(21, mem, rows*cols);
}

int Map::getKey()
{
	write(21, &numCallsToPostNotice, sizeof(rows));
	write(21, &numCallsToDrawMap, sizeof(cols));
	write(21, mem, rows*cols);
	
	char ret;
	
	read(20,&ret,sizeof(ret));
	
	return ret;
}
void Map::postNotice(const char* msg)
{
  ++numCallsToPostNotice;
  
}

//Calculate offset into memory array
char Map::operator()(int y, int x)
{
  if(y<0 || y>=rows)
    throw std::out_of_range("Y coordinate out of range");
  if(x<0 || x>=cols)
    throw std::out_of_range("X Coordinate out of range");
  return *(mem+y*cols+x);
}

//Draw and refresh map from memory array
void Map::drawMap()
{
  Map& mymap=*this; //ease referencing ourself
  ++numCallsToDrawMap;
}
