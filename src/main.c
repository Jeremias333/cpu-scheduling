#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "unistd.h"
#include "ctype.h"

//Error CONSTANTES
#define DEFAULT_ERR_MSG "Ocorreu um problema: "
#define CLOSE_MSG "PROGRAMA ENCERRADO\n"
#define DELIM_TRIM " \t\n"

//Global VARS
FILE *arq_address; //Endereço do arquivo de entrada total
int time_total = 0; //Tempo total de execução da CPU
int time_idle = 0; //Tempo de idle da CPU
char *words[3]; //Array de strings para armazenar as palavras de cada linha do arquivo de entrada
int count_lines = 0; //Conta a quantidade de linhas do arquivo de entrada que também é a variável de controle size_task
int char_count = 0; // Conta a quantidade de caracteres do arquivo de entrada


//Structs
typedef struct {
    int id; // Irá individualizar as tasks
    char *name; // nome da task
    int period; // de quanta em quantas unidades de tempo irá iniciar a task
    int cpu_burst; // tempo de execução necessário
    char state; // estado atual (P, F, H, K, L)
                // P = Processing
                // F = Finished
                // H = Hold
                // K = Killed
                // L = Lost
    int rest_burst; // tempo restante para terminar a execução
    int next_spawn; // tempo para a próxima execução
    int lost;
    int complete;
    int killed;
}task;


// Function PROTOTYPES
void print_err(char *err);
int process_file_address(char *file_name);
int exists_file_path(char *path);
void split_commands(char *line);
char *trim (char *string);
int count_lines_in_file();
int count_chars_in_file();
void start_variables();
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
void process();


int main(int argc, char **argv) {
    // printf("Prod area operating...\n");

     char *address_path = argv[1];

    if(argc < 2) {
        print_err("Nenhum arquivo de entrada foi passado como argumento");
    }else{

        //Verifica existencia de parametro passando path do arquivo
        if (exists_file_path(address_path) == 1){
            // Verifica existencia do arquivo
            // Verifica existencia de conteúdo no arquivo
            // Verifica extensão do arquivo
            int proceed = process_file_address(address_path);
            if (proceed == 0){
                //processamento do arquivo
                // printf("Inicializou o processo\n");
                start_variables();
                process();
            }else{
                //irá encerrar a aplicação com mensagem de erro
                exit(1);
            }
        }
    }

    return 0;
}

void print_err(char *err) {
    printf("%s\n", err);
    printf(CLOSE_MSG);
    exit(1);
}

int exists_file_path(char *path){
    if (path == NULL){
        return 0;
    }
    return 1;
}

int process_file_address(char *file_name){
    char *temp[3];
    *temp = &file_name[strlen(file_name)-3];

    if(strcmp(*temp, "txt") != 0){
        char *ERR = DEFAULT_ERR_MSG"O arquivo passado não é um arquivo .txt";
        print_err(ERR);
        return 1;
    }

    arq_address = fopen(file_name, "r");
    if(arq_address){
        int c = fgetc(arq_address);
        if(c == EOF){
            char *ERR = DEFAULT_ERR_MSG"Ao abrir o arquivo. Está vazio";
            print_err(ERR);
            fclose(arq_address);
            return 1;
        }

        ungetc(c, arq_address);
        
        // printf("Arquivo aberto com sucesso\n");
        fseek(arq_address, 0, SEEK_SET);
    }else{
        char *ERR = DEFAULT_ERR_MSG"Ao abrir o arquivo. Não existe";
        print_err(ERR);
        return 1;
    }

    return 0;
}

void split_commands(char *line){
    // printf("line: %s \n", line);
    line[strcspn(line, "\n")] = ' ';

    char delimit[] = " ";
    char *token = strtok(line, delimit);
    int count = 0;

    while(token != NULL){
        // printf("token: %s \n", token);
        words[count++] = trim(token);
        token = strtok(NULL, delimit);
    }
}

char *trim (char *string){
    size_t  beg = strspn(string, DELIM_TRIM), len = strlen (string);

    if (beg == len) {
        *string = 0;
        return string;
    }

    memmove (string, string + beg, len - beg + 1);
    for (int i = (int)(len - beg - 1); i >= 0; i--) {
        if (isspace(string[i])){
            string[i] = 0;
        }else{
            break;
        }
    }
    return string;
}

int count_lines_in_file(){
    /*
      RETORNA QUANTIDADE DE LINHAS DE UM ARQUIVO
     
      Necessário utilizar um endereço global para guardar dados do arquivo
      está retornando com -1 pois a questão pede isso. (primeira linha está sendo desconsiderada
      nesta questão)
    */
    int count = 0;
    char c;

    // arq_address = fopen("ratea.txt", "r");

    while(fread(&c, sizeof(char), 1, arq_address)){
        if(c == '\n'){
            count++;
        }
    }

    fseek(arq_address, 0, SEEK_SET);

    return count+1;
}

