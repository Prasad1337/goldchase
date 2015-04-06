#include <iostream>
#include <signal.h>
#include <unistd.h>

using namespace std;

void my_handler(int signum)
{
    if (signum == SIGUSR1)
    {
        cout<<signum<<"Received SIGUSR1!\n";
    }
}

int main()
{
	signal(SIGUSR1, my_handler);

	sleep(1);
	kill(getpid(),SIGUSR1);
}
