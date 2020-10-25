#ifndef _SYMTAB_H_
#define _SYMTAB_H_

/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
void st_insert( char * name, int lineno, int loc, int tipo, int tipoExp, char * escopo, char * param, char * vetor);

/* Function st_lookup returns the memory 
 * location of a variable or -1 if not found
 */
int st_lookup ( char * name );

int st_lookup_scope ( char *name, char * scope );

char* st_lookup_type ( char *name, char * scope );

char* st_lookup_decl ( char *name, char * scope );

char* st_lookup_param ( char *name, char * scope );

int st_lookup_location ( char *name, char * scope );

char* st_lookup_vetor ( char *name, char * scope );

char ** getVariableVector (char *scope, int *lenght);

/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */
void printSymTab(FILE * listing);

#endif