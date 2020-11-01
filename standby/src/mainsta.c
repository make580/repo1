#include<stdio.h>
#include<stdlib.h>
#include"stan_state.h"
#include"standby.h"
#include<unistd.h>
#include<errno.h>
#include<string.h>

int main(int arg,char **avg)
{
	mainStandby(arg,avg);
	myclose();
	return 0;
}
