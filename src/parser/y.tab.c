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
     T_INT = 258,
     T_FLOAT = 259,
     T_BOOL = 260,
     T_STRING = 261,
     T_DOUBLE = 262,
     T_LONG = 263,
     T_AUTOREF = 264,
     T_FORALL = 265,
     T_FOR = 266,
     T_P_INF = 267,
     T_INF = 268,
     T_N_INF = 269,
     T_FUNC = 270,
     T_IF = 271,
     T_ELSE = 272,
     T_WHILE = 273,
     T_RETURN = 274,
     T_DO = 275,
     T_IN = 276,
     T_FIXEDPOINT = 277,
     T_UNTIL = 278,
     T_FILTER = 279,
     T_ADD_ASSIGN = 280,
     T_SUB_ASSIGN = 281,
     T_MUL_ASSIGN = 282,
     T_DIV_ASSIGN = 283,
     T_MOD_ASSIGN = 284,
     T_AND_ASSIGN = 285,
     T_XOR_ASSIGN = 286,
     T_OR_ASSIGN = 287,
     T_INC_OP = 288,
     T_DEC_OP = 289,
     T_PTR_OP = 290,
     T_AND_OP = 291,
     T_OR_OP = 292,
     T_LE_OP = 293,
     T_GE_OP = 294,
     T_EQ_OP = 295,
     T_NE_OP = 296,
     T_AND = 297,
     T_OR = 298,
     T_SUM = 299,
     T_AVG = 300,
     T_COUNT = 301,
     T_PRODUCT = 302,
     T_MAX = 303,
     T_MIN = 304,
     T_GRAPH = 305,
     T_DIR_GRAPH = 306,
     T_NODE = 307,
     T_EDGE = 308,
     T_UPDATES = 309,
     T_CONTAINER = 310,
     T_NODEMAP = 311,
     T_VECTOR = 312,
     T_HASHMAP = 313,
     T_HASHSET = 314,
     T_NP = 315,
     T_EP = 316,
     T_LIST = 317,
     T_SET_NODES = 318,
     T_SET_EDGES = 319,
     T_FROM = 320,
     T_BFS = 321,
     T_REVERSE = 322,
     T_INCREMENTAL = 323,
     T_DECREMENTAL = 324,
     T_STATIC = 325,
     T_DYNAMIC = 326,
     T_BATCH = 327,
     T_ONADD = 328,
     T_ONDELETE = 329,
     ID = 330,
     INT_NUM = 331,
     FLOAT_NUM = 332,
     BOOL_VAL = 333,
     CHAR_VAL = 334,
     STRING_VAL = 335,
     return_func = 336
   };
#endif
/* Tokens.  */
#define T_INT 258
#define T_FLOAT 259
#define T_BOOL 260
#define T_STRING 261
#define T_DOUBLE 262
#define T_LONG 263
#define T_AUTOREF 264
#define T_FORALL 265
#define T_FOR 266
#define T_P_INF 267
#define T_INF 268
#define T_N_INF 269
#define T_FUNC 270
#define T_IF 271
#define T_ELSE 272
#define T_WHILE 273
#define T_RETURN 274
#define T_DO 275
#define T_IN 276
#define T_FIXEDPOINT 277
#define T_UNTIL 278
#define T_FILTER 279
#define T_ADD_ASSIGN 280
#define T_SUB_ASSIGN 281
#define T_MUL_ASSIGN 282
#define T_DIV_ASSIGN 283
#define T_MOD_ASSIGN 284
#define T_AND_ASSIGN 285
#define T_XOR_ASSIGN 286
#define T_OR_ASSIGN 287
#define T_INC_OP 288
#define T_DEC_OP 289
#define T_PTR_OP 290
#define T_AND_OP 291
#define T_OR_OP 292
#define T_LE_OP 293
#define T_GE_OP 294
#define T_EQ_OP 295
#define T_NE_OP 296
#define T_AND 297
#define T_OR 298
#define T_SUM 299
#define T_AVG 300
#define T_COUNT 301
#define T_PRODUCT 302
#define T_MAX 303
#define T_MIN 304
#define T_GRAPH 305
#define T_DIR_GRAPH 306
#define T_NODE 307
#define T_EDGE 308
#define T_UPDATES 309
#define T_CONTAINER 310
#define T_NODEMAP 311
#define T_VECTOR 312
#define T_HASHMAP 313
#define T_HASHSET 314
#define T_NP 315
#define T_EP 316
#define T_LIST 317
#define T_SET_NODES 318
#define T_SET_EDGES 319
#define T_FROM 320
#define T_BFS 321
#define T_REVERSE 322
#define T_INCREMENTAL 323
#define T_DECREMENTAL 324
#define T_STATIC 325
#define T_DYNAMIC 326
#define T_BATCH 327
#define T_ONADD 328
#define T_ONDELETE 329
#define ID 330
#define INT_NUM 331
#define FLOAT_NUM 332
#define BOOL_VAL 333
#define CHAR_VAL 334
#define STRING_VAL 335
#define return_func 336




/* Copy the first part of user declarations.  */
#line 3 "lrparser.y"

	#include <stdio.h>
	#include <string.h>
	#include <stdlib.h>
	#include <stdbool.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include "includeHeader.hpp"
	#include "../analyser/attachProp/attachPropAnalyser.h"
	#include "../analyser/dataRace/dataRaceAnalyser.h"
	#include "../analyser/deviceVars/deviceVarsAnalyser.h"
	#include "../analyser/pushpull/pushpullAnalyser.h"

	#include "../analyser/blockVars/blockVarsAnalyser.h"
	#include<getopt.h>
	//#include "../symbolutil/SymbolTableBuilder.cpp"
     
	void yyerror(const char *);
	int yylex(void);
    extern FILE* yyin;

	char mytext[100];
	char var[100];
	int num = 0;
	vector<map<int,vector<Identifier*>>> graphId(5);
	extern char *yytext;
	//extern SymbolTable* symbTab;
	FrontEndContext frontEndContext;
	map<string,int> push_map;
	set<string> allGpuUsedVars;
	char* backendTarget ;
    vector<Identifier*> tempIds; //stores graph vars in current function's param list.
    //symbTab=new SymbolTable();
	//symbolTableList.push_back(new SymbolTable());


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
#line 42 "lrparser.y"
{
    int  info;
    long ival;
	bool bval;
    double fval;
    char* text;
	char cval;
	ASTNode* node;
	paramList* pList;
	argList* aList;
	ASTNodeList* nodeList;
    tempNode* temporary;
     }
