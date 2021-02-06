#include "globals.h"
#include <string.h>

static int linhaA = 1;
char regB[10];


int getReg(char *line, int index){  
    int j = 0, i;
    for (i = index+1; i < 40 && line[i] != ',' && line[i] != '\n' && line[i] != ')'; i++) //pega numero do registrador
    {
        regB[j] = line[i];
        j++;
    }
    regB[j] = '\0';

    return i+1;
}

int getImd(char *line, int index){   
    int j = 0, i;
    for (i = index; i < 40 && line[i] != ',' && line[i] != '\n' && line[i] != '('; i++) //pega imediato
    {
        regB[j] = line[i];
        j++;
    }
    regB[j] = '\0';

    return i+1;
}

void addiBin(char *read, int index){   

    fprintf(code2, "instruc[%d] = {6'd3, ", linhaA);

    index = getReg(read, index);
    fprintf(code2, "5'd%d, ", atoi(regB));

    index = getImd(read, index);
    int imd = atoi(regB);

    index = getReg(read, index);
    fprintf(code2, "5'd%d, 16'd%d};    // addi\n", atoi(regB), imd);
}

void addBin(char *read, int index){

    fprintf(code2, "instruc[%d] = {6'd1, ", linhaA);

    index = getReg(read, index);
    fprintf(code2, "5'd%d, ", atoi(regB));

    index = getReg(read, index);
    fprintf(code2, "5'd%d, ", atoi(regB));

    index = getReg(read, index);
    fprintf(code2, "5'd%d, 5'd0, 6'd0};    // add\n", atoi(regB));
}

void subBin(char *read, int index){

    fprintf(code2, "instruc[%d] = {6'd2, ", linhaA);

    index = getReg(read, index);
    fprintf(code2, "5'd%d, ", atoi(regB));

    index = getReg(read, index);
    fprintf(code2, "5'd%d, ", atoi(regB));

    index = getReg(read, index);
    fprintf(code2, "5'd%d, 5'd0, 6'd0};    // sub\n", atoi(regB));
}

void multBin(char *read, int index){

    fprintf(code2, "instruc[%d] = {6'd6, ", linhaA);

    index = getReg(read, index);
    fprintf(code2, "5'd%d, ", atoi(regB));

    index = getReg(read, index);
    fprintf(code2, "5'd%d, ", atoi(regB));

    index = getReg(read, index);
    fprintf(code2, "5'd%d, 5'd0, 6'd0};    // mult\n", atoi(regB));
}

void divBin(char *read, int index){

    fprintf(code2, "instruc[%d] = {6'd7, ", linhaA);

    index = getReg(read, index);
    fprintf(code2, "5'd%d, ", atoi(regB));

    index = getReg(read, index);
    fprintf(code2, "5'd%d, ", atoi(regB));

    index = getReg(read, index);
    fprintf(code2, "5'd%d, 5'd0, 6'd0};    // div\n", atoi(regB));
}

void loadBin(char *read, int index){

    fprintf(code2, "instruc[%d] = {6'd16, ", linhaA);

    index = getImd(read, index);
    int imd = atoi(regB);

    index = getReg(read, index);
    fprintf(code2, "5'd%d, ", atoi(regB));

    index++;

    index = getReg(read, index);
    fprintf(code2, "5'd%d, ", atoi(regB));


    fprintf(code2, "16'd%d};    // load\n", imd);
}

void storeBin(char *read, int index){

    fprintf(code2, "instruc[%d] = {6'd18, ", linhaA);

    index = getReg(read, index);
    int result = atoi(regB);

    index = getImd(read, index);
    int imd = atoi(regB);

    index = getReg(read, index);
    fprintf(code2, "5'd%d, ", atoi(regB));


    fprintf(code2, "5'd%d, 16'd%d};    // store\n", result, imd);
}

void bneBin(char *read, int index){

    fprintf(code2, "instruc[%d] = {6'd21, ", linhaA);

    index = getReg(read, index);
    fprintf(code2, "5'd%d, ", atoi(regB));

    index = getReg(read, index);
    fprintf(code2, "5'd%d, ", atoi(regB));

    index = getImd(read, index);
    fprintf(code2, "16'd%d};    // bne\n", atoi(regB));
}

void beqBin(char *read, int index){

    fprintf(code2, "instruc[%d] = {6'd20, ", linhaA);

    index = getReg(read, index);
    fprintf(code2, "5'd%d, ", atoi(regB));

    index = getReg(read, index);
    fprintf(code2, "5'd%d, ", atoi(regB));

    index = getImd(read, index);
    fprintf(code2, "16'd%d};    // beq\n", atoi(regB));
}

void bltBin(char *read, int index){

    fprintf(code2, "instruc[%d] = {6'd22, ", linhaA);

    index = getReg(read, index);
    fprintf(code2, "5'd%d, ", atoi(regB));

    index = getReg(read, index);
    fprintf(code2, "5'd%d, ", atoi(regB));

    index = getImd(read, index);
    fprintf(code2, "16'd%d};    // blt\n", atoi(regB));
}

