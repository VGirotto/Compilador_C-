#include "globals.h"
#include "symtab.h"
#include "code.h"
#include "cgen.h"
#include "analyze.h"
#include <string.h>

/* tmpOffset is the memory offset for temps */
static int tmpOffset = 0;

// Controla os nomes dos labels
static int labelName = 0;

//Guarda o label da funcao atual
static int funcAtual = 0;

/* prototype for internal recursive code generator */
static int cGen (NoArvore * t);

static int callGen( NoArvore * t);

void checkWhile(NoArvore * t, int label);

int cont;   //contador de argumentos de funcao

/* Procedure genStmt generates code at a statement node */
static int genStmt( NoArvore * t)
{ 
   NoArvore * p1, * p2, * p3;
   int aux1, aux2, aux3;
   int labAux, labAux2;
   switch (t->tipo.dcl) {

      case FUNCAO:
         if(labelName == 0) emitLabel("goto", -1, "Inicia o programa indo para o main");

         if (TraceCode) emitComment("-> funcao");
         tmpOffset = 0;
         p1 = t->filho[0];
         p2 = t->filho[1];

         funcAtual = labelName++;

         emitNome("func", t->tipoExp, t->atr.nome, "_", "cabeçalho de funçao");

         //label do inicio da função
         if (strcmp(t->atr.nome, "main") == 0)
         {
            emitLabel("lab", -1, "label da funcao main");
         }else {
            emitLabel("lab", labelName++, "label da funcao");
         }         



         //conta os parametros
         cont = 0;
         NoArvore *pAux = t->filho[0];
         while (pAux != NULL)
         {
            cont++;
            pAux = pAux->irmao;
         } 

         if (cont > 0){
            for (int i = 0; i < cont; i++)   //se existirem, desempilha cada argumento e coloca num temporario
            {
               fprintf(code, "(load,0,$sp,$t%d)", i);
               if (TraceCode) fprintf(code, "\tLoad argumento");
               fprintf(code, "\n");
               fprintf(code, "(addi,_,1,$t%d)", ++tmpOffset);
               if (TraceCode) fprintf(code, "\t-1 em sp");
               fprintf(code, "\n");
               fprintf(code, "(sub,$sp,$t%d,$sp)", tmpOffset);
               if (TraceCode) fprintf(code, "\t-1 em sp");
               fprintf(code, "\n");
            }
            cont--;
         }

         cGen(p1);   //params
         
         cGen(p2);   //operacoes internas

         //label do fim da funcao
         emitLabel("lab", funcAtual, "label de fim da funcao");

         tmpOffset = 0;

         //Faz o retorno de onde foi chamado
         if (strcmp(t->atr.nome, "main") != 0)  //se nao for a main
         {
            //desempilho para obter o ra
            fprintf(code, "(load,0,$sp,$ra)");
            if (TraceCode) fprintf(code, "\tLoad do endereço de retorno ($ra)");
            fprintf(code, "\n");
            fprintf(code, "(addi,_,1,$t%d)", tmpOffset);
            if (TraceCode) fprintf(code, "\t-1 em sp");
            fprintf(code, "\n");
            fprintf(code, "(sub,$sp,$t%d,$sp)", tmpOffset++);
            if (TraceCode) fprintf(code, "\t-1 em sp");
            fprintf(code, "\n");

            fprintf(code, "(goto,$ra,_,_)");
            if (TraceCode) fprintf(code, "\tJump register");
            fprintf(code, "\n");
         }  

         emitNome("fim_func", -1, t->atr.nome, "_", "final da funcao");
         break;

      case PARAMETRO:
         if (TraceCode) emitComment("-> parametro");
         fprintf(code, "(store,$t%d,%d,0)", cont--, st_lookup_location(t->atr.nome, t->atr.escopo));
         if (TraceCode) fprintf(code, "\tStore do valor do argumento na location do parametro");
         fprintf(code, "\n");
         break;

      case VARIAVEL:
         if (TraceCode) emitComment("-> declara variavel");
         fprintf(code, "(store,0,%d,0)", st_lookup_location(t->atr.nome, t->atr.escopo));
         if (TraceCode) fprintf(code, "\tStore de valor 0 na location da variavel");
         fprintf(code, "\n");
         break;
      
      case VETORt:
         if (TraceCode) emitComment("-> declara vetor");
         fprintf(code, "(store,0,%d,0)", st_lookup_location(t->atr.nome, t->atr.escopo));
         if (TraceCode) fprintf(code, "\tStore de valor 0 na location do vetor no index 0");
         fprintf(code, "\n");
         break;

      case IFt:
         if (TraceCode) emitComment("-> if") ;
         p1 = t->filho[0];
         p2 = t->filho[1];
         p3 = t->filho[2];

         /* generate code for test expression */
         aux1 = cGen(p1);
         //Verifica condição -> verifica as 6 possibilidade e printa o if de acordo
         //Tem que mudar pro while tbm

         tmpOffset = 0; //zera os temporarios

         /* se for true */
         cGen(p2);

         //goto fim do if
         emitLabel("goto", labelName+1, "Pula pro fim do if");

         tmpOffset = 0; //zera os temporarios
         
         /* recurse on else part */
         emitLabel("lab", labelName++, "label do else");
         cGen(p3);
         
         emitLabel("lab", labelName++, "label de fim do if");
         if (TraceCode)  emitComment("<- if") ;
         break; /* if_k */

      case WHILEt:
         if (TraceCode) emitComment("-> while") ;
         p1 = t->filho[0];
         p2 = t->filho[1];

         labAux = labelName;  //fim do while
         
         /* generate code for test expression */
         aux1 = cGen(p1);  

         labelName++;

         labAux2 = labelName; //inicio do while

         emitLabel("lab", labelName++, "Inicio do while");

         tmpOffset = 0; //zera os temporarios

         /* recurse on then part */
         cGen(p2);

         tmpOffset = 0; //zera os temporarios

         //Verifica condicao novamente
         checkWhile(p1, labAux2);   //pula pro label do inicio do while se for verdadeiro
         
         tmpOffset = 0; //zera os temporarios

         emitLabel("lab", labAux, "label de fim do while");
         if (TraceCode)  emitComment("<- while") ;
         break; 

      case RETURNt:
         if (TraceCode) emitComment("-> return");
         p1 = t->filho[0];
         /* gera código pro valor de retorno */
         aux1 = cGen(p1);

         if (p1->tipo.dcl != CALL){
            //copia temporario para $a1
            fprintf(code, "(addi,$t%d,0,$a1)", aux1);
            if (TraceCode) fprintf(code, "\tCopia retorno para a1");
            fprintf(code, "\n");
         }
         

         //gera Return
         fprintf(code, "(ret,$a1,_,_)");
         if (TraceCode) fprintf(code, "\tRetorno de funcao");
         fprintf(code, "\n");

         //goto fim da funcao
         emitLabel("goto", funcAtual, "Pula pro fim da funcao");

         tmpOffset = 0; //zera os temporarios

         if (TraceCode)  emitComment("<- return");
         break;


      case IGUALt:
         if (TraceCode) emitComment("-> atribui") ;

         //gera código pro elemento da direita
         aux2 = cGen(t->filho[1]);
         

         if (t->filho[0]->tipo.exp == VETOR)
         {
            if (strcmp(st_lookup_param(t->filho[0]->atr.nome, t->filho[0]->atr.escopo), "sim") == 0) //se for parametro trata diferente
            {
               //load do valor no vetor-parametro, esse valor é o location do vetor original
               aux1 = tmpOffset;
               fprintf(code, "(load,%d,0,$t%d)", st_lookup_location(t->filho[0]->atr.nome, t->filho[0]->atr.escopo), tmpOffset++); //load com offset
               if (TraceCode) fprintf(code, "\tLoad da localizacao do vetor original");
               fprintf(code, "\n");

               aux3 = cGen(t->filho[0]->filho[0]); //Pega o index

               //somo o valor que estava armazenado com o deslocamento do index
               emitR("sum", aux1, aux3, tmpOffset, "Soma");
  
               fprintf(code, "(store,$t%d,0,$t%d)", aux2, tmpOffset++);
               if (TraceCode) fprintf(code, "\tStore do valor atribuido");
               fprintf(code, "\n");
            } 
            else {
               aux1 = cGen(t->filho[0]->filho[0]); //Pega o index

               if (t->filho[1]->tipo.dcl == CALL) {
                  fprintf(code, "(addi,$a1,0,$t%d)", aux2);
                  if (TraceCode) fprintf(code, "\tRetorno de funcao");
                  fprintf(code, "\n");
               } 

               fprintf(code, "(store,$t%d,%d,$t%d)", aux2, st_lookup_location(t->filho[0]->atr.nome, t->atr.escopo), aux1);
               if (TraceCode) fprintf(code, "\tStore do valor atribuido");
               fprintf(code, "\n");

            }
         } 
         else{
            if (t->filho[1]->tipo.dcl == CALL) {
               fprintf(code, "(addi,$a1,0,$t%d)", aux2);
               if (TraceCode) fprintf(code, "\tRetorno de funcao");
               fprintf(code, "\n");
            } 

            fprintf(code, "(store,$t%d,%d,0)", aux2, st_lookup_location(t->filho[0]->atr.nome, t->atr.escopo));
            if (TraceCode) fprintf(code, "\tStore do valor atribuido");
            fprintf(code, "\n");
         }

         tmpOffset = 0; //zera os temporarios

         if (TraceCode)  emitComment("<- atribui") ;
         break; /* assign_k */


      case CALL:
         if (TraceCode) emitComment("-> call");

         //conta os parametros
         cont = 0;
         pAux = t->filho[0];
         while (pAux != NULL)
         {
            cont++;
            pAux = pAux->irmao;
         }  

         p1 = t->filho[0];

         int lenght;
         char ** varVet;
         //se for recursiva
         if (strcmp(t->atr.escopo, t->atr.nome) == 0){
            lenght = 0;
            varVet = getVariableVector(t->atr.escopo, &lenght);
            for (int i = 0; i < lenght; i++)
            {
               fprintf(code, "(load,%d,0,$t%d)", st_lookup_location(varVet[i], t->atr.escopo), i);
               if (TraceCode) fprintf(code, "\tLoad valor da variavel");
               fprintf(code, "\n");
               fprintf(code, "(store,$t%d,0,$sp)", i);
               if (TraceCode) fprintf(code, "\tStore valor na pilha");
               fprintf(code, "\n");
               fprintf(code, "(addi,$sp,1,$sp)");
               if (TraceCode) fprintf(code, "\t+1 em sp");
               fprintf(code, "\n");
            }
         }

         //se nao for input ou output empilha retorno e argumentos
         if (strcmp(t->atr.nome, "input") != 0 && strcmp(t->atr.nome, "output") != 0)  
         {
            //empilha o endereço de retorno ($ra)
            fprintf(code, "(addi,_,PC,$t%d)", tmpOffset);
            if (TraceCode) fprintf(code, "\t+1 em sp");
            fprintf(code, "\n"); 

            fprintf(code, "(store,$t%d,0,$sp)", tmpOffset++);
            if (TraceCode) fprintf(code, "\tStore do endereço de PC+1");
            fprintf(code, "\n");

            fprintf(code, "(addi,$sp,1,$sp)");
            if (TraceCode) fprintf(code, "\t+1 em sp");
            fprintf(code, "\n"); 
         
            //verifica os parametros
            aux1 = callGen(p1);

            //subtrai 1 em sp pra manter o padrao
            fprintf(code, "(addi,_,1,$t%d)", tmpOffset);
            if (TraceCode) fprintf(code, "\t-1 em sp");
            fprintf(code, "\n");
            fprintf(code, "(sub,$sp,$t%d,$sp)", tmpOffset++);
            if (TraceCode) fprintf(code, "\t-1 em sp");
            fprintf(code, "\n");

            //gera Call
            fprintf(code, "(call,%s,_,_)", t->atr.nome);
            if (TraceCode) fprintf(code, "\tChamada de funcao");
            fprintf(code, "\n");
         }
         else {
            //se for input ou output

            aux1 = cGen(p1);

            if (strcmp(t->atr.nome, "input") == 0){  //input
               //gera Call
               fprintf(code, "(call,%s,_,_)", t->atr.nome);
               if (TraceCode) fprintf(code, "\tChamada de funcao");
               fprintf(code, "\n");
            } else {

               //se o filho for tbm uma chamada, antes armazena o valor de $a1 num temp
               if (p1->tipo.dcl == CALL)
               {
                  fprintf(code, "(addi,$a1,0,$t%d)", aux1);
                  //fprintf(code, "(store,$a1,0,$t%d)", aux1);
                  if (TraceCode) fprintf(code, "\tStore retorno no temporario para output");
                  fprintf(code, "\n");
               }

               //gera Call
               fprintf(code, "(call,%s,$t%d,_)", t->atr.nome, aux1);
               if (TraceCode) fprintf(code, "\tChamada de funcao");
               fprintf(code, "\n");
            }
  

         }


         //se for recursivo, desempilha as variaveis aqui
         if (strcmp(t->atr.escopo, t->atr.nome) == 0){
            for (int i = lenght-1; i >= 0; i--)
            {
               fprintf(code, "(load,0,$sp,$t0)");
               if (TraceCode) fprintf(code, "\tLoad valor da pilha");
               fprintf(code, "\n");
               fprintf(code, "(store,$t0,%d,0)", st_lookup_location(varVet[i], t->atr.escopo));
               if (TraceCode) fprintf(code, "\tStore valor da variavel");
               fprintf(code, "\n");
               fprintf(code, "(addi,_,1,$t%d)", tmpOffset);
               if (TraceCode) fprintf(code, "\t-1 em sp");
               fprintf(code, "\n");
               fprintf(code, "(sub,$sp,$t%d,$sp)", tmpOffset++);
               if (TraceCode) fprintf(code, "\t-1 em sp");
               fprintf(code, "\n");
            }
         }


         if (TraceCode)  emitComment("<- call");
         return tmpOffset++;

         break; 

      default:
         break;
    }
} /* genStmt */

