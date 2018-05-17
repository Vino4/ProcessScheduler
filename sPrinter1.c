#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>

int main(int argc, char *argv[]){
	char *end;
	for(end = argv[1]; *end != '\0'; end++);
    int counter = strtol(argv[1], &end, 10);
    	//printf("|1|Count-Down: %s\n", argv[1]);

    while (counter >= 0){
    	//printf("|1|%d\n", counter--);
    	counter--;
    	sleep(1);
    }
    return 0;
}
