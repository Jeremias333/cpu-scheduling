#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "unistd.h"

typedef struct {
    int id; // Irá individualizar as tasks
    char *name; // nome da task
    int period; // de quanta em quantas unidades de tempo irá iniciar a task
    int cpu_burst; // tempo de execução necessário
    char state; // estado atual (P, F, H, K)
    int rest_burst; // tempo restante para terminar a execução
}task;

int size_tasks = 10; // lembrar de criar esta variavel e baseado na quantidade de tasks criar um vetor de tasks

int main(){
    printf("Executando...\n");
    task array_tasks[size_tasks];
    for (int i = 0; i < size_tasks; i++){
        array_tasks[i].id = i;
        array_tasks[i].name = "task";
        array_tasks[i].period = 10;
        array_tasks[i].cpu_burst = 10;
        array_tasks[i].state = 'P';
        array_tasks[i].rest_burst = 10;
    }
    task t1;
    t1.id = 1;
    t1.name = "tarefa1";
    t1.period = 10;
    t1.cpu_burst = 5;
    t1.state = 'P';
    t1.rest_burst = 5;

    array_tasks[0] = t1;

    printf("id: %d \n", array_tasks[0].id);
    printf("name: %s \n", array_tasks[0].name);
    printf("period: %d \n", array_tasks[0].period);
    printf("cpu_burst: %d \n", array_tasks[0].cpu_burst);
    printf("state: %c \n", array_tasks[0].state);
    printf("rest_burst: %d \n", array_tasks[0].rest_burst);
}