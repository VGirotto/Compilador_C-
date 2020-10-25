#include "globals.h"
#include "code.h"


/* Procedure emitComment prints a comment line 
 * with comment c in the code file
 */
void emitComment( char * c )
{ 
  if (TraceCode) fprintf(code,"* %s\n",c);
}


/* Procedure emitNome emits uma instrução do formato R
 * opcode = the opcode
 * tp = tipo de dado
 * n = nome
 * e = escopo
 * c = a comment to be printed if TraceCode is TRUE
 */
void emitNome( char *opcode, int tp, char *n, char *e, char *c)
{ 
  char *tipoE;
  if(tp == 0)
    tipoE = "void";
  else if(tp == 1)
    tipoE = "int";
  else
    tipoE = "_";
  
  
  fprintf(code, "(%s,%s,%s,%s)", opcode, tipoE, n, e);
  if (TraceCode) fprintf(code, "\t%s", c) ;
  fprintf(code, "\n") ;
} /* emitNome */

/* Procedure emitI emits imediato
 */
void emitR( char *opcode, int r1, int r2, int rs, char *c)
{   
  fprintf(code, "(%s,$t%d,$t%d,$t%d)", opcode, r1, r2, rs);
  if (TraceCode) fprintf(code, "\t%s", c) ;
  fprintf(code, "\n") ;
} 


/* Procedure emitI emits imediato
 */
void emitI( char * op, char *r, int d, int s, char *c)
{ 
  if(d == -1) 
    fprintf(code, "(%s,%s,_,$t%d)", op, r, s);
  else
    fprintf(code, "(%s,_,%d,$t%d)", op, d, s);
  
  if (TraceCode) fprintf(code,"\t%s",c) ;
  fprintf(code,"\n") ;
} /* emitI */


/* Procedure emitLabel
 */
void emitLabel( char * op, int label, char *c)
{ 
  if (label  == -1)
    fprintf(code, "(%s,main,_,_)", op);
  else 
    fprintf(code, "(%s,L%d,_,_)", op, label);
  
  if (TraceCode) fprintf(code,"\t%s",c) ;
  fprintf(code,"\n") ;
} /* emitI */
