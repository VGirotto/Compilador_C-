#include "globals.h"
#include "util.h"
#include "parse.h"
#include "scan.h"
#include "analyze.h"
#include "code.h"
#include "cgen.h"
#include "assembly.h"
#include "binario.h"
#include <errno.h>

/* set NO_ANALYZE to TRUE to get a parser-only compiler */
#define NO_ANALYZE FALSE

/* allocate global variables */
int lineno = 0;
FILE * source;
FILE * listing;
FILE * code;
FILE * code2;

/* allocate and set tracing flags */
int EchoSource = FALSE;
int TraceScan = TRUE;
int TraceParse = TRUE;
int TraceAnalyze = TRUE;
int TraceCode = FALSE;

int Error = FALSE;

int main( int argc, char * argv[] )
{ 
  NoArvore * syntaxTree;
  char pgm[120]; /* source code file name */

  if (argc != 2)  //verifica chamada do executavel
  { 
      fprintf(stderr,"usage: %s <filename>\n",argv[0]);
      exit(1);
  }

  strcpy(pgm, argv[1]);   //pega o arquivo com o nome especificado
  if (strchr (pgm, '.') == NULL)
     strcat(pgm,".cms");

  source = fopen(pgm,"r");  //abre arquivo
  if (source == NULL)
  { 
    fprintf(stderr,"File %s not found\n",pgm);
    exit(1);
  }
  listing = stdout; /* send listing to screen */

  //Começa a compilação
  fprintf(listing,"\nCompilacao do C-: %s\n",pgm);


  syntaxTree = parse();   //constrói a árvore e faz o parser
  if (TraceParse) {
    fprintf(listing,"\nArvore Sintatica:\n");
    printTree(syntaxTree);
  }

    //Analisador semântico
  if (! Error)
  { 
    if (TraceAnalyze) fprintf(listing,"\nConstruindo a tabela de simbolos...\n");
    buildSymtab(syntaxTree);
    if (TraceAnalyze) fprintf(listing,"\nChecando os tipos...\n");
    typeCheck(syntaxTree);
    if (TraceAnalyze) fprintf(listing,"\nTerminada análise\n\n");
  }

  //Se teve erro até aqui para a execução
  if (Error == TRUE)
  {
    printf("\nErro encontrado durante a análise do código.\n");
    exit(0);
  }

  //Gerador de código intermediário
  char * codefile;
  int fnlen = strcspn(pgm, ".");
  codefile = (char *) calloc(fnlen+4, sizeof(char));
  strncpy(codefile, pgm, fnlen);
  strcat(codefile, ".inter");
  code = fopen(codefile, "w");
  if (code == NULL)
  { 
    printf("Unable to open %s\n", codefile);
    exit(1);
  }
  codeGen(syntaxTree, codefile);
  fclose(code);

  //Gerador de código assembly
  char * assembly_file;
  fnlen = strcspn(pgm, ".");
  assembly_file = (char *) calloc(fnlen+4, sizeof(char));
  strncpy(assembly_file, pgm, fnlen);
  strcat(assembly_file, ".ass");
  code2 = fopen(assembly_file, "w"); //abre arquivo .assembly para escrita
  if (code2 == NULL)
  { 
    int errnum = errno;
    printf("Unable to open a %s %d\n", assembly_file, errno);
    exit(1);
  }
  code = fopen(codefile, "r");  //abre arquivo .inter para leitura
  if (code == NULL)
  { 
    printf("Unable to open b %s\n", codefile);
    exit(1);
  }
  converteAssembly();
  fclose(code);
  fclose(code2);

  //Gerador de código Binário
  char * bin_file;
  fnlen = strcspn(pgm, ".");
  bin_file = (char *) calloc(fnlen+4, sizeof(char));
  strncpy(bin_file, pgm, fnlen);
  strcat(bin_file, ".bna");
  code2 = fopen(bin_file, "w"); //abre arquivo .assembly para escrita
  if (code2 == NULL)
  { 
    int errnum = errno;
    printf("Unable to open a %s %d\n", bin_file, errno);
    exit(1);
  }
  code = fopen(assembly_file, "r");  //abre arquivo .inter para leitura
  if (code == NULL)
  { 
    printf("Unable to open b %s\n", assembly_file);
    exit(1);
  }
  converteBin();
  fclose(code);
  fclose(code2);
  

  fclose(source);
  return 0;
}

