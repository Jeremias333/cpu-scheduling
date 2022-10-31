#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "unistd.h"
#include "ctype.h"

#define DELIM_TRIM " \t\n"

int size_tasks = 0;
FILE *arq_address;
char *words[3];

void split_commands(char *line);
char *trim (char *string);
int count_lines_in_file();
int count_higher_line();
int count_chars_in_file();



int main(){
    printf("Executando...\n");

    arq_address = fopen("ratea.txt", "r");

    char *line[80];
    int time = 0;
    int count = 0;

    typedef struct {
        int id; // Irá individualizar as tasks
        char *name; // nome da task
        int period; // de quanta em quantas unidades de tempo irá iniciar a task
        int cpu_burst; // tempo de execução necessário
        char state; // estado atual (D, F, H, K)
        int rest_burst; // tempo restante para terminar a execução
    }task;

    int count_lines = count_lines_in_file();
    int higher_line = count_higher_line();
    int char_count = count_chars_in_file();

    printf("count_lines: %d \t higher_line: %d \n", count_lines, higher_line);
    printf("char_count: %d \n", char_count);


    char act_line[char_count];
    task array_tasks[count_lines+1];
    // printf("Antes de abrir o arquivo\n");

    while (!feof(arq_address)){
        // printf("Entrou no while\n");
        fgets(act_line, sizeof(act_line), arq_address);

        if (count == 0){
            time = atoi(act_line);
            printf("time: %d\n", time);
        }else{
            task temp;
            
            temp.name = (char*)malloc(80);

            split_commands(act_line);
            
            // printf("words[0]: %s \n", words[0]);
            // printf("words[1]: %s \n", words[1]);
            // printf("words[2]: %s \n", words[2]);
            temp.id = count-1;
            strcpy(temp.name , words[0]);
            temp.period = atoi(words[1]);
            temp.cpu_burst = atoi(words[2]);
            temp.state = 'D';
            temp.rest_burst = 0;

            array_tasks[count-1] = temp;
        }
        
        count++;

    }

    fclose(arq_address);
    
    for(int i = 0; i < 3; i++){
        printf("id: %d \n", array_tasks[i].id);
        printf("name: %s \n", array_tasks[i].name);
        printf("period: %d \n", array_tasks[i].period);
        printf("cpu_burst: %d \n", array_tasks[i].cpu_burst);
        printf("state: %c \n", array_tasks[i].state);
        printf("rest_burst: %d \n", array_tasks[i].rest_burst);
    }

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

    arq_address = fopen("ratea.txt", "r");

    while(fread(&c, sizeof(char), 1, arq_address)){
        if(c == '\n'){
            count++;
        }
    }

    fseek(arq_address, 0, SEEK_SET);

    return count;
}

int count_higher_line(){
    /*
        RETORNA O TAMANHO DA MAIOR LINHA DE UM ARQUIVO
         
        Necessário utilizar um endereço global para guardar dados do arquivo
    */
    arq_address = fopen("ratea.txt", "r");

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
        RETORNA O TAMANHO DA MAIOR LINHA DE UM ARQUIVO
         
        Necessário utilizar um endereço global para guardar dados do arquivo
    */
    arq_address = fopen("ratea.txt", "r");

    int char_count = 0;
    char c;
    while((c = fgetc(arq_address)) != EOF){
        char_count++;
    }

    fseek(arq_address, 0, SEEK_SET);

    return char_count;
}