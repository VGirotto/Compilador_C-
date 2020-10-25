#ifndef _CODE_H_
#define _CODE_H_

/* code emitting utilities */

/* Procedure emitComment prints a comment line 
 * with comment c in the code file
 */
void emitComment( char * c );


void emitNome( char *opcode, int tp, char *n, char *e, char *c);
void emitI( char * op, char *r, int d, int s, char *c);
void emitLabel( char * op, int label, char *c);
void emitR( char *opcode, int r1, int r2, int rs, char *c);


#endif
