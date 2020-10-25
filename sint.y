%{
    #define YYPARSER    
    #include "globals.h"
    #include "util.h"
    #include "scan.h"
    #include "parse.h"

    #define YYSTYPE NoArvore *
    static NoArvore * arvSalva;

    static int yylex(void);
	void yyerror(char *);
%}

%start programa
%token ELSE IF RETURN WHILE VOID INT
%token NUM ID
%token CH1 CH2 COLC1 COLC2 PAR1 PAR2 PVIRG VIRG
%token SUM SUB
%token MULT DIV
%token IGUAL MENOR MAIOR COMP MAIGUAL MEIGUAL DIF
%token ERROR FIM
%expect 1

%%

programa:	
 	declaracao_lista{
 			arvSalva = $1;
 	};

declaracao_lista:
	declaracao_lista declaracao {
		YYSTYPE t = $1;
		if (t != NULL){
			while (t->irmao != NULL){
				t = t->irmao;
			}
			t->irmao = $2;
			$$ = $1;
		}
		else 
			$$ = $2;
	} 
	| declaracao {
		$$ = $1;
	};

declaracao:
	var_declaracao {
		$$ = $1;
	} 
	| fun_declaracao{
		$$ = $1;
	};

var_declaracao:
	tipo_especificador ident PVIRG {
		$$ = $1;
		$$->filho[0] = $2;
		$2->tipoNo = DECLARACAO;
		$2->tipo.dcl = VARIAVEL;
		$2->tipoExp = $1->tipoExp;
	} 
	| tipo_especificador ident COLC1 num COLC2 PVIRG{
		$$ = $1;
		$$->filho[0] = $2;
		$2->tipoNo = DECLARACAO;
		$2->tipo.dcl = VETORt;
		$2->atr.len = $4->atr.val;
		$2->tipoExp = $1->tipoExp;
	};

tipo_especificador:
	INT {
		$$ = newExpNode(TIPO);
		$$->tipoExp = INTEGER;
		$$->atr.nome = "Integer";
	}
	| VOID{
		$$ = newExpNode(TIPO);
		$$->tipoExp = Void;
		$$->atr.nome = "Void";
	};

fun_declaracao:
	tipo_especificador ident PAR1 params PAR2 composto_decl{
		$$ = $1;
		$$->filho[0] = $2;
		$2->filho[0] = $4;
		$2->filho[1] = $6;
		$2->tipoNo = DECLARACAO;
		$2->tipo.dcl = FUNCAO;
		$2->tipoExp = $1->tipoExp;
	} ;

params:
	param_lista{
		$$ = $1;
	} | VOID {
		
	};

param_lista:
	param_lista VIRG param {
		YYSTYPE t = $1;
		if (t != NULL){
			while (t->irmao != NULL){
				t = t->irmao;
			}
			t->irmao = $3;
			$$ = $1;
		}
		else 
			$$ = $1;
	} 
	| param{
		$$ = $1;
	};

param:
	tipo_especificador ident {
		$$ = $1;
		$$->filho[0] = $2;
		$2->tipoNo = DECLARACAO;
		$2->tipo.dcl = PARAMETRO;
		$2->tipoExp = $1->tipoExp;
	} 
	| tipo_especificador ident COLC1 COLC2 {
		$$ = $1;
		$$->filho[0] = $2;
		$2->tipoNo = DECLARACAO;
		$2->tipo.dcl = PARAMETRO;
		$2->tipoExp = $1->tipoExp;
	};
	

composto_decl:
	CH1 local_declaracoes statement_lista CH2{
		YYSTYPE t = $2;
		if (t != NULL){
			while (t->irmao != NULL){
				t = t->irmao;
			}
			t->irmao = $3;
			$$ = $2;
		}
		else 
			$$ = $3;
	}
	|   CH1 local_declaracoes CH2{
    		$$ = $2;
		
		}
	|   CH1 statement_lista CH2{
		
				$$ = $2;
			
		}
	|   CH1 CH2{
		};
	


local_declaracoes:
	local_declaracoes var_declaracao {
		YYSTYPE t = $1;
		if (t != NULL){
			while (t->irmao != NULL){
				t = t->irmao;
			}
			t->irmao = $2;
			$$ = $1;
		}
		else
			$$ = $2;
	} 
	| var_declaracao{

		$$ = $1;

	};


statement_lista:
    statement_lista statement {
    	YYSTYPE t = $1;
		if (t != NULL){
			while (t->irmao != NULL){
				t = t->irmao;
			}
			t->irmao = $2;
			$$ = $1;
		}
		else 
			$$ = $2;
    } 
    | statement
	{
		$$ = $1;
	};



statement:
    expressao_decl {
    	$$ = $1;
    } 
    | composto_decl{
    	$$ = $1;
    } 
    | selecao_decl{
    	$$ = $1;
    } 
    | iteracao_decl{
    	$$ = $1;
    }| retorno_decl{
    	$$ = $1;
    };



