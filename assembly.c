#include "globals.h"
#include "symtab.h"
#include <string.h>
#include "cgen.h"

int *labelPos;
int labelMain;
static int linhaAtual = 1;
static int qtdFunc = 0;

typedef struct {
    char instruction[10];
    char r1[40];
    char r2[40];
    char rs[10];
    char label[10];
} assembly;

assembly *commands;

typedef struct {
    char funName[40];
    int posLabel;
} labelFun;

labelFun *posLabelFun;

char regist[40];

typedef enum {
$zero, $at, $v0, $v1, $a0, $a1, $a2, $a3, $t0, $t1, $t2, $t3, $t4, $t5, $t6, $t7, $t8, $t9, $t10, $t11, $t12, $t13,
$t14, $t15, $t16, $t17, $k0, $k1, $sp, $hi, $lo, $ra
} reg;



int getRegister(char *regAux){ //retorna o número equivalente ao registrador no processador
    if (strcmp(regAux, "$zero") == 0 || strcmp(regist, "_") == 0 || strcmp(regist, "0") == 0){
        return $zero;
    } else if(strcmp(regAux, "$at") == 0){
        return $at;
    } else if(strcmp(regAux, "$v0") == 0){
        return $v0;
    } else if(strcmp(regAux, "$v1") == 0){
        return $v1;
    } else if(strcmp(regAux, "$a0") == 0){
        return $a0;
    } else if(strcmp(regAux, "$a1") == 0){
        return $a1;
    } else if(strcmp(regAux, "$a2") == 0){
        return $a2;
    } else if(strcmp(regAux, "$a3") == 0){
        return $a3;
    } else if(strcmp(regAux, "$t0") == 0){
        return $t0;
    } else if(strcmp(regAux, "$t1") == 0){
        return $t1;
    } else if(strcmp(regAux, "$t2") == 0){
        return $t2;
    } else if(strcmp(regAux, "$t3") == 0){
        return $t3;
    } else if(strcmp(regAux, "$t4") == 0){
        return $t4;
    } else if(strcmp(regAux, "$t5") == 0){
        return $t5;
    } else if(strcmp(regAux, "$t6") == 0){
        return $t6;
    } else if(strcmp(regAux, "$t7") == 0){
        return $t7;
    } else if(strcmp(regAux, "$t8") == 0){
        return $t8;
    } else if(strcmp(regAux, "$t9") == 0){
        return $t9;
    } else if(strcmp(regAux, "$t10") == 0){
        return $t10;
    } else if(strcmp(regAux, "$t11") == 0){
        return $t11;
    } else if(strcmp(regAux, "$t12") == 0){
        return $t12;
    } else if(strcmp(regAux, "$t13") == 0){
        return $t13;
    } else if(strcmp(regAux, "$t14") == 0){
        return $t14;
    } else if(strcmp(regAux, "$t15") == 0){
        return $t15;
    } else if(strcmp(regAux, "$t16") == 0){
        return $t16;
    } else if(strcmp(regAux, "$t17") == 0){
        return $t17;
    } else if(strcmp(regAux, "$k0") == 0){
        return $k0;
    } else if(strcmp(regAux, "$k1") == 0){
        return $k1;
    } else if(strcmp(regAux, "$sp") == 0){
        return $sp;
    } else if(strcmp(regAux, "$hi") == 0){
        return $hi;
    } else if(strcmp(regAux, "$lo") == 0){
        return $lo;
    } else if(strcmp(regAux, "$ra") == 0){
        return $ra;
    }
}

int getParam(char *line, int index){    //serve para segundo e terceiro parametros
    int j = 0, i;
    for (i = index; i < 40 && line[i] != ',' ; i++) //pega parametro
    {
        regist[j] = line[i];
        j++;
    }
    regist[j] = '\0';
    return i+1;
}

void getLastParam(char *line, int index){
    int j = 0, i;
    for (i = index; i < 40 && line[i] != ')' ; i++) //pega ultimo parametro
    {
        regist[j] = line[i];
        j++;
    }
    regist[j] = '\0';
}


