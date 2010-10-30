/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TIGER_SYMBOL = 258,
     TIGER_IDENT = 259,
     TIGER_TYPE_IDENT = 260,
     TIGER_EOL = 261,
     TIGER_EOF = 262,
     STRING_CONSTANT = 263,
     INTEGER_CONSTANT = 264,
     NIL = 265,
     TIGER_ERROR = 266,
     IF = 267,
     THEN = 268,
     ELSE = 269,
     WHILE = 270,
     DO = 271,
     FOR = 272,
     TO = 273,
     LET = 274,
     IN = 275,
     END = 276,
     FUNCTION = 277,
     VAR = 278,
     ARRAY = 279,
     OF = 280,
     BREAK = 281,
     ASSIGN = 282,
     PLUS = 283,
     MINUS = 284,
     TIMES = 285,
     DIVIDE = 286,
     EQ = 287,
     NE = 288,
     GT = 289,
     LT = 290,
     GE = 291,
     LE = 292,
     AND = 293,
     OR = 294,
     TYPE = 295
   };
#endif
/* Tokens.  */
#define TIGER_SYMBOL 258
#define TIGER_IDENT 259
#define TIGER_TYPE_IDENT 260
#define TIGER_EOL 261
#define TIGER_EOF 262
#define STRING_CONSTANT 263
#define INTEGER_CONSTANT 264
#define NIL 265
#define TIGER_ERROR 266
#define IF 267
#define THEN 268
#define ELSE 269
#define WHILE 270
#define DO 271
#define FOR 272
#define TO 273
#define LET 274
#define IN 275
#define END 276
#define FUNCTION 277
#define VAR 278
#define ARRAY 279
#define OF 280
#define BREAK 281
#define ASSIGN 282
#define PLUS 283
#define MINUS 284
#define TIMES 285
#define DIVIDE 286
#define EQ 287
#define NE 288
#define GT 289
#define LT 290
#define GE 291
#define LE 292
#define AND 293
#define OR 294
#define TYPE 295




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 24 "parse/tiger.y"
{
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
/* Line 1489 of yacc.c.  */
#line 148 "parse/parser.hpp"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

