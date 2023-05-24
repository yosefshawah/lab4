#include <stdlib.h>
#include <stdio.h>


int numbersCounter(char* input){
    int cnt = 0;
    while(*input != '\0'){
        if(*input >= '0' && *input <= '9'){
            cnt++;
        }
        input++;
    }
    return cnt;
}



int main(int argc, char** argv){
    if(argc == 2)
        printf("Count = %d\n", numbersCounter(argv[1]));
}