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

int size_tasks = 3; // lembrar de criar esta variavel e baseado na quantidade de tasks criar um vetor de tasks

int who_is_processing(task *array_tasks, int size_tasks);
void update_rest_burst(task *array_tasks, int size_tasks, int act_task_id);
void next_to_process(task *array_tasks, int size_tasks, int *task_queue_id, int act_task_id);
char should_change_state(task *array_tasks, int *task_queue_id, int size_tasks, int act_time, int time_total, int act_id);
void populate_queue_by_priority(task *array_tasks, int *task_queue_id, int size_tasks);
void update_next_spawn(task *array_tasks, int size_tasks);
int search_for_next_spawn(task *array_tasks, int *task_queue_id, int size_tasks);


int main(){
    task array_tasks[size_tasks];
    int task_queue_id[size_tasks];

    int time_total = 165+1;
    int time_idle = 0;
    
    task t1;
    t1.id = 1;
    t1.name = "T1";
    t1.period = 50;
    t1.cpu_burst = 25;
    t1.state = 'Q';
    t1.rest_burst = 25;
    t1.next_spawn = 50;

    task t2;
    t2.id = 2;
    t2.name = "T2";
    t2.period = 80;
    t2.cpu_burst = 35;
    t2.state = 'Q';
    t2.rest_burst = 35;
    t2.next_spawn = 80;

    // task t3;
    // t3.id = 3;
    // t3.name = "T3";
    // t3.period = 10;
    // t3.cpu_burst = 165;
    // t3.state = 'C';
    // t3.rest_burst = 165;
    // t3.next_spawn = 10;

    array_tasks[1] = t1;
    array_tasks[2] = t2;
    // array_tasks[3] = t3;

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

    printf("\n");

    //Inicializando a fila de prioridades
    populate_queue_by_priority(array_tasks, task_queue_id, size_tasks);

    //Area count time
    int p;
    int n;
    int count_time = 0;
    char state_result = 'C';
    char state_before = 'C';
    
    //Inicializando a primeira task
    array_tasks[task_queue_id[1]].state = 'P';
    int temp_time = 0;
    //Contador de tempo
    for(int i = 1; i < time_total; i++){
        temp_time += 1;
        count_time++;
        //Verificando qual task está em execução nesta unidade de tempo
        p = who_is_processing(array_tasks, size_tasks);

        //Atualizando o rest_burst de cada task
        update_rest_burst(array_tasks, size_tasks, p);
        update_next_spawn(array_tasks, size_tasks);

        n = search_for_next_spawn(array_tasks, task_queue_id, size_tasks);
        
        if(p != n && n != 0){
            
        }

        if (p == -1){
            //CPU estará em IDLE para este caso
            time_idle += 1;
        }else{
            // printf("Processing: %d \n", p);
            // printf("Rest Burst: %d \n", array_tasks[p].rest_burst);
            // printf("Next Spawn 1: %d \n", array_tasks[p].next_spawn);
            // printf("Next Spawn 2: %d \n", array_tasks[p].next_spawn);

            state_before = array_tasks[p].state;
            // printf("State before: %c \n", state_before);
            state_result = should_change_state(array_tasks, task_queue_id, size_tasks, i, time_total, p);
            // printf("State result: %c \n", state_result);

            if (state_before != state_result){
                array_tasks[p].state = state_result;
                
                // Chamar a próxima task da fila
                next_to_process(array_tasks, size_tasks, task_queue_id, p);

                
                //Chamar aqui o relatório
                printf("[%s] for %d units - %c\n", array_tasks[p].name, count_time, array_tasks[p].state);
                count_time = 0;
            }
        }
    }

    printf("Time total: %d \n", temp_time);

}

//Revisado
int who_is_processing(task *array_tasks, int size_tasks){
    //Irá retornar o id da task que está sendo processada
    int id_processing = 0;
    int temp = 0;
    for(int i = 1; i < size_tasks; i++){
        if(array_tasks[i].state == 'P'){
            id_processing = i;
            return id_processing;
        }if(array_tasks[i].state == 'K' || array_tasks[i].state == 'L' || array_tasks[i].state == 'H'){
            temp += 1;
            if(temp == size_tasks-1){
                id_processing = -1;
                return id_processing;
            }
        }
    }
    return id_processing;//em caso de retorno 0 nenhuma delas estão em execução.
}

