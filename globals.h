
#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>



#ifndef YYPARSER


#include "sint.tab.h"


#define ENDFILE 0

#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define MAXRESERVED 6
#define MAXFILHOS 3

extern FILE* source; 
extern FILE* listing; 
extern FILE* code; 
extern FILE* code2; 

extern int lineno; 


typedef int TipoToken;

typedef enum
{
	DECLARACAO, EXP
} TipoNo;

typedef enum
{
	IFt, WHILEt, IGUALt, VARIAVEL, VETORt, FUNCAO, PARAMETRO, CALL, RETURNt, READt, WRITEt

} TipoDeclaracao;

typedef enum
{
	OPERACAO, CONSTANTE, IDt, VETOR, TIPO

} IdExpressao;

/* ExpType is used for type checking */
typedef enum
{

	Void, INTEGER, BOOLEAN
	
} TipoExpressao;



typedef struct noArvore
{ 
   struct noArvore * filho[MAXFILHOS];
   struct noArvore * irmao;
   int lineno;
   TipoNo tipoNo;

   union 
   { 
      TipoDeclaracao dcl; 
      IdExpressao exp;
   } tipo;

   struct 
   { 
      TipoToken op;
      int val;
      int len;
      char * nome; 
      char * escopo;
   } atr;

   TipoExpressao tipoExp; /* for type checking of exps */
} NoArvore;


extern int EchoSource;

extern int TraceScan;

extern int TraceParse;

extern int TraceAnalyze;

extern int TraceCode;

extern int Error; 
#endif
