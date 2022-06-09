#include "lib.h"


void umain()
{
    int fdnum;
    char buf[512];
	writef("before list dir\n");    
	list_dir("/", buf);
    writef("dir of / : %s\n", buf);
    fdnum = open("/created_file", O_RDWR|O_CREAT);
    close(fdnum);
    list_dir("/", buf);
    writef("dir of / : %s\n", buf);
    while(1);
}

