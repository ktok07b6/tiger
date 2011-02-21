%{
#include <stdio.h>
#include "Absyn.h"
//#include "parser.hpp"
#include "debug.h"
#include "parserUtils.h"

extern "C" int yywrap(void)
{
    return 1;
}


#define YY_INPUT(buf,result,max_size)  {\
  result = getNextChar(buf, max_size); \
  if (result <= 0) \
    result = YY_NULL; \
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

if    { beginToken(yytext); return IF;}
then  { beginToken(yytext); return THEN;}
else  { beginToken(yytext); return ELSE;}
while { beginToken(yytext); return WHILE;}
do    { beginToken(yytext); return DO;}
for   { beginToken(yytext); return FOR;}
to    { beginToken(yytext); return TO;}
let   { beginToken(yytext); return LET;}
in    { beginToken(yytext); return IN;}
end   { beginToken(yytext); return END;}
function { beginToken(yytext); return FUNCTION;}
var   { beginToken(yytext); return VAR;}
array { beginToken(yytext); return ARRAY;}
of    { beginToken(yytext); return OF;}
break { beginToken(yytext); return BREAK;}
type  { beginToken(yytext); return TYPE;}
nil   { beginToken(yytext); return NIL; }
":="    { beginToken(yytext); return ASSIGN;}
"+"    { beginToken(yytext); yylval.op = PlusOp; return PLUS; }
"-"    { beginToken(yytext); yylval.op = MinusOp; return MINUS; }
"*"    { beginToken(yytext); yylval.op = TimesOp; return TIMES; }
"/"    { beginToken(yytext); yylval.op = DivideOp; return DIVIDE; }
"="    { beginToken(yytext); yylval.op = EqOp; return EQ; }
"<>"    { beginToken(yytext); yylval.op = NeOp; return NE; }
">"     { beginToken(yytext); yylval.op = GtOp; return GT; }
"<"     { beginToken(yytext); yylval.op = LtOp; return LT; }
">="    { beginToken(yytext); yylval.op = GeOp; return GE; }
"<="    { beginToken(yytext); yylval.op = LeOp; return LE; }
"&"     { beginToken(yytext); yylval.op = AndOp; return AND; }
"|"     { beginToken(yytext); yylval.op = OrOp; return OR; }

{NUMBER} { 
  beginToken(yytext); 
  sscanf(yytext, "%d", &yylval.n);
  return INTEGER_CONSTANT;
}

{IDENT} {
  beginToken(yytext);
  yylval.s = yytext;
  if (isTypeID(yytext)) {
    return TIGER_TYPE_IDENT;
  } else {
    return TIGER_IDENT;
  }
}

{SYMBOLS} {
  beginToken(yytext);
  return yytext[0];
}

{WSPACE} { beginToken(yytext);}

\" {BEGIN(STRING); yymore();}
<STRING>\n {
  yylval.s = yytext;
  BEGIN(INITIAL);
  return TIGER_ERROR;
}
<STRING>\"      { 
  beginToken(yytext);
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

\n { beginToken(yytext);}

<<EOF>> { return 0; }

. {
  beginToken(yytext);
  printf("unknown token %s\n", yytext);
  return TIGER_ERROR;
}
%%
