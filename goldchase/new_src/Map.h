#ifndef MAP_H
#define MAP_H

#include<ncurses.h>
#include<panel.h>
#include <unistd.h>

/////
// The Map class uses the Screen class to paint a map
/////
class Map {
  public:
    Map(const char* mapmem, int r, int c);
    void drawMap();
    void postNotice(const char* msg);
    int getKey();
  private:
    char operator()(int y, int x);
    const char* mem;
    int rows;
    int cols;
    
    int numCallsToPostNotice;
    int numCallsToDrawMap;
};

#endif //MAP_H
