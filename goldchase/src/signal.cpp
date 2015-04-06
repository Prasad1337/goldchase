#include<iostream>
#include<unistd.h>
#include <signal.h>
using namespace std;

void raspberry(int x)
{
  cerr << "nya!" << endl;
}

int main()
{
//  int sigaction(int signum, const struct sigaction *act,
//                           struct sigaction *oldact);
  struct sigaction my_sig_handler;

  my_sig_handler.sa_handler=raspberry; //handle with this function
  sigemptyset(&my_sig_handler.sa_mask);
  my_sig_handler.sa_flags=0;

  sigaction(SIGINT, &my_sig_handler, NULL); //tell how to handle SIGINT
  cout << getpid() << endl;
  for(int i=0; i<1000; ++i,sleep(2),cout << i << endl)
    ;
}