#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int request_resources(FILE *result, int customer_num, int noc, int nor, int request[nor], int allocation[noc][nor], int need[noc][nor], int available[nor])
{
        int cont1 = 0, cont2 = 0, cont3 = 0;
        int aux1[nor], aux2[nor], aux[nor];

        for(int i = 0; i < nor; i++){
                aux1[i] = need[customer_num][i];
                aux2[i] = available[i];
                if((aux1[i] -= request[i]) >= 0){
                        cont1++;
                }
                if((aux2[i] -= request[i]) >= 0){
                        cont2++;
                } 
                if(aux1[i] > aux2[i]){
                        cont3++;
                }
        }
        unsafeState(noc, nor, request, need, available);
        
        if(cont1 != nor){
                fprintf(result, "The customer %d request ", customer_num);
                for(int j = 0; j < nor; j++){
                        fprintf(result, "%d ", request[j]);
                }
                fprintf(result,"was denied because exceed its maximum allocation\n");
                return -1;
        }
        if(cont2 != nor){
                fprintf(result,"The resources ");
                for(int j = 0; j < nor; j++){
                        fprintf(result,"%d ", available[j]);
                }
                fprintf(result,"are not enough to customer %d request ", customer_num);
                for(int j = 0; j < nor; j++){
                        fprintf(result,"%d ", request[j]);
                }fprintf(result,"\n");

                return -1;
        }
        if(cont3 != 0){
                fprintf(result,"The customer %d request ", customer_num);
                for(int j = 0; j < nor; j++){
                        fprintf(result,"%d ", request[j]);
                }
                fprintf(result,"was denied because result in an unsafe state\n");
                return -1;
        }
        for(int i = 0; i < nor; i++){
                if(cont1 == nor && cont2 == nor){
                        allocation[customer_num][i] += request[i];
                        need[customer_num][i] -= request[i];
                        available[i] -= request[i];
                }           
        }

        fprintf(result,"Allocate to customer %d the resources ", customer_num);
        for(int j = 0; j < nor; j++){
                fprintf(result,"%d ", request[j]);
        }fprintf(result,"\n");  
  
        
        return 0;
}

int release_resources(FILE *result,int customer_num, int noc, int nor, int release[nor], int allocation[noc][nor], int need[noc][nor], int available[nor])
{
        int cont1 = 0;
        int aux1[nor];
        for(int i = 0; i < nor; i++){
                aux1[i] = allocation[customer_num][i];
                if(aux1[i] -= release[i] <= 0){
                        cont1++;
                }
        }
        if(cont1 != nor){
                fprintf(result,"Memory release error\n");
                return -1;
        }
        for(int i = 0; i < nor; i++){
                allocation[customer_num][i] -= release[i];
                need[customer_num][i] += release[i];
                available[i] += release[i];
        }
        fprintf(result,"Release from customer %d the resources ", customer_num);
        for(int i = 0; i < nor; i++){
                fprintf(result,"%d ", release[i]);
        }fprintf(result,"\n"); 

        return 0;
}

int contarLinhas(FILE *arq)
{
        int cont = 0;
        int prev = 0;
        char c;

        for(c = getc(arq); c != EOF; c = getc(arq)){
                if(c == '\n'){
                        cont ++;
                        prev = 0;
                }
                else{
                        prev = 1;
                }
        }
        if (prev){
                cont++;
        }
        
        return cont;
}

int contarColunasClientes(FILE *matriz, int linhas){
        
        int aux[linhas];
        int i = 0, colunas = 1;
        int caract, valorCol;

        for (caract = fgetc(matriz); !feof(matriz); caract = fgetc(matriz)){
                if(caract == ',') {
                        colunas++;
                }
                if(caract == '\n'){
                        aux[i] = colunas;
                        i++;
                        colunas = 1; 
                }
    }

        aux[i] = colunas;
        valorCol = aux[0];

        for(int k = 0; k < linhas; k++){
                if(aux[k] != aux[0]){
                        printf("Matriz não coerente.\n");
                        return 0;
                }
    }

        return valorCol;
}

int contarColunasComandos(FILE *matriz, int linhas){
        
        int aux[linhas];
        int i = 0, colunas = 1;
        int caract, valorCol;

        for (caract = fgetc(matriz); !feof(matriz); caract = fgetc(matriz)){
                if(caract == ' ') {
                        colunas++;
                }
                if(caract == '\n'){
                        aux[i] = colunas;
                        i++;
                        colunas = 1; 
                }
    }

        aux[i] = colunas;
        valorCol = aux[0];

        return valorCol;
}

void printarMatriz(FILE *result, int noc, int nor, int a, int maximum[noc][nor], int allocation[noc][nor], int need[noc][nor], int available[a-1]){
        

        fprintf(result,"MAXIMUM   | ");
        fprintf(result,"ALLOCATION |   ");
        fprintf(result,"NEED\n");
        for(int i = 0; i < noc; i++){
                fprintf(result," ");
                for(int j = 0; j < nor; j++){
                        fprintf(result,"%d ", maximum[i][j]);
                }
                fprintf(result,"   |   ");
                for(int j = 0; j< nor; j++){
                        fprintf(result,"%d ",allocation[i][j]);
                } 
                fprintf(result,"   |   ");
                for(int j = 0; j < nor; j++){
                        fprintf(result,"%d ", need[i][j]);
                }
                fprintf(result,"\n");
        }

        fprintf(result,"AVAILABLE: ");
        for(int i = 0; i < (a-1); i++){
                fprintf(result,"%d ", available[i]);
        }fprintf(result,"\n");

}

