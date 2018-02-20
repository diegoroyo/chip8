%{
#include "y.tab.h"
#include "string.h"
#define YYSTYPE union { char* str; int num; }
%}

%option caseless

%x comm

E [^a-z0-9_@]

%%

"cls"/{E}					{return(CLS);}
"ret"/{E}					{return(RET);}
"sys"/{E}					{return(SYS);}
"jp"/{E}					{return(JP);}
"call"/{E}					{return(CALL);}
"se"/{E}					{return(SE);}
"sne"/{E}					{return(SNE);}
"ld"/{E}					{return(LD);}
"skp"/{E}					{return(SKP);}
"sknp"/{E}					{return(SKNP);}
		
"or"/{E}					{return(OR);}
"and"/{E}					{return(AND);}
"xor"/{E}					{return(XOR);}
"add"/{E}					{return(ADD);}
"sub"/{E}					{return(SUB);}
"subn"/{E}					{return(SUBN);}
"shr"/{E}					{return(SHR);}
"shl"/{E}					{return(SHL);}
"rnd"/{E}					{return(RND);}
		
"drw"/{E}					{return(DRW);}

".define"/{E}				{/* TODO hacer define aqui */}
".byte"/{E}					{return(BYTE);}

[0-9]+/{E}					{yylval.str = yytext; return(DECVAL);}
"0x"+[0-9]+/{E}				{yylval.str = &yytext[2]; return(HEXVAL);}
"0b"+[0-9]+/{E}				{yylval.str = &yytext[2]; return(BINVAL);}

"v"+[0-9a-f]/{E}			{yylval.num = yytext[1] - '0'; return(VX);}
"dt"/{E}					{return(DT);}
"st"/{E}					{return(ST);}
"i"/{E}						{return(I);}

"k"/{E}						{return(K);}
"f"/{E}						{return(F);}
"b"/{E}						{return(B);}

[a-z_@][a-z0-9_@]++":"		{/*yytext[strlen(yytext) - 1] = '\0'; yylval = yytext; return(LABEL);*/}
"\""+.*++"\""				{/*yytext[strlen(yytext) - 1] = '\0'; yylval = yytext[1]; return(STRING);*/}
[a-z_@][a-z0-9_@]+			{/*yylval = yytext; return(VAR);*/}

","							{return(COMMA);}
"["							{return(OB);}
"]"							{return(CB);}

";"							{BEGIN(comm);}

[ \n]						{/* Come espacios y saltos de linea */}

<comm>[^\n]*				{/* Come todo hasta el \n */}
<comm>\n					{BEGIN(INITIAL);}

. 							{printf("Error: unexpected character %c\n", *yytext); /* TODO mejorar el error */}

%%