int count_higher_line(){
    /*
        RETORNA O TAMANHO DA MAIOR LINHA DE UM ARQUIVO
         
        Necessário utilizar um endereço global para guardar dados do arquivo
    */
    // arq_address = fopen("ratea.txt", "r");

    int higher_line = 0;
    int temp_line = 0;
    char c;
    while((c = fgetc(arq_address)) != EOF){
        if(c == '\n'){
            if(temp_line > higher_line){
                higher_line = temp_line;
            }
            temp_line = 0;
        }else{
            temp_line++;
        }
    }

    fseek(arq_address, 0, SEEK_SET);

    return higher_line;

}

int count_chars_in_file(){
     /*
        RETORNA A QUANTIDADE DE CARACTERES DE UM ARQUIVO
         
        Necessário utilizar um endereço global para guardar dados do arquivo
    */

    int char_count = 0;
    char c;
    while((c = fgetc(arq_address)) != EOF){
        char_count++;
    }

    fseek(arq_address, 0, SEEK_SET);

    return char_count;
}

void start_variables(){
    /*
        inicializa as variáveis globais
    */
    count_lines = count_lines_in_file();
    char_count = count_chars_in_file();
}

//REVISADO
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

//Revisar
void next_to_process(task *array_tasks, int size_tasks, int *task_queue_id, int act_task_id){
    //Irá verificar se outra task deve ser processada agora.
    //E também habilita esta a ser processada
    if(array_tasks[act_task_id].state != 'P'){
        for (int i = 1; i < size_tasks; i++){
            if(array_tasks[task_queue_id[i]].state == 'H'){
                array_tasks[task_queue_id[i]].state = 'P';
                break;
            }else if(array_tasks[task_queue_id[i]].state == 'F' 
                && array_tasks[task_queue_id[i]].next_spawn == 0){
                    array_tasks[task_queue_id[i]].state = 'P';
                    array_tasks[task_queue_id[i]].next_spawn = array_tasks[task_queue_id[i]].period;
                    break;
            }else if (array_tasks[task_queue_id[i]].state == 'L'
                && array_tasks[task_queue_id[i]].next_spawn == 0){
                    array_tasks[task_queue_id[i]].state = 'P';
                    array_tasks[task_queue_id[i]].next_spawn = array_tasks[task_queue_id[i]].period;
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
        array_tasks[act_id].complete += 1;
        return 'F';

    }else if(array_tasks[act_id].rest_burst > 0 && act_time >= time_total-2){
        //Para cair em Killed é necessário rest_burst ser maior que zero e o tempo atual ser
        //maior ou igual ao tempo total.
        //Se uma tarefa tomou kill, provavelmente todas outras P ou H também irão tomar
        for (int i = 1; i < size_tasks; i++){
            if(array_tasks[i].state == 'P' || array_tasks[i].state == 'H' || array_tasks[i].next_spawn == 0){
                array_tasks[i].state = 'K';
                array_tasks[i].killed += 1;
            }
        }
        return 'K';

    }else if(array_tasks[act_id].rest_burst > 0 && array_tasks[act_id].next_spawn == 0){
        array_tasks[act_id].state = 'L';
        array_tasks[act_id].rest_burst = array_tasks[act_id].cpu_burst;
        array_tasks[act_id].lost += 1;
        return 'L';

    }else if(array_tasks[act_id].rest_burst > 0 && act_time <= time_total){
        //Entrar em hold quando a task em questão tiver o rest_burst > 0 mas o tempo atual
        //sendo maior que o tempo total e alguma outra task irá ser executada
        int temp_sfns = search_for_next_spawn(array_tasks, task_queue_id, size_tasks, act_id);
        if (temp_sfns == -1){
            return 'P';
        }else if (temp_sfns != act_id){
            if (array_tasks[temp_sfns].period > array_tasks[act_id].period){
                array_tasks[temp_sfns].state = 'H';
                return 'P';
            }else{
                array_tasks[act_id].state = 'H';
                return 'H';
            }
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

int search_for_next_spawn(task *array_tasks, int *task_queue_id, int size_tasks, int act_task_id){
    //Irá retornar o id da task que está com next_spawn em 0 (irá executar agora)
    for(int i = 1; i < size_tasks; i++){
        if(array_tasks[task_queue_id[i]].next_spawn == 0){
            return task_queue_id[i];
        }
    }
    return -1; //Irá retornar -1 sempre que o next_spawn de todas as tasks for maior que 0
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
    fprintf(output_file, "\nLOST DEADLINES\n");
    for(int i = 1; i < size_tasks; i++){
        fprintf(output_file, "[%s] %d\n", array_tasks[i].name, array_tasks[i].lost);
    }
    fprintf(output_file, "\nCOMPLETE EXECUTION\n");
    for(int i = 1; i < size_tasks; i++){
        fprintf(output_file, "[%s] %d\n", array_tasks[i].name, array_tasks[i].complete);
    }
    fprintf(output_file, "\nKILLED\n");
    for(int i = 1; i < size_tasks; i++){
        fprintf(output_file, "[%s] %d\n", array_tasks[i].name, array_tasks[i].killed);
    }
}


void process(){
    /*
        TODO PROCESSAMENTO ESTARÁ CONCENTRADO AQUI
    */
   
    char act_line[char_count];
    task array_tasks[count_lines];
    int task_queue_id[count_lines];
    int count = 0;
    int size_tasks = count_lines; // Quantidade de tasks que serão processadas

    //Area count time
    int p_id = 1;
    int n_id = 1;
    int count_time = 1;
    char state_result = 'H';
    char state_before = 'H';

    task t0;
    t0.id = 0;
    t0.name = "IDLE";
    t0.period = 9999;
    t0.cpu_burst = 9999;
    t0.state = 'H';
    t0.rest_burst = 9999;
    t0.next_spawn = 9999;

    array_tasks[0] = t0;//incializando a task idle

    while(!feof(arq_address)){
        fgets(act_line, sizeof(act_line), arq_address);

        if (count == 0){
            time_total = atoi(act_line)+2;
            // printf("time: %d\n", time_total);
        }else{
            task temp;
            
            temp.name = (char*)malloc(80);

            split_commands(act_line);

            temp.id = count;
            strcpy(temp.name, words[0]);
            temp.period = atoi(words[1]);
            temp.cpu_burst = atoi(words[2]);
            temp.state = 'H'; //Atribuindo valor padrão ao estado da task (Hold)
            temp.rest_burst = atoi(words[2]);
            temp.next_spawn = atoi(words[1]);
            temp.killed = 0;
            temp.complete = 0;
            temp.lost = 0;

            array_tasks[count] = temp;

        }
        count++;
    }

    fclose(arq_address);

    initialize_output();

    // for(int i = 1; i < count_lines; i++){
    //     printf("id: %d \n", array_tasks[i].id);
    //     printf("name: %s \n", array_tasks[i].name);
    //     printf("period: %d \n", array_tasks[i].period);
    //     printf("cpu_burst: %d \n", array_tasks[i].cpu_burst);
    //     printf("state: %c \n", array_tasks[i].state);
    //     printf("rest_burst: %d \n", array_tasks[i].rest_burst);
    //     printf("next_spawn: %d \n", array_tasks[i].next_spawn);
    // }

    //Inicializando a fila de prioridades
    populate_queue_by_priority(array_tasks, task_queue_id, size_tasks);
    
    //Inicializando a primeira task
    array_tasks[task_queue_id[1]].state = 'P';

    //Contador de tempo
    for(int i = 1; i < time_total; i++){

        //Verificando qual task está em execução nesta unidade de tempo
        p_id = who_is_processing(array_tasks, size_tasks);


        update_next_spawn(array_tasks, size_tasks);


        if (p_id == 0){
            if (time_idle == 0){
                count_time = 0;
            }
            time_idle += 1;
            next_to_process(array_tasks, size_tasks, task_queue_id, p_id);
        }else{
            if (time_idle > 0){
                FILE *output_file;
                output_file = fopen("rate_jos.out", "a");
                if (output_file == NULL){
                    printf("Erro ao abrir o arquivo de saída \n");
                    exit(1);
                }
                fprintf(output_file, "idle for %d units\n", time_idle);
                fclose(output_file);
                time_idle = 0;
                count_time = 1;
            }

            //Atualizando o rest_burst de cada task
            update_rest_burst(array_tasks, size_tasks, p_id);

            state_before = array_tasks[p_id].state;

            state_result = should_change_state(array_tasks, task_queue_id, size_tasks, i, time_total, p_id);

            if (state_before != state_result){

                if (state_result != 'P'){
                    //Chamar aqui o relatório
                    if (i < time_total-1){
                        write_line_output(array_tasks[p_id].name, count_time, array_tasks[p_id].state);
                        count_time = 0;
                    }
                    
                }
                
            }

            next_to_process(array_tasks, size_tasks, task_queue_id, p_id);
            for (int i = 1; i < size_tasks; i++){
                if (array_tasks[i].next_spawn == 0){
                    array_tasks[i].next_spawn = array_tasks[i].period;
                }
            }
            
        }

        count_time++;
    }

    report_write(array_tasks, size_tasks);
    // printf("Fim do processamento \n");

}