/* Line 193 of yacc.c.  */
#line 308 "y.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 321 "y.tab.c"

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
# if defined YYENABLE_NLS && YYENABLE_NLS
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
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   912

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  102
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  55
/* YYNRULES -- Number of rules.  */
#define YYNRULES  173
/* YYNRULES -- Number of states.  */
#define YYNSTATES  407

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   336

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    99,     2,     2,     2,    90,     2,     2,
      91,    92,    88,    86,    93,    87,    95,    89,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    83,    94,
      84,    98,    85,    82,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   100,     2,   101,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    96,     2,    97,     2,     2,     2,     2,
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
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     7,    10,    16,    22,    27,    32,
      38,    40,    44,    46,    48,    50,    53,    56,    62,    64,
      65,    68,    71,    74,    77,    79,    82,    84,    86,    89,
      92,    94,    96,    98,   102,   110,   121,   132,   134,   136,
     139,   142,   147,   150,   155,   160,   162,   164,   166,   168,
     170,   172,   174,   176,   178,   180,   182,   184,   186,   191,
     196,   201,   203,   205,   207,   209,   211,   221,   229,   234,
     244,   252,   257,   262,   269,   274,   276,   278,   280,   285,
     290,   295,   300,   305,   310,   314,   318,   320,   322,   326,
     330,   334,   338,   342,   346,   350,   354,   358,   362,   366,
     370,   373,   377,   379,   381,   383,   385,   390,   393,   396,
     401,   406,   411,   418,   420,   422,   424,   426,   428,   430,
     432,   434,   436,   445,   451,   459,   470,   478,   486,   497,
     505,   513,   514,   520,   522,   528,   536,   540,   550,   554,
     558,   560,   562,   564,   566,   568,   572,   574,   578,   582,
     584,   586,   591,   593,   595,   597,   599,   601,   603,   605,
     607,   609,   610,   614,   618,   620,   622,   636,   649,   652,
     658,   662,   669,   675
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     103,     0,    -1,    -1,   103,   104,    -1,   105,   109,    -1,
      15,   156,    91,   106,    92,    -1,    70,   156,    91,   106,
      92,    -1,    68,    91,   106,    92,    -1,    69,    91,   106,
      92,    -1,    71,   156,    91,   106,    92,    -1,   108,    -1,
     108,    93,   106,    -1,   120,    -1,   130,    -1,   122,    -1,
     120,   156,    -1,   130,   156,    -1,   130,   156,    91,   156,
      92,    -1,   112,    -1,    -1,   110,   111,    -1,   119,    94,
      -1,   132,    94,    -1,   137,    94,    -1,   139,    -1,   144,
      94,    -1,   152,    -1,   112,    -1,   136,    94,    -1,   118,
      94,    -1,   113,    -1,   114,    -1,   115,    -1,   116,   110,
     117,    -1,    72,    91,   156,    83,   134,    92,   112,    -1,
      73,    91,   156,    21,   156,    95,   137,    92,    83,   112,
      -1,    74,    91,   156,    21,   156,    95,   137,    92,    83,
     112,    -1,    96,    -1,    97,    -1,    19,   134,    -1,   120,
     156,    -1,   120,   156,    98,   133,    -1,   130,   156,    -1,
     130,   156,    98,   133,    -1,   122,   156,    98,   133,    -1,
     121,    -1,   123,    -1,   124,    -1,     3,    -1,     4,    -1,
       5,    -1,     7,    -1,     8,    -1,     6,    -1,     9,    -1,
      50,    -1,    51,    -1,    62,    -1,    63,    84,   156,    85,
      -1,    64,    84,   156,    85,    -1,    54,    84,   156,    85,
      -1,   125,    -1,   126,    -1,   127,    -1,   128,    -1,   129,
      -1,    55,    84,   107,    85,    91,   151,    93,   107,    92,
      -1,    55,    84,   107,    85,    91,   151,    92,    -1,    55,
      84,   107,    85,    -1,    57,    84,   107,    85,    91,   151,
      93,   107,    92,    -1,    57,    84,   107,    85,    91,   151,
      92,    -1,    57,    84,   107,    85,    -1,    56,    91,   107,
      92,    -1,    58,    84,   107,    93,   107,    85,    -1,    59,
      84,   107,    85,    -1,    52,    -1,    53,    -1,   131,    -1,
      60,    84,   121,    85,    -1,    61,    84,   121,    85,    -1,
      60,    84,   124,    85,    -1,    61,    84,   124,    85,    -1,
      60,    84,    52,    85,    -1,    60,    84,    53,    85,    -1,
     150,    98,   133,    -1,   135,    98,   133,    -1,   134,    -1,
     137,    -1,   134,    86,   134,    -1,   134,    87,   134,    -1,
     134,    88,   134,    -1,   134,    89,   134,    -1,   134,    36,
     134,    -1,   134,    37,   134,    -1,   134,    38,   134,    -1,
     134,    39,   134,    -1,   134,    84,   134,    -1,   134,    85,
     134,    -1,   134,    40,   134,    -1,   134,    41,   134,    -1,
      99,   134,    -1,    91,   134,    92,    -1,   138,    -1,   150,
      -1,   136,    -1,   135,    -1,   134,   100,   134,   101,    -1,
     134,    33,    -1,   134,    34,    -1,   150,    91,   151,    92,
      -1,    68,    91,   151,    92,    -1,    69,    91,   151,    92,
      -1,   135,    95,   150,    91,   151,    92,    -1,    76,    -1,
      77,    -1,    78,    -1,    80,    -1,    13,    -1,    12,    -1,
      14,    -1,   143,    -1,   140,    -1,    22,    23,    91,   156,
      83,   134,    92,   112,    -1,    18,    91,   142,    92,   112,
      -1,    20,   112,    18,    91,   142,    92,    94,    -1,    10,
      91,   156,    21,   156,    95,   137,   141,    92,   112,    -1,
      10,    91,   156,    21,   150,    92,   112,    -1,    11,    91,
     156,    21,   150,    92,   112,    -1,    11,    91,   156,    21,
     156,    95,   137,   141,    92,   112,    -1,    11,    91,   156,
      21,   135,    92,   112,    -1,    10,    91,   156,    21,   135,
      92,   112,    -1,    -1,    95,    24,    91,   142,    92,    -1,
     134,    -1,    16,    91,   142,    92,   111,    -1,    16,    91,
     142,    92,   111,    17,   111,    -1,   150,    98,   148,    -1,
      84,   146,    85,    98,    84,   148,    93,   147,    85,    -1,
     150,   145,   134,    -1,   134,   145,   134,    -1,    25,    -1,
      27,    -1,    32,    -1,    30,    -1,    26,    -1,   150,    93,
     146,    -1,   150,    -1,   138,    93,   147,    -1,   150,    93,
     147,    -1,   138,    -1,   150,    -1,   149,    91,   151,    92,
      -1,    44,    -1,    46,    -1,    47,    -1,    48,    -1,    49,
      -1,   156,    -1,   154,    -1,   155,    -1,   135,    -1,    -1,
     132,    93,   151,    -1,   134,    93,   151,    -1,   134,    -1,
     132,    -1,    66,    91,   156,    21,   156,    95,   137,    65,
     156,    92,   141,   112,   153,    -1,    66,    91,   156,    21,
     156,    95,   137,    65,   156,    92,   141,   112,    -1,    67,
     112,    -1,    67,    91,   142,    92,   112,    -1,   156,    95,
     156,    -1,   156,    95,   156,   100,   156,   101,    -1,   156,
      95,   156,    95,   156,    -1,    75,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   110,   110,   111,   113,   120,   126,   132,   138,   144,
     151,   152,   155,   156,   157,   159,   170,   174,   183,   186,
     187,   189,   190,   191,   192,   193,   194,   195,   196,   197,
     198,   199,   200,   203,   205,   207,   209,   211,   213,   215,
     218,   226,   229,   232,   235,   239,   240,   241,   244,   245,
     246,   247,   248,   249,   251,   253,   254,   256,   257,   259,
     261,   262,   263,   264,   265,   266,   268,   269,   270,   273,
     274,   275,   277,   279,   282,   285,   286,   287,   289,   290,
     291,   292,   293,   295,   298,   299,   302,   304,   305,   306,
     307,   308,   309,   310,   311,   312,   313,   314,   315,   316,
     317,   318,   321,   322,   323,   324,   326,   328,   329,   331,
     336,   340,   344,   355,   356,   357,   358,   359,   360,   361,
     364,   365,   367,   368,   369,   370,   372,   373,   374,   375,
     376,   378,   379,   381,   383,   384,   387,   388,   390,   391,
     394,   395,   396,   397,   398,   400,   402,   404,   405,   407,
     408,   417,   419,   420,   421,   422,   423,   425,   426,   427,
     428,   431,   435,   452,   459,   464,   472,   473,   477,   478,
     481,   485,   492,   496
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_INT", "T_FLOAT", "T_BOOL", "T_STRING",
  "T_DOUBLE", "T_LONG", "T_AUTOREF", "T_FORALL", "T_FOR", "T_P_INF",
  "T_INF", "T_N_INF", "T_FUNC", "T_IF", "T_ELSE", "T_WHILE", "T_RETURN",
  "T_DO", "T_IN", "T_FIXEDPOINT", "T_UNTIL", "T_FILTER", "T_ADD_ASSIGN",
  "T_SUB_ASSIGN", "T_MUL_ASSIGN", "T_DIV_ASSIGN", "T_MOD_ASSIGN",
  "T_AND_ASSIGN", "T_XOR_ASSIGN", "T_OR_ASSIGN", "T_INC_OP", "T_DEC_OP",
  "T_PTR_OP", "T_AND_OP", "T_OR_OP", "T_LE_OP", "T_GE_OP", "T_EQ_OP",
  "T_NE_OP", "T_AND", "T_OR", "T_SUM", "T_AVG", "T_COUNT", "T_PRODUCT",
  "T_MAX", "T_MIN", "T_GRAPH", "T_DIR_GRAPH", "T_NODE", "T_EDGE",
  "T_UPDATES", "T_CONTAINER", "T_NODEMAP", "T_VECTOR", "T_HASHMAP",
  "T_HASHSET", "T_NP", "T_EP", "T_LIST", "T_SET_NODES", "T_SET_EDGES",
  "T_FROM", "T_BFS", "T_REVERSE", "T_INCREMENTAL", "T_DECREMENTAL",
  "T_STATIC", "T_DYNAMIC", "T_BATCH", "T_ONADD", "T_ONDELETE", "ID",
  "INT_NUM", "FLOAT_NUM", "BOOL_VAL", "CHAR_VAL", "STRING_VAL",
  "return_func", "'?'", "':'", "'<'", "'>'", "'+'", "'-'", "'*'", "'/'",
  "'%'", "'('", "')'", "','", "';'", "'.'", "'{'", "'}'", "'='", "'!'",
  "'['", "']'", "$accept", "program", "function_def", "function_data",
  "paramList", "type", "param", "function_body", "statements", "statement",
  "blockstatements", "batch_blockstmt", "on_add_blockstmt",
  "on_delete_blockstmt", "block_begin", "block_end", "return_stmt",
  "declaration", "type1", "primitive", "type3", "graph", "collections",
  "container", "vector", "nodemap", "hashmap", "hashset", "type2",
  "property", "assignment", "rhs", "expression", "indexExpr", "unary_expr",
  "proc_call", "val", "control_flow", "iteration_cf", "filterExpr",
  "boolean_expr", "selection_cf", "reduction", "reduce_op", "leftList",
  "rightList", "reductionCall", "reduction_calls", "leftSide", "arg_list",
  "bfs_abstraction", "reverse_abstraction", "oid", "tid", "id", 0
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
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,    63,    58,    60,    62,    43,    45,    42,    47,
      37,    40,    41,    44,    59,    46,   123,   125,    61,    33,
      91,    93
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   102,   103,   103,   104,   105,   105,   105,   105,   105,
     106,   106,   107,   107,   107,   108,   108,   108,   109,   110,
     110,   111,   111,   111,   111,   111,   111,   111,   111,   111,
     111,   111,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   119,   119,   119,   119,   120,   120,   120,   121,   121,
     121,   121,   121,   121,   122,   123,   123,   124,   124,   124,
     124,   124,   124,   124,   124,   124,   125,   125,   125,   126,
     126,   126,   127,   128,   129,   130,   130,   130,   131,   131,
     131,   131,   131,   131,   132,   132,   133,   134,   134,   134,
     134,   134,   134,   134,   134,   134,   134,   134,   134,   134,
     134,   134,   134,   134,   134,   134,   135,   136,   136,   137,
     137,   137,   137,   138,   138,   138,   138,   138,   138,   138,
     139,   139,   140,   140,   140,   140,   140,   140,   140,   140,
     140,   141,   141,   142,   143,   143,   144,   144,   144,   144,
     145,   145,   145,   145,   145,   146,   146,   147,   147,   147,
     147,   148,   149,   149,   149,   149,   149,   150,   150,   150,
     150,   151,   151,   151,   151,   151,   152,   152,   153,   153,
     154,   154,   155,   156
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     2,     5,     5,     4,     4,     5,
       1,     3,     1,     1,     1,     2,     2,     5,     1,     0,
       2,     2,     2,     2,     1,     2,     1,     1,     2,     2,
       1,     1,     1,     3,     7,    10,    10,     1,     1,     2,
       2,     4,     2,     4,     4,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     4,     4,
       4,     1,     1,     1,     1,     1,     9,     7,     4,     9,
       7,     4,     4,     6,     4,     1,     1,     1,     4,     4,
       4,     4,     4,     4,     3,     3,     1,     1,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       2,     3,     1,     1,     1,     1,     4,     2,     2,     4,
       4,     4,     6,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     8,     5,     7,    10,     7,     7,    10,     7,
       7,     0,     5,     1,     5,     7,     3,     9,     3,     3,
       1,     1,     1,     1,     1,     3,     1,     3,     3,     1,
       1,     4,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     0,     3,     3,     1,     1,    13,    12,     2,     5,
       3,     6,     5,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,     0,     3,     0,
     173,     0,     0,     0,     0,     0,    37,     4,    18,    19,
       0,    48,    49,    50,    53,    51,    52,    55,    56,    75,
      76,     0,     0,     0,     0,     0,     0,     0,     0,    57,
       0,     0,     0,    10,     0,    45,    46,    47,    61,    62,
      63,    64,    65,     0,    77,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       7,     0,    15,    16,     8,     0,     0,    54,     0,     0,
     118,   117,   119,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   113,   114,   115,   116,     0,     0,
      38,     0,    20,    27,    30,    31,    32,    33,     0,     0,
       0,     0,     0,     0,     0,   105,   104,    87,   102,    24,
     121,   120,     0,   103,    26,   158,   159,   157,     5,     0,
       0,    12,    14,    13,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    11,     0,     6,     9,
       0,     0,     0,     0,    39,   105,   104,    87,   103,     0,
       0,     0,   161,   161,     0,     0,     0,     0,     0,   103,
       0,   100,    29,    21,    40,     0,    42,    22,   140,   144,
     141,   143,   142,   107,   108,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    28,    23,    25,   161,     0,     0,     0,    60,    68,
      72,    71,     0,    74,    82,    83,    78,    80,    79,    81,
      58,    59,     0,     0,     0,   133,     0,     0,     0,     0,
       0,   165,   164,   103,     0,     0,     0,     0,     0,     0,
       0,   101,     0,     0,     0,    92,    93,    94,    95,    98,
      99,    96,    97,    88,    89,    90,    91,     0,   139,   103,
      85,    86,     0,   152,   153,   154,   155,   156,    84,   136,
       0,   138,   170,   161,   161,     0,    17,     0,     0,     0,
       0,     0,     0,     0,   161,   161,     0,   110,   111,     0,
       0,     0,     0,   145,    41,    44,    43,   106,   161,   109,
     161,     0,     0,     0,     0,    73,   105,   103,   157,   105,
     103,   157,   134,   123,     0,     0,     0,   162,   163,     0,
       0,     0,     0,     0,     0,   172,     0,    67,     0,    70,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   109,   151,   171,     0,     0,
     130,   126,    87,   157,   129,   127,    87,   135,   124,     0,
      87,    34,    87,    87,     0,    66,    69,     0,     0,     0,
       0,   122,     0,     0,     0,   102,     0,   103,     0,     0,
     170,     0,     0,     0,     0,     0,   137,     0,     0,   125,
     128,   131,    35,    36,   147,   148,     0,     0,   132,   167,
       0,   166,     0,   168,     0,     0,   169
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     8,     9,    42,   130,    43,    17,    58,   102,
     103,   104,   105,   106,    19,   107,   108,   109,   131,    45,
     132,    46,    47,    48,    49,    50,    51,    52,   133,    54,
     231,   268,   167,   155,   156,   157,   118,   119,   120,   368,
     226,   121,   122,   198,   168,   376,   269,   270,   158,   234,
     124,   401,   125,   126,   127
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -351
static const yytype_int16 yypact[] =
{
    -351,    12,  -351,   -51,   -45,   -32,   -51,   -51,  -351,   -41,
    -351,   -22,   831,   831,    -6,    -2,  -351,  -351,  -351,  -351,
     831,  -351,  -351,  -351,  -351,  -351,  -351,  -351,  -351,  -351,
    -351,    36,    72,    75,    83,    89,    97,    99,   100,  -351,
     104,   107,    26,   108,   -51,  -351,  -351,  -351,  -351,  -351,
    -351,  -351,  -351,   -51,  -351,   112,   831,   831,   404,   117,
     -51,   812,   812,   812,   812,   812,   848,   435,   -51,   -51,
    -351,   831,  -351,   109,  -351,   118,   120,  -351,   122,   123,
    -351,  -351,  -351,   124,   127,    22,   -41,   196,   129,   130,
     132,   134,   135,   139,  -351,  -351,  -351,  -351,    22,    22,
    -351,    22,  -351,  -351,  -351,  -351,  -351,  -351,   137,   142,
     -51,   -51,   -51,   143,   502,    55,   146,   151,  -351,  -351,
    -351,  -351,   154,    13,  -351,  -351,  -351,   155,  -351,   164,
     166,  -351,  -351,  -351,   162,   170,   163,   172,   174,   175,
     176,   177,   178,   180,   181,   184,  -351,   -51,  -351,  -351,
     -51,   -51,    22,    22,   681,   -25,  -351,  -351,   183,   249,
     185,   -51,    22,    22,   -51,   -51,   -51,   681,   187,    10,
     589,   681,  -351,  -351,   179,   188,   192,  -351,  -351,  -351,
    -351,  -351,  -351,  -351,  -351,    22,    22,    22,    22,    22,
      22,    22,    22,    22,    22,    22,    22,    22,    22,    22,
      22,  -351,  -351,  -351,    22,   570,    22,   -51,  -351,   194,
    -351,   200,   812,  -351,  -351,  -351,  -351,  -351,  -351,  -351,
    -351,  -351,   205,   252,   258,   681,   209,   225,   228,   -51,
     299,   234,   646,   -76,   229,   237,   251,   314,   315,   240,
      22,  -351,    22,    22,    22,   744,   704,    74,    74,   761,
     761,    74,    74,   -14,   -14,    17,    17,   571,   681,   248,
    -351,   681,   254,  -351,  -351,  -351,  -351,  -351,  -351,  -351,
     250,   681,   -63,    22,    22,   259,  -351,    22,    22,   501,
     -41,    22,   260,   -51,    22,    22,    22,  -351,  -351,    22,
     -51,   -51,   263,  -351,  -351,  -351,  -351,  -351,    22,  -351,
      22,   -51,   -51,     1,    31,  -351,    80,   -13,   253,    85,
      34,   257,   332,  -351,   261,    22,   264,  -351,  -351,   663,
     265,   269,   150,   262,   273,  -351,   255,  -351,   812,  -351,
     812,   -41,   -41,    22,   -41,   -41,    22,   501,   272,   672,
      22,   -41,    22,    22,   278,   286,  -351,  -351,   285,   291,
    -351,  -351,   -62,    -8,  -351,  -351,   -62,  -351,  -351,   -41,
     290,  -351,   293,   294,    22,  -351,  -351,   365,   298,   -51,
     300,  -351,   -51,   310,   311,   302,   312,    19,   307,   -41,
      27,   -41,   308,   -41,   -41,    22,  -351,    22,    22,  -351,
    -351,   304,  -351,  -351,  -351,  -351,   309,   -41,  -351,   335,
     -31,  -351,    22,  -351,   313,   -41,  -351
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -351,  -351,  -351,  -351,   182,   -60,  -351,  -351,  -351,  -266,
      -9,  -351,  -351,  -351,  -351,  -351,  -351,  -351,    -4,    78,
     -37,  -351,    92,  -351,  -351,  -351,  -351,  -351,     5,  -351,
     -35,   -36,   -57,    30,   -27,   -44,  -338,  -351,  -351,  -350,
    -124,  -351,  -351,   280,   189,  -329,    82,  -351,    84,   -95,
    -351,  -351,  -351,  -351,     4
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -173
static const yytype_int16 yytable[] =
{
      18,   114,   134,   135,   136,   137,   370,    11,    44,    44,
      14,    15,     2,   312,   117,   204,    44,    53,    53,   183,
     184,   111,   286,   113,    10,    53,   375,     3,   154,   227,
    -131,   116,   301,   367,    80,    81,    82,   302,   178,   179,
     180,   397,   170,   181,   171,   182,    12,   375,    72,   375,
     183,   184,    44,    44,   110,    16,   394,    73,   395,    13,
     402,    53,    53,   112,   129,    16,  -160,    44,   235,    20,
     199,   357,   144,   145,   195,   196,    53,   159,   204,   332,
       4,     5,     6,     7,  -170,    56,   197,   369,   115,    57,
      89,    90,   302,   327,   328,   225,   225,    10,    94,    95,
      96,   204,    97,   240,   204,   232,   232,   183,   184,   262,
     204,   205,   387,    99,   174,   175,   176,   197,    70,  -172,
      60,   101,   301,   329,   330,   204,   335,   302,   245,   246,
     247,   248,   249,   250,   251,   252,   253,   254,   255,   256,
     257,   258,   123,   261,   140,   142,  -160,   232,   261,   271,
     199,   222,   275,   200,   223,   224,    61,   314,   141,   143,
     193,   194,   195,   196,   260,   230,    62,    63,   236,   237,
     238,  -160,   331,    64,   197,   199,  -160,   334,   303,   304,
     199,    65,   169,    66,    67,   261,   261,   261,    68,   317,
     318,    69,   115,   115,   263,    55,   264,   265,   266,   267,
     147,    71,    59,   323,    74,   324,   294,   295,   296,   128,
     148,   272,   149,   150,   151,   152,   232,   232,   153,   160,
     161,   162,   114,   163,   225,   164,   165,   232,   232,   261,
     166,   172,   319,   282,   115,   117,   173,   177,    75,    76,
     201,   232,   111,   232,   113,   202,   233,   233,   203,   208,
     207,   209,   116,   146,   210,   211,   212,   213,   339,   214,
     215,   216,   217,   218,   396,   219,   220,   228,   348,   221,
     349,   313,   239,   277,   204,   110,   229,   242,   404,   278,
     114,   308,   311,   259,   112,   273,   243,   316,   233,   352,
     244,   274,   356,   117,   320,   321,   360,   276,   362,   363,
     111,   279,   113,   115,   115,   325,   326,   306,   309,   115,
     116,  -112,  -112,  -112,   115,   115,  -112,   280,  -112,   281,
     283,   287,   350,   351,   169,   354,   355,   284,   115,   288,
     115,   225,   361,   110,   289,   290,   291,   353,   292,   298,
     353,   300,   112,   315,   305,   225,   299,   322,   333,   337,
     371,  -112,   336,   338,   345,   372,   347,   233,   233,   340,
     342,   307,   310,   123,   343,   346,   358,   115,   233,   233,
     389,   364,   390,   380,   392,   393,   382,   365,  -112,  -112,
    -112,  -112,   233,   366,   233,   373,   374,  -112,   399,   378,
     379,   403,   381,   383,   384,   385,   406,   386,   388,   367,
     391,   398,   400,   206,   344,   405,     0,    21,    22,    23,
      24,    25,    26,    77,    78,    79,    80,    81,    82,     0,
      83,   123,    84,    85,    86,     0,    87,     0,     0,   293,
       0,     0,     0,     0,     0,     0,     0,     0,    21,    22,
      23,    24,    25,    26,     0,     0,     0,     0,   377,     0,
       0,     0,     0,     0,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,   377,
      88,   377,    89,    90,     0,     0,    91,    92,    93,    10,
      94,    95,    96,     0,    97,     0,     0,     0,    98,    31,
      32,    33,    34,    35,    36,    99,     0,    39,    40,    41,
      16,   100,     0,   101,    21,    22,    23,    24,    25,    26,
      77,    78,    79,    80,    81,    82,     0,    83,     0,    84,
      85,    86,     0,    87,     0,     0,     0,   178,   179,   180,
       0,     0,   181,     0,   182,   183,   184,     0,   185,   186,
     187,   188,   189,   190,     0,     0,     0,     0,     0,     0,
       0,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,     0,    88,     0,    89,
      90,     0,     0,    91,    92,    93,    10,    94,    95,    96,
       0,    97,    80,    81,    82,    98,   191,   192,   193,   194,
     195,   196,    99,     0,     0,     0,     0,    16,     0,     0,
     101,     0,   197,     0,   183,   184,     0,   185,   186,   187,
     188,   189,   190,     0,   263,     0,   264,   265,   266,   267,
       0,     0,   183,   184,     0,   185,   186,   187,   188,   189,
     190,     0,     0,     0,     0,     0,     0,     0,    89,    90,
       0,     0,     0,     0,     0,    10,    94,    95,    96,     0,
      97,     0,     0,     0,     0,   191,   192,   193,   194,   195,
     196,    99,     0,     0,     0,     0,     0,     0,     0,   101,
       0,   197,   297,   191,   192,   193,   194,   195,   196,   183,
     184,   241,   185,   186,   187,   188,   189,   190,     0,   197,
       0,     0,     0,     0,     0,     0,   183,   184,     0,   185,
     186,   187,   188,   189,   190,   183,   184,     0,   185,   186,
     187,   188,   189,   190,   183,   184,     0,   185,   186,   187,
     188,   189,   190,     0,     0,     0,     0,     0,     0,     0,
     191,   192,   193,   194,   195,   196,     0,   183,   184,   285,
     185,     0,   187,   188,   189,   190,   197,   191,   192,   193,
     194,   195,   196,     0,     0,   341,   191,   192,   193,   194,
     195,   196,     0,   197,   359,   191,   192,   193,   194,   195,
     196,     0,   197,     0,     0,     0,     0,   183,   184,     0,
       0,   197,   187,   188,   189,   190,     0,     0,   191,   192,
     193,   194,   195,   196,   183,   184,     0,     0,     0,   187,
     188,     0,     0,     0,   197,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    21,    22,    23,    24,    25,
      26,    77,     0,     0,     0,     0,     0,     0,   191,   192,
     193,   194,   195,   196,    21,    22,    23,    24,    25,    26,
       0,     0,     0,     0,   197,   191,   192,   193,   194,   195,
     196,    21,    22,    23,    24,    25,    26,     0,     0,     0,
       0,   197,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,     0,     0,     0,
       0,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,     0,     0,     0,     0,
     138,   139,    31,    32,    33,    34,    35,    36,     0,     0,
      39,    40,    41
};

static const yytype_int16 yycheck[] =
{
       9,    58,    62,    63,    64,    65,   356,     3,    12,    13,
       6,     7,     0,   279,    58,    91,    20,    12,    13,    33,
      34,    58,    98,    58,    75,    20,   364,    15,    85,   153,
      92,    58,    95,    95,    12,    13,    14,   100,    25,    26,
      27,   391,    99,    30,   101,    32,    91,   385,    44,   387,
      33,    34,    56,    57,    58,    96,   385,    53,   387,    91,
      91,    56,    57,    58,    60,    96,    91,    71,   163,    91,
      95,   337,    68,    69,    88,    89,    71,    86,    91,    92,
      68,    69,    70,    71,    92,    91,   100,    95,    58,    91,
      68,    69,   100,    92,    93,   152,   153,    75,    76,    77,
      78,    91,    80,    93,    91,   162,   163,    33,    34,   204,
      91,    98,    93,    91,   110,   111,   112,   100,    92,    92,
      84,    99,    95,    92,    93,    91,    92,   100,   185,   186,
     187,   188,   189,   190,   191,   192,   193,   194,   195,   196,
     197,   198,    58,   200,    66,    67,    91,   204,   205,   206,
      95,   147,   212,    98,   150,   151,    84,   281,    66,    67,
      86,    87,    88,    89,   200,   161,    91,    84,   164,   165,
     166,    91,    92,    84,   100,    95,    91,    92,   273,   274,
      95,    84,    98,    84,    84,   242,   243,   244,    84,   284,
     285,    84,   162,   163,    44,    13,    46,    47,    48,    49,
      91,    93,    20,   298,    92,   300,   242,   243,   244,    92,
      92,   207,    92,    91,    91,    91,   273,   274,    91,    23,
      91,    91,   279,    91,   281,    91,    91,   284,   285,   286,
      91,    94,   289,   229,   204,   279,    94,    94,    56,    57,
      94,   298,   279,   300,   279,    94,   162,   163,    94,    85,
      95,    85,   279,    71,    92,    85,    93,    85,   315,    85,
      85,    85,    85,    85,   388,    85,    85,    18,   328,    85,
     330,   280,    85,    21,    91,   279,    91,    98,   402,    21,
     337,   277,   278,   199,   279,    91,    98,   283,   204,   333,
      98,    91,   336,   337,   290,   291,   340,    92,   342,   343,
     337,    92,   337,   273,   274,   301,   302,   277,   278,   279,
     337,    25,    26,    27,   284,   285,    30,    92,    32,    91,
      21,    92,   331,   332,   240,   334,   335,    93,   298,    92,
     300,   388,   341,   337,    83,    21,    21,   333,    98,    91,
     336,    91,   337,    83,    85,   402,    92,    84,    95,    17,
     359,    65,    95,    92,    92,    65,   101,   273,   274,    95,
      95,   277,   278,   279,    95,    92,    94,   337,   284,   285,
     379,    93,   381,   369,   383,   384,   372,    92,    92,    93,
      94,    95,   298,    92,   300,    92,    92,   101,   397,    24,
      92,   400,    92,    83,    83,    93,   405,    85,    91,    95,
      92,    92,    67,   123,   322,    92,    -1,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    -1,
      16,   337,    18,    19,    20,    -1,    22,    -1,    -1,   240,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,     6,     7,     8,    -1,    -1,    -1,    -1,   364,    -1,
      -1,    -1,    -1,    -1,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,   385,
      66,   387,    68,    69,    -1,    -1,    72,    73,    74,    75,
      76,    77,    78,    -1,    80,    -1,    -1,    -1,    84,    54,
      55,    56,    57,    58,    59,    91,    -1,    62,    63,    64,
      96,    97,    -1,    99,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    -1,    16,    -1,    18,
      19,    20,    -1,    22,    -1,    -1,    -1,    25,    26,    27,
      -1,    -1,    30,    -1,    32,    33,    34,    -1,    36,    37,
      38,    39,    40,    41,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    -1,    66,    -1,    68,
      69,    -1,    -1,    72,    73,    74,    75,    76,    77,    78,
      -1,    80,    12,    13,    14,    84,    84,    85,    86,    87,
      88,    89,    91,    -1,    -1,    -1,    -1,    96,    -1,    -1,
      99,    -1,   100,    -1,    33,    34,    -1,    36,    37,    38,
      39,    40,    41,    -1,    44,    -1,    46,    47,    48,    49,
      -1,    -1,    33,    34,    -1,    36,    37,    38,    39,    40,
      41,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    68,    69,
      -1,    -1,    -1,    -1,    -1,    75,    76,    77,    78,    -1,
      80,    -1,    -1,    -1,    -1,    84,    85,    86,    87,    88,
      89,    91,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    99,
      -1,   100,   101,    84,    85,    86,    87,    88,    89,    33,
      34,    92,    36,    37,    38,    39,    40,    41,    -1,   100,
      -1,    -1,    -1,    -1,    -1,    -1,    33,    34,    -1,    36,
      37,    38,    39,    40,    41,    33,    34,    -1,    36,    37,
      38,    39,    40,    41,    33,    34,    -1,    36,    37,    38,
      39,    40,    41,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      84,    85,    86,    87,    88,    89,    -1,    33,    34,    93,
      36,    -1,    38,    39,    40,    41,   100,    84,    85,    86,
      87,    88,    89,    -1,    -1,    92,    84,    85,    86,    87,
      88,    89,    -1,   100,    92,    84,    85,    86,    87,    88,
      89,    -1,   100,    -1,    -1,    -1,    -1,    33,    34,    -1,
      -1,   100,    38,    39,    40,    41,    -1,    -1,    84,    85,
      86,    87,    88,    89,    33,    34,    -1,    -1,    -1,    38,
      39,    -1,    -1,    -1,   100,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,     7,
       8,     9,    -1,    -1,    -1,    -1,    -1,    -1,    84,    85,
      86,    87,    88,    89,     3,     4,     5,     6,     7,     8,
      -1,    -1,    -1,    -1,   100,    84,    85,    86,    87,    88,
      89,     3,     4,     5,     6,     7,     8,    -1,    -1,    -1,
      -1,   100,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    -1,    -1,    -1,
      -1,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    -1,    -1,    -1,    -1,
      52,    53,    54,    55,    56,    57,    58,    59,    -1,    -1,
      62,    63,    64
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,   103,     0,    15,    68,    69,    70,    71,   104,   105,
      75,   156,    91,    91,   156,   156,    96,   109,   112,   116,
      91,     3,     4,     5,     6,     7,     8,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,   106,   108,   120,   121,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   106,    91,    91,   110,   106,
      84,    84,    91,    84,    84,    84,    84,    84,    84,    84,
      92,    93,   156,   156,    92,   106,   106,     9,    10,    11,
      12,    13,    14,    16,    18,    19,    20,    22,    66,    68,
      69,    72,    73,    74,    76,    77,    78,    80,    84,    91,
      97,    99,   111,   112,   113,   114,   115,   117,   118,   119,
     120,   122,   130,   132,   134,   135,   136,   137,   138,   139,
     140,   143,   144,   150,   152,   154,   155,   156,    92,   156,
     107,   120,   122,   130,   107,   107,   107,   107,    52,    53,
     121,   124,   121,   124,   156,   156,   106,    91,    92,    92,
      91,    91,    91,    91,   134,   135,   136,   137,   150,   112,
      23,    91,    91,    91,    91,    91,    91,   134,   146,   150,
     134,   134,    94,    94,   156,   156,   156,    94,    25,    26,
      27,    30,    32,    33,    34,    36,    37,    38,    39,    40,
      41,    84,    85,    86,    87,    88,    89,   100,   145,    95,
      98,    94,    94,    94,    91,    98,   145,    95,    85,    85,
      92,    85,    93,    85,    85,    85,    85,    85,    85,    85,
      85,    85,   156,   156,   156,   134,   142,   142,    18,    91,
     156,   132,   134,   150,   151,   151,   156,   156,   156,    85,
      93,    92,    98,    98,    98,   134,   134,   134,   134,   134,
     134,   134,   134,   134,   134,   134,   134,   134,   134,   150,
     133,   134,   151,    44,    46,    47,    48,    49,   133,   148,
     149,   134,   156,    91,    91,   107,    92,    21,    21,    92,
      92,    91,   156,    21,    93,    93,    98,    92,    92,    83,
      21,    21,    98,   146,   133,   133,   133,   101,    91,    92,
      91,    95,   100,   151,   151,    85,   135,   150,   156,   135,
     150,   156,   111,   112,   142,    83,   156,   151,   151,   134,
     156,   156,    84,   151,   151,   156,   156,    92,    93,    92,
      93,    92,    92,    95,    92,    92,    95,    17,    92,   134,
      95,    92,    95,    95,   148,    92,    92,   101,   107,   107,
     112,   112,   137,   156,   112,   112,   137,   111,    94,    92,
     137,   112,   137,   137,    93,    92,    92,    95,   141,    95,
     141,   112,    65,    92,    92,   138,   147,   150,    24,    92,
     156,    92,   156,    83,    83,    93,    85,    93,    91,   112,
     112,    92,   112,   112,   147,   147,   142,   141,    92,   112,
      67,   153,    91,   112,   142,    92,   112
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
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
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
        case 3:
#line 111 "lrparser.y"
    {/* printf("LIST SIZE %d",frontEndContext.getFuncList().size())  ;*/ }
    break;

  case 4:
#line 113 "lrparser.y"
    { 
	                                          Function* func=(Function*)(yyvsp[(1) - (2)].node);
                                              blockStatement* block=(blockStatement*)(yyvsp[(2) - (2)].node);
                                              func->setBlockStatement(block);
											   Util::addFuncToList(func);
											}
    break;

  case 5:
#line 120 "lrparser.y"
    { 
										   (yyval.node)=Util::createFuncNode((yyvsp[(2) - (5)].node),(yyvsp[(4) - (5)].pList)->PList);
                                           Util::setCurrentFuncType(GEN_FUNC);
										   Util::resetTemp(tempIds);
										   tempIds.clear();
	                                      }
    break;

  case 6:
#line 126 "lrparser.y"
    { 
										   (yyval.node)=Util::createStaticFuncNode((yyvsp[(2) - (5)].node),(yyvsp[(4) - (5)].pList)->PList);
                                            Util::setCurrentFuncType(STATIC_FUNC);
											Util::resetTemp(tempIds);
											tempIds.clear();
	                                      }
    break;

  case 7:
#line 132 "lrparser.y"
    { 
										   (yyval.node)=Util::createIncrementalNode((yyvsp[(3) - (4)].pList)->PList);
                                            Util::setCurrentFuncType(INCREMENTAL_FUNC);
											Util::resetTemp(tempIds);
											tempIds.clear();
	                                      }
    break;

  case 8:
#line 138 "lrparser.y"
    { 
										   (yyval.node)=Util::createDecrementalNode((yyvsp[(3) - (4)].pList)->PList);
                                            Util::setCurrentFuncType(DECREMENTAL_FUNC);
											Util::resetTemp(tempIds);
											tempIds.clear();
	                                      }
    break;

  case 9:
#line 144 "lrparser.y"
    { (yyval.node)=Util::createDynamicFuncNode((yyvsp[(2) - (5)].node),(yyvsp[(4) - (5)].pList)->PList);
                                            Util::setCurrentFuncType(DYNAMIC_FUNC);
											Util::resetTemp(tempIds);
											tempIds.clear();
											}
    break;

  case 10:
#line 151 "lrparser.y"
    {(yyval.pList)=Util::createPList((yyvsp[(1) - (1)].node));}
    break;

  case 11:
#line 152 "lrparser.y"
    {(yyval.pList)=Util::addToPList((yyvsp[(3) - (3)].pList),(yyvsp[(1) - (3)].node)); 
			                           }
    break;

  case 12:
#line 155 "lrparser.y"
    {(yyval.node) = (yyvsp[(1) - (1)].node);}
    break;

  case 13:
#line 156 "lrparser.y"
    {(yyval.node) = (yyvsp[(1) - (1)].node);}
    break;

  case 14:
#line 157 "lrparser.y"
    {(yyval.node) = (yyvsp[(1) - (1)].node);}
    break;

  case 15:
#line 159 "lrparser.y"
    {  //Identifier* id=(Identifier*)Util::createIdentifierNode($2);
                        Type* type=(Type*)(yyvsp[(1) - (2)].node);
	                     Identifier* id=(Identifier*)(yyvsp[(2) - (2)].node);
						 
						 if(type->isGraphType())
						    {
							 tempIds.push_back(id);
						   
							}
					printf("\n");
                    (yyval.node)=Util::createParamNode((yyvsp[(1) - (2)].node),(yyvsp[(2) - (2)].node)); }
    break;

  case 16:
#line 170 "lrparser.y"
    { // Identifier* id=(Identifier*)Util::createIdentifierNode($2);
			  
					
                             (yyval.node)=Util::createParamNode((yyvsp[(1) - (2)].node),(yyvsp[(2) - (2)].node));}
    break;

  case 17:
#line 174 "lrparser.y"
    { // Identifier* id1=(Identifier*)Util::createIdentifierNode($4);
			                            //Identifier* id=(Identifier*)Util::createIdentifierNode($2);
				                        Type* tempType=(Type*)(yyvsp[(1) - (5)].node);
			                            if(tempType->isNodeEdgeType())
										  tempType->addSourceGraph((yyvsp[(4) - (5)].node));
				                         (yyval.node)=Util::createParamNode(tempType,(yyvsp[(2) - (5)].node));
									 }
    break;

  case 18:
#line 183 "lrparser.y"
    {(yyval.node)=(yyvsp[(1) - (1)].node);}
    break;

  case 19:
#line 186 "lrparser.y"
    {}
    break;

  case 20:
#line 187 "lrparser.y"
    {printf ("found one statement\n") ; Util::addToBlock((yyvsp[(2) - (2)].node)); }
    break;

  case 21:
#line 189 "lrparser.y"
    {(yyval.node)=(yyvsp[(1) - (2)].node);}
    break;

  case 22:
#line 190 "lrparser.y"
    {printf ("found an assignment type statement" ); (yyval.node)=(yyvsp[(1) - (2)].node);}
    break;

  case 23:
#line 191 "lrparser.y"
    {printf ("found an proc call type statement" );(yyval.node)=Util::createNodeForProcCallStmt((yyvsp[(1) - (2)].node));}
    break;

  case 24:
#line 192 "lrparser.y"
    {printf ("found an control flow type statement" );(yyval.node)=(yyvsp[(1) - (1)].node);}
    break;

  case 25:
#line 193 "lrparser.y"
    {printf ("found an reduction type statement" );(yyval.node)=(yyvsp[(1) - (2)].node);}
    break;

  case 26:
#line 194 "lrparser.y"
    {printf ("found bfs\n") ;(yyval.node)=(yyvsp[(1) - (1)].node); }
    break;

  case 27:
#line 195 "lrparser.y"
    {printf ("found block\n") ;(yyval.node)=(yyvsp[(1) - (1)].node);}
    break;

  case 28:
#line 196 "lrparser.y"
    {printf ("found unary\n") ;(yyval.node)=Util::createNodeForUnaryStatements((yyvsp[(1) - (2)].node));}
    break;

  case 29:
#line 197 "lrparser.y"
    {printf ("found return\n") ;(yyval.node) = (yyvsp[(1) - (2)].node) ;}
    break;

  case 30:
#line 198 "lrparser.y"
    {printf ("found batch\n") ;(yyval.node) = (yyvsp[(1) - (1)].node);}
    break;

  case 31:
#line 199 "lrparser.y"
    {printf ("found on add block\n") ;(yyval.node) = (yyvsp[(1) - (1)].node);}
    break;

  case 32:
#line 200 "lrparser.y"
    {printf ("found delete block\n") ;(yyval.node) = (yyvsp[(1) - (1)].node);}
    break;

  case 33:
#line 203 "lrparser.y"
    { (yyval.node)=Util::finishBlock();}
    break;

  case 34:
#line 205 "lrparser.y"
    {(yyval.node) = Util::createBatchBlockStmt((yyvsp[(3) - (7)].node), (yyvsp[(5) - (7)].node), (yyvsp[(7) - (7)].node));}
    break;

  case 35:
#line 207 "lrparser.y"
    {(yyval.node) = Util::createOnAddBlock((yyvsp[(3) - (10)].node), (yyvsp[(5) - (10)].node), (yyvsp[(7) - (10)].node), (yyvsp[(10) - (10)].node));}
    break;

  case 36:
#line 209 "lrparser.y"
    {(yyval.node) = Util::createOnDeleteBlock((yyvsp[(3) - (10)].node), (yyvsp[(5) - (10)].node), (yyvsp[(7) - (10)].node), (yyvsp[(10) - (10)].node));}
    break;

  case 37:
#line 211 "lrparser.y"
    { Util::createNewBlock(); }
    break;

  case 39:
#line 215 "lrparser.y"
    {(yyval.node) = Util::createReturnStatementNode((yyvsp[(2) - (2)].node));}
    break;

  case 40:
#line 218 "lrparser.y"
    {
	                     Type* type=(Type*)(yyvsp[(1) - (2)].node);
	                     Identifier* id=(Identifier*)(yyvsp[(2) - (2)].node);
						 
						 if(type->isGraphType())
						    Util::storeGraphId(id);

                         (yyval.node)=Util::createNormalDeclNode((yyvsp[(1) - (2)].node),(yyvsp[(2) - (2)].node));}
    break;

  case 41:
#line 226 "lrparser.y"
    {//Identifier* id=(Identifier*)Util::createIdentifierNode($2);
	                    
	                    (yyval.node)=Util::createAssignedDeclNode((yyvsp[(1) - (4)].node),(yyvsp[(2) - (4)].node),(yyvsp[(4) - (4)].node));}
    break;

  case 42:
#line 229 "lrparser.y"
    {//Identifier* id=(Identifier*)Util::createIdentifierNode($2);
	            
                         (yyval.node)=Util::createNormalDeclNode((yyvsp[(1) - (2)].node),(yyvsp[(2) - (2)].node)); }
    break;

  case 43:
#line 232 "lrparser.y"
    {//Identifier* id=(Identifier*)Util::createIdentifierNode($2);
	                   
	                    (yyval.node)=Util::createAssignedDeclNode((yyvsp[(1) - (4)].node),(yyvsp[(2) - (4)].node),(yyvsp[(4) - (4)].node));}
    break;

  case 44:
#line 235 "lrparser.y"
    {//Identifier* id=(Identifier*)Util::createIdentifierNode($2);
	                   
	                    (yyval.node)=Util::createAssignedDeclNode((yyvsp[(1) - (4)].node),(yyvsp[(2) - (4)].node),(yyvsp[(4) - (4)].node));}
    break;

  case 45:
#line 239 "lrparser.y"
    {(yyval.node)=(yyvsp[(1) - (1)].node); }
    break;

  case 46:
#line 240 "lrparser.y"
    {(yyval.node)=(yyvsp[(1) - (1)].node);}
    break;

  case 47:
#line 241 "lrparser.y"
    { (yyval.node)=(yyvsp[(1) - (1)].node);}
    break;

  case 48:
#line 244 "lrparser.y"
    { (yyval.node)=Util::createPrimitiveTypeNode(TYPE_INT);}
    break;

  case 49:
#line 245 "lrparser.y"
    { (yyval.node)=Util::createPrimitiveTypeNode(TYPE_FLOAT);}
    break;

  case 50:
#line 246 "lrparser.y"
    { (yyval.node)=Util::createPrimitiveTypeNode(TYPE_BOOL);}
    break;

  case 51:
#line 247 "lrparser.y"
    { (yyval.node)=Util::createPrimitiveTypeNode(TYPE_DOUBLE); }
    break;

  case 52:
#line 248 "lrparser.y"
    {(yyval.node)=(yyval.node)=Util::createPrimitiveTypeNode(TYPE_LONG);}
    break;

  case 53:
#line 249 "lrparser.y"
    {(yyval.node)=(yyval.node)=Util::createPrimitiveTypeNode(TYPE_STRING);}
    break;

  case 54:
#line 251 "lrparser.y"
    { (yyval.node)=Util::createPrimitiveTypeNode(TYPE_AUTOREF);}
    break;

  case 55:
#line 253 "lrparser.y"
    { (yyval.node)=Util::createGraphTypeNode(TYPE_GRAPH,NULL);}
    break;

  case 56:
#line 254 "lrparser.y"
    {(yyval.node)=Util::createGraphTypeNode(TYPE_DIRGRAPH,NULL);}
    break;

  case 57:
#line 256 "lrparser.y"
    { (yyval.node)=Util::createCollectionTypeNode(TYPE_LIST,NULL);}
    break;

  case 58:
#line 257 "lrparser.y"
    {//Identifier* id=(Identifier*)Util::createIdentifierNode($3);
			                     (yyval.node)=Util::createCollectionTypeNode(TYPE_SETN,(yyvsp[(3) - (4)].node));}
    break;

  case 59:
#line 259 "lrparser.y"
    {// Identifier* id=(Identifier*)Util::createIdentifierNode($3);
					                    (yyval.node)=Util::createCollectionTypeNode(TYPE_SETE,(yyvsp[(3) - (4)].node));}
    break;

  case 60:
#line 261 "lrparser.y"
    { (yyval.node)=Util::createCollectionTypeNode(TYPE_UPDATES,(yyvsp[(3) - (4)].node));}
    break;

  case 61:
#line 262 "lrparser.y"
    {(yyval.node) = (yyvsp[(1) - (1)].node);}
    break;

  case 62:
#line 263 "lrparser.y"
    {(yyval.node) = (yyvsp[(1) - (1)].node);}
    break;

  case 63:
#line 264 "lrparser.y"
    {(yyval.node) = (yyvsp[(1) - (1)].node);}
    break;

  case 64:
#line 265 "lrparser.y"
    {(yyval.node) = (yyvsp[(1) - (1)].node);}
    break;

  case 65:
#line 266 "lrparser.y"
    {(yyval.node) = (yyvsp[(1) - (1)].node);}
    break;

  case 66:
#line 268 "lrparser.y"
    {(yyval.node) = Util::createContainerTypeNode(TYPE_CONTAINER, (yyvsp[(3) - (9)].node), (yyvsp[(6) - (9)].aList)->AList, (yyvsp[(8) - (9)].node));}
    break;

  case 67:
#line 269 "lrparser.y"
    { (yyval.node) =  Util::createContainerTypeNode(TYPE_CONTAINER, (yyvsp[(3) - (7)].node), (yyvsp[(6) - (7)].aList)->AList, NULL);}
    break;

  case 68:
#line 270 "lrparser.y"
    { list<argument*> argList;
			                          (yyval.node) = Util::createContainerTypeNode(TYPE_CONTAINER, (yyvsp[(3) - (4)].node), argList, NULL);}
    break;

  case 69:
#line 273 "lrparser.y"
    {(yyval.node) = Util::createContainerTypeNode(TYPE_VECTOR, (yyvsp[(3) - (9)].node), (yyvsp[(6) - (9)].aList)->AList, (yyvsp[(8) - (9)].node));}
    break;

  case 70:
#line 274 "lrparser.y"
    { (yyval.node) =  Util::createContainerTypeNode(TYPE_VECTOR, (yyvsp[(3) - (7)].node), (yyvsp[(6) - (7)].aList)->AList, NULL);}
    break;

  case 71:
#line 275 "lrparser.y"
    { list<argument*> argList;
			                          (yyval.node) = Util::createContainerTypeNode(TYPE_VECTOR, (yyvsp[(3) - (4)].node), argList, NULL);}
    break;

  case 72:
#line 277 "lrparser.y"
    {(yyval.node) = Util::createNodeMapTypeNode(TYPE_NODEMAP, (yyvsp[(3) - (4)].node));}
    break;

  case 73:
#line 279 "lrparser.y"
    { list<argument*> argList;
			                          (yyval.node) = Util::createHashMapTypeNode(TYPE_HASHMAP, (yyvsp[(3) - (6)].node), argList, (yyvsp[(5) - (6)].node));}
    break;

  case 74:
#line 282 "lrparser.y"
    { list<argument*> argList;
			                          (yyval.node) = Util::createHashSetTypeNode(TYPE_HASHSET, (yyvsp[(3) - (4)].node), argList, NULL);}
    break;

  case 75:
#line 285 "lrparser.y"
    {(yyval.node)=Util::createNodeEdgeTypeNode(TYPE_NODE) ;}
    break;

  case 76:
#line 286 "lrparser.y"
    {(yyval.node)=Util::createNodeEdgeTypeNode(TYPE_EDGE);}
    break;

  case 77:
#line 287 "lrparser.y"
    {(yyval.node)=(yyvsp[(1) - (1)].node);}
    break;

  case 78:
#line 289 "lrparser.y"
    { (yyval.node)=Util::createPropertyTypeNode(TYPE_PROPNODE,(yyvsp[(3) - (4)].node)); }
    break;

  case 79:
#line 290 "lrparser.y"
    { (yyval.node)=Util::createPropertyTypeNode(TYPE_PROPEDGE,(yyvsp[(3) - (4)].node)); }
    break;

  case 80:
#line 291 "lrparser.y"
    {  (yyval.node)=Util::createPropertyTypeNode(TYPE_PROPNODE,(yyvsp[(3) - (4)].node)); }
    break;

  case 81:
#line 292 "lrparser.y"
    {(yyval.node)=Util::createPropertyTypeNode(TYPE_PROPEDGE,(yyvsp[(3) - (4)].node));}
    break;

  case 82:
#line 293 "lrparser.y"
    {ASTNode* type = Util::createNodeEdgeTypeNode(TYPE_NODE);
			                         (yyval.node)=Util::createPropertyTypeNode(TYPE_PROPNODE, type); }
    break;

  case 83:
#line 295 "lrparser.y"
    {ASTNode* type = Util::createNodeEdgeTypeNode(TYPE_EDGE);
			                         (yyval.node)=Util::createPropertyTypeNode(TYPE_PROPNODE, type); }
    break;

  case 84:
#line 298 "lrparser.y"
    { printf("testassign\n");(yyval.node)=Util::createAssignmentNode((yyvsp[(1) - (3)].node),(yyvsp[(3) - (3)].node));}
    break;

  case 85:
#line 299 "lrparser.y"
    {printf ("called assign for count\n") ; (yyval.node)=Util::createAssignmentNode((yyvsp[(1) - (3)].node) , (yyvsp[(3) - (3)].node));}
    break;

  case 86:
#line 302 "lrparser.y"
    { (yyval.node)=(yyvsp[(1) - (1)].node);}
    break;

  case 87:
#line 304 "lrparser.y"
    { (yyval.node)=(yyvsp[(1) - (1)].node);}
    break;

  case 88:
#line 305 "lrparser.y"
    { (yyval.node)=Util::createNodeForArithmeticExpr((yyvsp[(1) - (3)].node),(yyvsp[(3) - (3)].node),OPERATOR_ADD);}
    break;

  case 89:
#line 306 "lrparser.y"
    { (yyval.node)=Util::createNodeForArithmeticExpr((yyvsp[(1) - (3)].node),(yyvsp[(3) - (3)].node),OPERATOR_SUB);}
    break;

  case 90:
#line 307 "lrparser.y"
    {(yyval.node)=Util::createNodeForArithmeticExpr((yyvsp[(1) - (3)].node),(yyvsp[(3) - (3)].node),OPERATOR_MUL);}
    break;

  case 91:
#line 308 "lrparser.y"
    {(yyval.node)=Util::createNodeForArithmeticExpr((yyvsp[(1) - (3)].node),(yyvsp[(3) - (3)].node),OPERATOR_DIV);}
    break;

  case 92:
#line 309 "lrparser.y"
    {(yyval.node)=Util::createNodeForLogicalExpr((yyvsp[(1) - (3)].node),(yyvsp[(3) - (3)].node),OPERATOR_AND);}
    break;

  case 93:
#line 310 "lrparser.y"
    {(yyval.node)=Util::createNodeForLogicalExpr((yyvsp[(1) - (3)].node),(yyvsp[(3) - (3)].node),OPERATOR_OR);}
    break;

  case 94:
#line 311 "lrparser.y"
    {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[(1) - (3)].node),(yyvsp[(3) - (3)].node),OPERATOR_LE);}
    break;

  case 95:
