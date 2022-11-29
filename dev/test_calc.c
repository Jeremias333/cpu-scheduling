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
    int lost;
    int complete;
    int killed;
}task;

int size_tasks = 3; // lembrar de criar esta variavel e baseado na quantidade de tasks criar um vetor de tasks

int who_is_processing(task *array_tasks, int size_tasks);
void update_rest_burst(task *array_tasks, int size_tasks, int act_task_id);
void next_to_process(task *array_tasks, int size_tasks, int *task_queue_id, int act_task_id);
char should_change_state(task *array_tasks, int *task_queue_id, int size_tasks, int act_time, int time_total, int act_id);
void populate_queue_by_priority(task *array_tasks, int *task_queue_id, int size_tasks);
void update_next_spawn(task *array_tasks, int size_tasks);
int search_for_next_spawn(task *array_tasks, int *task_queue_id, int size_tasks, int act_task_id);
void write_line_output(char *text, int time, char state);
void report_write(task *array_tasks, int size_tasks);
void initialize_output();


int main(){
    task array_tasks[size_tasks];
    int task_queue_id[size_tasks];

    initialize_output();

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
    t1.lost = 0;
    t1.complete = 0;
    t1.killed = 0;

    task t2;
    t2.id = 2;
    t2.name = "T2";
    t2.period = 80;
    t2.cpu_burst = 35;
    t2.state = 'Q';
    t2.rest_burst = 35;
    t2.next_spawn = 80;
    t2.lost = 0;
    t2.complete = 0;
    t2.killed = 0;

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
    int aux_count = 0;
    
    //Inicializando a primeira task
    array_tasks[task_queue_id[1]].state = 'P';
    int temp_time = 0;
    //Contador de tempo
    for(int i = 1; i < time_total; i++){
        temp_time += 1;

        //Verificando qual task está em execução nesta unidade de tempo
        p = who_is_processing(array_tasks, size_tasks);
        next_to_process(array_tasks, size_tasks, task_queue_id, p);

        //Atualizando o rest_burst de cada task
        update_rest_burst(array_tasks, size_tasks, p);
        update_next_spawn(array_tasks, size_tasks);

        n = search_for_next_spawn(array_tasks, task_queue_id, size_tasks, p);

        printf("n = %d\n", n);

        if (n != 0 && n != p){
            next_to_process(array_tasks, size_tasks, task_queue_id, p);
            p = n;
        }

        // if(p != n && n != 0){
        //     p = n;
        // }

        // if (p == 0){
        //     p = n;
        //     // printf("p = %d\n", p);
        //     // printf("array_tasks[1].state = %c\n", array_tasks[1].state);
        //     // printf("array_tasks[2].state = %c\n", array_tasks[2].state);

        // }

        // if (p == -1){
        //     //CPU estará em IDLE para este caso
        //     time_idle += 1;
        //     if (n != 0){
        //         p = n;
        //     }
        // }else{
            // printf("Processing: %d \n", p);
            // printf("Rest Burst: %d \n", array_tasks[p].rest_burst);
            // printf("Next Spawn 1: %d \n", array_tasks[p].next_spawn);
            // printf("Next Spawn 2: %d \n", array_tasks[p].next_spawn);

            state_before = array_tasks[p].state;
            // printf("State before: %c \n", state_before);
            state_result = should_change_state(array_tasks, task_queue_id, size_tasks, i, time_total, p);
            // printf("State result: %c \n", state_result);
            aux_count = count_time;

            if (state_before != state_result){
                // array_tasks[p].state = state_result;
                
                // Chamar a próxima task da fila
                next_to_process(array_tasks, size_tasks, task_queue_id, p);

                if (state_result != 'P'){
                    //Chamar aqui o relatório
                    // printf("[%s] for %d units - %c\n", array_tasks[p].name, count_time, array_tasks[p].state);
                    write_line_output(array_tasks[p].name, count_time, array_tasks[p].state);
                    // printf("p: %d \n", p);
                    printf("Valor atual de i: %d \n", i);
                    aux_count = count_time;
                    count_time = 0;
                }
                
            }
        // }
        printf("[%s] for %d units - %c\n", array_tasks[p].name, aux_count, array_tasks[p].state);
        getchar();
        count_time++;
    }

    report_write(array_tasks, size_tasks);

}

//Revisado
int who_is_processing(task *array_tasks, int size_tasks){
    //Irá retornar o id da task que está sendo processada
    int id_processing = 0;
    int temp = 0;

    for(int i = 1; i < size_tasks; i++){
        if(array_tasks[i].state == 'P' || array_tasks[i].state == 'Q'){
            printf("Task %d está sendo processada", array_tasks[i].id);
            id_processing = i;
            return id_processing;
        }
    }

    // for(int i = 1; i < size_tasks; i++){
    //     if(array_tasks[i].state == 'K' || array_tasks[i].state == 'L'){
    //         temp += 1;
    //         if(temp == size_tasks-1){
    //             id_processing = -1;
    //             return id_processing;
    //         }
    //     }
    // }
    printf("Value id_processing: %d \n", id_processing);
    return id_processing;//em caso de retorno 0 nenhuma delas estão em execução.
}

