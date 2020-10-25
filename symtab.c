#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

/* SIZE is the size of the hash table */
#define SIZE 211

/* SHIFT is the power of two used as multiplier
   in hash function  */
#define SHIFT 4

/* the hash function */
static int hash ( char * key )
{ 
    int temp = 0;
    int i = 0;
    while (key[i] != '\0')
    { 
        temp = ((temp << SHIFT) + key[i]) % SIZE;
        ++i;
    }
    return temp;
}

/* the list of line numbers of the source 
 * code in which a variable is referenced
 */
typedef struct LineListRec
{ 
    int lineno;
    struct LineListRec * next;
} * LineList;

/* The record in the bucket lists for
 * each variable, including name, 
 * assigned memory location, and
 * the list of line numbers in which
 * it appears in the source code
 */
typedef struct BucketListRec
   { 
    char * name;
    LineList lines;
    char * tipo;
    char * tipoExp;
    int memloc ; /* memory location for variable */
    char * escopo;
    char * param;
    char * vetor;

    struct BucketListRec * next;
   } * BucketList;

/* the hash table */
static BucketList hashTable[SIZE];

/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
void st_insert( char * name, int lineno, int loc , int tipo, int tipoExp, char * escopo, char * param, char * vetor)
{ 
    int h = hash(name);
    BucketList l =  hashTable[h];

    while ((l != NULL)){
        if(escopo != NULL){
            if ((strcmp(escopo, l->escopo) == 0) && (strcmp(name, l->name) == 0)) //nome e escopo iguais ao alvo
            {
                //já existe na tabela com esse nome e esse escopo
                break;
            }   
        } else {
            if((strcmp(name, l->name) == 0)) break;
        }
        
        l = l->next;
        
    }  
    if (l == NULL) /* variable not yet in table */
    { 
        l = (BucketList) malloc(sizeof(struct BucketListRec));
        l->name = name;
        l->lines = (LineList) malloc(sizeof(struct LineListRec));
        l->lines->lineno = lineno;
        l->memloc = loc;
        l->escopo = escopo;
        l->param = param;
        l->vetor = vetor;
        if (tipo == 3)
        {
            l->tipo = "var";
        } else if (tipo == 4) {
            l->tipo = "fun";
        }
        if (tipoExp == 0)
        {
            l->tipoExp = "void";
        } else if(tipoExp == 1) {
            l->tipoExp = "int";
        } else {
            l->tipoExp = "bool";
        }
        
        
        l->lines->next = NULL;
        l->next = hashTable[h];
        hashTable[h] = l; 
    }
    else /* found in table, so just add line number */
    { 
        LineList t = l->lines;
        while (t->next != NULL) 
            t = t->next;
        t->next = (LineList) malloc(sizeof(struct LineListRec));
        t->next->lineno = lineno;
        t->next->next = NULL;
  }
} /* st_insert */

/* Function st_lookup returns the memory 
 * location of a variable or -1 if not found
 */
int st_lookup ( char * name )
{ 
    int h = hash(name);
    BucketList l =  hashTable[h];
    while ((l != NULL) && (strcmp(name,l->name) != 0))
        l = l->next;
    if (l == NULL) return -1;
    else return l->memloc;
}

/* Function st_lookup_scop returns if the variable exist in the same scope
 */
int st_lookup_scope ( char *name, char * scope )
{ 
    int h = hash(name);
    BucketList l =  hashTable[h];
    while ((l != NULL)){
        if ((strcmp(scope, l->escopo) == 0) && (strcmp(name, l->name) == 0)) //nome e escopo iguais ao alvo
        {
            //já existe na tabela com esse nome e esse escopo
            break;
        }
        l = l->next;
        
    }  
        
    if (l == NULL) return -1;
    else return 1;
}

/* Function st_lookup_type returns if the variable exist in the same scope with the right data type
 */
char* st_lookup_type ( char *name, char * scope )
{ 
    int h = hash(name);
    BucketList l =  hashTable[h];
    while ((l != NULL)){        
        if ((strcmp(scope, l->escopo) == 0) && (strcmp(name, l->name) == 0)) //nome e escopo iguais ao alvo
        {
            //já existe na tabela com esse nome e esse escopo
            break;
        }
        l = l->next;
    }  
        
    if (l == NULL) return " ";
    else return l->tipoExp;
}

