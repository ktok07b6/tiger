%{
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include "tiger.h"
#include "Absyn.h"
#include <vector>
#include <set>
#include <string>
#include "debug.h"
#include "HeapManager.h"

using namespace std;
//#define YYDEBUG 1
extern "C" void yyerror(char *s) {
	printf("%s\n",s);
}

int yylex(void);
extern Absyn *result_syntax_tree;

%}
%union {
	Absyn *absyn;
	Var *var;
	Exp *exp;
	Dec *dec;
	ExpList *exps;
	Seq *seq;
	DecList *decs;
	RecordField *recfield;
	RecordFieldList *recfields;
	TypeField *field;
	TypeFieldList *fields;
	Ty *ty;
	Symbol *sym;
	Oper op;
	const char *s;
	int n;
}

%token <s> TIGER_SYMBOL
%token <s> TIGER_IDENT
%token <s> TIGER_TYPE_IDENT
%token TIGER_EOL
%token TIGER_EOF
%token <s> STRING_CONSTANT
%token <n> INTEGER_CONSTANT
%token NIL
%token TIGER_ERROR
%token IF
%token THEN
%token ELSE
%token WHILE
%token DO
%token FOR
%token TO
%token LET
%token IN
%token END
%token FUNCTION
%token VAR
%token ARRAY
%token OF
%token BREAK
//%token ASSIGN
%right ASSIGN
%nonassoc <op> AND OR
%nonassoc <op> EQ NE LT GT LE GE
%left <op> PLUS MINUS
%left <op> TIMES DIVIDE
 //%token <op> PLUS
 //%token <op> MINUS
 //%token <op> TIMES
 //%token <op> DIVIDE
 //%token <op> EQ
 //%token <op> NE
 //%token <op> GT
 //%token <op> LT
 //%token <op> GE
 //%token <op> LE
 //%token <op> AND
 //%token <op> OR
%token TYPE

%type<sym> id
%type<sym> type_id
%type<op> arithmetic_op
%type<op> compare_op
%type<op> boolean_op
%type<exp> expr
%type<exp> string_expr
%type<exp> integer_expr
%type<exp> nil_expr
%type<exp> lvalue_expr
%type<var> lvalue
%type<exp> neg_expr
%type<exp> binary_op_expr
%type<exp> assignment_expr
%type<exp> function_call
%type<exps> expr_list
%type<seq> seq
%type<exp> seq_expr
%type<exp> let_seq_expr
%type<exp> record_creation_expr
%type<recfield> record_field
%type<recfields> record_field_list
%type<exp> array_creation_expr
%type<exp> if_expr
%type<exp> while_expr
%type<exp> for_expr
%type<exp> break_expr
%type<exp> let_expr
%type<decs> decs
%type<dec> dec
%type<dec> tydec 
%type<dec> vardec
%type<dec> fundec
%type<ty> ty
%type<field> tyfield
%type<fields> tyfields



%type<exp> program
%%

program
: expr
{
	result_syntax_tree = $1;
	PARSER_DBG("program %p", result_syntax_tree);
}
;

//------------------------------------
expr
: string_expr
| integer_expr
| nil_expr
| lvalue_expr
| neg_expr
| binary_op_expr
| assignment_expr
| function_call
| seq_expr
| record_creation_expr
| array_creation_expr
| if_expr
| while_expr
| for_expr
| break_expr
| let_expr
{
	$$ = $1;
	PARSER_DBG("expr %p", $$);
}
;

//------------------------------------
string_expr
: STRING_CONSTANT
{
	PARSER_DBG("str = %s", $1);
	$$ = gcnew(StringExp, ($1, 0));
	PARSER_DBG("StringExp %p", $$);
}
;

//------------------------------------
integer_expr
: INTEGER_CONSTANT
{
	$$ = gcnew(IntExp, ($1));
	PARSER_DBG("IntExp %p", $$);
}
;

//------------------------------------
nil_expr
: NIL
{
	$$ = gcnew(NilExp, ());
	PARSER_DBG("NilExp %p", $$);
}
;

//------------------------------------
lvalue_expr
: lvalue
{
	$$ = gcnew(VarExp, ($1));
	PARSER_DBG("VarExp %p", $$);
}
;

//------------------------------------
lvalue
: id
{
	$$ = gcnew(SimpleVar, ($1, 0));
	PARSER_DBG("SimpleVar %p", $$);
}
| lvalue '.' id
{
	$$ = gcnew(FieldVar, ($1, $3, 0));
	PARSER_DBG("FieldVar %p", $$);
}
| type_id
{
	$$ = gcnew(SimpleVar, ($1, 0));
	PARSER_DBG("SimpleVar %p", $$);
}
| lvalue '.' type_id
{
	$$ = gcnew(FieldVar, ($1, $3, 0));
	PARSER_DBG("FieldVar %p", $$);
}
| lvalue '[' expr ']'
{
	$$ = gcnew(SubscriptVar, ($1, $3, 0));
	PARSER_DBG("SubscriptVar %p", $$);
}
;

