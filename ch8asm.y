%{
	
#include <stdio.h>
#include <string.h>
#include <stdint.h>

/* Los programas se cargan a partir de 0x200 */
int pc = 0x200;
/* File pointer para el archivo de salida */
FILE *fp;

/* TODO Puse esto aqui para quitar warnings, ver que se puede hacer */
int atoi();
int yylex();
int yyerror(const char* s) {
   printf("\n%s\n", s);
   return 0;
}

/* Va creando una instrucción en llamadas sucesivas y cuando finalmente la tiene,
   la devuelve. Cada llamada añade (<len> * 4) bits a la instrucción,
   los cuales estan almacenados en <bits> */
int op(int bits, int len) {
	static int opcode = 0;
	static int pos = 0;
	if (pos + len > 4) {
		printf("Error: Writing too much.\n"); /* TODO mejorar errores */
		return 0;
	} else if (bits >= 1 << len * 4) {
		printf("Error: Numeric constant is too big.\n");
		return 0;
	}
	pos += len;
	opcode |= (bits & 0xFFFF >> (4 - len) * 4) << (4 - pos) * 4;
	if (pos == 4) {
		int aux = opcode;
		pos = 0;
		opcode = 0;
		return aux;
	}
}

%}



%union {
	char* str;
	int num;
}

%token CLS RET SYS JP CALL SE SNE LD SKP SKNP
%token OR AND XOR ADD SUB SUBN SHR SHL RND
%token DRW
%token BYTE
%token<str> DECVAL HEXVAL BINVAL
%token<num> VX
%token DT ST I
%token K F B
%token<str> LABEL STRING VAR
%token COMMA OB CB

%type<num> value instruction

%start program

%%

program: program statement
	   | %empty
	   ;
	   
statement: instruction						{
												/* Escribir byte a byte para asegurarnos que
												   sigue el orden correcto (primero el de mas peso) */
												uint8_t first = $1 >> 8;
												uint8_t second = $1 & 0xFF; 
												fwrite((void*)&first, 1, 1, fp);
												fwrite((void*)&second, 1, 1, fp);			
												/* Actualizar pc */
												pc += 2;
											}
		 | BYTE svalue_list
		 | LABEL
		 | VAR EQ value
		 ;
		 