void addi(char *line, int index){
    // addi r1, Imd, r2   =>   r2 = r1 + Imd

    //coloca instrucao
    strcpy(commands[linhaAtual].instruction, "addi");

    index = getParam(line, index);  //pega primeiro parametro

    //verifica qual é o primeiro parametro e armazena de acordo
    char aux[5];
    sprintf(aux, "$%d,", getRegister(regist));   //formata a string
    strcpy(commands[linhaAtual].r1, aux);   //insere na struct na linha certa

    index = getParam(line, index);  //pega segundo parametro

    if (strcmp(regist, "PC") == 0)
    {
        strcpy(commands[linhaAtual].r2, "PC");
    }
    else {
        strcpy(commands[linhaAtual].r2, regist);    //como é um imediato, só armazena o próprio numero
    }


    getLastParam(line, index);  //pega ultimo parametro

    sprintf(aux, ",$%d", getRegister(regist));   //formata a string
    strcpy(commands[linhaAtual].rs, aux);   //insere na struct na linha certa
}

void jump(char *line, int index){
    // jump r1   =>   jump r1 (r1 = algum endereço)

    //coloca instrucao
    strcpy(commands[linhaAtual].instruction, "jump");

    index = getParam(line, index);  //pega primeiro parametro

    strcpy(commands[linhaAtual].r1, regist);    //coloca o nome da label no primeiro parametro
                                                //vou tratar isso depois
    strcpy(commands[linhaAtual].r2, ""); 
    strcpy(commands[linhaAtual].rs, ""); 
}


void label(char *line, int index){
    // label r1   =>   label r1 (r1 = algum endereço)

    //Não tem instrução pro assembly

    int j = 0, i;
    for (i = index+1; i < 40 && line[i] != ',' ; i++) //pega parametro
    {
        regist[j] = line[i];
        j++;
    }
    regist[j] = '\0';

    if (strcmp(regist, "ain") == 0)
    {
        labelMain = linhaAtual;
    } else {
        labelPos[atoi(regist)] = linhaAtual;
    }

    index = getParam(line, index);  //pega primeiro parametro
    strcpy(commands[linhaAtual].label, regist);

    linhaAtual--;
}

void funcao(char *line, int index){
    // armazeno qual é a label relacionada à essa funcao

    //Não tem instrução pro assembly

    index = getParam(line, index);  //pega primeiro parametro
    index = getParam(line, index);  //pega segundo parametro -> nome da função


    if (strcmp(regist, "main") != 0)
    {
        //aloco mais uma posicao pro vetor
        posLabelFun = (labelFun *) realloc(posLabelFun, (qtdFunc+1) * sizeof(labelFun));

        //coloco o nome da funcao no vetor
        strcpy(posLabelFun[qtdFunc].funName, regist);
        posLabelFun[qtdFunc].posLabel = linhaAtual;

        qtdFunc++;
    }

    linhaAtual--;
    
}

void load(char *line, int index){
    // load Imd(r2), rs   =>   rs = val[r2 + Imd]
    //                        r2 = desloc (0 ou registrador)

    //coloca instrucao
    strcpy(commands[linhaAtual].instruction, "load");

    index = getParam(line, index);  //pega primeiro parametro

    strcpy(commands[linhaAtual].r1, regist);    //como é um imediato, só armazena o próprio numero

    index = getParam(line, index);  //pega segundo parametro

    char aux[10];
    sprintf(aux, "($%d),", getRegister(regist));   //formata a string
    strcpy(commands[linhaAtual].r2, aux);


    getLastParam(line, index);  //pega ultimo parametro

    sprintf(aux, "$%d", getRegister(regist));   //formata a string
    strcpy(commands[linhaAtual].rs, aux);
}

void store(char *line, int index){
    // store r1,Imd(r2)   =>   val[r2 + Imd] = r1
    //                         r2 = desloc (0 ou registrador)

    //coloca instrucao
    strcpy(commands[linhaAtual].instruction, "store");

    index = getParam(line, index);  //pega primeiro parametro

    char aux[10];
    sprintf(aux, "$%d,", getRegister(regist));   //formata a string
    strcpy(commands[linhaAtual].r1, aux); 

    index = getParam(line, index);  //pega segundo parametro

    strcpy(commands[linhaAtual].r2, regist);    //como é um imediato, só armazena o próprio numero

    getLastParam(line, index);  //pega ultimo parametro

    sprintf(aux, "($%d)", getRegister(regist));   //formata a string
    strcpy(commands[linhaAtual].rs, aux);
}