/* Procedure genExp generates code at an expression node */
static int genExp( NoArvore * t)
{ 
   int aux1, aux2, aux3;
   NoArvore * p1, * p2;
   switch (t->tipo.exp) {

      case TIPO:
         cGen(t->filho[0]);
         break;

      case CONSTANTE :
         if (TraceCode) emitComment("-> Const") ;
         emitI("addi", t->atr.nome, t->atr.val, tmpOffset, "Inicia constante");
         if (TraceCode)  emitComment("<- Const") ;
         return tmpOffset++;
         break; /* ConstK */
      
      case IDt :
         if (TraceCode) emitComment("-> Id");
         fprintf(code, "(load,%d,0,$t%d)", st_lookup_location(t->atr.nome, t->atr.escopo), tmpOffset); //load
         if (TraceCode) fprintf(code, "\tLoad ID");
         fprintf(code, "\n");
         if (TraceCode)  emitComment("<- Id") ;
         return tmpOffset++;

         break; /* IdK */

      case VETOR:
         if (TraceCode) emitComment("-> vetor") ;

         p1 = t->filho[0];
         aux1 = cGen(p1);       //pega index

         if (strcmp(st_lookup_param(t->atr.nome, t->atr.escopo), "sim") == 0) //se for parametro trata diferente
         {
            //load do valor no vetor-parametro, esse valor é o location do vetor original
            aux2 = tmpOffset;
            fprintf(code, "(load,%d,0,$t%d)", st_lookup_location(t->atr.nome, t->atr.escopo), tmpOffset++); //load com offset
            if (TraceCode) fprintf(code, "\tLoad do vetor original");
            fprintf(code, "\n");

            //somo o valor que estava armazenado com o deslocamento do index
            aux3 = tmpOffset;
            emitR("sum", aux1, aux2, tmpOffset++, "Soma");

            //uso o valor somado como deslocamento e deixo a location 0
            fprintf(code, "(load,0,$t%d,$t%d)", aux3, tmpOffset); //load com offset
            if (TraceCode) fprintf(code, "\tLoad do valor buscado no vetor original");
            fprintf(code, "\n");
         }
         else {
            fprintf(code, "(load,%d,$t%d,$t%d)", st_lookup_location(t->atr.nome, t->atr.escopo), aux1, tmpOffset); //load com offset
            if (TraceCode) fprintf(code, "\tLoad com deslocamento");
            fprintf(code, "\n");

         }
 

         if (TraceCode)  emitComment("<- vetor") ;
         return tmpOffset++;

         break;

      case OPERACAO :
            if (TraceCode) emitComment("-> Op") ;
            p1 = t->filho[0];
            p2 = t->filho[1];
            /* gen code for left arg */
            aux1 = cGen(p1);
            /* gen code for right operand */
            aux2 = cGen(p2);

            switch (t->atr.op) {
               case SUM :
                  emitR("sum", aux1, aux2, tmpOffset, "Soma");
                  break;

               case SUB :
                  emitR("sub", aux1, aux2, tmpOffset, "Subtrai");
                  break;

               case MULT :
                  emitR("mult", aux1, aux2, tmpOffset, "Multiplica");
                  break;

               case DIV :
                  emitR("div", aux1, aux2, tmpOffset, "Divide");
                  break;

               case MENOR :
                  fprintf(code, "(if_maigual,$t%d,$t%d,L%d)", aux1, aux2, labelName);
                  if (TraceCode) fprintf(code, "\tPula se for maior ou igual");
                  fprintf(code, "\n");
                  break;

               case MAIOR :
                  fprintf(code, "(if_meigual,$t%d,$t%d,L%d)", aux1, aux2, labelName);
                  if (TraceCode) fprintf(code, "\tPula se for menor ou igual");
                  fprintf(code, "\n");
                  break;

               case MEIGUAL :
                  fprintf(code, "(if_maior,$t%d,$t%d,L%d)", aux1, aux2, labelName);
                  if (TraceCode) fprintf(code, "\tPula se for maior");
                  fprintf(code, "\n");
                  break;

               case MAIGUAL :
                  fprintf(code, "(if_menor,$t%d,$t%d,L%d)", aux1, aux2, labelName);
                  if (TraceCode) fprintf(code, "\tPula se for menor");
                  fprintf(code, "\n");
                  break;

               case COMP:
                  fprintf(code, "(if_dif,$t%d,$t%d,L%d)", aux1, aux2, labelName);
                  if (TraceCode) fprintf(code, "\tPula se for diferente");
                  fprintf(code, "\n");
                  break;
               
               case DIF:
                  fprintf(code, "(if_igual,$t%d,$t%d,L%d)", aux1, aux2, labelName);
                  if (TraceCode) fprintf(code, "\tPula se for igual");
                  fprintf(code, "\n");
                  break;

               default:
                  emitComment("BUG: Unknown operator");
                  break;
            } /* case op */
            if (TraceCode)  emitComment("<- Op") ;
            return tmpOffset++;
            break; /* OpK */

      default:
         break;
  }
} /* genExp */