#line 312 "lrparser.y"
    {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[(1) - (3)].node),(yyvsp[(3) - (3)].node),OPERATOR_GE);}
    break;

  case 96:
#line 313 "lrparser.y"
    {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[(1) - (3)].node),(yyvsp[(3) - (3)].node),OPERATOR_LT);}
    break;

  case 97:
#line 314 "lrparser.y"
    {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[(1) - (3)].node),(yyvsp[(3) - (3)].node),OPERATOR_GT);}
    break;

  case 98:
#line 315 "lrparser.y"
    {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[(1) - (3)].node),(yyvsp[(3) - (3)].node),OPERATOR_EQ);}
    break;

  case 99:
#line 316 "lrparser.y"
    {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[(1) - (3)].node),(yyvsp[(3) - (3)].node),OPERATOR_NE);}
    break;

  case 100:
#line 317 "lrparser.y"
    {(yyval.node)=Util::createNodeForUnaryExpr((yyvsp[(2) - (2)].node),OPERATOR_NOT);}
    break;

  case 101:
#line 318 "lrparser.y"
    { Expression* expr=(Expression*)(yyvsp[(2) - (3)].node);
				                     expr->setEnclosedBrackets();
			                        (yyval.node)=expr;}
    break;

  case 102:
#line 321 "lrparser.y"
    {(yyval.node)=(yyvsp[(1) - (1)].node);}
    break;

  case 103:
#line 322 "lrparser.y"
    { (yyval.node)=Util::createNodeForId((yyvsp[(1) - (1)].node));}
    break;

  case 104:
#line 323 "lrparser.y"
    {(yyval.node)=(yyvsp[(1) - (1)].node);}
    break;

  case 105:
#line 324 "lrparser.y"
    {(yyval.node) = (yyvsp[(1) - (1)].node);}
    break;

  case 106:
#line 326 "lrparser.y"
    {printf("first done this \n");(yyval.node) = Util::createNodeForIndexExpr((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node), OPERATOR_INDEX);}
    break;

  case 107:
#line 328 "lrparser.y"
    {(yyval.node)=Util::createNodeForUnaryExpr((yyvsp[(1) - (2)].node),OPERATOR_INC);}
    break;

  case 108:
#line 329 "lrparser.y"
    {(yyval.node)=Util::createNodeForUnaryExpr((yyvsp[(1) - (2)].node),OPERATOR_DEC);}
    break;

  case 109:
#line 331 "lrparser.y"
    { 
                                       
                                       (yyval.node) = Util::createNodeForProcCall((yyvsp[(1) - (4)].node),(yyvsp[(3) - (4)].aList)->AList,NULL); 

									    }
    break;

  case 110:
#line 336 "lrparser.y"
    { ASTNode* id = Util::createIdentifierNode("Incremental");
			                                   (yyval.node) = Util::createNodeForProcCall(id, (yyvsp[(3) - (4)].aList)->AList,NULL); 

				                               }
    break;

  case 111:
#line 340 "lrparser.y"
    { ASTNode* id = Util::createIdentifierNode("Decremental");
			                                   (yyval.node) = Util::createNodeForProcCall(id, (yyvsp[(3) - (4)].aList)->AList,NULL); 

				                               }
    break;

  case 112:
#line 344 "lrparser.y"
    {
                                                   
													 Expression* expr = (Expression*)(yyvsp[(1) - (6)].node);
                                                     (yyval.node) = Util::createNodeForProcCall((yyvsp[(3) - (6)].node) , (yyvsp[(5) - (6)].aList)->AList, expr); 

									                 }
    break;

  case 113:
#line 355 "lrparser.y"
    { (yyval.node) = Util::createNodeForIval((yyvsp[(1) - (1)].ival)); }
    break;

  case 114:
#line 356 "lrparser.y"
    {(yyval.node) = Util::createNodeForFval((yyvsp[(1) - (1)].fval));}
    break;

  case 115:
#line 357 "lrparser.y"
    { (yyval.node) = Util::createNodeForBval((yyvsp[(1) - (1)].bval));}
    break;

  case 116:
#line 358 "lrparser.y"
    { (yyval.node) = Util::createNodeForSval((yyvsp[(1) - (1)].text));}
    break;

  case 117:
#line 359 "lrparser.y"
    {(yyval.node)=Util::createNodeForINF(true);}
    break;

  case 118:
#line 360 "lrparser.y"
    {(yyval.node)=Util::createNodeForINF(true);}
    break;

  case 119:
#line 361 "lrparser.y"
    {(yyval.node)=Util::createNodeForINF(false);}
    break;

  case 120:
#line 364 "lrparser.y"
    { (yyval.node)=(yyvsp[(1) - (1)].node); }
    break;

  case 121:
#line 365 "lrparser.y"
    { (yyval.node)=(yyvsp[(1) - (1)].node); }
    break;

  case 122:
#line 367 "lrparser.y"
    { (yyval.node)=Util::createNodeForFixedPointStmt((yyvsp[(4) - (8)].node),(yyvsp[(6) - (8)].node),(yyvsp[(8) - (8)].node));}
    break;

  case 123:
#line 368 "lrparser.y"
    {(yyval.node)=Util::createNodeForWhileStmt((yyvsp[(3) - (5)].node),(yyvsp[(5) - (5)].node)); }
    break;

  case 124:
#line 369 "lrparser.y"
    {(yyval.node)=Util::createNodeForDoWhileStmt((yyvsp[(5) - (7)].node),(yyvsp[(2) - (7)].node));  }
    break;

  case 125:
#line 370 "lrparser.y"
    { 
																				(yyval.node)=Util::createNodeForForAllStmt((yyvsp[(3) - (10)].node),(yyvsp[(5) - (10)].node),(yyvsp[(7) - (10)].node),(yyvsp[(8) - (10)].node),(yyvsp[(10) - (10)].node),true);}
    break;

  case 126:
#line 372 "lrparser.y"
    { (yyval.node)=Util::createNodeForForStmt((yyvsp[(3) - (7)].node),(yyvsp[(5) - (7)].node),(yyvsp[(7) - (7)].node),true);}
    break;

  case 127:
#line 373 "lrparser.y"
    { (yyval.node)=Util::createNodeForForStmt((yyvsp[(3) - (7)].node),(yyvsp[(5) - (7)].node),(yyvsp[(7) - (7)].node),false);}
    break;

  case 128:
#line 374 "lrparser.y"
    {(yyval.node)=Util::createNodeForForAllStmt((yyvsp[(3) - (10)].node),(yyvsp[(5) - (10)].node),(yyvsp[(7) - (10)].node),(yyvsp[(8) - (10)].node),(yyvsp[(10) - (10)].node),false);}
    break;

  case 129:
#line 375 "lrparser.y"
    {(yyval.node) = Util::createNodeForForStmt((yyvsp[(3) - (7)].node), (yyvsp[(5) - (7)].node), (yyvsp[(7) - (7)].node), false);}
    break;

  case 130:
#line 376 "lrparser.y"
    {(yyval.node) = Util::createNodeForForStmt((yyvsp[(3) - (7)].node), (yyvsp[(5) - (7)].node), (yyvsp[(7) - (7)].node), true);}
    break;

  case 131:
#line 378 "lrparser.y"
    { (yyval.node)=NULL;}
    break;

  case 132:
#line 379 "lrparser.y"
    { (yyval.node)=(yyvsp[(4) - (5)].node);}
    break;

  case 133:
#line 381 "lrparser.y"
    { (yyval.node)=(yyvsp[(1) - (1)].node) ;}
    break;

  case 134:
#line 383 "lrparser.y"
    { (yyval.node)=Util::createNodeForIfStmt((yyvsp[(3) - (5)].node),(yyvsp[(5) - (5)].node),NULL); }
    break;

  case 135:
#line 384 "lrparser.y"
    {(yyval.node)=Util::createNodeForIfStmt((yyvsp[(3) - (7)].node),(yyvsp[(5) - (7)].node),(yyvsp[(7) - (7)].node)); }
    break;

  case 136:
#line 387 "lrparser.y"
    { (yyval.node)=Util::createNodeForReductionStmt((yyvsp[(1) - (3)].node),(yyvsp[(3) - (3)].node)) ;}
    break;

  case 137:
#line 388 "lrparser.y"
    { reductionCall* reduc=(reductionCall*)(yyvsp[(6) - (9)].node);
		                                                               (yyval.node)=Util::createNodeForReductionStmtList((yyvsp[(2) - (9)].nodeList)->ASTNList,reduc,(yyvsp[(8) - (9)].nodeList)->ASTNList);}
    break;

  case 138:
#line 390 "lrparser.y"
    {(yyval.node)=Util::createNodeForReductionOpStmt((yyvsp[(1) - (3)].node),(yyvsp[(2) - (3)].ival),(yyvsp[(3) - (3)].node));}
    break;

  case 139:
#line 391 "lrparser.y"
    {printf ("here calling creation for red op\n") ;(yyval.node)=Util::createNodeForReductionOpStmt ((yyvsp[(1) - (3)].node),(yyvsp[(2) - (3)].ival),(yyvsp[(3) - (3)].node));}
    break;

  case 140:
#line 394 "lrparser.y"
    {(yyval.ival)=OPERATOR_ADDASSIGN;}
    break;

  case 141:
#line 395 "lrparser.y"
    {(yyval.ival)=OPERATOR_MULASSIGN;}
    break;

  case 142:
#line 396 "lrparser.y"
    {(yyval.ival)=OPERATOR_ORASSIGN;}
    break;

  case 143:
#line 397 "lrparser.y"
    {(yyval.ival)=OPERATOR_ANDASSIGN;}
    break;

  case 144:
#line 398 "lrparser.y"
    {(yyval.ival)=OPERATOR_SUBASSIGN;}
    break;

  case 145:
#line 400 "lrparser.y"
    { (yyval.nodeList)=Util::addToNList((yyvsp[(3) - (3)].nodeList),(yyvsp[(1) - (3)].node));
                                         }
    break;

  case 146:
#line 402 "lrparser.y"
    { (yyval.nodeList)=Util::createNList((yyvsp[(1) - (1)].node));;}
    break;

  case 147:
#line 404 "lrparser.y"
    { (yyval.nodeList)=Util::addToNList((yyvsp[(3) - (3)].nodeList),(yyvsp[(1) - (3)].node));}
    break;

  case 148:
#line 405 "lrparser.y"
    { ASTNode* node = Util::createNodeForId((yyvsp[(1) - (3)].node));
			                         (yyval.nodeList)=Util::addToNList((yyvsp[(3) - (3)].nodeList),node);}
    break;

  case 149:
#line 407 "lrparser.y"
    { (yyval.nodeList)=Util::createNList((yyvsp[(1) - (1)].node));}
    break;

  case 150:
#line 408 "lrparser.y"
    { ASTNode* node = Util::createNodeForId((yyvsp[(1) - (1)].node));
			            (yyval.nodeList)=Util::createNList(node);}
    break;

  case 151:
#line 417 "lrparser.y"
    {(yyval.node)=Util::createNodeforReductionCall((yyvsp[(1) - (4)].ival),(yyvsp[(3) - (4)].aList)->AList);}
    break;

  case 152:
#line 419 "lrparser.y"
    { (yyval.ival)=REDUCE_SUM;}
    break;

  case 153:
#line 420 "lrparser.y"
    {(yyval.ival)=REDUCE_COUNT;}
    break;

  case 154:
#line 421 "lrparser.y"
    {(yyval.ival)=REDUCE_PRODUCT;}
    break;

  case 155:
#line 422 "lrparser.y"
    {(yyval.ival)=REDUCE_MAX;}
    break;

  case 156:
#line 423 "lrparser.y"
    {(yyval.ival)=REDUCE_MIN;}
    break;

  case 157:
#line 425 "lrparser.y"
    { (yyval.node)=(yyvsp[(1) - (1)].node); }
    break;

  case 158:
#line 426 "lrparser.y"
    { printf("Here hello \n"); (yyval.node)=(yyvsp[(1) - (1)].node); }
    break;

  case 159:
