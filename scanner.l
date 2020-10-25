%{
#include "globals.h"
#include "util.h"
#include "scan.h"

char tokenString[MAXTOKENLEN+1];
static int yylex(void);
%}

DIGIT [0-9]
LETRA [a-zA-Z]
newline     \n


%%
"else" return ELSE;
"if" return IF;
"int" return INT;
"return" return RETURN;
"while" return WHILE;
"void" return VOID;
{DIGIT}+ return NUM;
{LETRA}+	return ID;
"+" return SUM; 
"-" return SUB;
"*" return MULT;
"/" return DIV;
"<" return MENOR;
"<=" return MEIGUAL;
">" return MAIOR;
">=" return MAIGUAL;
"==" return COMP;
"!=" return DIF;
, return VIRG;
"{" return CH1;
"}" return CH2;
"[" return COLC1;
"]" return COLC2;
= return IGUAL;
; return PVIRG;
"(" return PAR1;
")" return PAR2;
"/*"            { char c;
                  do
                  { 
                    c = input();
                    if (c == EOF) break;
                    if (c == '\n') lineno++;
                    if (c == '*') 
                    { 
                        if(input() == '/')
                          break;
                    }		
                  } while (1);
                }
<<EOF>> 									return FIM;
{newline}       {lineno++;}                           
[ \t]+
{LETRA}*{DIGIT}*	return ERROR;
.|{DIGIT}{LETRA}* return ERROR;

%%

TipoToken getToken(void)
{ 
    static int firstTime = TRUE;
    TipoToken currentToken;
    if (firstTime)
    { 
      firstTime = FALSE;
      lineno++;
      yyin = source;
      yyout = listing;
    }
    currentToken = yylex();
    strncpy(tokenString, yytext, MAXTOKENLEN);
    
    if (TraceScan) {
      if(currentToken != FIM){ 
        fprintf(listing,"\t%d: ",lineno);
        printToken(currentToken, tokenString);
      }
    }
    return currentToken;
}