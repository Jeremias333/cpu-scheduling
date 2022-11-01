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
int count_lines = 0; //Conta a quantidade de linhas do arquivo de entrada
int char_count = 0; // Conta a quantidade de caracteres do arquivo de entrada


//Structs
typedef struct {
    int id; // Irá individualizar as tasks
    char *name; // nome da task
    int period; // de quanta em quantas unidades de tempo irá iniciar a task
    int cpu_burst; // tempo de execução necessário
    char state; // estado atual (P, C, F, H, K)
                // P = Processing
                // C = Created
                // F = Finished
                // H = Hold
                // K = Killed
    int rest_burst; // tempo restante para terminar a execução
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
void process();


int main(int argc, char **argv) {
    printf("Dev area operating...\n");

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
                printf("Inicializou o processo\n");
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
        
        printf("Arquivo aberto com sucesso\n");
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

void process(){
    /*
        TODO PROCESSAMENTO ESTARÁ CONCENTRADO AQUI
    */
   
    char act_line[char_count];
    task array_tasks[count_lines];
    int count = 0;

    while(!feof(arq_address)){
        fgets(act_line, sizeof(act_line), arq_address);

        if (count == 0){
            time_total = atoi(act_line);
            printf("time: %d\n", time_total);
        }else{
            task temp;
            
            temp.name = (char*)malloc(80);

            split_commands(act_line);
            
            // printf("words[0]: %s \n", words[0]);
            // printf("words[1]: %s \n", words[1]);
            // printf("words[2]: %s \n", words[2]);
            temp.id = count;
            strcpy(temp.name, words[0]);
            temp.period = atoi(words[1]);
            temp.cpu_burst = atoi(words[2]);
            temp.state = 'C'; //Atribuindo valor padrão ao estado da task (created)
            temp.rest_burst = atoi(words[2]);

            array_tasks[count] = temp;

        }
        count++;
    }

    fclose(arq_address);

    for(int i = 1; i < count_lines; i++){
        printf("id: %d \n", array_tasks[i].id);
        printf("name: %s \n", array_tasks[i].name);
        printf("period: %d \n", array_tasks[i].period);
        printf("cpu_burst: %d \n", array_tasks[i].cpu_burst);
        printf("state: %c \n", array_tasks[i].state);
        printf("rest_burst: %d \n", array_tasks[i].rest_burst);
    }

}