/* Procedure cGen recursively generates code by
 * tree traversal
 */
static int cGen( NoArvore * t)
{ 
   int auxTemp = -1;

   if (t != NULL)
  { 
      switch (t->tipoNo) {
         case DECLARACAO:
            auxTemp = genStmt(t);
            break;

         case EXP:
            auxTemp = genExp(t);
            break;

         default:
            break;
      }
    if(t->irmao != NULL) auxTemp = cGen(t->irmao);
  }
  return auxTemp;
}

/* Procedure callGen recursively generates code by
 * tree traversal for call function
 */
static int callGen( NoArvore * t)
{ 
   int auxTemp = -1, auxVet = 1;

   if (t != NULL)
  { 
      switch (t->tipoNo) {
         case DECLARACAO:
            auxTemp = genStmt(t);
            break;

         case EXP:
            if(t->tipo.exp == VETOR){  // se for vetor verifica se passa o endereço ou o próprio valor
               if ( t->filho[0] == NULL){

                  if (strcmp(st_lookup_param(t->atr.nome, t->atr.escopo), "nao") == 0 ||
                     strcmp(st_lookup_param(t->atr.nome, "Escopo Global"), "nao") == 0) //se nao for param
                  {
                     fprintf(code, "(addi,_,%d,$t%d)", st_lookup_location(t->atr.nome, t->atr.escopo), tmpOffset);
                     if (TraceCode) fprintf(code, "\tColoca posição num temporario");
                     fprintf(code, "\n");
                     fprintf(code, "(store,$t%d,0,$sp)", tmpOffset++);
                     auxVet = 0;
                  }
                  else {
                     fprintf(code, "(load,%d,0,$t%d)", st_lookup_location(t->atr.nome, t->atr.escopo), tmpOffset); //load
                     if (TraceCode) fprintf(code, "\tLoad Conteudo do vetor");
                     fprintf(code, "\n");
                     auxTemp = tmpOffset++;
                  }
                  
                  break;
               }
            }
            auxTemp = genExp(t);
            break;

         default:
            break;
      }
   if (auxVet) fprintf(code, "(store,$t%d,0,$sp)", auxTemp);
   if (TraceCode) fprintf(code, "\tStore do argumento na pilha");
   fprintf(code, "\n");
   fprintf(code, "(addi,$sp,1,$sp)");
   if (TraceCode) fprintf(code, "\t+1 em sp");
   fprintf(code, "\n");

   if(t->irmao != NULL){
      auxTemp = callGen(t->irmao);
   } 
   auxVet = 1;
  }
  return auxTemp;
}

