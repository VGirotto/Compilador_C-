#ifndef _UTIL_H_
#define _UTIL_H_

//guarda o número de erros encontrados
int numErrs;

/* Procedure printToken prints a token 
 * and its lexeme to the listing file
 */
void printToken( TipoToken, const char* );

/* Function newStmtNode creates a new statement
 * node for syntax tree construction
 */
NoArvore * newStmtNode(TipoDeclaracao);

/* Function newExpNode creates a new expression 
 * node for syntax tree construction
 */
NoArvore * newExpNode(IdExpressao);

/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char * copyString( char * );

/* procedure printTree prints a syntax tree to the 
 * listing file using indentation to indicate subtrees
 */
void printTree( NoArvore * );

#endif