//Revisar
void next_to_process(task *array_tasks, int size_tasks, int *task_queue_id, int act_task_id){
    //Irá verificar se outra task deve ser processada agora.
    if(array_tasks[act_task_id].state != 'P'){
        for (int i = 1; i < size_tasks; i++){
            if(array_tasks[task_queue_id[i]].next_spawn == 0 || array_tasks[task_queue_id[i]].state == 'Q'){
                array_tasks[task_queue_id[i]].state = 'P';
                // array_tasks[task_queue_id[i]].next_spawn = array_tasks[task_queue_id[i]].period;
                break;
            }
        }
    }
}

// Revisar
char should_change_state(task *array_tasks, int *task_queue_id, int size_tasks, int act_time, int time_total, int act_id){
    //Irá verificar se a task deve mudar de estado
    if(array_tasks[act_id].rest_burst == 0 && array_tasks[act_id].state == 'P'){
        //Para cair em Finished a task precisa ter rest_burst = 0 e estar em execução.
        array_tasks[act_id].state = 'F';
        array_tasks[act_id].rest_burst = array_tasks[act_id].cpu_burst;
        return 'F';
    }else if(array_tasks[act_id].rest_burst > 0 && act_time >= time_total){
        //Para cair em Killed é necessário rest_burst ser maior que zero e o tempo atual ser
        //maior ou igual ao tempo total.
        array_tasks[act_id].state = 'K';
        array_tasks[act_id].rest_burst = array_tasks[act_id].cpu_burst;
        array_tasks[act_id].next_spawn = array_tasks[act_id].period;
        return 'K';
    }else if(array_tasks[act_id].rest_burst > 0 && array_tasks[act_id].next_spawn == 0){
        array_tasks[act_id].state = 'L';
        array_tasks[act_id].rest_burst = array_tasks[act_id].cpu_burst;
        array_tasks[act_id].next_spawn = array_tasks[act_id].period;
        return 'L';
    }else if(array_tasks[act_id].rest_burst > 0 && act_time < time_total){
        //Entrar em hold quando a task em questão tiver o rest_burst > 0 mas o tempo atual
        // ser maior que o tempo total e alguma outra task irá ser executada
        int temp_sfns = search_for_next_spawn(array_tasks, task_queue_id, size_tasks);
        if (temp_sfns == 0){
            return 'P';
        }else{
            if (temp_sfns != act_id){
                array_tasks[act_id].state = 'H';
                array_tasks[act_id].rest_burst = array_tasks[act_id].cpu_burst;
                array_tasks[act_id].next_spawn = array_tasks[act_id].period;
                return 'H';
            }
            return 'P';
        }
    }else{
        return 'P';
    }
}

//Revisado
void update_rest_burst(task *array_tasks, int size_tasks, int act_task_id){
    //Irá atualizar o tempo restante para a task finalizar.
    //Para esse caso estamos diminuindo.
    if(array_tasks[act_task_id].state == 'P'){
        array_tasks[act_task_id].rest_burst--;
    }
}

//Revisado
void update_next_spawn(task *array_tasks, int size_tasks){
    //Irá atualizar o tempo restante para a task reiniciar.
    //Para esse caso estamos diminuindo baseado no periodo
    for(int i = 1; i < size_tasks; i++){
        if (array_tasks[i].next_spawn > 0){
            array_tasks[i].next_spawn--;
        }
    }
}

int search_for_next_spawn(task *array_tasks, int *task_queue_id, int size_tasks){
    //Irá retornar o id da task que está com next_spawn em 0 (irá executar agora)
    for(int i = 1; i < size_tasks; i++){
        if(array_tasks[task_queue_id[i]].next_spawn == 0){
            return task_queue_id[i];
        }
    }
    return 0;
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