#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "unistd.h"

int size_tasks = 0;
FILE *arq_address;

int main(){
    printf("Executando...\n");

    arq_address = fopen("ratea.txt", "r");
    
    

}