//------------------------------------
neg_expr
: MINUS expr
{
	$$ = gcnew(OpExp, (gcnew(IntExp, (0)), MinusOp, $2, 0));
	PARSER_DBG("OpExp %p", $$);
}
;

//------------------------------------
binary_op_expr
: expr arithmetic_op expr
{
	$$ = gcnew(OpExp, ($1, $2, $3, 0));
	PARSER_DBG("OpExp %p", $$);
}
| expr compare_op expr
{
	$$ = gcnew(OpExp, ($1, $2, $3, 0));
	PARSER_DBG("OpExp %p", $$);
}
| expr boolean_op expr
{
	if ($2 == AndOp) {
		//TODO:
	} else if ($2 == OrOp) {
		//TODO:
	}
	$$ = gcnew(OpExp, ($1, $2, $3, 0));
	PARSER_DBG("OpExp %p", $$);
}
;

//------------------------------------
arithmetic_op
: PLUS
| MINUS
| TIMES
| DIVIDE
{}
;

//------------------------------------
compare_op
: EQ
| NE
| GT
| LT
| GE
| LE
{}
;

//------------------------------------
boolean_op
: AND
| OR
{}
;

//------------------------------------
assignment_expr
: lvalue ASSIGN expr
{
	$$ = gcnew(AssignExp, ($1, $3, 0));
	PARSER_DBG("AssignExp %p", $$);
}
;

//------------------------------------
function_call
: id '(' expr_list ')'
{
	$$ = gcnew(CallExp, ($1, $3, 0));
	PARSER_DBG("CallExp %p", $$);
}
;

//------------------------------------
expr_list
: /* empty */
{
	ExpList *list = gcnew(ExpList, ());

	$$ = list;
	PARSER_DBG("ExpList %p", $$);
}
| expr
{
	ExpList *list = gcnew(ExpList, ());
	list->push_back($1);
	$$ = list;
	PARSER_DBG("ExpList %p", $$);
}
| expr_list ',' expr
{
	$1->push_back($3);
	$$ = $1;
	PARSER_DBG("ExpList %p", $$);
}
;

//------------------------------------
seq
: /*empty*/
{
	Seq *seq = gcnew(Seq, ());
	seq->push_back(gcnew(NilExp, ()));
	$$ = seq;
	PARSER_DBG("Seq %p", $$);
}
| expr
{
	Seq *seq = gcnew(Seq, ());
	seq->push_back($1);
	$$ = seq;
	PARSER_DBG("Seq %p", $$);
}
| seq ';' expr
{
	$1->push_back($3);
	$$ = $1;
	PARSER_DBG("Seq %p", $$);
}
;

//------------------------------------
seq_expr
: '(' seq ')'
{
	Seq *seq = ($2);
	size_t sz = seq->size();
	if (sz == 0) {
		$$ = gcnew(NilExp, ());
	} else if (sz == 1) {
		$$ = seq->at(0);
	} else {
		$$ = gcnew(SeqExp, (seq));
	}
	PARSER_DBG("SeqExp %p", $$);
}
;

//------------------------------------
let_seq_expr
: seq
{
	Seq *seq = ($1);
	size_t sz = seq->size();
	if (sz == 0) {
		$$ = gcnew(NilExp, ());
	} else if (sz == 1) {
		$$ = seq->at(0);
	} else {
		$$ = gcnew(SeqExp, (seq));
	}
	PARSER_DBG("SeqExp %p", $$);
}
;

//------------------------------------
record_creation_expr
: type_id '{' record_field_list '}'
{
	$$ = gcnew(RecordExp, ($3, $1, 0));
	PARSER_DBG("RecordExp %p", $$);
}
;

//------------------------------------
record_field
: id EQ expr
{
	$$ = gcnew(RecordField, ($1, $3, 0));
	PARSER_DBG("RecordField %p", $$);
}
| type_id EQ expr
{
	$$ = gcnew(RecordField, ($1, $3, 0));
	PARSER_DBG("RecordField %p", $$);
}

;

//------------------------------------
record_field_list
: record_field
{
	RecordFieldList *list = gcnew(RecordFieldList, ());
	list->push_back($1);
	$$ = list;
	PARSER_DBG("RecordFieldList %p", $$);
}
| record_field_list ',' record_field
{
	$1->push_back($3);
	$$ = $1;
	PARSER_DBG("RecordFieldList %p", $$);
}
;

//------------------------------------
array_creation_expr
: type_id '[' expr ']' OF expr
{
	$$ = gcnew(ArrayExp, ($1, $3, $6, 0));
	PARSER_DBG("ArrayExp %p", $$);
}
;

//------------------------------------
if_expr
: IF expr THEN expr
{
	$$ = gcnew(IfExp, ($2, $4, 0));
	PARSER_DBG("IfExp %p", $$);
}
| IF expr THEN expr ELSE expr
{
	$$ = gcnew(IfExp, ($2, $4, $6, 0)); 
	PARSER_DBG("IfExp %p", $$);
}
;

//------------------------------------
while_expr
: WHILE expr DO expr
{
	$$ = gcnew(WhileExp, ($2, $4, 0));
	PARSER_DBG("WhileExp %p", $$);
}
;

