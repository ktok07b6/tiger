/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



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




/* Copy the first part of user declarations.  */
#line 1 "parse/tiger.y"

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



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

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
/* Line 187 of yacc.c.  */
#line 219 "parse/parser.cpp"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 232 "parse/parser.cpp"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  49
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   392

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  51
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  38
/* YYNRULES -- Number of rules.  */
#define YYNRULES  92
/* YYNRULES -- Number of states.  */
#define YYNSTATES  174

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   295

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      44,    45,     2,     2,    46,     2,    41,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    50,    47,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    42,     2,    43,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    48,     2,    49,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     7,     9,    11,    13,    15,    17,
      19,    21,    23,    25,    27,    29,    31,    33,    35,    37,
      39,    41,    43,    45,    47,    51,    53,    57,    62,    65,
      69,    73,    77,    79,    81,    83,    85,    87,    89,    91,
      93,    95,    97,    99,   101,   105,   110,   111,   113,   117,
     118,   120,   124,   128,   130,   135,   139,   143,   145,   149,
     156,   161,   168,   173,   182,   191,   193,   199,   201,   204,
     206,   208,   210,   215,   220,   222,   224,   228,   232,   236,
     240,   244,   248,   249,   251,   255,   260,   267,   272,   279,
     287,   297,   299
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      52,     0,    -1,    53,    -1,    54,    -1,    55,    -1,    56,
      -1,    57,    -1,    59,    -1,    60,    -1,    64,    -1,    65,
      -1,    68,    -1,    70,    -1,    73,    -1,    74,    -1,    75,
      -1,    76,    -1,    77,    -1,    78,    -1,     8,    -1,     9,
      -1,    10,    -1,    58,    -1,    87,    -1,    58,    41,    87,
      -1,    88,    -1,    58,    41,    88,    -1,    58,    42,    53,
      43,    -1,    29,    53,    -1,    53,    61,    53,    -1,    53,
      62,    53,    -1,    53,    63,    53,    -1,    28,    -1,    29,
      -1,    30,    -1,    31,    -1,    32,    -1,    33,    -1,    34,
      -1,    35,    -1,    36,    -1,    37,    -1,    38,    -1,    39,
      -1,    58,    27,    53,    -1,    87,    44,    66,    45,    -1,
      -1,    53,    -1,    66,    46,    53,    -1,    -1,    53,    -1,
      67,    47,    53,    -1,    44,    67,    45,    -1,    67,    -1,
      88,    48,    72,    49,    -1,    87,    32,    53,    -1,    88,
      32,    53,    -1,    71,    -1,    72,    46,    71,    -1,    88,
      42,    53,    43,    25,    53,    -1,    12,    53,    13,    53,
      -1,    12,    53,    13,    53,    14,    53,    -1,    15,    53,
      16,    53,    -1,    17,    87,    27,    53,    18,    53,    16,
      53,    -1,    17,    88,    27,    53,    18,    53,    16,    53,
      -1,    26,    -1,    19,    79,    20,    69,    21,    -1,    80,
      -1,    79,    80,    -1,    81,    -1,    85,    -1,    86,    -1,
      40,    87,    32,    82,    -1,    40,    88,    32,    82,    -1,
      88,    -1,    87,    -1,    48,    84,    49,    -1,    24,    25,
      88,    -1,    24,    25,    87,    -1,    87,    50,    88,    -1,
      87,    50,    87,    -1,    88,    50,    88,    -1,    -1,    83,
      -1,    84,    46,    83,    -1,    23,    87,    27,    53,    -1,
      23,    87,    50,    88,    27,    53,    -1,    23,    88,    27,
      53,    -1,    23,    88,    50,    88,    27,    53,    -1,    22,
      87,    44,    84,    45,    32,    53,    -1,    22,    87,    44,
      84,    45,    50,    88,    32,    53,    -1,     4,    -1,     5,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   129,   129,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   162,
     172,   181,   190,   199,   204,   209,   214,   219,   228,   237,
     242,   247,   261,   262,   263,   264,   270,   271,   272,   273,
     274,   275,   281,   282,   288,   297,   307,   313,   320,   331,
     337,   344,   354,   371,   388,   397,   402,   412,   419,   429,
     438,   443,   452,   461,   466,   476,   485,   494,   501,   511,
     512,   513,   518,   524,   535,   540,   546,   551,   556,   565,
     570,   575,   585,   590,   597,   607,   612,   617,   622,   631,
     636,   645,   654
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "TIGER_SYMBOL", "TIGER_IDENT",
  "TIGER_TYPE_IDENT", "TIGER_EOL", "TIGER_EOF", "STRING_CONSTANT",
  "INTEGER_CONSTANT", "NIL", "TIGER_ERROR", "IF", "THEN", "ELSE", "WHILE",
  "DO", "FOR", "TO", "LET", "IN", "END", "FUNCTION", "VAR", "ARRAY", "OF",
  "BREAK", "ASSIGN", "PLUS", "MINUS", "TIMES", "DIVIDE", "EQ", "NE", "GT",
  "LT", "GE", "LE", "AND", "OR", "TYPE", "'.'", "'['", "']'", "'('", "')'",
  "','", "';'", "'{'", "'}'", "':'", "$accept", "program", "expr",
  "string_expr", "integer_expr", "nil_expr", "lvalue_expr", "lvalue",
  "neg_expr", "binary_op_expr", "arithmetic_op", "compare_op",
  "boolean_op", "assignment_expr", "function_call", "expr_list", "seq",
  "seq_expr", "let_seq_expr", "record_creation_expr", "record_field",
  "record_field_list", "array_creation_expr", "if_expr", "while_expr",
  "for_expr", "break_expr", "let_expr", "decs", "dec", "tydec", "ty",
  "tyfield", "tyfields", "vardec", "fundec", "id", "type_id", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,    46,    91,    93,    40,    41,    44,    59,   123,   125,
      58
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    51,    52,    53,    53,    53,    53,    53,    53,    53,
      53,    53,    53,    53,    53,    53,    53,    53,    53,    54,
      55,    56,    57,    58,    58,    58,    58,    58,    59,    60,
      60,    60,    61,    61,    61,    61,    62,    62,    62,    62,
      62,    62,    63,    63,    64,    65,    66,    66,    66,    67,
      67,    67,    68,    69,    70,    71,    71,    72,    72,    73,
      74,    74,    75,    76,    76,    77,    78,    79,    79,    80,
      80,    80,    81,    81,    82,    82,    82,    82,    82,    83,
      83,    83,    84,    84,    84,    85,    85,    85,    85,    86,
      86,    87,    88
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     1,     3,     4,     2,     3,
       3,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     4,     0,     1,     3,     0,
       1,     3,     3,     1,     4,     3,     3,     1,     3,     6,
       4,     6,     4,     8,     8,     1,     5,     1,     2,     1,
       1,     1,     4,     4,     1,     1,     3,     3,     3,     3,
       3,     3,     0,     1,     3,     4,     6,     4,     6,     7,
       9,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,    91,    92,    19,    20,    21,     0,     0,     0,     0,
      65,     0,    49,     0,     2,     3,     4,     5,     6,    22,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    23,    25,     0,     0,     0,     0,     0,     0,
       0,     0,    67,    69,    70,    71,    28,    50,     0,     1,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,     0,     0,     0,     0,     0,     0,    46,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      49,    68,    52,     0,    29,    30,    31,    44,    24,    26,
       0,    47,     0,     0,    57,     0,     0,     0,    60,    62,
       0,     0,    82,     0,     0,     0,     0,     0,     0,    53,
       0,    51,    27,    45,     0,     0,     0,    54,     0,     0,
       0,     0,     0,    83,     0,     0,     0,    85,     0,    87,
       0,     0,    82,    72,    75,    74,    73,    66,    48,     0,
      58,    55,    56,    61,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    59,     0,     0,     0,     0,    84,
      80,    79,    81,    86,    88,    78,    77,    76,    63,    64,
      89,     0,     0,    90
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    13,    47,    15,    16,    17,    18,    19,    20,    21,
      62,    63,    64,    22,    23,    92,    48,    24,   110,    25,
      94,    95,    26,    27,    28,    29,    30,    31,    41,    42,
      43,   133,   123,   124,    44,    45,    32,    33
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -58
static const yytype_int16 yypact[] =
{
       5,   -58,   -58,   -58,   -58,   -58,     5,     5,    33,    10,
     -58,     5,     5,    12,   353,   -58,   -58,   -58,   -58,    -2,
     -58,   -58,   -58,   -58,   -58,   -58,   -58,   -58,   -58,   -58,
     -58,   -58,   -36,    -7,   152,   217,    15,    16,    40,    33,
      33,    -4,   -58,   -58,   -58,   -58,   110,   353,   -17,   -58,
     -58,   -58,   -58,   -58,   -58,   -58,   -58,   -58,   -58,   -58,
     -58,   -58,     5,     5,     5,     5,    33,     5,     5,     5,
      33,     5,     5,     5,     5,    13,   -24,   -23,    24,    26,
       5,   -58,   -58,     5,   353,   353,   353,   353,   -58,   -58,
     321,   353,     1,   337,   -58,     2,    27,    34,   181,   353,
     287,   309,    33,     5,    65,     5,    65,    -3,    -3,    28,
      55,   353,   -58,   -58,     5,    52,    33,   -58,     5,     5,
       5,     5,     5,   -58,     8,    29,    30,   353,    51,   353,
      54,    57,    33,   -58,   -58,   -58,   -58,   -58,   353,     5,
     -58,   353,   353,   353,   241,   265,   -27,    33,    33,    65,
       5,     5,    33,     6,   353,     5,     5,     5,    65,   -58,
     -58,   -58,   -58,   353,   353,   -58,   -58,   -58,   353,   353,
     353,    53,     5,   353
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -58,   -58,     0,   -58,   -58,   -58,   -58,   -58,   -58,   -58,
     -58,   -58,   -58,   -58,   -58,   -58,     4,   -58,   -58,   -58,
     -30,   -58,   -58,   -58,   -58,   -58,   -58,   -58,   -58,    47,
     -58,   -19,   -57,   -40,   -58,   -58,    92,    21
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      14,     1,     2,   103,   105,   157,    34,    35,    68,     1,
       2,    46,    49,     3,     4,     5,    80,     6,    38,    39,
       7,   131,     8,   158,     9,    65,   104,   106,    82,    37,
      83,    10,    38,    39,    11,    69,    40,     1,     2,    66,
      67,    70,    73,    74,     1,   132,   113,   114,   116,    12,
      40,   117,   147,   146,   147,   167,   107,   102,   108,   118,
      77,    79,    84,    85,    86,    87,   119,    90,    91,    93,
       2,    98,    99,   100,   101,    83,   137,   139,   150,   148,
     149,   151,   152,   111,   109,   172,   140,    89,    81,   136,
     159,    97,   153,     0,     0,     0,     0,     0,     0,     0,
      36,     0,     0,   127,     0,   129,     0,     0,     0,     0,
       0,     0,     0,     0,   138,     0,     0,     0,   141,   142,
     143,   144,   145,   126,     0,   128,     0,   130,   135,   135,
      75,    76,    78,     0,     0,     0,     0,    97,     0,   154,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
     163,   164,     0,   126,     0,   168,   169,   170,    88,     0,
       0,     0,    96,     0,     0,    71,     0,     0,   126,   161,
     162,     0,   173,   166,     0,     0,     0,     0,     0,   171,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,     0,     0,   125,   120,     0,     0,     0,   134,
     134,     0,     0,     0,     0,     0,     0,     0,    96,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,     0,     0,     0,   125,     0,     0,     0,     0,     0,
       0,     0,     0,    72,     0,     0,     0,     0,     0,   125,
     160,     0,     0,     0,   165,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,   155,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,   156,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,   121,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,   122,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,     0,     0,     0,   112,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,     0,     0,     0,
     115,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61
};