void call(char *line, int index){
    // jal func     =>      func = posLabelFun.posLabel

    //coloca instrucao
    strcpy(commands[linhaAtual].instruction, "jal");

    index = getParam(line, index);  //pega segundo parametro


    if (strcmp(regist, "input") == 0)
    {
        //coloca instrucao
        strcpy(commands[linhaAtual].instruction, "in");
        strcpy(commands[linhaAtual].r1, "$5");  // $a1
    }
    else if (strcmp(regist, "output") == 0){
        //coloca instrucao
        strcpy(commands[linhaAtual].instruction, "out");

        index = getParam(line, index);  //pega terceiro parametro
        char aux[5];
        sprintf(aux, "$%d", getRegister(regist));   //formata a string
        strcpy(commands[linhaAtual].r1, aux); //coloca o temporario para printar
    }
    else {
        int i = 0;
        while ( strcmp(posLabelFun[i].funName, regist) != 0 )
        {
            i++;
        }
        char aux[5];
        sprintf(aux, "%d", posLabelFun[i].posLabel);   //formata a string
        strcpy(commands[linhaAtual].r1, aux); //coloca a pos do label da funcao 

        //Atribui o valor de PC+1 ao determinado lugar
        int j;
        for (j = linhaAtual; strcmp(commands[j].r2, "PC") != 0; j--)    //itera pra tras procurando onde está o PC
        { }
        sprintf(aux, "%d", linhaAtual+1);
        strcpy(commands[j].r2, aux);
        
    }

    strcpy(commands[linhaAtual].r2, ""); 
    strcpy(commands[linhaAtual].rs, ""); 
      
}

void branchNE(char *line, int index){
    // bne r1,r2,Imd     =>      r1 != r2 ? L

    //coloca instrucao
    strcpy(commands[linhaAtual].instruction, "bne");

    index = getParam(line, index);  //pega primeiro parametro

    //verifica qual é o primeiro parametro e armazena de acordo
    char aux[5];
    sprintf(aux, "$%d,", getRegister(regist));   //formata a string
    strcpy(commands[linhaAtual].r1, aux);   //insere na struct na linha certa

    index = getParam(line, index);  //pega segundo parametro

    sprintf(aux, "$%d,", getRegister(regist));   //formata a string
    strcpy(commands[linhaAtual].r2, aux);   //insere na struct na linha certa

    getLastParam(line, index);  //pega ultimo parametro

    strcpy(commands[linhaAtual].rs, regist);    //coloca o nome da label no ultimo parametro
                                                //vou tratar isso depois

}

void branchEQ(char *line, int index){
    // beq r1,r2,Imd     =>      r1 == r2 ? L

    //coloca instrucao
    strcpy(commands[linhaAtual].instruction, "beq");

    index = getParam(line, index);  //pega primeiro parametro

    //verifica qual é o primeiro parametro e armazena de acordo
    char aux[5];
    sprintf(aux, "$%d,", getRegister(regist));   //formata a string
    strcpy(commands[linhaAtual].r1, aux);   //insere na struct na linha certa

    index = getParam(line, index);  //pega segundo parametro

    sprintf(aux, "$%d,", getRegister(regist));   //formata a string
    strcpy(commands[linhaAtual].r2, aux);   //insere na struct na linha certa

    getLastParam(line, index);  //pega ultimo parametro

    strcpy(commands[linhaAtual].rs, regist);    //coloca o nome da label no ultimo parametro
                                                //vou tratar isso depois

}

