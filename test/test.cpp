#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

pid_t pid;
void bar(int sig) {
printf("wabbit");
kill(pid, SIGUSR1);
}
void baz(int sig) {
printf("tweribble");
}
void foo(int sig) {
printf("waskly");
kill(pid, SIGUSR1);
exit(0);
}
main() {
signal(SIGUSR1, baz);
signal(SIGCHLD, bar);
pid = fork();
if (pid == 0) {
signal(SIGUSR1, foo);
kill(getpid(), SIGUSR1);
for(;;);
}
else {
pid_t p; int status;
if ((p = wait(&status)) > 0) {
printf("that");
}
}
}