#line 427 "lrparser.y"
    {(yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 160:
#line 428 "lrparser.y"
    {(yyval.node)=(yyvsp[(1) - (1)].node);}
    break;

  case 161:
#line 431 "lrparser.y"
    {
                 argList* aList=new argList();
				 (yyval.aList)=aList;  }
    break;

  case 162:
#line 435 "lrparser.y"
    {argument* a1=new argument();
		                          assignment* assign=(assignment*)(yyvsp[(1) - (3)].node);
		                     a1->setAssign(assign);
							 a1->setAssignFlag();
		                 //a1->assignExpr=(assignment*)$1;
						 // a1->assign=true;
						  (yyval.aList)=Util::addToAList((yyvsp[(3) - (3)].aList),a1);
						  /*
						  for(argument* arg:$$->AList)
						  {
							  printf("VALUE OF ARG %d",arg->getAssignExpr()); //rm for warnings
						  }
						  */ 
						  
                          }
    break;

  case 163:
#line 452 "lrparser.y"
    {argument* a1=new argument();
		                                Expression* expr=(Expression*)(yyvsp[(1) - (3)].node);
										a1->setExpression(expr);
										a1->setExpressionFlag();
						               // a1->expressionflag=true;
										 (yyval.aList)=Util::addToAList((yyvsp[(3) - (3)].aList),a1);
						                }
    break;

  case 164:
#line 459 "lrparser.y"
    {argument* a1=new argument();
		                 Expression* expr=(Expression*)(yyvsp[(1) - (1)].node);
						 a1->setExpression(expr);
						a1->setExpressionFlag();
						  (yyval.aList)=Util::createAList(a1); }
    break;

  case 165:
#line 464 "lrparser.y"
    { argument* a1=new argument();
		                   assignment* assign=(assignment*)(yyvsp[(1) - (1)].node);
		                     a1->setAssign(assign);
							 a1->setAssignFlag();
						   (yyval.aList)=Util::createAList(a1);
						   }
    break;

  case 166:
#line 472 "lrparser.y"
    {(yyval.node)=Util::createIterateInBFSNode((yyvsp[(3) - (13)].node),(yyvsp[(5) - (13)].node),(yyvsp[(7) - (13)].node),(yyvsp[(9) - (13)].node),(yyvsp[(11) - (13)].node),(yyvsp[(12) - (13)].node),(yyvsp[(13) - (13)].node)) ;}
    break;

  case 167:
#line 473 "lrparser.y"
    {(yyval.node)=Util::createIterateInBFSNode((yyvsp[(3) - (12)].node),(yyvsp[(5) - (12)].node),(yyvsp[(7) - (12)].node),(yyvsp[(9) - (12)].node),(yyvsp[(11) - (12)].node),(yyvsp[(12) - (12)].node),NULL) ; }
    break;

  case 168:
#line 477 "lrparser.y"
    {(yyval.node)=Util::createIterateInReverseBFSNode(NULL,(yyvsp[(2) - (2)].node));}
    break;

  case 169:
#line 478 "lrparser.y"
    {(yyval.node)=Util::createIterateInReverseBFSNode((yyvsp[(3) - (5)].node),(yyvsp[(5) - (5)].node));}
    break;

  case 170:
#line 481 "lrparser.y"
    { //Identifier* id1=(Identifier*)Util::createIdentifierNode($1);
                  // Identifier* id2=(Identifier*)Util::createIdentifierNode($1);
				   (yyval.node) = Util::createPropIdNode((yyvsp[(1) - (3)].node),(yyvsp[(3) - (3)].node));
				    }
    break;

  case 171:
#line 485 "lrparser.y"
    { ASTNode* expr1 = Util::createNodeForId((yyvsp[(3) - (6)].node));
	                          ASTNode* expr2 = Util::createNodeForId((yyvsp[(5) - (6)].node));
							  ASTNode* indexexpr =  Util::createNodeForIndexExpr(expr1, expr2, OPERATOR_INDEX);
	                          (yyval.node) = Util::createPropIdNode((yyvsp[(1) - (6)].node) , indexexpr);}
    break;

  case 172:
#line 492 "lrparser.y"
    {// Identifier* id1=(Identifier*)Util::createIdentifierNode($1);
                  // Identifier* id2=(Identifier*)Util::createIdentifierNode($1);
				   (yyval.node)=Util::createPropIdNode((yyvsp[(1) - (5)].node),(yyvsp[(3) - (5)].node));
				    }
    break;

  case 173:
#line 496 "lrparser.y"
    { 
	         (yyval.node)=Util::createIdentifierNode((yyvsp[(1) - (1)].text));  

            
            }
    break;


/* Line 1267 of yacc.c.  */
#line 2978 "y.tab.c"
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


#line 504 "lrparser.y"


void create_directory(const char *backendTarget) {
    char directory_name[256];
    snprintf(directory_name, sizeof(directory_name), "../graphcode/generated_S", backendTarget);

    // Check if directory already exists
    struct stat st = {0};
    if (stat(directory_name, &st) == -1) {
        // Create the directory
        if (mkdir(directory_name, 0700) == 0) {
            printf("Directory created: %s\n", directory_name);
        } else {
            perror("mkdir failed");
        }
    } else {
        printf("Directory already exists: %s\n", directory_name);
    }
}

void yyerror(const char *s) {
    fprintf(stderr, "%s\n", s);
}


int main(int argc,char **argv) 
{
  
  if(argc<4){
    std::cout<< "Usage: " << argv[0] << " [-s|-d] -f <dsl.sp> -b [cuda|omp|mpi|acc|multigpu|amd] " << '\n';
    std::cout<< "E.g. : " << argv[0] << " -s -f ../graphcode/sssp_dslV2 -b omp " << '\n';
    exit(-1);
  }
  
    //dsl_cpp_generator cpp_backend;
    SymbolTableBuilder stBuilder;
     FILE    *fd;
     
  int opt;
  char* fileName=NULL;
  backendTarget = NULL;
  bool staticGen = false;
  bool dynamicGen = false;
  bool optimize = false;

  while ((opt = getopt(argc, argv, "sdf:b:o")) != -1) 
  {
     switch (opt) 
     {
      case 'f':
        fileName = optarg;
        break;
      case 'b':
        backendTarget = optarg;
        break;
      case 's':
	    staticGen = true;
		break;
	  case 'd':
	    dynamicGen = true;
        break;	
	  case 'o':
	  	optimize = true;
		break;	
      case '?':
        fprintf(stderr,"Unknown option: %c\n", optopt);
		exit(-1);
        break;
      case ':':
        fprintf(stderr,"Missing arg for %c\n", optopt);
		exit(-1);
        break;
     }
  }
   
   printf("fileName %s\n",fileName);
   printf("Backend Target %s\n",backendTarget);

   
   if(fileName==NULL||backendTarget==NULL)
   {
	   if(fileName==NULL)
	      fprintf(stderr,"FileName option Error!\n");
	   if(backendTarget==NULL)
	      fprintf(stderr,"backendTarget option Error!\n")	;
	   exit(-1);	    
   }
   else
    {

		if(!((strcmp(backendTarget,"omp")==0)|| (strcmp(backendTarget,"amd")==0) || (strcmp(backendTarget,"mpi")==0)||(strcmp(backendTarget,"cuda")==0) || (strcmp(backendTarget,"acc")==0) || (strcmp(backendTarget,"sycl")==0)|| (strcmp(backendTarget,"multigpu")==0)))

		   {
			  fprintf(stderr, "Specified backend target is not implemented in the current version!\n");
			   exit(-1);
		   }
	}

   if(!(staticGen || dynamicGen)) {
		fprintf(stderr, "Type of graph(static/dynamic) not specified!\n");
		exit(-1);
     }
	  
     

   // only create a directory if the passed option is a valid option
   create_directory(backendTarget);
   yyin= fopen(fileName,"r");
   
   if(!yyin) {
	printf("file doesn't exists!\n");
	exit(-1);
   }
   
   
   int error=yyparse();
   printf("error val %d\n",error);


	if(error!=1)
	{
     //TODO: redirect to different backend generator after comparing with the 'b' option
    std::cout << "at 1" << std::endl;
	stBuilder.buildST(frontEndContext.getFuncList());
	frontEndContext.setDynamicLinkFuncs(stBuilder.getDynamicLinkedFuncs());
	std::cout << "at 2" << std::endl;

	if(staticGen)
	  {
		  /*
		  if(optimize)
		  {
			  attachPropAnalyser apAnalyser;
			  apAnalyser.analyse(frontEndContext.getFuncList());

			  dataRaceAnalyser drAnalyser;
			  drAnalyser.analyse(frontEndContext.getFuncList());
			  
			  if(strcmp(backendTarget,"cuda")==0)
			  {
			  	deviceVarsAnalyser dvAnalyser;
				//cpp_backend.setOptimized();
			  	dvAnalyser.analyse(frontEndContext.getFuncList());
			  }
		  }
		  */
	  //cpp_backend.setFileName(fileName);
	  //cpp_backend.generate();
     if (strcmp(backendTarget, "cuda") == 0) {
        spcuda::dsl_cpp_generator cpp_backend;
        cpp_backend.setFileName(fileName);
	//~ cpp_backend.setOptimized();
	
	if (optimize) {
	  attachPropAnalyser apAnalyser;
	  apAnalyser.analyse(frontEndContext.getFuncList());

	  dataRaceAnalyser drAnalyser;
	  drAnalyser.analyse(frontEndContext.getFuncList());

	  deviceVarsAnalyser dvAnalyser;
	  dvAnalyser.analyse(frontEndContext.getFuncList());
	  cpp_backend.setOptimized();
	}
		  
        cpp_backend.generate();
      } 
      else if (strcmp(backendTarget, "omp") == 0) {
        spomp::dsl_cpp_generator cpp_backend;
	std::cout<< "size:" << frontEndContext.getFuncList().size() << '\n';
        cpp_backend.setFileName(fileName);
        cpp_backend.generate();
      } 
	  else if (strcmp(backendTarget, "mpi") == 0) {
        spmpi::dsl_cpp_generator cpp_backend;
		std::cout<< "size:" << frontEndContext.getFuncList().size() << '\n';
        cpp_backend.setFileName(fileName);
        cpp_backend.generate();
      } 
      else if (strcmp(backendTarget, "acc") == 0) {
        spacc::dsl_cpp_generator cpp_backend;
        cpp_backend.setFileName(fileName);
		if(optimize) {
			cpp_backend.setOptimized();
			blockVarsAnalyser bvAnalyser;
			bvAnalyser.analyse(frontEndContext.getFuncList());
		}
        cpp_backend.generate();
      }
	  else if(strcmp(backendTarget, "multigpu") == 0){
		spmultigpu::dsl_cpp_generator cpp_backend;
		pushpullAnalyser pp;
		pp.analyse(frontEndContext.getFuncList());
		cpp_backend.setFileName(fileName);
		cpp_backend.generate();
}
	  else if (strcmp(backendTarget, "sycl") == 0) {
		std::cout<<"GENERATING SYCL CODE"<<std::endl;
        spsycl::dsl_cpp_generator cpp_backend;
        cpp_backend.setFileName(fileName);
        cpp_backend.generate();
	  }
	  else if (strcmp(backendTarget, "amd") == 0) {
		std::cout<<"GENERATING OPENCL CODE"<<std::endl;
        spamd::dsl_cpp_generator cpp_backend;
        cpp_backend.setFileName(fileName);
        cpp_backend.generate();
	  }
      else
	    std::cout<< "invalid backend" << '\n';
	  }
	else 
	 {
		if(strcmp(backendTarget, "omp") == 0) {
		   spdynomp::dsl_dyn_cpp_generator cpp_dyn_gen;
		   cpp_dyn_gen.setFileName(fileName);
	       cpp_dyn_gen.generate();
		}
		if(strcmp(backendTarget, "mpi") == 0){
		   spdynmpi::dsl_dyn_cpp_generator cpp_dyn_gen;
		   std::cout<<"created dyn mpi"<<std::endl;
		   cpp_dyn_gen.setFileName(fileName);
		   std::cout<<"file name set"<<std::endl;
	       cpp_dyn_gen.generate();	
		}
	 }
	
   }

	printf("finished successfully\n");
   
   /* to generate code, ./finalcode -s/-d -f "filename" -b "backendname"*/
	return 0;   
	 
}