expressao_decl:
    expressao PVIRG{
    	$$ = $1;
    } 
    | PVIRG{
    	//VAZIO
    };



selecao_decl:
    IF PAR1 expressao PAR2 statement{
    	$$ = newStmtNode(IFt);
    	$$->filho[0]  =$3;
    	$$->filho[1] = $5;
    } 
    | IF PAR1 expressao PAR2 statement ELSE statement{
    	$$ = newStmtNode(IFt);
    	$$->filho[0] = $3;
    	$$->filho[1] = $5;
    	$$->filho[2] = $7;
    };


iteracao_decl:
    WHILE PAR1 expressao PAR2 statement{
    	$$ = newStmtNode(WHILEt);
    	$$->filho[0] = $3;
    	$$->filho[1] = $5;
    };


retorno_decl:
    RETURN PVIRG{
    	$$ = newStmtNode(RETURNt);
    } 
    | RETURN expressao PVIRG{
    	$$ = newStmtNode(RETURNt);
    	$$->filho[0] = $2;
    };


expressao:
    var IGUAL expressao{
    	$$ = newStmtNode(IGUALt);
    	$$->filho[0] = $1;
    	$$->filho[1] = $3;
    } 
    | simples_expressao{
    	$$ = $1;
    };


var:
    ident{
    	$$ = $1;
    } 
    | ident COLC1 expressao COLC2{
    	$$ = $1;
    	$$->filho[0] = $3;
    	$$->tipo.exp = VETOR;
    };


simples_expressao:
    soma_expressao relacional soma_expressao{
    	$$ = $2;
    	$$->filho[0] = $1;
    	$$->filho[1] = $3;
    }
    | soma_expressao{
    	$$ = $1;
    };

    
relacional:
    MEIGUAL{
    	$$ = newExpNode(OPERACAO);
    	$$->atr.op = MEIGUAL;

    }
    | MAIOR{
		$$ = newExpNode(OPERACAO);
    	$$->atr.op = MAIOR;
    } 
    | MENOR{
    	$$ = newExpNode(OPERACAO);
    	$$->atr.op = MENOR;
    } 
    | MAIGUAL{
    	$$ = newExpNode(OPERACAO);
    	$$->atr.op = MAIGUAL;

    } 
    | COMP{
    	$$ = newExpNode(OPERACAO);
    	$$->atr.op = COMP;
    } 
    | DIF{
    	$$ = newExpNode(OPERACAO);
    	$$->atr.op = DIF;
    };



soma_expressao:
    soma_expressao  soma termo{
    	$$ = $2;
    	$$->filho[0] = $1;
    	$$->filho[1] = $3;
    } 
    | termo{
    	$$ = $1;
    };

soma:
    SUM{
    	$$ = newExpNode(OPERACAO);
    	$$->atr.op = SUM;
    } 
    | SUB{
    	$$ = newExpNode(OPERACAO);
    	$$->atr.op = SUB;
    };

termo:
    termo mult fator {
    	$$ = $2;
    	$$->filho[0] = $1;
    	$$->filho[1] = $3;
    }| fator{
    	$$ = $1;
    };

mult:
    MULT{
    	$$ = newExpNode(OPERACAO);
    	$$->atr.op = MULT;
    } 
    | DIV{
    	$$ = newExpNode(OPERACAO);
    	$$->atr.op = DIV;
    };

fator:
    PAR1 expressao PAR2{
    	$$ = $2;
    } 
    | var{
    	$$ = $1;
    } 
    | ativacao{
    	$$ = $1;
    } 
    | num{
    	$$ = $1;
    };

ativacao:
    ident PAR1 arg_lista PAR2{
    	$$ = $1;
    	$$->filho[0] = $3;
    	$$->tipoNo = DECLARACAO;
    	$$->tipo.dcl = CALL;
    }
	| ident PAR1 PAR2{

		$$ = $1;
		$$->tipoNo = DECLARACAO;
		$$->tipo.dcl = CALL;

	};

arg_lista: 
	arg_lista VIRG expressao{
	 	YYSTYPE t = $1;
		if (t != NULL){
			while (t->irmao != NULL){
				t = t->irmao;
			}
			t->irmao = $3;
			$$ = $1;
		}
		else 
			$$ = $3;	
	} 
	| expressao{
		$$ = $1;
	};

ident:
	ID {
		$$ = newExpNode(IDt);
		$$->atr.nome = copyString(tokenString);
	};

num:
	NUM {
		$$ = newExpNode(CONSTANTE);
		$$->atr.val = atoi(tokenString);
	}
%%

void yyerror(char* msg){
    if(yychar != FIM){	
    	fprintf(listing, "Erro sintático na linha %d: %s\n", lineno, msg);
    	fprintf(listing, "Token atual: ");
    	printToken(yychar, tokenString);
    	Error = TRUE;
		numErrs = yynerrs;
	}
}

static int yylex(void){
    return getToken();
}

NoArvore * parse(void){
    yyparse();
    return arvSalva;
}