instruction: CLS							{ /* 00E0 */ $$ = 0x00E0; }
		   | RET							{ /* 00EE */ $$ = 0x00EE; }
		   | SYS value						{ /* 0nnn */ op(0x0, 1); $$ = op($2, 3); }
		   | JP value						{ /* 1nnn */ op(0x1, 1); $$ = op($2, 3); }
		   | JP VX COMMA value 				{ /* Bnnn */ if ($2 == 0) {
															 op(0xB, 1); $$ = op($4, 3);
														 } else {
															 return yyerror("Error: JP must use V0\n");
														 }													}
		   | CALL value						{ /* 2nnn */ op(0x2, 1); $$ = op($2, 3); }
		   | SE VX COMMA value				{ /* 3xkk */ op(0x3, 1); op($2, 1); $$ = op($4, 2); }
		   | SE VX COMMA VX					{ /* 5xy0 */ op(0x5, 1); op($2, 1); op($4, 1); $$ = op(0x0, 1); }
		   | SNE VX COMMA value				{ /* 4xkk */ op(0x4, 1); op($2, 1); $$ = op($4, 2); }
		   | SNE VX COMMA VX				{ /* 9xy0 */ op(0x9, 1); op($2, 1); op($4, 1); $$ = op(0x0, 1); }
		   | ADD VX COMMA value				{ /* 7xkk */ op(0x7, 1); op($2, 1); $$ = op($4, 2); }
		   | ADD VX COMMA VX				{ /* 8xy4 */ op(0x8, 1); op($2, 1); op($4, 1); $$ = op(0x4, 1); }
		   | ADD I COMMA VX					{ /* Fx1E */ op(0xF, 1); op($4, 1); $$ = op(0x1E, 2); }
		   | OR VX COMMA VX					{ /* 8xy1 */ op(0x8, 1); op($2, 1); op($4, 1); $$ = op(0x1, 1); }
		   | AND VX COMMA VX				{ /* 8xy2 */ op(0x8, 1); op($2, 1); op($4, 1); $$ = op(0x2, 1); }
		   | XOR VX COMMA VX 				{ /* 8xy3 */ op(0x8, 1); op($2, 1); op($4, 1); $$ = op(0x3, 1); }
		   | SUB VX COMMA VX				{ /* 8xy5 */ op(0x8, 1); op($2, 1); op($4, 1); $$ = op(0x5, 1); }
		   | SHR VX							{ /* 8x06 */ op(0x8, 1); op($2, 1); $$ = op(0x06, 2); }
		   | SUBN VX COMMA VX				{ /* 8xy7 */ op(0x8, 1); op($2, 1); op($4, 1); $$ = op(0x7, 1); }
		   | SHL VX							{ /* 8x0E */ op(0x8, 1); op($2, 1); $$ = op(0x0E, 2); }
		   | RND VX COMMA value				{ /* Cxkk */ op(0xC, 1); op($2, 1); $$ = op($4, 2); }
		   | SKP VX							{ /* Ex9E */ op(0xE, 1); op($2, 1); $$ = op(0x9E, 2); }
		   | SKNP VX						{ /* ExA1 */ op(0xE, 1); op($2, 1); $$ = op(0xA1, 2); }
		   | DRW VX COMMA VX COMMA value	{ /* Dxyn */ op(0xD, 1); op($2, 1); op($4, 1); $$ = op($6, 1); }
		   | LD VX COMMA value				{ /* 6xkk */ op(0x6, 1); op($2, 1); $$ = op($4, 2); }
		   | LD VX COMMA VX					{ /* 8xy0 */ op(0x8, 1); op($2, 1); op($4, 1); $$ = op(0x0, 1); }
		   | LD VX COMMA DT					{ /* Fx07 */ op(0xF, 1); op($2, 1); $$ = op(0x07, 2); }
		   | LD VX COMMA K					{ /* Fx0A */ op(0xF, 1); op($2, 1); $$ = op(0x0A, 2); }
		   | LD VX COMMA OB I CB			{ /* Fx65 */ op(0xF, 1); op($2, 1); $$ = op(0x65, 2); }
		   | LD I COMMA value				{ /* Annn */ op(0xA, 1); $$ = op($4, 3); }
		   | LD DT COMMA VX					{ /* Fx15 */ op(0xF, 1); op($4, 1); $$ = op(0x15, 2); }
		   | LD ST COMMA VX					{ /* Fx18 */ op(0xF, 1); op($4, 1); $$ = op(0x18, 2); }
		   | LD F COMMA VX					{ /* Fx29 */ op(0xF, 1); op($4, 1); $$ = op(0x29, 2); }
		   | LD B COMMA VX					{ /* Fx33 */ op(0xF, 1); op($4, 1); $$ = op(0x33, 2); }
		   | LD OB I CB COMMA VX			{ /* Fx55 */ op(0xF, 1); op($6, 1); $$ = op(0x55, 2); }
		   ;
		   
svalue_list: svalue_list COMMA svalue
		   | svalue
		   ;
		 
svalue: STRING
	  | value
	  ;
		 
value: DECVAL	{ $$ = atoi($1); }
	 | HEXVAL	{ $$ = atoi($1); /* TODO conversion */ }
	 | BINVAL	{ $$ = atoi($1); /* TODO conversion */ }
	 | VAR		{ $$ = 0; /* TODO crear/cargar variables */ }
	 ;

%%

int main(int argc, char *argv[]) {
	if (argc != 2) printf("Error: invalid number of arguments\n"); /* TODO error */
	fp = fopen(argv[1], "w");
	yyparse();
	fclose(fp);
}