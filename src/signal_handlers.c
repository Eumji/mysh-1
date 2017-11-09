#include "signal_handlers.h"
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>
#include <stdio.h>

void catch_sigint(int signalNo)
{
  // char *s;
  //s=strsignal(signalNo);
  //signal(SIGINT, SIG_IGN);
 // char buf[4];
 // fgets(buf, sizeof(char)*4, stdin);
  signal(signalNo, SIG_IGN);
 return;
}

void catch_sigtstp(int signalNo)
{
  //char *s;
  //s=strsignal(signalNo);
  //signal(SIGINT, SIG_IGN);
  //char buf[4];
  //fgets(buf, sizeof(char)*4, stdin);
  signal(signalNo, SIG_IGN);
  return;
}
