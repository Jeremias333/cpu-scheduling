#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "unistd.h"

//Error CONSTANTES
#define DEFAULT_ERR_MSG "Ocorreu um problema: "
#define CLOSE_MSG "PROGRAMA ENCERRADO\n"

//Global VARS
FILE *arq_address; //Endereço do arquivo de entrada total




// Function PROTOTYPES
void print_err(char *err);
int process_file_address(char *file_name);
int exists_file_path(char *path);



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
                printf("Inicializou o processo");
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
        
        printf("Arquivo aberto com sucesso");
        fclose(arq_address);
    }else{
        char *ERR = DEFAULT_ERR_MSG"Ao abrir o arquivo. Não existe";
        print_err(ERR);
        return 1;
    }

    return 0;
}