//Revisar
void next_to_process(task *array_tasks, int size_tasks, int *task_queue_id, int act_task_id){
    //Irá verificar se outra task deve ser processada agora.
    //E também habilita esta a ser processada
    if(array_tasks[act_task_id].state != 'P'){
        for (int i = 1; i < size_tasks; i++){
            printf("Entrou no next process loop \n");
            if((array_tasks[task_queue_id[i]].state == 'Q' || array_tasks[task_queue_id[i]].state == 'H') &&
                array_tasks[task_queue_id[i]].next_spawn >= 1){
                    printf("Entrou no next process loop IF\n");
                array_tasks[task_queue_id[i]].state = 'P';
                array_tasks[task_queue_id[i]].next_spawn = array_tasks[task_queue_id[i]].period;
                break;
            }
            // if(array_tasks[task_queue_id[i]].next_spawn == 0 || array_tasks[task_queue_id[i]].state == 'Q'){
            //     array_tasks[task_queue_id[i]].state = 'P';
            //     array_tasks[task_queue_id[i]].next_spawn = array_tasks[task_queue_id[i]].period;
            //     break;
            // }
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
        array_tasks[act_id].complete += 1;
        return 'F';
    }else if(array_tasks[act_id].rest_burst > 0 && act_time >= time_total){
        //Para cair em Killed é necessário rest_burst ser maior que zero e o tempo atual ser
        //maior ou igual ao tempo total.
        array_tasks[act_id].state = 'K';
        array_tasks[act_id].rest_burst = array_tasks[act_id].cpu_burst;
        array_tasks[act_id].next_spawn = array_tasks[act_id].period;
        array_tasks[act_id].killed += 1;
        return 'K';
    }else if(array_tasks[act_id].rest_burst > 0 && array_tasks[act_id].next_spawn == 0){
        array_tasks[act_id].state = 'L';
        array_tasks[act_id].rest_burst = array_tasks[act_id].cpu_burst;
        array_tasks[act_id].next_spawn = array_tasks[act_id].period;
        array_tasks[act_id].lost += 1;
        return 'L';
    }else if(array_tasks[act_id].rest_burst > 0 && act_time < time_total){
        //Entrar em hold quando a task em questão tiver o rest_burst > 0 mas o tempo atual
        // ser maior que o tempo total e alguma outra task irá ser executada
        int temp_sfns = search_for_next_spawn(array_tasks, task_queue_id, size_tasks, act_id);
        if (temp_sfns == 0){
            printf("Entrou em 1 \n");
            return 'P';
        }else{
            if (temp_sfns != act_id){
                array_tasks[act_id].state = 'H';
                // array_tasks[act_id].rest_burst = array_tasks[act_id].cpu_burst;
                // array_tasks[act_id].next_spawn = array_tasks[act_id].period;
                return 'H';
            }
            printf("Entrou em 2 \n");
            return 'P';
        }
    }else{
        printf("Entrou em 3 \n");
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

int search_for_next_spawn(task *array_tasks, int *task_queue_id, int size_tasks, int act_task_id){
    //Irá retornar o id da task que está com next_spawn em 0 (irá executar agora)
    for(int i = 1; i < size_tasks; i++){
        printf("Value task_queue_id[i]: %d \n", task_queue_id[i]);
        if(array_tasks[task_queue_id[i]].next_spawn == 0){
            return task_queue_id[i];
        }else{
            return act_task_id;
        }
    }
    return act_task_id;
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

void initialize_output(){
    //Irá inicializar o arquivo de saída
    FILE *output_file;
    output_file = fopen("rate_jos.out", "w");
    fprintf(output_file, "EXECUTION BY RATE\n");
    fclose(output_file);
}

void write_line_output(char *text, int time, char state){
    //Irá escrever a linha de saída no arquivo de saída
    FILE *output_file;
    output_file = fopen("rate_jos.out", "a");
    if (output_file == NULL){
        printf("Erro ao abrir o arquivo de saída \n");
        exit(1);
    }
    fprintf(output_file, "[%s] for %d units - %c\n", text, time, state);
    fclose(output_file);
}

void report_write(task *array_tasks, int size_tasks){
    //Irá escrever o relatório no arquivo de saída
    FILE *output_file;
    output_file = fopen("rate_jos.out", "a");
    if (output_file == NULL){
        printf("Erro ao abrir o arquivo de saída \n");
        exit(1);
    }
    fprintf(output_file, "LOST DEADLINES\n");
    for(int i = 1; i < size_tasks; i++){
        fprintf(output_file, "[%s] %d\n", array_tasks[i].name, array_tasks[i].lost);
    }
    fprintf(output_file, "COMPLETE EXECUTION\n");
    for(int i = 1; i < size_tasks; i++){
        fprintf(output_file, "[%s] %d\n", array_tasks[i].name, array_tasks[i].complete);
    }
    fprintf(output_file, "KILLED\n");
    for(int i = 1; i < size_tasks; i++){
        fprintf(output_file, "[%s] %d\n", array_tasks[i].name, array_tasks[i].killed);
    }
}