int main(int argc, char *argv[])
{
        int NUMBER_OF_CUSTOMERS, NUMBER_OF_RESOURCES, i, j, colunas_clientes, linhas_comandos, colunas_comandos, num, v;
        int customer_num;
        char c;
        
        FILE *clientes;
        FILE *comandos;
        FILE *result;

        clientes = fopen("customer.txt", "r");
        comandos = fopen("commands.txt", "r");

        if(clientes == NULL || comandos == NULL){
                printf("Arquivo não existe ou não pode ser aberto.\n");
                return 0;
        }
  
        NUMBER_OF_CUSTOMERS = contarLinhas(clientes); //Conta quantidade de clientes (linhas)
        linhas_comandos = contarLinhas(comandos); //Conta a quantidade de comandos (linhas)

        fclose(clientes); //Fecha arquivo
        fclose(comandos); //Fecha arquivo

        clientes = fopen("customer.txt", "r"); //Abre arquivo clientes
        comandos = fopen("commands.txt", "r"); //Abre arquivo comandos

        colunas_clientes = contarColunasClientes(clientes, NUMBER_OF_CUSTOMERS); //Conta quantidade de colunas clientes
        colunas_comandos = contarColunasComandos(comandos, linhas_comandos); //Conta quantidade de colunas comandos

        fclose(clientes); //Fecha arquivo clientes
        fclose(comandos); //Fecha arquivo comandos

        NUMBER_OF_RESOURCES = (argc - 1); //Número de recursos

        if(colunas_clientes != NUMBER_OF_RESOURCES){
                printf("Número de argumentos não coerente com a quantidade de recursos indicados.\n");
                return 0;
        }
        
        int available[NUMBER_OF_RESOURCES];
        int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES]; 
        int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
        int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
        int commands[linhas_comandos][colunas_comandos];
        int request[NUMBER_OF_RESOURCES], release[NUMBER_OF_RESOURCES];

        clientes = fopen("customer.txt", "r"); //Abre arquivo novamente

        int aux = 0, l = 0;

        for(i = 0; i < NUMBER_OF_CUSTOMERS; i++){
                for(j = 0; j < colunas_clientes; j++){
                        fscanf(clientes, "%d%c", &aux,&c);
                        maximum[i][j] = aux;
                }
        }

        fclose(clientes);

        //Passando valores pro matriz ALLOCATION
        for(int i = 0; i < NUMBER_OF_CUSTOMERS; i++){
                for(int j = 0; j < NUMBER_OF_RESOURCES; j++){
                        allocation[i][j] = 0;
                }
        }
        //Passando valores pro matriz NEED
        for(int i = 0; i < NUMBER_OF_CUSTOMERS; i++){
                for(int j = 0; j < NUMBER_OF_RESOURCES; j++){
                        need[i][j] = maximum[i][j];
                }
        }
        //Passando valores para o array AVAILABLE
        int val, k = 0;
        for(i = 1; i < argc; i++){
                val = atoi(argv[i]);
                available[k] = val;
                k++;
        }

        comandos = fopen("commands.txt", "r"); //Abre arquivo comandos

        int lin = 0;

        for(c = getc(comandos); c != EOF; c = getc(comandos)){
                if(c == 'Q' || c == 'L' || c == '*'){
                        commands[lin][0] = c;
                        lin++;                      
                }
         
        } 

        fclose(comandos);
        comandos = fopen("commands.txt", "r"); //Abre arquivo comandos       
        
        lin = 0;
        int col = 1;

        for(c = getc(comandos); c != EOF; c = getc(comandos)){
                if(c != ' ' && c != 'R' && c != 'Q' && c != 'L' && c != '*' && c != '\n'){
                        num = atoi(&c);
                        commands[lin][col] = num;
                        col++;                     
                }
                if(c == '\n'){
                        lin++;
                        col = 1;
                }
         
        }

        fclose(comandos);

        for(i = 0; i < linhas_comandos; i++){
                if(commands[i][0] == 'Q'){
                        int x = 0;
                        for(j = 2; j < colunas_comandos; j++){
                                request[x] = commands[i][j];
                                x++;
                        }
                        result = fopen("result.txt", "a");
                        request_resources(result,commands[i][1], NUMBER_OF_CUSTOMERS, NUMBER_OF_RESOURCES, request, allocation, need, available);
                        fclose(result);
                }
                if(commands[i][0] == 'L'){
                        int y = 0;
                        for(j = 2; j < colunas_comandos; j++){
                                release[y] = commands[i][j];
                                y++;
                        }
                        result = fopen("result.txt", "a");
                        release_resources(result,commands[i][1], NUMBER_OF_CUSTOMERS, NUMBER_OF_RESOURCES, release, allocation, need, available);
                        fclose(result);
                }
                if(commands[i][0] == '*'){
                        result = fopen("result.txt", "a");
                        printarMatriz(result, NUMBER_OF_CUSTOMERS, NUMBER_OF_RESOURCES, argc, maximum, allocation, need, available);
                        fclose(result);
                }
        }
           
        return 0;

}