//------------------------------------
for_expr
: FOR id ASSIGN expr TO expr DO expr
{
	$$ = gcnew(ForExp, ($2, false, $4, $6, $8, 0));
	PARSER_DBG("ForExp %p", $$);
}
| FOR type_id ASSIGN expr TO expr DO expr
{
	$$ = gcnew(ForExp, ($2, false, $4, $6, $8, 0));
	PARSER_DBG("ForExp %p", $$);
}

;

//------------------------------------
break_expr
: BREAK
{
	$$ = gcnew(BreakExp, (0));
	PARSER_DBG("BreakExp %p", $$);
}
;

//------------------------------------
let_expr
: LET decs IN let_seq_expr END
{
	$$ = gcnew(LetExp, ($2, (SeqExp*)$4, 0));
	PARSER_DBG("LetExp %p", $$);
}
;

//------------------------------------
decs
: dec
{
	DecList *list  = gcnew(DecList, ());
	list->push_back($1);
	$$ = list;
	PARSER_DBG("DecList %p", $$);
}
| decs dec
{
	$1->push_back($2);
	$$ = $1;
	PARSER_DBG("DecList %p", $$);
}
;

//------------------------------------
dec
: tydec
| vardec
| fundec
;

//------------------------------------
tydec
: TYPE id EQ ty
{
	$$ = gcnew(TypeDec, ($2, $4, 0));
	typeIDs.insert($2->name);
	PARSER_DBG("TypeDec %p", $$);
}
| TYPE type_id EQ ty
{
	$$ = gcnew(TypeDec, ($2, $4, 0));
	typeIDs.insert($2->name);
	PARSER_DBG("TypeDec %p", $$);
}

;

//------------------------------------
ty
: type_id
{
	$$ = gcnew(NameTy, ($1, 0));
	PARSER_DBG("NameTy %p", $$);
}
| id
{
	$$ = gcnew(NameTy, ($1, 0));
	PARSER_DBG("NameTy %p", $$);
}

| '{' tyfields '}'
{
	$$ = gcnew(RecordTy, ($2));
	PARSER_DBG("RecordTy %p", $$);
}
| ARRAY OF type_id
{
	$$ = gcnew(ArrayTy, ($3, 0));
	PARSER_DBG("ArrayTy %p", $$);
}
| ARRAY OF id
{
	$$ = gcnew(ArrayTy, ($3, 0));
	PARSER_DBG("ArrayTy %p", $$);
}
;

//------------------------------------
tyfield
: id ':' type_id
{
	$$ = gcnew(TypeField, ($1, false, $3, 0));
	PARSER_DBG("TypeField %p", $$);
}
| id ':' id
{
	$$ = gcnew(TypeField, ($1, false, $3, 0));
	PARSER_DBG("TypeField %p", $$);
}
| type_id ':' type_id
{
	$$ = gcnew(TypeField, ($1, false, $3, 0));
	PARSER_DBG("TypeField %p", $$);
}
;

//------------------------------------
tyfields
: /* empty */
{
	TypeFieldList *list = gcnew(TypeFieldList, ());
	$$ = list;
	PARSER_DBG("TypeFieldList %p", $$);
}
| tyfield
{
	TypeFieldList *list = gcnew(TypeFieldList, ());
	list->push_back($1);
	$$ = list;
	PARSER_DBG("TypeFieldList %p", $$);
}
| tyfields ',' tyfield 
{
	$1->push_back($3);
	$$ = $1;
	PARSER_DBG("TypeFieldList %p", $$);
}
;

//------------------------------------
vardec
: VAR id ASSIGN expr
{
	$$ = gcnew(VarDec, ($2, false, Symbol::symbol(""), $4, 0));
	PARSER_DBG("VarDec %p", $$);
}
| VAR id ':' type_id ASSIGN expr
{
	$$ = gcnew(VarDec, ($2, false, $4, $6, 0));
	PARSER_DBG("VarDec %p", $$);
}
| VAR type_id ASSIGN expr
{
	$$ = gcnew(VarDec, ($2, false, Symbol::symbol(""), $4, 0));
	PARSER_DBG("VarDec %p", $$);
}
| VAR type_id ':' type_id ASSIGN expr
{
	$$ = gcnew(VarDec, ($2, false, $4, $6, 0));
	PARSER_DBG("VarDec %p", $$);
}
;

//------------------------------------
fundec
: FUNCTION id '(' tyfields ')' EQ expr
{
	$$ = gcnew(FunDec, ($2, $4, Symbol::symbol(""), $7, 0));
	PARSER_DBG("FunDec %p", $$);
}
| FUNCTION id '(' tyfields ')' ':' type_id EQ expr
{
	$$ = gcnew(FunDec, ($2, $4, $7, $9, 0));
	PARSER_DBG("FunDec %p", $$);
}
;

//------------------------------------
id
:TIGER_IDENT
{
	$$ = Symbol::symbol($1);
	PARSER_DBG("Symbol %p", $$);
}
;

//------------------------------------
type_id
:TIGER_TYPE_IDENT
{
	$$ = Symbol::symbol($1);
	PARSER_DBG("Symbol(type) %p", $$);
}
;

%%
