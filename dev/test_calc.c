#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "unistd.h"

typedef struct {
    int id; // Irá individualizar as tasks
    char *name; // nome da task
    int period; // de quanta em quantas unidades de tempo irá iniciar a task
    int cpu_burst; // tempo de execução necessário
    char state; // estado atual (P, C, Q, F, H, L, K)
                // P = Processing
                // C = Created
                // Q = Queued
                // F = Finished
                // H = Hold
                // L = Lost
                // K = Killed
    int rest_burst; // tempo restante para terminar a execução
    int next_spawn;
}task;

int size_tasks = 4; // lembrar de criar esta variavel e baseado na quantidade de tasks criar um vetor de tasks

int who_is_processing(task *array_tasks, int size_tasks);
void update_rest_burst(task *array_tasks, int size_tasks, int act_task_id);
void should_process_now(task *array_tasks, int size_tasks, int act_time);
void should_change_state(task *array_tasks, int size_tasks, int act_time, int time_total, int act_id);
void populate_queue_by_priority(task *array_tasks, int *task_queue_id, int size_tasks);
void update_next_spawn(task *array_tasks, int size_tasks);


int main(){
    task array_tasks[size_tasks];
    int task_queue_id[size_tasks];

    int time_total = 165;
    
    task t1;
    t1.id = 1;
    t1.name = "T1";
    t1.period = 25;
    t1.cpu_burst = 30;
    t1.state = 'C';
    t1.rest_burst = 30;
    t1.next_spawn = 25;

    task t2;
    t2.id = 2;
    t2.name = "T2";
    t2.period = 80;
    t2.cpu_burst = 120;
    t2.state = 'C';
    t2.rest_burst = 120;
    t2.next_spawn = 80;

    task t3;
    t3.id = 3;
    t3.name = "T3";
    t3.period = 10;
    t3.cpu_burst = 165;
    t3.state = 'C';
    t3.rest_burst = 165;
    t3.next_spawn = 10;

    array_tasks[1] = t1;
    array_tasks[2] = t2;
    array_tasks[3] = t3;

    // printf("array_tasks[1].id = %d", array_tasks[1].id);

    for(int i = 1; i < size_tasks; i++){
        printf("\n");
        // printf("iteracao: %d\n", i);
        printf("id: %d \n", array_tasks[i].id);
        printf("name: %s \n", array_tasks[i].name);
        printf("period: %d \n", array_tasks[i].period);
        printf("cpu_burst: %d \n", array_tasks[i].cpu_burst);
        printf("state: %c \n", array_tasks[i].state);
        printf("rest_burst: %d \n", array_tasks[i].rest_burst);
        printf("next_spawn: %d \n", array_tasks[i].next_spawn);
    }

    //Inicializando a fila de prioridades
    populate_queue_by_priority(array_tasks, task_queue_id, size_tasks);

    //Area count time struct
    int p;
    int count_time = 0;
    
    //Inicializando a primeira task
    array_tasks[1].state = 'P';

    //Contador de tempo
    for(int i = 0; i < time_total; i++){
        //Verificando qual task está em execução nesta unidade de tempo
        p = who_is_processing(array_tasks, size_tasks);

        //Atualizando o rest_burst de cada task
        update_rest_burst(array_tasks, size_tasks, p);
        // printf("Processing: %d \n", p);
        printf("Rest Burst: %d \n", array_tasks[p].rest_burst);

        count_time++;
    }



}

//Revisado
int who_is_processing(task *array_tasks, int size_tasks){
    //Irá retornar o id da task que está sendo processada
    int id_processing = 0;
    for(int i = 1; i < size_tasks; i++){
        if(array_tasks[i].state == 'P'){
            id_processing = i;
            return id_processing;
        }
    }
    return id_processing;//em caso de retorno 0 nenhuma delas estão em execução.
}

//Refazer! --------------------------------------------------------------
void should_process_now(task *array_tasks, int size_tasks, int act_time){
    //Irá verificar se a task deve ser processada agora
    for(int i = 0; i < size_tasks; i++){
        if(array_tasks[i].period == 0){
            array_tasks[i].state = 'P';
        }
    }
}

//Continuar desenvolvendo esta função ------------------------------------
void should_change_state(task *array_tasks, int size_tasks, int act_time, int time_total, int act_id){
    //Irá verificar se a task deve mudar de estado
    if(array_tasks[act_id].rest_burst == 0){
        array_tasks[act_id].state = 'F';
        array_tasks[act_id].rest_burst = array_tasks[act_id].cpu_burst;
    }else if(array_tasks[act_id].rest_burst > 0 && act_time == time_total){
        array_tasks[act_id].state = 'K';
        array_tasks[act_id].rest_burst = array_tasks[act_id].cpu_burst;
        array_tasks[act_id].next_spawn = array_tasks[act_id].period;
    }else if(array_tasks[act_id].rest_burst > 0 && act_time < time_total){
        array_tasks[act_id].state = 'H';
    }else if(array_tasks[act_id].rest_burst > 0 && array_tasks[act_id].next_spawn == 0){
        array_tasks[act_id].state = 'L';
        array_tasks[act_id].rest_burst = array_tasks[act_id].cpu_burst;
    }else{
        printf("Erro ao mudar estado da task");
    }
}

//Revisado
void update_rest_burst(task *array_tasks, int size_tasks, int act_task_id){
    //Irá atualizar o tempo restante para a task finalizar.
    //Para esse caso estamos diminuindo.
    array_tasks[act_task_id].rest_burst--;
}

//Revisado
void update_next_spawn(task *array_tasks, int size_tasks){
    //Irá atualizar o tempo restante para a task reiniciar.
    //Para esse caso estamos diminuindo baseado no periodo
    //Tanto para quem está processando como para quem está na espera.
    for(int i = 1; i < size_tasks; i++){
        if(array_tasks[i].state == 'P' || array_tasks[i].state == 'H'){
            array_tasks[i].next_spawn--;
        }
    }
}


//Revisado
void populate_queue_by_priority(task *array_tasks, int *task_queue_id, int size_tasks){
    //Irá popular a fila de prioridades
    
    int array_tasks_queue_temp[size_tasks][2];
    int aux_id = 0;
    int aux_priority = 0;
    
    //Preenchendo a matriz com os valores de id e periodo
    for(int i = 1; i < size_tasks; i++){
        array_tasks_queue_temp[i][0] = array_tasks[i].id;
        array_tasks_queue_temp[i][1] = array_tasks[i].period;
    }

    //Ordenando a matriz
    for(int i = 1; i < size_tasks; i++){
        for(int j = 1; j < size_tasks; j++){
            if(array_tasks_queue_temp[i][1] < array_tasks_queue_temp[j][1]){
                aux_id = array_tasks_queue_temp[i][0];
                aux_priority = array_tasks_queue_temp[i][1];
                array_tasks_queue_temp[i][0] = array_tasks_queue_temp[j][0];
                array_tasks_queue_temp[i][1] = array_tasks_queue_temp[j][1];
                array_tasks_queue_temp[j][0] = aux_id;
                array_tasks_queue_temp[j][1] = aux_priority;
            }
        }
    }

    //Preenchendo a fila de prioridades
    for(int i = 1; i < size_tasks; i++){
        // printf("id: %d \n", array_tasks_queue_temp[i][0]);
        // printf("period: %d \n", array_tasks_queue_temp[i][1]);
        task_queue_id[i] = array_tasks_queue_temp[i][0];
    }

}