static const yytype_int16 yycheck[] =
{
       0,     4,     5,    27,    27,    32,     6,     7,    44,     4,
       5,    11,     0,     8,     9,    10,    20,    12,    22,    23,
      15,    24,    17,    50,    19,    27,    50,    50,    45,     8,
      47,    26,    22,    23,    29,    42,    40,     4,     5,    41,
      42,    48,    27,    27,     4,    48,    45,    46,    46,    44,
      40,    49,    46,    45,    46,    49,    32,    44,    32,    32,
      39,    40,    62,    63,    64,    65,    32,    67,    68,    69,
       5,    71,    72,    73,    74,    47,    21,    25,    27,    50,
      50,    27,    25,    83,    80,    32,   116,    66,    41,   108,
     147,    70,   132,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       8,    -1,    -1,   103,    -1,   105,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   114,    -1,    -1,    -1,   118,   119,
     120,   121,   122,   102,    -1,   104,    -1,   106,   107,   108,
      38,    39,    40,    -1,    -1,    -1,    -1,   116,    -1,   139,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
     150,   151,    -1,   132,    -1,   155,   156,   157,    66,    -1,
      -1,    -1,    70,    -1,    -1,    13,    -1,    -1,   147,   148,
     149,    -1,   172,   152,    -1,    -1,    -1,    -1,    -1,   158,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    -1,    -1,   102,    14,    -1,    -1,    -1,   107,
     108,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   116,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    -1,    -1,    -1,   132,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    16,    -1,    -1,    -1,    -1,    -1,   147,
     148,    -1,    -1,    -1,   152,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    16,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    16,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    18,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    18,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    -1,    -1,    -1,    43,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    -1,    -1,    -1,
      43,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     4,     5,     8,     9,    10,    12,    15,    17,    19,
      26,    29,    44,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    64,    65,    68,    70,    73,    74,    75,    76,
      77,    78,    87,    88,    53,    53,    87,    88,    22,    23,
      40,    79,    80,    81,    85,    86,    53,    53,    67,     0,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    61,    62,    63,    27,    41,    42,    44,    42,
      48,    13,    16,    27,    27,    87,    87,    88,    87,    88,
      20,    80,    45,    47,    53,    53,    53,    53,    87,    88,
      53,    53,    66,    53,    71,    72,    87,    88,    53,    53,
      53,    53,    44,    27,    50,    27,    50,    32,    32,    67,
      69,    53,    43,    45,    46,    43,    46,    49,    32,    32,
      14,    18,    18,    83,    84,    87,    88,    53,    88,    53,
      88,    24,    48,    82,    87,    88,    82,    21,    53,    25,
      71,    53,    53,    53,    53,    53,    45,    46,    50,    50,
      27,    27,    25,    84,    53,    16,    16,    32,    50,    83,
      87,    88,    88,    53,    53,    87,    88,    49,    53,    53,
      53,    88,    32,    53
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 130 "parse/tiger.y"
    {
	result_syntax_tree = (yyvsp[(1) - (1)].exp);
	PARSER_DBG("program %p", result_syntax_tree);
}
    break;

  case 18:
#line 154 "parse/tiger.y"
    {
	(yyval.exp) = (yyvsp[(1) - (1)].exp);
	PARSER_DBG("expr %p", (yyval.exp));
}
    break;

  case 19:
#line 163 "parse/tiger.y"
    {
	PARSER_DBG("str = %s", (yyvsp[(1) - (1)].s));
	(yyval.exp) = gcnew(StringExp, ((yyvsp[(1) - (1)].s), 0));
	PARSER_DBG("StringExp %p", (yyval.exp));
}
    break;

  case 20:
#line 173 "parse/tiger.y"
    {
	(yyval.exp) = gcnew(IntExp, ((yyvsp[(1) - (1)].n)));
	PARSER_DBG("IntExp %p", (yyval.exp));
}
    break;

  case 21:
#line 182 "parse/tiger.y"
    {
	(yyval.exp) = gcnew(NilExp, ());
	PARSER_DBG("NilExp %p", (yyval.exp));
}
    break;

  case 22:
#line 191 "parse/tiger.y"
    {
	(yyval.exp) = gcnew(VarExp, ((yyvsp[(1) - (1)].var)));
	PARSER_DBG("VarExp %p", (yyval.exp));
}
    break;

  case 23:
#line 200 "parse/tiger.y"
    {
	(yyval.var) = gcnew(SimpleVar, ((yyvsp[(1) - (1)].sym), 0));
	PARSER_DBG("SimpleVar %p", (yyval.var));
}
    break;

  case 24:
#line 205 "parse/tiger.y"
    {
	(yyval.var) = gcnew(FieldVar, ((yyvsp[(1) - (3)].var), (yyvsp[(3) - (3)].sym), 0));
	PARSER_DBG("FieldVar %p", (yyval.var));
}
    break;

  case 25:
#line 210 "parse/tiger.y"
    {
	(yyval.var) = gcnew(SimpleVar, ((yyvsp[(1) - (1)].sym), 0));
	PARSER_DBG("SimpleVar %p", (yyval.var));
}
    break;

  case 26:
#line 215 "parse/tiger.y"
    {
	(yyval.var) = gcnew(FieldVar, ((yyvsp[(1) - (3)].var), (yyvsp[(3) - (3)].sym), 0));
	PARSER_DBG("FieldVar %p", (yyval.var));
}
    break;

  case 27:
#line 220 "parse/tiger.y"
    {
	(yyval.var) = gcnew(SubscriptVar, ((yyvsp[(1) - (4)].var), (yyvsp[(3) - (4)].exp), 0));
	PARSER_DBG("SubscriptVar %p", (yyval.var));
}
    break;

  case 28:
#line 229 "parse/tiger.y"
    {
	(yyval.exp) = gcnew(OpExp, (gcnew(IntExp, (0)), MinusOp, (yyvsp[(2) - (2)].exp), 0));
	PARSER_DBG("OpExp %p", (yyval.exp));
}
    break;

  case 29:
#line 238 "parse/tiger.y"
    {
	(yyval.exp) = gcnew(OpExp, ((yyvsp[(1) - (3)].exp), (yyvsp[(2) - (3)].op), (yyvsp[(3) - (3)].exp), 0));
	PARSER_DBG("OpExp %p", (yyval.exp));
}
    break;

  case 30:
#line 243 "parse/tiger.y"
    {
	(yyval.exp) = gcnew(OpExp, ((yyvsp[(1) - (3)].exp), (yyvsp[(2) - (3)].op), (yyvsp[(3) - (3)].exp), 0));
	PARSER_DBG("OpExp %p", (yyval.exp));
}
    break;

  case 31:
#line 248 "parse/tiger.y"
    {
	if ((yyvsp[(2) - (3)].op) == AndOp) {
		//TODO:
	} else if ((yyvsp[(2) - (3)].op) == OrOp) {
		//TODO:
	}
	(yyval.exp) = gcnew(OpExp, ((yyvsp[(1) - (3)].exp), (yyvsp[(2) - (3)].op), (yyvsp[(3) - (3)].exp), 0));
	PARSER_DBG("OpExp %p", (yyval.exp));
}
    break;

  case 35:
#line 265 "parse/tiger.y"
    {}
    break;

  case 41:
#line 276 "parse/tiger.y"
    {}
    break;

  case 43:
#line 283 "parse/tiger.y"
    {}
    break;

  case 44:
#line 289 "parse/tiger.y"
    {
	(yyval.exp) = gcnew(AssignExp, ((yyvsp[(1) - (3)].var), (yyvsp[(3) - (3)].exp), 0));
	PARSER_DBG("AssignExp %p", (yyval.exp));
}
    break;

  case 45:
#line 298 "parse/tiger.y"
    {
	(yyval.exp) = gcnew(CallExp, ((yyvsp[(1) - (4)].sym), (yyvsp[(3) - (4)].exps), 0));
	PARSER_DBG("CallExp %p", (yyval.exp));
}
    break;

  case 46:
#line 307 "parse/tiger.y"
    {
	ExpList *list = gcnew(ExpList, ());

	(yyval.exps) = list;
	PARSER_DBG("ExpList %p", (yyval.exps));
}
    break;

  case 47:
#line 314 "parse/tiger.y"
    {
	ExpList *list = gcnew(ExpList, ());
	list->push_back((yyvsp[(1) - (1)].exp));
	(yyval.exps) = list;
	PARSER_DBG("ExpList %p", (yyval.exps));
}
    break;

  case 48:
#line 321 "parse/tiger.y"
    {
	(yyvsp[(1) - (3)].exps)->push_back((yyvsp[(3) - (3)].exp));
	(yyval.exps) = (yyvsp[(1) - (3)].exps);
	PARSER_DBG("ExpList %p", (yyval.exps));
}
    break;

  case 49:
#line 331 "parse/tiger.y"
    {
	Seq *seq = gcnew(Seq, ());
	seq->push_back(gcnew(NilExp, ()));
	(yyval.seq) = seq;
	PARSER_DBG("Seq %p", (yyval.seq));
}
    break;

  case 50:
#line 338 "parse/tiger.y"
    {
	Seq *seq = gcnew(Seq, ());
	seq->push_back((yyvsp[(1) - (1)].exp));
	(yyval.seq) = seq;
	PARSER_DBG("Seq %p", (yyval.seq));
}
    break;

  case 51:
#line 345 "parse/tiger.y"
    {
	(yyvsp[(1) - (3)].seq)->push_back((yyvsp[(3) - (3)].exp));
	(yyval.seq) = (yyvsp[(1) - (3)].seq);
	PARSER_DBG("Seq %p", (yyval.seq));
}
    break;

  case 52:
#line 355 "parse/tiger.y"
    {
	Seq *seq = ((yyvsp[(2) - (3)].seq));
	size_t sz = seq->size();
	if (sz == 0) {
		(yyval.exp) = gcnew(NilExp, ());
	} else if (sz == 1) {
		(yyval.exp) = seq->at(0);
	} else {
		(yyval.exp) = gcnew(SeqExp, (seq));
	}
	PARSER_DBG("SeqExp %p", (yyval.exp));
}
    break;

  case 53:
#line 372 "parse/tiger.y"
    {
	Seq *seq = ((yyvsp[(1) - (1)].seq));
	size_t sz = seq->size();
	if (sz == 0) {
		(yyval.exp) = gcnew(NilExp, ());
	} else if (sz == 1) {
		(yyval.exp) = seq->at(0);
	} else {
		(yyval.exp) = gcnew(SeqExp, (seq));
	}
	PARSER_DBG("SeqExp %p", (yyval.exp));
}
    break;

  case 54:
#line 389 "parse/tiger.y"
    {
	(yyval.exp) = gcnew(RecordExp, ((yyvsp[(3) - (4)].recfields), (yyvsp[(1) - (4)].sym), 0));
	PARSER_DBG("RecordExp %p", (yyval.exp));
}
    break;

  case 55:
#line 398 "parse/tiger.y"
    {
	(yyval.recfield) = gcnew(RecordField, ((yyvsp[(1) - (3)].sym), (yyvsp[(3) - (3)].exp), 0));
	PARSER_DBG("RecordField %p", (yyval.recfield));
}
    break;

  case 56:
#line 403 "parse/tiger.y"
    {
	(yyval.recfield) = gcnew(RecordField, ((yyvsp[(1) - (3)].sym), (yyvsp[(3) - (3)].exp), 0));
	PARSER_DBG("RecordField %p", (yyval.recfield));
}
    break;

  case 57:
#line 413 "parse/tiger.y"
    {
	RecordFieldList *list = gcnew(RecordFieldList, ());
	list->push_back((yyvsp[(1) - (1)].recfield));
	(yyval.recfields) = list;
	PARSER_DBG("RecordFieldList %p", (yyval.recfields));
}
    break;

  case 58:
#line 420 "parse/tiger.y"
    {
	(yyvsp[(1) - (3)].recfields)->push_back((yyvsp[(3) - (3)].recfield));
	(yyval.recfields) = (yyvsp[(1) - (3)].recfields);
	PARSER_DBG("RecordFieldList %p", (yyval.recfields));
}
    break;

  case 59:
#line 430 "parse/tiger.y"
    {
	(yyval.exp) = gcnew(ArrayExp, ((yyvsp[(1) - (6)].sym), (yyvsp[(3) - (6)].exp), (yyvsp[(6) - (6)].exp), 0));
	PARSER_DBG("ArrayExp %p", (yyval.exp));
}
    break;

  case 60:
#line 439 "parse/tiger.y"
    {
	(yyval.exp) = gcnew(IfExp, ((yyvsp[(2) - (4)].exp), (yyvsp[(4) - (4)].exp), 0));
	PARSER_DBG("IfExp %p", (yyval.exp));
}
    break;

  case 61:
#line 444 "parse/tiger.y"
    {
	(yyval.exp) = gcnew(IfExp, ((yyvsp[(2) - (6)].exp), (yyvsp[(4) - (6)].exp), (yyvsp[(6) - (6)].exp), 0)); 
	PARSER_DBG("IfExp %p", (yyval.exp));
}
    break;

  case 62:
#line 453 "parse/tiger.y"
    {
	(yyval.exp) = gcnew(WhileExp, ((yyvsp[(2) - (4)].exp), (yyvsp[(4) - (4)].exp), 0));
	PARSER_DBG("WhileExp %p", (yyval.exp));
}
    break;

  case 63:
#line 462 "parse/tiger.y"
    {
	(yyval.exp) = gcnew(ForExp, ((yyvsp[(2) - (8)].sym), false, (yyvsp[(4) - (8)].exp), (yyvsp[(6) - (8)].exp), (yyvsp[(8) - (8)].exp), 0));
	PARSER_DBG("ForExp %p", (yyval.exp));
}
    break;

  case 64:
#line 467 "parse/tiger.y"
    {
	(yyval.exp) = gcnew(ForExp, ((yyvsp[(2) - (8)].sym), false, (yyvsp[(4) - (8)].exp), (yyvsp[(6) - (8)].exp), (yyvsp[(8) - (8)].exp), 0));
	PARSER_DBG("ForExp %p", (yyval.exp));
}
    break;

  case 65:
#line 477 "parse/tiger.y"
    {
	(yyval.exp) = gcnew(BreakExp, (0));
	PARSER_DBG("BreakExp %p", (yyval.exp));
}
    break;

  case 66:
#line 486 "parse/tiger.y"
    {
	(yyval.exp) = gcnew(LetExp, ((yyvsp[(2) - (5)].decs), (SeqExp*)(yyvsp[(4) - (5)].exp), 0));
	PARSER_DBG("LetExp %p", (yyval.exp));
}
    break;

  case 67:
#line 495 "parse/tiger.y"
    {
	DecList *list  = gcnew(DecList, ());
	list->push_back((yyvsp[(1) - (1)].dec));
	(yyval.decs) = list;
	PARSER_DBG("DecList %p", (yyval.decs));
}
    break;

  case 68:
#line 502 "parse/tiger.y"
    {
	(yyvsp[(1) - (2)].decs)->push_back((yyvsp[(2) - (2)].dec));
	(yyval.decs) = (yyvsp[(1) - (2)].decs);
	PARSER_DBG("DecList %p", (yyval.decs));
}
    break;

  case 72:
#line 519 "parse/tiger.y"
    {
	(yyval.dec) = gcnew(TypeDec, ((yyvsp[(2) - (4)].sym), (yyvsp[(4) - (4)].ty), 0));
	typeIDs.insert((yyvsp[(2) - (4)].sym)->name);
	PARSER_DBG("TypeDec %p", (yyval.dec));
}
    break;

  case 73:
#line 525 "parse/tiger.y"
    {
	(yyval.dec) = gcnew(TypeDec, ((yyvsp[(2) - (4)].sym), (yyvsp[(4) - (4)].ty), 0));
	typeIDs.insert((yyvsp[(2) - (4)].sym)->name);
	PARSER_DBG("TypeDec %p", (yyval.dec));
}
    break;

  case 74:
#line 536 "parse/tiger.y"
    {
	(yyval.ty) = gcnew(NameTy, ((yyvsp[(1) - (1)].sym), 0));
	PARSER_DBG("NameTy %p", (yyval.ty));
}
    break;

  case 75:
#line 541 "parse/tiger.y"
    {
	(yyval.ty) = gcnew(NameTy, ((yyvsp[(1) - (1)].sym), 0));
	PARSER_DBG("NameTy %p", (yyval.ty));
}
    break;

  case 76:
#line 547 "parse/tiger.y"
    {
	(yyval.ty) = gcnew(RecordTy, ((yyvsp[(2) - (3)].fields)));
	PARSER_DBG("RecordTy %p", (yyval.ty));
}
    break;

  case 77:
#line 552 "parse/tiger.y"
    {
	(yyval.ty) = gcnew(ArrayTy, ((yyvsp[(3) - (3)].sym), 0));
	PARSER_DBG("ArrayTy %p", (yyval.ty));
}
    break;

  case 78:
#line 557 "parse/tiger.y"
    {
	(yyval.ty) = gcnew(ArrayTy, ((yyvsp[(3) - (3)].sym), 0));
	PARSER_DBG("ArrayTy %p", (yyval.ty));
}
    break;

  case 79:
#line 566 "parse/tiger.y"
    {
	(yyval.field) = gcnew(TypeField, ((yyvsp[(1) - (3)].sym), false, (yyvsp[(3) - (3)].sym), 0));
	PARSER_DBG("TypeField %p", (yyval.field));
}
    break;

  case 80:
#line 571 "parse/tiger.y"
    {
	(yyval.field) = gcnew(TypeField, ((yyvsp[(1) - (3)].sym), false, (yyvsp[(3) - (3)].sym), 0));
	PARSER_DBG("TypeField %p", (yyval.field));
}
    break;

  case 81:
#line 576 "parse/tiger.y"
    {
	(yyval.field) = gcnew(TypeField, ((yyvsp[(1) - (3)].sym), false, (yyvsp[(3) - (3)].sym), 0));
	PARSER_DBG("TypeField %p", (yyval.field));
}
    break;

  case 82:
#line 585 "parse/tiger.y"
    {
	TypeFieldList *list = gcnew(TypeFieldList, ());
	(yyval.fields) = list;
	PARSER_DBG("TypeFieldList %p", (yyval.fields));
}
    break;

  case 83:
#line 591 "parse/tiger.y"
    {
	TypeFieldList *list = gcnew(TypeFieldList, ());
	list->push_back((yyvsp[(1) - (1)].field));
	(yyval.fields) = list;
	PARSER_DBG("TypeFieldList %p", (yyval.fields));
}
    break;

  case 84:
#line 598 "parse/tiger.y"
    {
	(yyvsp[(1) - (3)].fields)->push_back((yyvsp[(3) - (3)].field));
	(yyval.fields) = (yyvsp[(1) - (3)].fields);
	PARSER_DBG("TypeFieldList %p", (yyval.fields));
}
    break;

  case 85:
#line 608 "parse/tiger.y"
    {
	(yyval.dec) = gcnew(VarDec, ((yyvsp[(2) - (4)].sym), false, Symbol::symbol(""), (yyvsp[(4) - (4)].exp), 0));
	PARSER_DBG("VarDec %p", (yyval.dec));
}
    break;

  case 86:
#line 613 "parse/tiger.y"
    {
	(yyval.dec) = gcnew(VarDec, ((yyvsp[(2) - (6)].sym), false, (yyvsp[(4) - (6)].sym), (yyvsp[(6) - (6)].exp), 0));
	PARSER_DBG("VarDec %p", (yyval.dec));
}
    break;

  case 87:
#line 618 "parse/tiger.y"
    {
	(yyval.dec) = gcnew(VarDec, ((yyvsp[(2) - (4)].sym), false, Symbol::symbol(""), (yyvsp[(4) - (4)].exp), 0));
	PARSER_DBG("VarDec %p", (yyval.dec));
}
    break;

  case 88:
#line 623 "parse/tiger.y"
    {
	(yyval.dec) = gcnew(VarDec, ((yyvsp[(2) - (6)].sym), false, (yyvsp[(4) - (6)].sym), (yyvsp[(6) - (6)].exp), 0));
	PARSER_DBG("VarDec %p", (yyval.dec));
}
    break;

  case 89:
#line 632 "parse/tiger.y"
    {
	(yyval.dec) = gcnew(FunDec, ((yyvsp[(2) - (7)].sym), (yyvsp[(4) - (7)].fields), Symbol::symbol(""), (yyvsp[(7) - (7)].exp), 0));
	PARSER_DBG("FunDec %p", (yyval.dec));
}
    break;

  case 90:
#line 637 "parse/tiger.y"
    {
	(yyval.dec) = gcnew(FunDec, ((yyvsp[(2) - (9)].sym), (yyvsp[(4) - (9)].fields), (yyvsp[(7) - (9)].sym), (yyvsp[(9) - (9)].exp), 0));
	PARSER_DBG("FunDec %p", (yyval.dec));
}
    break;

  case 91:
#line 646 "parse/tiger.y"
    {
	(yyval.sym) = Symbol::symbol((yyvsp[(1) - (1)].s));
	PARSER_DBG("Symbol %p", (yyval.sym));
}
    break;

  case 92:
#line 655 "parse/tiger.y"
    {
	(yyval.sym) = Symbol::symbol((yyvsp[(1) - (1)].s));
	PARSER_DBG("Symbol(type) %p", (yyval.sym));
}
    break;


/* Line 1267 of yacc.c.  */
#line 2180 "parse/parser.cpp"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 661 "parse/tiger.y"