void branchGR(char *line, int index){
    // bgt r1,r2,Imd     =>      r1 > r2 ? L

    //coloca instrucao
    strcpy(commands[linhaAtual].instruction, "bgt");

    index = getParam(line, index);  //pega primeiro parametro

    //verifica qual é o primeiro parametro e armazena de acordo
    char aux[5];
    sprintf(aux, "$%d,", getRegister(regist));   //formata a string
    strcpy(commands[linhaAtual].r1, aux);   //insere na struct na linha certa

    index = getParam(line, index);  //pega segundo parametro

    sprintf(aux, "$%d,", getRegister(regist));   //formata a string
    strcpy(commands[linhaAtual].r2, aux);   //insere na struct na linha certa

    getLastParam(line, index);  //pega ultimo parametro

    strcpy(commands[linhaAtual].rs, regist);    //coloca o nome da label no ultimo parametro
                                                //vou tratar isso depois

}

void branchLS(char *line, int index){
    // blt r1,r2,Imd     =>      r1 < r2 ? L

    //coloca instrucao
    strcpy(commands[linhaAtual].instruction, "blt");

    index = getParam(line, index);  //pega primeiro parametro

    //verifica qual é o primeiro parametro e armazena de acordo
    char aux[5];
    sprintf(aux, "$%d,", getRegister(regist));   //formata a string
    strcpy(commands[linhaAtual].r1, aux);   //insere na struct na linha certa

    index = getParam(line, index);  //pega segundo parametro

    sprintf(aux, "$%d,", getRegister(regist));   //formata a string
    strcpy(commands[linhaAtual].r2, aux);   //insere na struct na linha certa

    getLastParam(line, index);  //pega ultimo parametro

    strcpy(commands[linhaAtual].rs, regist);    //coloca o nome da label no ultimo parametro
                                                //vou tratar isso depois

}

void branchGE(char *line, int index){
    // branch greater or equal
    // bge r1,r2,Imd     =>      r2 >= r1 ? L

    //coloca instrucao
    strcpy(commands[linhaAtual].instruction, "bge");

    index = getParam(line, index);  //pega primeiro parametro

    //verifica qual é o primeiro parametro e armazena na segunda posicao
    char aux[5];
    sprintf(aux, "$%d,", getRegister(regist));   //formata a string
    strcpy(commands[linhaAtual].r1, aux);   //insere na struct na linha certa

    index = getParam(line, index);  //pega segundo parametro e armazena na primeira posicao

    sprintf(aux, "$%d,", getRegister(regist));   //formata a string
    strcpy(commands[linhaAtual].r2, aux);   //insere na struct na linha certa

    getLastParam(line, index);  //pega ultimo parametro

    strcpy(commands[linhaAtual].rs, regist);    //coloca o nome da label no ultimo parametro
                                                //vou tratar isso depois

}

void branchLE(char *line, int index){
    // branch less or equal
    // ble r1,r2,Imd     =>      r1 > r2 ? L

    //coloca instrucao
    strcpy(commands[linhaAtual].instruction, "ble");

    index = getParam(line, index);  //pega primeiro parametro

    //verifica qual é o primeiro parametro e armazena de acordo
    char aux[5];
    sprintf(aux, "$%d,", getRegister(regist));   //formata a string
    strcpy(commands[linhaAtual].r1, aux);   //insere na struct na linha certa

    index = getParam(line, index);  //pega segundo parametro

    sprintf(aux, "$%d,", getRegister(regist));   //formata a string
    strcpy(commands[linhaAtual].r2, aux);   //insere na struct na linha certa

    getLastParam(line, index);  //pega ultimo parametro

    strcpy(commands[linhaAtual].rs, regist);    //coloca o nome da label no ultimo parametro
                                                //vou tratar isso depois

}

void sohPassa(char *line, int index){
    // nao tem instrucao pro assembly
    linhaAtual--;

}

void add(char *line, int index){
    // add r1,r2,rs   =>   rs = r1 + r2

    //coloca instrucao
    strcpy(commands[linhaAtual].instruction, "add");

    index = getParam(line, index);  //pega primeiro parametro

    //verifica qual é o primeiro parametro e armazena de acordo
    char aux[5];
    sprintf(aux, "$%d,", getRegister(regist));   //formata a string
    strcpy(commands[linhaAtual].r1, aux);   //insere na struct na linha certa

    index = getParam(line, index);  //pega segundo parametro

    sprintf(aux, "$%d,", getRegister(regist));   //formata a string
    strcpy(commands[linhaAtual].r2, aux);   //insere na struct na linha certa

    getLastParam(line, index);  //pega ultimo parametro

    sprintf(aux, "$%d", getRegister(regist));   //formata a string
    strcpy(commands[linhaAtual].rs, aux);   //insere na struct na linha certa
}

