#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "unistd.h"

typedef struct {
    int id; // Irá individualizar as tasks
    char *name; // nome da task
    int period; // de quanta em quantas unidades de tempo irá iniciar a task
    int cpu_burst; // tempo de execução necessário
    char state; // estado atual (P, F, H, K, L)
    int rest_burst; // tempo restante para terminar a execução
}task;

int size_tasks = 2; // lembrar de criar esta variavel e baseado na quantidade de tasks criar um vetor de tasks

int who_is_processing(task *array_tasks, int size_tasks);

int main(){
    task array_tasks[size_tasks];
    int time_total = 165;
    
    task t1;
    t1.id = 0;
    t1.name = "T1";
    t1.period = 25;
    t1.cpu_burst = 30;
    t1.state = 'P';
    t1.rest_burst = 30;

    task t2;
    t2.id = 1;
    t2.name = "T2";
    t2.period = 80;
    t2.cpu_burst = 120;
    t2.state = 'D';
    t2.rest_burst = 120;

    array_tasks[0] = t1;
    array_tasks[1] = t2;

    for(int i = 0; i < 2; i++){
        printf("\n");
        printf("id: %d \n", array_tasks[i].id);
        printf("name: %s \n", array_tasks[i].name);
        printf("period: %d \n", array_tasks[i].period);
        printf("cpu_burst: %d \n", array_tasks[i].cpu_burst);
        printf("state: %c \n", array_tasks[i].state);
        printf("rest_burst: %d \n", array_tasks[i].rest_burst);
    }

    //Area count time struct
    int p;

    for(int i = 0; i < time_total; i++){
        p = who_is_processing(array_tasks, size_tasks);
        printf("Processing: %d \n", p);
    }



}

int who_is_processing(task *array_tasks, int size_tasks){
    //Irá retornar o id da task que está sendo processada
    int id_processing = 0;
    for(int i = 0; i < size_tasks; i++){
        if(array_tasks[i].state == 'P'){
            id_processing = i;
            return id_processing;
        }
    }
    return id_processing;
}

void should_process_now(task *array_tasks, int size_tasks, int act_time){
    //Irá verificar se a task deve ser processada agora
    for(int i = 0; i < size_tasks; i++){
        if(array_tasks[i].period == 0){
            array_tasks[i].state = 'P';
        }
    }
}

void should_change_state(task *array_tasks, int size_tasks, int act_time, int time_total, int act_id){
    //Irá verificar se a task deve mudar de estado
    if(act_time == array_tasks[act_id].period){
        array_tasks[act_id].state = 'F';
    }else if(array_tasks[act_id].rest_burst == 0){
        array_tasks[act_id].state = 'H';
    }
}

void update_rest_burst(task *array_tasks, int size_tasks, int act_time, int time_total, int act_id){
    //Irá atualizar o tempo restante para a task reiniciar.
    //Para esse caso estamos diminuindo.
    for(int i = 1; i < size_tasks; i++){
        if(array_tasks[i].rest_burst >= 0){
            array_tasks[i].rest_burst--;
        }
    }
}