%{
#include <iostream>
#include"instruction_map.h"
#include"y.tab.h"
#include<fstream>
using namespace std;

// fstream debug;
#define YY_DECL extern "C" int yylex()

%}

%%
add|ADD         return ADD;
sub|SUB        return SUB; 
mul|MUL         return MUL; 
mov|MOV         return MOV; 
ldr|LDR         return LDR; 
str|STR         return STR; 
cmp|CMP         return CMP; 
bne|BNE         return BNE; 
b|B    			 return B; 
bge|BGE        	 return BGE; 
bl|BL 	      	 return BL; 
swi|SWI		      return SWI; 
SWI_Exit		return EXIT;
".data"			return DATA;
".end"			return END;
".text"			return TEXT;
".equ"			return EQU;
".space"			return SPACE;
r[0-9][0-9]?		return REGISTER;
[a-zA-Z_]+				return CHARS;
[0-9]+              return NUMBERS;
":"                   return COLON;
\[                  return BRCTO;
\]                  return BRCTC;
"="                   return EQUAL;
"#"                   return HATCH;
","                   return COMMA;
\n                    return NEWLINE;
[ \t]                 ;
.                     cout<<"something else"<<yytext<<endl;

%%

int yywrap(void){
    return 1;
}

int main(int, char**) {
// lex through the input:
    yylex();
}