void sub(char *line, int index){
    // sub r1,r2,rs   =>   rs = r1 - r2

    //coloca instrucao
    strcpy(commands[linhaAtual].instruction, "sub");

    index = getParam(line, index);  //pega primeiro parametro

    //verifica qual é o primeiro parametro e armazena de acordo
    char aux[5];
    sprintf(aux, "$%d,", getRegister(regist));   //formata a string
    strcpy(commands[linhaAtual].r1, aux);   //insere na struct na linha certa

    index = getParam(line, index);  //pega segundo parametro

    sprintf(aux, "$%d,", getRegister(regist));   //formata a string
    strcpy(commands[linhaAtual].r2, aux);   //insere na struct na linha certa

    getLastParam(line, index);  //pega ultimo parametro

    sprintf(aux, "$%d", getRegister(regist));   //formata a string
    strcpy(commands[linhaAtual].rs, aux);   //insere na struct na linha certa
}

void divisao(char *line, int index){
    // div r1,r2,rs   =>   rs = r1 / r2

    //coloca instrucao
    strcpy(commands[linhaAtual].instruction, "div");

    index = getParam(line, index);  //pega primeiro parametro

    //verifica qual é o primeiro parametro e armazena de acordo
    char aux[5];
    sprintf(aux, "$%d,", getRegister(regist));   //formata a string
    strcpy(commands[linhaAtual].r1, aux);   //insere na struct na linha certa

    index = getParam(line, index);  //pega segundo parametro

    sprintf(aux, "$%d,", getRegister(regist));   //formata a string
    strcpy(commands[linhaAtual].r2, aux);   //insere na struct na linha certa

    getLastParam(line, index);  //pega ultimo parametro

    sprintf(aux, "$%d", getRegister(regist));   //formata a string
    strcpy(commands[linhaAtual].rs, aux);   //insere na struct na linha certa
}

void mult(char *line, int index){
    // mult r1,r2,rs   =>   rs = r1 * r2

    //coloca instrucao
    strcpy(commands[linhaAtual].instruction, "mult");

    index = getParam(line, index);  //pega primeiro parametro

    //verifica qual é o primeiro parametro e armazena de acordo
    char aux[5];
    sprintf(aux, "$%d,", getRegister(regist));   //formata a string
    strcpy(commands[linhaAtual].r1, aux);   //insere na struct na linha certa

    index = getParam(line, index);  //pega segundo parametro

    sprintf(aux, "$%d,", getRegister(regist));   //formata a string
    strcpy(commands[linhaAtual].r2, aux);   //insere na struct na linha certa

    getLastParam(line, index);  //pega ultimo parametro

    sprintf(aux, "$%d", getRegister(regist));   //formata a string
    strcpy(commands[linhaAtual].rs, aux);   //insere na struct na linha certa
}


void getInstruction(char *read){
    char instruction[15];
    int j = 0, i;
    for (i = 1; read[i] != ',' && i < 15; i++)  //pega o primeiro parametro da quadrupla
    {
        instruction[j] = read[i];
        j++;
    }
    i++;
    instruction[j] = '\0';

    //verifica qual comando é e chama a respectiva função
    if (strcmp(instruction, "addi") == 0) 
    {
        addi(read, i);
    } 
    else if (strcmp(instruction, "goto") == 0)
    {
        jump(read, i);
    }
    else if (strcmp(instruction, "lab") == 0)
    {
        label(read, i);
    }
    else if (strcmp(instruction, "func") == 0)
    {
        funcao(read, i);
    }
    else if (strcmp(instruction, "load") == 0)
    {
        load(read, i);
    }
    else if (strcmp(instruction, "store") == 0)
    {
        store(read, i);
    }
    else if (strcmp(instruction, "call") == 0)
    {
        call(read, i);
    }
    else if (strcmp(instruction, "if_dif") == 0)
    {
        branchNE(read, i);
    }
    else if (strcmp(instruction, "if_igual") == 0)
    {
        branchEQ(read, i);
    }
    else if (strcmp(instruction, "if_maigual") == 0)
    {
        branchGE(read, i);
    }
    else if (strcmp(instruction, "if_maior") == 0)
    {
        branchGR(read, i);
    }
    else if (strcmp(instruction, "if_meigual") == 0)
    {
        branchLE(read, i);
    }
    else if (strcmp(instruction, "if_menor") == 0)
    {
        branchLS(read, i);
    }
    else if (strcmp(instruction, "ret") == 0 || strcmp(instruction, "fim_func") == 0 || strcmp(instruction, "halt") == 0)
    {
        sohPassa(read, i);
    }
    else if (strcmp(instruction, "sum") == 0)
    {
        add(read, i);
    }
    else if (strcmp(instruction, "sub") == 0)
    {
        sub(read, i);
    }
    else if (strcmp(instruction, "div") == 0)
    {
        divisao(read, i);
    }
    else if (strcmp(instruction, "mult") == 0)
    {
        mult(read, i);
    }



    else {
         strcpy(commands[linhaAtual].instruction, instruction);
    }
    
}


