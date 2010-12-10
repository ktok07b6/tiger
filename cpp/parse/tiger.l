%{
#include <stdio.h>
#include "Absyn.h"
#include "parser.hpp"
#include "debug.h"

extern "C" int yywrap(void)
{
    return 1;
}

extern bool isTypeID(const char *id);
#define ECHO
%}

%x STRING
%x COMMENT

WSPACE [[:blank:]]+
DIGIT [0-9]
NUMBER {DIGIT}+
SYMBOLS [,.\:;()\{\}\[\]]
IDENT [a-zA-Z][_a-zA-Z0-9]*
%%

if    { return IF;}
then  { return THEN;}
else  { return ELSE;}
while { return WHILE;}
do    { return DO;}
for   {return FOR;}
to    {return TO;}
let   {return LET;}
in    {return IN;}
end   {return END;}
function {return FUNCTION;}
var   {return VAR;}
array {return ARRAY;}
of    {return OF;}
break {return BREAK;}
type  {return TYPE;}
nil   {return NIL; }
":="    {return ASSIGN;}
"+"    {yylval.op = PlusOp; return PLUS; }
"-"    {yylval.op = MinusOp; return MINUS; }
"*"    {yylval.op = TimesOp; return TIMES; }
"/"    {yylval.op = DivideOp; return DIVIDE; }
"="    {yylval.op = EqOp; return EQ; }
"<>"    {yylval.op = NeOp; return NE; }
">"     {yylval.op = GtOp; return GT; }
"<"     {yylval.op = LtOp; return LT; }
">="    {yylval.op = GeOp; return GE; }
"<="    {yylval.op = LeOp; return LE; }
"&"     {yylval.op = AndOp; return AND; }
"|"     {yylval.op = OrOp; return OR; }

{NUMBER} { 
  sscanf(yytext, "%d", &yylval.n);
  return INTEGER_CONSTANT;
}

{IDENT} {
  yylval.s = yytext;
  if (isTypeID(yytext)) {
    return TIGER_TYPE_IDENT;
  } else {
    return TIGER_IDENT;
  }
}

{SYMBOLS} {
  return yytext[0];
}

{WSPACE} {}

\" {BEGIN(STRING); yymore();}
<STRING>\n {
  yylval.s = yytext;
  BEGIN(INITIAL);
  return TIGER_ERROR;
}
<STRING>\"      { 
  yylval.s = yytext;
  BEGIN(INITIAL);
  return STRING_CONSTANT;
}
<STRING>. {yymore();}

"/*" {BEGIN(COMMENT); yymore();}
<COMMENT>\n {yymore();}
<COMMENT>"*/"  {
	BEGIN(INITIAL);
}

\n ;//{ return TIGER_EOL;}

<<EOF>> { return 0; }

. {
  printf("unknown token %s\n", yytext);
  return TIGER_ERROR;
}
%%
