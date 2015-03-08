#include<ncurses.h>
#include<panel.h>
#include<cstdlib>
#include<cstring>
#include<utility> //for std::pair
#include<iostream>
#include<stdexcept>

#include"goldchase.h"
#include"Map.h"



//Initialize the object and draw the map
Map::Map(const char* mmem, int ylength, int xwidth) 
  : mapHeight(ylength), mapWidth(xwidth), mapmem(mmem)
{
  drawMap();
  
}

int Map::getKey()
{
	
  return getch();
}
void Map::postNotice(const char* msg)
{
  
  
}

//Calculate offset into memory array
char Map::operator()(int y, int x)
{
  if(y<0 || y>=mapHeight)
    throw std::out_of_range("Y coordinate out of range");
  if(x<0 || x>=mapWidth)
    throw std::out_of_range("X Coordinate out of range");
  return *(mapmem+y*mapWidth+x);
}

//Draw and refresh map from memory array
void Map::drawMap()
{
  Map& mymap=*this; //ease referencing ourself
  
}