void printFile(){
    for (int i = 1; i < linhaAtual; i++)
    {
        fprintf(code2, "%s %s%s%s", commands[i].instruction, commands[i].r1, commands[i].r2, commands[i].rs); 
        if (commands[i].label[0] == 'L' || strcmp(commands[i].label, "main") == 0)
        {
            fprintf(code2, "\t//%s", commands[i].label);   //printa nome da label
        }
        
        fprintf(code2, "\n");    
    }
}

void fixLabels(){
    char aux[10];
    int j;
    for (int i = 0; i < linhaAtual; i++)    //percorre todas as instruções procurando quais são de jump
    {
        if(strcmp(commands[i].instruction, "jump") == 0){   //achando o jump, coloca as posições das labels corretamente
            if (commands[i].r1[0] == 'L')   
            {
                j = 1;
                while(commands[i].r1[j] != '\0'){   //pego o número da label
                    aux[j-1] = commands[i].r1[j];
                    j++;
                }
                aux[j] = '\0';

                char aux2[10];
                sprintf(aux2, "%d", labelPos[atoi(aux)]);   //pego a posição salva dessa label
                strcpy(commands[i].r1, aux2);
            }
            else if(strcmp(commands[i].r1, "$ra") == 0){
                strcpy(commands[i].instruction, "jr");
                strcpy(commands[i].r1, "$31");
            } else if(strcmp(commands[i].r1, "main") == 0){
                char aux2[10];
                sprintf(aux2, "%d", labelMain);   //pego a posição salva da label do main
                strcpy(commands[i].r1, aux2);
            }

            //achando o branch, coloca as posições das labels corretamente
        } else if(strcmp(commands[i].instruction, "bne") == 0 || strcmp(commands[i].instruction, "beq") == 0 ||
                strcmp(commands[i].instruction, "blt") == 0 || strcmp(commands[i].instruction, "bgt") == 0 ||
                strcmp(commands[i].instruction, "ble") == 0 || strcmp(commands[i].instruction, "bge") == 0){  
            j = 1;
            while(commands[i].rs[j] != '\0'){   //pego o número da label
                aux[j-1] = commands[i].rs[j];
                j++;
            }
            aux[j] = '\0';

            char aux2[10];
            sprintf(aux2, "%d", labelPos[atoi(aux)]);   //pego a posição salva dessa label
            strcpy(commands[i].rs, aux2);
                    
        }
    }


}

void converteAssembly(){
    char read[51];
    commands = (assembly *) malloc(sizeof(assembly));
    labelPos = (int *) malloc(sizeof(int) * returnLabel()+1);
    posLabelFun = (labelFun *) malloc(sizeof(labelFun));

    while(fgets(read, 50, code) != NULL){   //Faz a leitura linha por linha

        while(read[0] != '(')
            fgets(read, 50, code);
        
        commands = (assembly *) realloc(commands, (linhaAtual+1) * sizeof(assembly));
        getInstruction(read);
        linhaAtual++;
    }
    fixLabels();
    printFile();

    free(commands);
    free(labelPos);
    free(posLabelFun);

}
