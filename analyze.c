#include "globals.h"
#include "symtab.h"
#include "analyze.h"
#include <string.h>

/* counter for variable memory locations */
static int location = 0;

//Controlar escopo na análise semântica
char *escopoAtual;

/* Procedure traverse is a generic recursive 
 * syntax tree traversal routine:
 * it applies preProc in preorder and postProc 
 * in postorder to tree pointed to by t
 */

static void traverse( NoArvore * t,
               void (* preProc) (NoArvore *),
               void (* postProc) (NoArvore *) )
{ 
    if (t != NULL)
    { 
        preProc(t);
        { 
            int i;
            for (i=0; i < MAXFILHOS; i++){
                traverse(t->filho[i],preProc,postProc);
            }
        }
        postProc(t);{
            traverse(t->irmao,preProc,postProc);
        }
    }
}

/* nullProc is a do-nothing procedure to 
 * generate preorder-only or postorder-only
 * traversals from traverse
 */
static void nullProc(NoArvore * t)
{ 
    if (t==NULL) return;
    else return;
}

//Printa erro de tipo
static void typeError(NoArvore * t, char * message)
{ 
  fprintf(listing,"Type error at line %d: %s\n",t->lineno, message);
  Error = TRUE;
}

//Printa erro de declaracao
static void declarationError(NoArvore * t, char * message)
{ 
  fprintf(listing,"Erro na linha %d: %s: %s\n",t->lineno, message, t->atr.nome);
  Error = TRUE;
}

/* Procedure insertNode inserts 
 * identifiers stored in t into 
 * the symbol table 
 */