/* Function st_lookup_decl returns if the variable exist in the same scope with the right declaration type
 */
char* st_lookup_decl ( char *name, char * scope )
{ 
    int h = hash(name);
    BucketList l =  hashTable[h];
    while ((l != NULL)){        
        if ((strcmp(scope, l->escopo) == 0) && (strcmp(name, l->name) == 0)) //nome e escopo iguais ao alvo
        {
            //já existe na tabela com esse nome e esse escopo
            break;
        }
        l = l->next; 
    }  
        
    if (l == NULL) return " ";
    else return l->tipo;
}

char* st_lookup_param ( char *name, char * scope ){
    int h = hash(name);
    BucketList l =  hashTable[h];
    while ((l != NULL)){        
        if ((strcmp(scope, l->escopo) == 0) && (strcmp(name, l->name) == 0)) //nome e escopo iguais ao alvo
        {
            //já existe na tabela com esse nome e esse escopo
            break;
        }
        l = l->next; 
    }  
        
    if (l == NULL) return " ";
    else return l->param;
}

char* st_lookup_vetor ( char *name, char * scope ){
    int h = hash(name);
    BucketList l =  hashTable[h];
    while ((l != NULL)){        
        if ((strcmp(scope, l->escopo) == 0) && (strcmp(name, l->name) == 0)) //nome e escopo iguais ao alvo
        {
            //já existe na tabela com esse nome e esse escopo
            break;
        }
        l = l->next; 
    }  
        
    if (l == NULL) return " ";
    else return l->vetor;
}

/* Function st_lookup_location returns the memloc of a variable
 */
int st_lookup_location ( char *name, char * scope )
{ 
    int h = hash(name);
    BucketList l =  hashTable[h];
    while ((l != NULL)){        
        if (((strcmp(scope, l->escopo) == 0) && (strcmp(name, l->name) == 0)) ||    //nome e escopo iguais ao alvo
        ((strcmp("Escopo Global", l->escopo) == 0) && (strcmp(name, l->name) == 0))) //nome igual e escopo global
        {
            //já existe na tabela com esse nome e esse escopo
            break;
        }
        l = l->next; 
    }  
        
    if (l == NULL) return -1;
    else return l->memloc;
}

char **getVariableVector(char *scope, int *lenght){
    int j=0;
    char **vect = (char **) malloc((j+1)*sizeof(char*));
    for (int i=0;i<SIZE;++i)
    { 
        if (hashTable[i] != NULL)
        { 
            BucketList l = hashTable[i];
            while (l != NULL)
            {   
                if (strcmp(l->escopo, scope) == 0)
                {
                    if(strcmp(l->vetor, "nao") == 0 || strcmp(l->vetor, ".") == 0){
                        vect = (char **) realloc(vect, (j+1)*sizeof(char*));
                        vect[j] = l->name;
                        j++;
                        (*lenght)++;
                    }
                }
                
                l = l->next;
            }
        }
    }
    return vect;

}

/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */
void printSymTab(FILE * listing)
{ 
    int i;
    fprintf(listing,"Nome Variavel  Tipo Variavel   Tipo Dado   Eh Param   Nome Escopo   Loc Memoria   Numeros Linhas\n");
    fprintf(listing,"-------------  -------------   ---------   --------   -----------   -----------   --------------\n");
    for (i=0;i<SIZE;++i)
    { 
        if (hashTable[i] != NULL)
        { 
            BucketList l = hashTable[i];
            while (l != NULL)
            { 
                LineList t = l->lines;
                fprintf(listing,"%-15s ",l->name);
                fprintf(listing,"%-14s  ",l->tipo);
                fprintf(listing,"%-9s  ",l->tipoExp);
                fprintf(listing,"%-9s  ",l->param);
                fprintf(listing,"%-13s  ",l->escopo);
                fprintf(listing,"%-10d  ",l->memloc);
                while (t != NULL)
                { 
                    fprintf(listing,"%4d ",t->lineno);
                    t = t->next;
                }
            fprintf(listing,"\n");
            l = l->next;
            }
        }
    }
} /* printSymTab */