void checkWhile(NoArvore * t, int label){
   NoArvore * p1, * p2;
   int aux1, aux2, aux3;
   if (TraceCode) emitComment("-> CheckWhile") ;
            p1 = t->filho[0];
            p2 = t->filho[1];
            /* gen code for left arg */
            aux1 = cGen(p1);
            /* gen code for right operand */
            aux2 = cGen(p2);

            switch (t->atr.op) {
               case MENOR :
                  fprintf(code, "(if_menor,$t%d,$t%d,L%d)", aux1, aux2, label);
                  if (TraceCode) fprintf(code, "\tPula se for menor");
                  fprintf(code, "\n");
                  break;

               case MAIOR :
                  fprintf(code, "(if_maior,$t%d,$t%d,L%d)", aux1, aux2, label);
                  if (TraceCode) fprintf(code, "\tPula se for maior");
                  fprintf(code, "\n");
                  break;

               case MEIGUAL :
                  fprintf(code, "(if_meigual,$t%d,$t%d,L%d)", aux1, aux2, label);
                  if (TraceCode) fprintf(code, "\tPula se for menor ou igual");
                  fprintf(code, "\n");
                  break;

               case MAIGUAL :
                  fprintf(code, "(if_maigual,$t%d,$t%d,L%d)", aux1, aux2, label);
                  if (TraceCode) fprintf(code, "\tPula se for maior ou igual");
                  fprintf(code, "\n");
                  break;

               case COMP:
                  fprintf(code, "(if_igual,$t%d,$t%d,L%d)", aux1, aux2, label);
                  if (TraceCode) fprintf(code, "\tPula se for igual");
                  fprintf(code, "\n");
                  break;
               
               case DIF:
                  fprintf(code, "(if_dif,$t%d,$t%d,L%d)", aux1, aux2, label);
                  if (TraceCode) fprintf(code, "\tPula se for diferente");
                  fprintf(code, "\n");
                  break;

               default:
                  emitComment("BUG: Unknown operator");
                  break;
            } /* case op */
            if (TraceCode)  emitComment("<- CheckWhile") ;
}

int returnLabel(){
   return labelName;
}


/**********************************************/
/* the primary function of the code generator */
/**********************************************/
/* Procedure codeGen generates code to a code
 * file by traversal of the syntax tree. The
 * second parameter (codefile) is the file name
 * of the code file, and is used to print the
 * file name as a comment in the code file
 */
void codeGen(NoArvore * syntaxTree, char * codefile)
{  
   char * s = malloc(strlen(codefile)+7);
   strcpy(s,"File: ");
   strcat(s,codefile);
   emitComment("Gerador de código intermediário");
   emitComment(s);

   fprintf(code, "Código intermediário:\n\n");
   
   //inicia ponteiro do topo da pilha
   fprintf(code, "(addi,_,%d,$sp)", getLocation()+2);
   if (TraceCode) fprintf(code, "\tInicia sp");
   fprintf(code, "\n");

   /* generate code */
   cGen(syntaxTree);
   /* finish */
   emitComment("End of execution.");
   emitNome("halt",-1,"_","_","Parada do codigo");
}