static void insertNode( NoArvore * t)
{ 
  NoArvore *aux;
  switch (t->tipoNo)
  {
      case DECLARACAO:
      switch (t->tipo.dcl)
      { 
          case VARIAVEL:
              if (st_lookup(t->atr.nome) == -1){
                if (t->tipoExp == Void)
                {
                  declarationError(t, "Variavel declarada como void");
                }
                /* not yet in table, so treat as new definition */
                st_insert(t->atr.nome, t->lineno, location++, 3, t->tipoExp, t->atr.escopo, "nao", "nao");
                
              }
              else {
                if (st_lookup_scope(t->atr.nome, t->atr.escopo) == -1)  //escopo diferente
                {
                  //verifico se está no global
                  if (st_lookup_scope(t->atr.nome, "Escopo Global") == 1){
                    declarationError(t, "Nome de variavel já usado");
                  } else {
                    /* not yet in table, so treat as new definition */
                    st_insert(t->atr.nome, t->lineno, location++, 3, t->tipoExp, t->atr.escopo, "nao", "nao");
                  }
                  
                } else {
                  //já está na tabela, entao está declarando de novo
                  declarationError(t, "Variavel já declarada");
                }

              }
              break;

            case VETORt:
              if (st_lookup(t->atr.nome) == -1){
                if (t->tipoExp == Void)
                {
                  declarationError(t, "Variavel declarada como void");
                }
                /* not yet in table, so treat as new definition */
                st_insert(t->atr.nome, t->lineno, location, 3, t->tipoExp, t->atr.escopo, "nao", "sim");
                location += t->atr.len;
                
              }
              else {
                if (st_lookup_scope(t->atr.nome, t->atr.escopo) == -1)  //escopo diferente
                {
                  //verifico se está no global
                  if (st_lookup_scope(t->atr.nome, "Escopo Global") == 1){
                    declarationError(t, "Nome de variavel já usado");
                  } else {
                    /* not yet in table, so treat as new definition */
                    st_insert(t->atr.nome, t->lineno, location, 3, t->tipoExp, t->atr.escopo, "nao", "sim");
                    location += t->atr.len;
                  }
                  
                } else {
                  //já está na tabela, entao está declarando de novo
                  declarationError(t, "Variavel já declarada");
                }

              }
              break;

            case PARAMETRO:
              if (st_lookup(t->atr.nome) == -1){
                if (t->tipoExp == Void)
                {
                  declarationError(t, "Variavel declarada como void");
                }
                /* not yet in table, so treat as new definition */
                st_insert(t->atr.nome, t->lineno, location++, 3, t->tipoExp, t->atr.escopo, "sim", ".");
                
              }
              else {
                if (st_lookup_scope(t->atr.nome, t->atr.escopo) == -1)  //escopo diferente
                {
                  //verifico se está no global
                  if (st_lookup_scope(t->atr.nome, "Escopo Global") == 1){
                    declarationError(t, "Nome de variavel já usado");
                  } else {
                    /* not yet in table, so treat as new definition */
                    st_insert(t->atr.nome, t->lineno, location++, 3, t->tipoExp, t->atr.escopo, "sim", ".");
                  }
                  
                } else {
                  //já está na tabela, entao está declarando de novo
                  declarationError(t, "Variavel já declarada");
                }

              }
              break;

          case FUNCAO:
            //escopo nas declarações
            aux = t->filho[1];
            while(aux != NULL){
              aux->atr.escopo = t->atr.nome;
              aux = aux->irmao;
            }

            //escopo no parametro
            aux = t->filho[0];
            while(aux != NULL){
              if (aux->tipoNo == EXP){
                if (aux->tipo.exp == TIPO){
                  aux->atr.escopo = t->atr.nome;
                }
              }
              aux = aux->irmao;
            }
            
            if (st_lookup(t->atr.nome) == -1)
              /* not yet in table, so treat as new definition */
              st_insert(t->atr.nome, t->lineno, 0, 4, t->tipoExp, t->atr.escopo, ".", ".");
            else
              //nome inválido
              declarationError(t, "Nome já utilizado");
            break;

          case CALL:

            aux = t->filho[0];
            while(aux != NULL){
              aux->atr.escopo = t->atr.escopo;
              aux = aux->irmao;
            }

            if( strcmp(st_lookup_decl(t->atr.nome, "Escopo Global"), " ") == 0) {
              //Não existe a função na tabela
              declarationError(t, "A função chamada não existe ou foi declarada erroneamente");
              
            } 
            break;

          case WHILEt:
            //tratar escopo dentro do while
            aux = t->filho[0];
            while(aux != NULL){
              aux->atr.escopo = t->atr.escopo;
              aux = aux->irmao;
            }

            aux = t->filho[1];
            while(aux != NULL){
              aux->atr.escopo = t->atr.escopo;
              aux = aux->irmao;
            }

            break;

          case IFt:
            //tratar escopo dentro do if e do else
            aux = t->filho[0];
            while(aux != NULL){
              aux->atr.escopo = t->atr.escopo;
              aux = aux->irmao;
            }


            aux = t->filho[1];
            while(aux != NULL){
              aux->atr.escopo = t->atr.escopo;
              aux = aux->irmao;
            }

            aux = t->filho[2];
            while(aux != NULL){
              aux->atr.escopo = t->atr.escopo;
              aux = aux->irmao;
            }

            break;

          case IGUALt:
            //tratar escopo dos dois filhos
            aux = t->filho[0];
            while(aux != NULL){
              aux->atr.escopo = t->atr.escopo;
              aux = aux->irmao;
            }

            aux = t->filho[1];
            while(aux != NULL){
              aux->atr.escopo = t->atr.escopo;
              aux = aux->irmao;
            }

            break;

          case RETURNt:
            //tratar escopo do filho
            aux = t->filho[0];
            while(aux != NULL){
              aux->atr.escopo = t->atr.escopo;
              aux = aux->irmao;
            }

            break;

          default:
              break;
      }
      break;
      case EXP:
          switch (t->tipo.exp)
          { 
            case IDt:
              if (st_lookup(t->atr.nome) == -1)
              /* não está na tabela, então não foi declarada */
                declarationError(t, "Variavel nao declarada");
              else {
                  //verifico se está no global
                  if (st_lookup_scope(t->atr.nome, "Escopo Global") == 1){
                    /* already in table, so ignore location, 
                    add line number of use only */ 
                    st_insert(t->atr.nome,t->lineno,0, 3, t->tipoExp, "Escopo Global", ".", "nao");
                  } else {
                    st_insert(t->atr.nome,t->lineno,0, 3, t->tipoExp, t->atr.escopo, ".", "nao");                    
              
                  }
                  if (st_lookup_vetor(t->atr.nome, t->atr.escopo) == "sim"){
                    t->tipo.exp = VETOR;
                  } else if (st_lookup_vetor(t->atr.nome, "Escopo Global") == "sim"){
                    t->tipo.exp = VETOR;
                  }

              }
              break;

            case TIPO:
              t->filho[0]->atr.escopo = t->atr.escopo;
              break;

            case VETOR:
              aux = t->filho[0];
              while(aux != NULL){
                aux->atr.escopo = t->atr.escopo;
                aux = aux->irmao;
              }

              if (st_lookup(t->atr.nome) == -1)
              /* não está na tabela, então não foi declarada */
                declarationError(t, "Variavel nao declarada");
              else {
                  //verifico se está no global
                  if (st_lookup_scope(t->atr.nome, "Escopo Global") == 1){
                    /* already in table, so ignore location, 
                    add line number of use only */ 
                    st_insert(t->atr.nome,t->lineno,0, 3, t->tipoExp, "Escopo Global", ".", "sim");
                  } else {
                    st_insert(t->atr.nome,t->lineno,0, 3, t->tipoExp, t->atr.escopo, ".", "sim");                    
              
                  }

              }
              break;
              
            
            case OPERACAO:
              //tratar escopo dos dois filhos
              aux = t->filho[0];
              while(aux != NULL){
                aux->atr.escopo = t->atr.escopo;
                aux = aux->irmao;
              }

              aux = t->filho[1];
              while(aux != NULL){
                aux->atr.escopo = t->atr.escopo;
                aux = aux->irmao;
              }

            break;

            default:
              break;
          }
          break;

      default:
        break;
  }
}