void bgtBin(char *read, int index){

    fprintf(code2, "instruc[%d] = {6'd23, ", linhaA);

    index = getReg(read, index);
    fprintf(code2, "5'd%d, ", atoi(regB));

    index = getReg(read, index);
    fprintf(code2, "5'd%d, ", atoi(regB));

    index = getImd(read, index);
    fprintf(code2, "16'd%d};    // bgt\n", atoi(regB));
}

void bgeBin(char *read, int index){

    fprintf(code2, "instruc[%d] = {6'd19, ", linhaA);

    index = getReg(read, index);
    fprintf(code2, "5'd%d, ", atoi(regB));

    index = getReg(read, index);
    fprintf(code2, "5'd%d, ", atoi(regB));

    index = getImd(read, index);
    fprintf(code2, "16'd%d};    // bge\n", atoi(regB));
}

void bleBin(char *read, int index){

    fprintf(code2, "instruc[%d] = {6'd17, ", linhaA);

    index = getReg(read, index);
    fprintf(code2, "5'd%d, ", atoi(regB));

    index = getReg(read, index);
    fprintf(code2, "5'd%d, ", atoi(regB));

    index = getImd(read, index);
    fprintf(code2, "16'd%d};    // ble\n", atoi(regB));
}

void inBin(char *read, int index){

    index = getReg(read, index);
    fprintf(code2, "instruc[%d] = {6'd24, 5'd0, 5'd%d, 16'd0}; // in\n", linhaA, atoi(regB));
}

void outBin(char *read, int index){

    index = getReg(read, index);
    fprintf(code2, "instruc[%d] = {6'd25, 5'd0, 5'd%d, 16'd0}; // out\n", linhaA, atoi(regB));
}

void jrBin(char *read, int index){

    //sempre pula pro 31, então nao precisa especificar
    fprintf(code2, "instruc[%d] = {6'd28, 5'd0, 5'd0, 5'd0, 5'd0, 6'd0}; 	//jr $ra\n", linhaA);
}

void jumpBin(char *read, int index){

    index = getImd(read, index);
    fprintf(code2, "instruc[%d] = {6'd26, 26'd%d}; \t\t\t// jump\n", linhaA, atoi(regB));
}

void jalBin(char *read, int index){

    index = getImd(read, index);
    fprintf(code2, "instruc[%d] = {6'd27, 26'd%d}; \t\t\t// jal\n", linhaA, atoi(regB));
}


void getInstruBin(char *read){
    char instruction[15];
    int j = 0, i;
    for (i = 0; read[i] != ' ' && i < 15; i++)  //pega a instrucao do assembly
    {
        instruction[j] = read[i];
        j++;
    }
    i++;
    instruction[j] = '\0';


    //verifica qual comando é e chama a respectiva função
    if (strcmp(instruction, "addi") == 0) 
    {
        addiBin(read, i);
    } 
    else if (strcmp(instruction, "jump") == 0)
    {
        jumpBin(read, i);
    }
    else if (strcmp(instruction, "load") == 0)
    {
        loadBin(read, i);
    }
    else if (strcmp(instruction, "store") == 0)
    {
        storeBin(read, i);
    }
    else if (strcmp(instruction, "jal") == 0)
    {
        jalBin(read, i);
    }
    else if (strcmp(instruction, "bne") == 0)
    {
        bneBin(read, i);
    }
    else if (strcmp(instruction, "beq") == 0)
    {
        beqBin(read, i);
    }
    else if (strcmp(instruction, "bge") == 0)
    {
        bgeBin(read, i);
    }
    else if (strcmp(instruction, "bgt") == 0)
    {
        bgtBin(read, i);
    }
    else if (strcmp(instruction, "ble") == 0)
    {
        bleBin(read, i);
    }
    else if (strcmp(instruction, "blt") == 0)
    {
        bltBin(read, i);
    }
    else if (strcmp(instruction, "add") == 0)
    {
        addBin(read, i);
    }
    else if (strcmp(instruction, "sub") == 0)
    {
        subBin(read, i);
    }
    else if (strcmp(instruction, "div") == 0)
    {
        divBin(read, i);
    }
    else if (strcmp(instruction, "mult") == 0)
    {
        multBin(read, i);
    }
    else if (strcmp(instruction, "in") == 0)
    {
        inBin(read, i);
    }
    else if (strcmp(instruction, "out") == 0)
    {
        outBin(read, i);
    }
    else if (strcmp(instruction, "jr") == 0)
    {
        jrBin(read, i);
    }

}

void converteBin(){
    char read[51];

    while(fgets(read, 50, code) != NULL){   //Faz a leitura linha por linha
        getInstruBin(read);
        linhaA++;
    }

}