#include "globals.h"
#include "util.h"


void printToken( TipoToken token, const char* tokenString )
{ 
  switch (token)
  { 
    case IF: fprintf(listing, "Reserved Word: %s\n",tokenString); break;
    case ELSE: fprintf(listing, "Reserved Word: %s\n",tokenString); break;
    case INT: fprintf(listing, "Reserved Word: %s\n",tokenString); break;
    case RETURN: fprintf(listing, "Reserved Word: %s\n",tokenString); break;
    case VOID: fprintf(listing, "Reserved Word: %s\n",tokenString); break;
    case WHILE: fprintf(listing, "Reserved Word: %s\n",tokenString); break;
    case IGUAL: fprintf(listing,"Symbol: =\n"); break;
    case MENOR: fprintf(listing,"<\n"); break;
    case COMP: fprintf(listing,"==\n"); break;
    case MAIOR: fprintf(listing,">\n"); break; 
    case MEIGUAL: fprintf(listing, "<=\n"); break;
    case MAIGUAL: fprintf(listing, ">=\n"); break; 
    case DIF: fprintf(listing, "!=\n"); break;
    case COLC1: fprintf(listing, "[\n"); break;
    case COLC2: fprintf(listing, "]\n"); break;
    case CH1: fprintf(listing, "{\n"); break;
    case CH2: fprintf(listing, "}\n"); break;	
    case PAR1: fprintf(listing,"(\n"); break;
    case PAR2: fprintf(listing,")\n"); break;
    case PVIRG: fprintf(listing,";\n"); break;
    case VIRG: fprintf(listing,",\n"); break;
    case SUM: fprintf(listing,"+\n"); break;
    case SUB: fprintf(listing,"-\n"); break;
    case MULT: fprintf(listing,"*\n"); break;
    case DIV: fprintf(listing,"/\n"); break;
    case FIM: fprintf(listing,"EOF\n"); break;
    case NUM: fprintf(listing,"Number: %s\n",tokenString); break;
    case ID: fprintf(listing,"ID: %s\n",tokenString); break;
    case ERROR: fprintf(listing,"ERROR: %s\n",tokenString); break;
    default: fprintf(listing,"Token Desconhecido: %d\n",token);
  }
}

NoArvore * newStmtNode(TipoDeclaracao tipo)
{ 
  NoArvore * t = (NoArvore *) malloc(sizeof(NoArvore));
  int i;
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else {
    for (i=0;i<MAXFILHOS;i++) t->filho[i] = NULL;
    t->irmao = NULL;
    t->tipoNo = DECLARACAO;
    t->tipo.dcl = tipo;
    t->lineno = lineno;
  }
  return t;
}


NoArvore * newExpNode(IdExpressao tipo)
{ 
  NoArvore * t = (NoArvore *) malloc(sizeof(NoArvore));
  int i;
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else {
    for (i=0;i<MAXFILHOS;i++) 
      t->filho[i] = NULL;
    t->irmao = NULL;
    t->tipoNo = EXP;
    t->tipo.exp = tipo;
    t->lineno = lineno;
    t->tipoExp = Void;
  }
  return t;
}

char * copyString(char * s)
{ 
  int n;
  char * t;
  if (s==NULL) return NULL;
  n = strlen(s)+1;
  t = malloc(n);
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else strcpy(t,s);
  return t;
}

static int indentno = 0;


#define INDENT indentno+=2
#define UNINDENT indentno-=2


static void printSpaces(void)
{ 
  int i;
  for (i=0;i<indentno;i++)
    fprintf(listing," ");
}


void printTree( NoArvore * tree )
{ 
  int i;
  INDENT;
  while (tree != NULL) {
    printSpaces();
    if (tree->tipoNo==DECLARACAO)
    { 
      switch (tree->tipo.dcl) {
        case IFt:
          fprintf(listing,"If\n");
          break;
        case IGUALt:
          fprintf(listing,"Atribuicao\n");
          break;
        case WHILEt:
          fprintf(listing,"While\n");
          break;
        case VARIAVEL:
          fprintf(listing,"Variavel %s\n", tree->atr.nome);
          break;
        case VETORt:
          fprintf(listing,"Vetor %s\n", tree->atr.nome);
          break;
        case PARAMETRO:
          fprintf(listing,"Parametro %s\n", tree->atr.nome);
          break;
        case FUNCAO:
          fprintf(listing,"Funcao %s\n", tree->atr.nome);
          break;
        case CALL:
          fprintf(listing,"Chamada da funcao %s\n", tree->atr.nome);
          break;        
        case RETURNt:
          fprintf(listing, "Return\n");
          break;        
              
        default:
            fprintf(listing,"Tipo de nó de declaracao desconhecido\n");
            break;
      }
    }
    else if (tree->tipoNo==EXP)
    { 
      switch (tree->tipo.exp) {
        case OPERACAO:
          fprintf(listing,"Operacao: ");
          printToken(tree->atr.op,"\0");
          break;
        case CONSTANTE:
          fprintf(listing,"Constante: %d\n",tree->atr.val);
          break;
        case IDt:
          fprintf(listing,"Id: %s\n",tree->atr.nome);
          break;
	      case VETOR:
          fprintf(listing,"Vetor: %s\n",tree->atr.nome);
          break;
	      case TIPO:
          fprintf(listing,"Tipo %s\n",tree->atr.nome);
          break;
        
        default:
          fprintf(listing,"Tipo de nó de expressao desconhecido\n");
          break;
      }
    }
    else fprintf(listing,"Unknown node kind\n");
    for (i=0;i<MAXFILHOS;i++)
      printTree(tree->filho[i]);
    tree = tree->irmao;
  }
  UNINDENT;
}