/* Function buildSymtab constructs the symbol 
 * table by preorder traversal of the syntax tree
 */
void buildSymtab(NoArvore * syntaxTree)
{ 
  st_insert("input", 0, 0, 4, 1, "Escopo Global", ".", ".");
  st_insert("output", 0, 0, 4, 1, "Escopo Global", ".", ".");
  NoArvore *aux = syntaxTree->irmao;
  syntaxTree->atr.escopo = "Escopo Global";
  while(aux != NULL){
    if (aux->tipoNo == EXP){
      if (aux->tipo.exp == TIPO){
        aux->atr.escopo = "Escopo Global";
      }
    }
    aux = aux->irmao;
  }
    
    traverse(syntaxTree, insertNode, nullProc);
    if (TraceAnalyze)
    { 
        fprintf(listing,"\nTabela de Simbolos:\n\n");
        printSymTab(listing);
    }
}


/* Procedure checkNode performs
 * type checking at a single tree node
 */
static void checkNode(NoArvore * t)
{ 
  //verifica escopo atual
  if (t->atr.escopo != NULL)
  {
    escopoAtual = t->atr.escopo;
  }

  switch (t->tipoNo)
  { 
    case EXP:
      switch (t->tipo.exp)
      { 
        case OPERACAO:
          if ((t->filho[0]->tipoExp != INTEGER) || (t->filho[1]->tipoExp != INTEGER))
            typeError(t,"Op applied to non-integer");
          if ((t->atr.op == COMP) || (t->atr.op == MEIGUAL) || (t->atr.op == MAIOR) ||
           (t->atr.op == MAIGUAL) || (t->atr.op == MENOR) || (t->atr.op == DIF))
            t->tipoExp = BOOLEAN;
          else
            t->tipoExp = INTEGER;
          break;

        case CONSTANTE:
          t->tipoExp = INTEGER;
          break;

        case IDt:
          if (strcmp(st_lookup_type(t->atr.nome, escopoAtual), "int") == 0) //se existe nesse escopo como int
          {
            t->tipoExp = INTEGER;
          } 
          else if (strcmp(st_lookup_type(t->atr.nome, "Escopo Global"), "int") == 0)  //se existe no global como int
          {
            t->tipoExp = INTEGER;
          }
          else if (strcmp(st_lookup_type(t->atr.nome, escopoAtual), " ") == 0 
                  || strcmp(st_lookup_type(t->atr.nome, "Escopo Global"), " ") == 0) {  //se existe no escopo atual ou global como outro tipo
            t->tipoExp = Void;
          }
          else  //não foi declarada
          {
            typeError(t,"Variável não declarada");
          }

        case VETOR:
          if (strcmp(st_lookup_type(t->atr.nome, escopoAtual), "int") == 0) //se existe nesse escopo como int
          {
            t->tipoExp = INTEGER;
          } 
          else if (strcmp(st_lookup_type(t->atr.nome, "Escopo Global"), "int") == 0)  //se existe no global como int
          {
            t->tipoExp = INTEGER;
          }
          else if (strcmp(st_lookup_type(t->atr.nome, escopoAtual), " ") == 0 
                  || strcmp(st_lookup_type(t->atr.nome, "Escopo Global"), " ") == 0) {  //se existe no escopo atual ou global como outro tipo
            t->tipoExp = Void;
          }
          else  //não foi declarada
          {
            typeError(t,"Variável não declarada");
          }
          
          break;
        default:
          break;
      }
      break;

    case DECLARACAO:
      switch (t->tipo.dcl)
      { 
        case IFt:
          if (t->filho[0]->tipoExp == INTEGER)
            typeError(t->filho[0],"A condição não é boolean");
          break;

        case IGUALt:
          if (t->filho[0]->tipoExp != INTEGER)
            typeError(t->filho[0],"Atribuição para um não inteiro");
          break;
        default:
          break;
      }
      break;
    default:
      break;

  }
}

// Verifica se a função main existe
void searchMain(){
  if (st_lookup_scope("main", "Escopo Global") == -1){
    printf("Função main não declarada.\n\n");
    Error = TRUE;
  }
}

/* Procedure typeCheck performs type checking 
 * by a postorder syntax tree traversal
 */
void typeCheck(NoArvore * syntaxTree)
{ 
  searchMain();
  traverse(syntaxTree,nullProc,checkNode);
}

int getLocation(){
  return location;
}