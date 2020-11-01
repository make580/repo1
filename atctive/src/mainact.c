#include<stdio.h>
#include"active.h"
#include<string.h>
#include<stdlib.h>
#include<signal.h>

int main(int argc,char **argv)
{
	
	mainactive(argc,argv);
	myclose();
	return 0;
}
