/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
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

#line 107 "y.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    T_INT = 258,                   /* T_INT  */
    T_FLOAT = 259,                 /* T_FLOAT  */
    T_BOOL = 260,                  /* T_BOOL  */
    T_STRING = 261,                /* T_STRING  */
    T_DOUBLE = 262,                /* T_DOUBLE  */
    T_LONG = 263,                  /* T_LONG  */
    T_AUTOREF = 264,               /* T_AUTOREF  */
    T_FORALL = 265,                /* T_FORALL  */
    T_FOR = 266,                   /* T_FOR  */
    T_P_INF = 267,                 /* T_P_INF  */
    T_INF = 268,                   /* T_INF  */
    T_N_INF = 269,                 /* T_N_INF  */
    T_FUNC = 270,                  /* T_FUNC  */
    T_IF = 271,                    /* T_IF  */
    T_ELSE = 272,                  /* T_ELSE  */
    T_WHILE = 273,                 /* T_WHILE  */
    T_RETURN = 274,                /* T_RETURN  */
    T_DO = 275,                    /* T_DO  */
    T_IN = 276,                    /* T_IN  */
    T_FIXEDPOINT = 277,            /* T_FIXEDPOINT  */
    T_UNTIL = 278,                 /* T_UNTIL  */
    T_FILTER = 279,                /* T_FILTER  */
    T_ADD_ASSIGN = 280,            /* T_ADD_ASSIGN  */
    T_SUB_ASSIGN = 281,            /* T_SUB_ASSIGN  */
    T_MUL_ASSIGN = 282,            /* T_MUL_ASSIGN  */
    T_DIV_ASSIGN = 283,            /* T_DIV_ASSIGN  */
    T_MOD_ASSIGN = 284,            /* T_MOD_ASSIGN  */
    T_AND_ASSIGN = 285,            /* T_AND_ASSIGN  */
    T_XOR_ASSIGN = 286,            /* T_XOR_ASSIGN  */
    T_OR_ASSIGN = 287,             /* T_OR_ASSIGN  */
    T_INC_OP = 288,                /* T_INC_OP  */
    T_DEC_OP = 289,                /* T_DEC_OP  */
    T_PTR_OP = 290,                /* T_PTR_OP  */
    T_AND_OP = 291,                /* T_AND_OP  */
    T_OR_OP = 292,                 /* T_OR_OP  */
    T_LE_OP = 293,                 /* T_LE_OP  */
    T_GE_OP = 294,                 /* T_GE_OP  */
    T_EQ_OP = 295,                 /* T_EQ_OP  */
    T_NE_OP = 296,                 /* T_NE_OP  */
    T_AND = 297,                   /* T_AND  */
    T_OR = 298,                    /* T_OR  */
    T_SUM = 299,                   /* T_SUM  */
    T_AVG = 300,                   /* T_AVG  */
    T_COUNT = 301,                 /* T_COUNT  */
    T_PRODUCT = 302,               /* T_PRODUCT  */
    T_MAX = 303,                   /* T_MAX  */
    T_MIN = 304,                   /* T_MIN  */
    T_GRAPH = 305,                 /* T_GRAPH  */
    T_GNN = 306,                   /* T_GNN  */
    T_DIR_GRAPH = 307,             /* T_DIR_GRAPH  */
    T_NODE = 308,                  /* T_NODE  */
    T_EDGE = 309,                  /* T_EDGE  */
    T_UPDATES = 310,               /* T_UPDATES  */
    T_CONTAINER = 311,             /* T_CONTAINER  */
    T_NODEMAP = 312,               /* T_NODEMAP  */
    T_VECTOR = 313,                /* T_VECTOR  */
    T_HASHMAP = 314,               /* T_HASHMAP  */
    T_HASHSET = 315,               /* T_HASHSET  */
    T_NP = 316,                    /* T_NP  */
    T_EP = 317,                    /* T_EP  */
    T_LIST = 318,                  /* T_LIST  */
    T_SET_NODES = 319,             /* T_SET_NODES  */
    T_SET_EDGES = 320,             /* T_SET_EDGES  */
    T_FROM = 321,                  /* T_FROM  */
    T_BFS = 322,                   /* T_BFS  */
    T_REVERSE = 323,               /* T_REVERSE  */
    T_INCREMENTAL = 324,           /* T_INCREMENTAL  */
    T_DECREMENTAL = 325,           /* T_DECREMENTAL  */
    T_STATIC = 326,                /* T_STATIC  */
    T_DYNAMIC = 327,               /* T_DYNAMIC  */
    T_BATCH = 328,                 /* T_BATCH  */
    T_ONADD = 329,                 /* T_ONADD  */
    T_ONDELETE = 330,              /* T_ONDELETE  */
    ID = 331,                      /* ID  */
    INT_NUM = 332,                 /* INT_NUM  */
    FLOAT_NUM = 333,               /* FLOAT_NUM  */
    BOOL_VAL = 334,                /* BOOL_VAL  */
    CHAR_VAL = 335,                /* CHAR_VAL  */
    STRING_VAL = 336,              /* STRING_VAL  */
    return_func = 337              /* return_func  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
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
#define T_GNN 306
#define T_DIR_GRAPH 307
#define T_NODE 308
#define T_EDGE 309
#define T_UPDATES 310
#define T_CONTAINER 311
#define T_NODEMAP 312
#define T_VECTOR 313
#define T_HASHMAP 314
#define T_HASHSET 315
#define T_NP 316
#define T_EP 317
#define T_LIST 318
#define T_SET_NODES 319
#define T_SET_EDGES 320
#define T_FROM 321
#define T_BFS 322
#define T_REVERSE 323
#define T_INCREMENTAL 324
#define T_DECREMENTAL 325
#define T_STATIC 326
#define T_DYNAMIC 327
#define T_BATCH 328
#define T_ONADD 329
#define T_ONDELETE 330
#define ID 331
#define INT_NUM 332
#define FLOAT_NUM 333
#define BOOL_VAL 334
#define CHAR_VAL 335
#define STRING_VAL 336
#define return_func 337

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 42 "lrparser.y"

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
     

#line 339 "y.tab.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_T_INT = 3,                      /* T_INT  */
  YYSYMBOL_T_FLOAT = 4,                    /* T_FLOAT  */
  YYSYMBOL_T_BOOL = 5,                     /* T_BOOL  */
  YYSYMBOL_T_STRING = 6,                   /* T_STRING  */
  YYSYMBOL_T_DOUBLE = 7,                   /* T_DOUBLE  */
  YYSYMBOL_T_LONG = 8,                     /* T_LONG  */
  YYSYMBOL_T_AUTOREF = 9,                  /* T_AUTOREF  */
  YYSYMBOL_T_FORALL = 10,                  /* T_FORALL  */
  YYSYMBOL_T_FOR = 11,                     /* T_FOR  */
  YYSYMBOL_T_P_INF = 12,                   /* T_P_INF  */
  YYSYMBOL_T_INF = 13,                     /* T_INF  */
  YYSYMBOL_T_N_INF = 14,                   /* T_N_INF  */
  YYSYMBOL_T_FUNC = 15,                    /* T_FUNC  */
  YYSYMBOL_T_IF = 16,                      /* T_IF  */
  YYSYMBOL_T_ELSE = 17,                    /* T_ELSE  */
  YYSYMBOL_T_WHILE = 18,                   /* T_WHILE  */
  YYSYMBOL_T_RETURN = 19,                  /* T_RETURN  */
  YYSYMBOL_T_DO = 20,                      /* T_DO  */
  YYSYMBOL_T_IN = 21,                      /* T_IN  */
  YYSYMBOL_T_FIXEDPOINT = 22,              /* T_FIXEDPOINT  */
  YYSYMBOL_T_UNTIL = 23,                   /* T_UNTIL  */
  YYSYMBOL_T_FILTER = 24,                  /* T_FILTER  */
  YYSYMBOL_T_ADD_ASSIGN = 25,              /* T_ADD_ASSIGN  */
  YYSYMBOL_T_SUB_ASSIGN = 26,              /* T_SUB_ASSIGN  */
  YYSYMBOL_T_MUL_ASSIGN = 27,              /* T_MUL_ASSIGN  */
  YYSYMBOL_T_DIV_ASSIGN = 28,              /* T_DIV_ASSIGN  */
  YYSYMBOL_T_MOD_ASSIGN = 29,              /* T_MOD_ASSIGN  */
  YYSYMBOL_T_AND_ASSIGN = 30,              /* T_AND_ASSIGN  */
  YYSYMBOL_T_XOR_ASSIGN = 31,              /* T_XOR_ASSIGN  */
  YYSYMBOL_T_OR_ASSIGN = 32,               /* T_OR_ASSIGN  */
  YYSYMBOL_T_INC_OP = 33,                  /* T_INC_OP  */
  YYSYMBOL_T_DEC_OP = 34,                  /* T_DEC_OP  */
  YYSYMBOL_T_PTR_OP = 35,                  /* T_PTR_OP  */
  YYSYMBOL_T_AND_OP = 36,                  /* T_AND_OP  */
  YYSYMBOL_T_OR_OP = 37,                   /* T_OR_OP  */
  YYSYMBOL_T_LE_OP = 38,                   /* T_LE_OP  */
  YYSYMBOL_T_GE_OP = 39,                   /* T_GE_OP  */
  YYSYMBOL_T_EQ_OP = 40,                   /* T_EQ_OP  */
  YYSYMBOL_T_NE_OP = 41,                   /* T_NE_OP  */
  YYSYMBOL_T_AND = 42,                     /* T_AND  */
  YYSYMBOL_T_OR = 43,                      /* T_OR  */
  YYSYMBOL_T_SUM = 44,                     /* T_SUM  */
  YYSYMBOL_T_AVG = 45,                     /* T_AVG  */
  YYSYMBOL_T_COUNT = 46,                   /* T_COUNT  */
  YYSYMBOL_T_PRODUCT = 47,                 /* T_PRODUCT  */
  YYSYMBOL_T_MAX = 48,                     /* T_MAX  */
  YYSYMBOL_T_MIN = 49,                     /* T_MIN  */
  YYSYMBOL_T_GRAPH = 50,                   /* T_GRAPH  */
  YYSYMBOL_T_GNN = 51,                     /* T_GNN  */
  YYSYMBOL_T_DIR_GRAPH = 52,               /* T_DIR_GRAPH  */
  YYSYMBOL_T_NODE = 53,                    /* T_NODE  */
  YYSYMBOL_T_EDGE = 54,                    /* T_EDGE  */
  YYSYMBOL_T_UPDATES = 55,                 /* T_UPDATES  */
  YYSYMBOL_T_CONTAINER = 56,               /* T_CONTAINER  */
  YYSYMBOL_T_NODEMAP = 57,                 /* T_NODEMAP  */
  YYSYMBOL_T_VECTOR = 58,                  /* T_VECTOR  */
  YYSYMBOL_T_HASHMAP = 59,                 /* T_HASHMAP  */
  YYSYMBOL_T_HASHSET = 60,                 /* T_HASHSET  */
  YYSYMBOL_T_NP = 61,                      /* T_NP  */
  YYSYMBOL_T_EP = 62,                      /* T_EP  */
  YYSYMBOL_T_LIST = 63,                    /* T_LIST  */
  YYSYMBOL_T_SET_NODES = 64,               /* T_SET_NODES  */
  YYSYMBOL_T_SET_EDGES = 65,               /* T_SET_EDGES  */
  YYSYMBOL_T_FROM = 66,                    /* T_FROM  */
  YYSYMBOL_T_BFS = 67,                     /* T_BFS  */
  YYSYMBOL_T_REVERSE = 68,                 /* T_REVERSE  */
  YYSYMBOL_T_INCREMENTAL = 69,             /* T_INCREMENTAL  */
  YYSYMBOL_T_DECREMENTAL = 70,             /* T_DECREMENTAL  */
  YYSYMBOL_T_STATIC = 71,                  /* T_STATIC  */
  YYSYMBOL_T_DYNAMIC = 72,                 /* T_DYNAMIC  */
  YYSYMBOL_T_BATCH = 73,                   /* T_BATCH  */
  YYSYMBOL_T_ONADD = 74,                   /* T_ONADD  */
  YYSYMBOL_T_ONDELETE = 75,                /* T_ONDELETE  */
  YYSYMBOL_ID = 76,                        /* ID  */
  YYSYMBOL_INT_NUM = 77,                   /* INT_NUM  */
  YYSYMBOL_FLOAT_NUM = 78,                 /* FLOAT_NUM  */
  YYSYMBOL_BOOL_VAL = 79,                  /* BOOL_VAL  */
  YYSYMBOL_CHAR_VAL = 80,                  /* CHAR_VAL  */
  YYSYMBOL_STRING_VAL = 81,                /* STRING_VAL  */
  YYSYMBOL_return_func = 82,               /* return_func  */
  YYSYMBOL_83_ = 83,                       /* '?'  */
  YYSYMBOL_84_ = 84,                       /* ':'  */
  YYSYMBOL_85_ = 85,                       /* '<'  */
  YYSYMBOL_86_ = 86,                       /* '>'  */
  YYSYMBOL_87_ = 87,                       /* '+'  */
  YYSYMBOL_88_ = 88,                       /* '-'  */
  YYSYMBOL_89_ = 89,                       /* '*'  */
  YYSYMBOL_90_ = 90,                       /* '/'  */
  YYSYMBOL_91_ = 91,                       /* '%'  */
  YYSYMBOL_92_ = 92,                       /* '('  */
  YYSYMBOL_93_ = 93,                       /* ')'  */
  YYSYMBOL_94_ = 94,                       /* ','  */
  YYSYMBOL_95_ = 95,                       /* ';'  */
  YYSYMBOL_96_ = 96,                       /* '.'  */
  YYSYMBOL_97_ = 97,                       /* '{'  */
  YYSYMBOL_98_ = 98,                       /* '}'  */
  YYSYMBOL_99_ = 99,                       /* '='  */
  YYSYMBOL_100_ = 100,                     /* '!'  */
  YYSYMBOL_101_ = 101,                     /* '['  */
  YYSYMBOL_102_ = 102,                     /* ']'  */
  YYSYMBOL_YYACCEPT = 103,                 /* $accept  */
  YYSYMBOL_program = 104,                  /* program  */
  YYSYMBOL_function_def = 105,             /* function_def  */
  YYSYMBOL_function_data = 106,            /* function_data  */
  YYSYMBOL_paramList = 107,                /* paramList  */
  YYSYMBOL_type = 108,                     /* type  */
  YYSYMBOL_param = 109,                    /* param  */
  YYSYMBOL_function_body = 110,            /* function_body  */
  YYSYMBOL_statements = 111,               /* statements  */
  YYSYMBOL_statement = 112,                /* statement  */
  YYSYMBOL_blockstatements = 113,          /* blockstatements  */
  YYSYMBOL_batch_blockstmt = 114,          /* batch_blockstmt  */
  YYSYMBOL_on_add_blockstmt = 115,         /* on_add_blockstmt  */
  YYSYMBOL_on_delete_blockstmt = 116,      /* on_delete_blockstmt  */
  YYSYMBOL_block_begin = 117,              /* block_begin  */
  YYSYMBOL_block_end = 118,                /* block_end  */
  YYSYMBOL_return_stmt = 119,              /* return_stmt  */
  YYSYMBOL_declaration = 120,              /* declaration  */
  YYSYMBOL_type1 = 121,                    /* type1  */
  YYSYMBOL_primitive = 122,                /* primitive  */
  YYSYMBOL_type3 = 123,                    /* type3  */
  YYSYMBOL_graph = 124,                    /* graph  */
  YYSYMBOL_gnn = 125,                      /* gnn  */
  YYSYMBOL_collections = 126,              /* collections  */
  YYSYMBOL_container = 127,                /* container  */
  YYSYMBOL_vector = 128,                   /* vector  */
  YYSYMBOL_nodemap = 129,                  /* nodemap  */
  YYSYMBOL_hashmap = 130,                  /* hashmap  */
  YYSYMBOL_hashset = 131,                  /* hashset  */
  YYSYMBOL_type2 = 132,                    /* type2  */
  YYSYMBOL_property = 133,                 /* property  */
  YYSYMBOL_assignment = 134,               /* assignment  */
  YYSYMBOL_rhs = 135,                      /* rhs  */
  YYSYMBOL_expression = 136,               /* expression  */
  YYSYMBOL_indexExpr = 137,                /* indexExpr  */
  YYSYMBOL_unary_expr = 138,               /* unary_expr  */
  YYSYMBOL_proc_call = 139,                /* proc_call  */
  YYSYMBOL_val = 140,                      /* val  */
  YYSYMBOL_control_flow = 141,             /* control_flow  */
  YYSYMBOL_iteration_cf = 142,             /* iteration_cf  */
  YYSYMBOL_filterExpr = 143,               /* filterExpr  */
  YYSYMBOL_boolean_expr = 144,             /* boolean_expr  */
  YYSYMBOL_selection_cf = 145,             /* selection_cf  */
  YYSYMBOL_reduction = 146,                /* reduction  */
  YYSYMBOL_reduce_op = 147,                /* reduce_op  */
  YYSYMBOL_leftList = 148,                 /* leftList  */
  YYSYMBOL_rightList = 149,                /* rightList  */
  YYSYMBOL_reductionCall = 150,            /* reductionCall  */
  YYSYMBOL_reduction_calls = 151,          /* reduction_calls  */
  YYSYMBOL_leftSide = 152,                 /* leftSide  */
  YYSYMBOL_arg_list = 153,                 /* arg_list  */
  YYSYMBOL_bfs_abstraction = 154,          /* bfs_abstraction  */
  YYSYMBOL_reverse_abstraction = 155,      /* reverse_abstraction  */
  YYSYMBOL_oid = 156,                      /* oid  */
  YYSYMBOL_tid = 157,                      /* tid  */
  YYSYMBOL_id = 158                        /* id  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   964

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  103
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  56
/* YYNRULES -- Number of rules.  */
#define YYNRULES  176
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  419

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   337


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   100,     2,     2,     2,    91,     2,     2,
      92,    93,    89,    87,    94,    88,    96,    90,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    84,    95,
      85,    99,    86,    83,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   101,     2,   102,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    97,     2,    98,     2,     2,     2,     2,
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
      75,    76,    77,    78,    79,    80,    81,    82
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   110,   110,   111,   113,   120,   126,   132,   138,   144,
     151,   152,   155,   156,   157,   159,   175,   179,   188,   191,
     192,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   208,   210,   212,   214,   216,   218,   220,
     223,   232,   235,   238,   241,   245,   246,   247,   248,   251,
     252,   253,   254,   255,   256,   258,   260,   261,   263,   265,
     266,   268,   270,   271,   272,   273,   274,   275,   277,   278,
     279,   282,   283,   284,   286,   288,   291,   294,   295,   296,
     298,   299,   300,   301,   302,   304,   307,   308,   311,   313,
     314,   315,   316,   317,   318,   319,   320,   321,   322,   323,
     324,   325,   326,   327,   330,   331,   332,   333,   335,   337,
     338,   340,   345,   349,   353,   364,   365,   366,   367,   368,
     369,   370,   373,   374,   376,   377,   378,   379,   381,   382,
     383,   384,   385,   387,   389,   390,   392,   394,   395,   398,
     399,   401,   402,   405,   406,   407,   408,   409,   411,   413,
     415,   416,   418,   419,   428,   430,   431,   432,   433,   434,
     436,   437,   438,   439,   442,   446,   463,   470,   475,   483,
     484,   488,   489,   492,   496,   503,   507
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "T_INT", "T_FLOAT",
  "T_BOOL", "T_STRING", "T_DOUBLE", "T_LONG", "T_AUTOREF", "T_FORALL",
  "T_FOR", "T_P_INF", "T_INF", "T_N_INF", "T_FUNC", "T_IF", "T_ELSE",
  "T_WHILE", "T_RETURN", "T_DO", "T_IN", "T_FIXEDPOINT", "T_UNTIL",
  "T_FILTER", "T_ADD_ASSIGN", "T_SUB_ASSIGN", "T_MUL_ASSIGN",
  "T_DIV_ASSIGN", "T_MOD_ASSIGN", "T_AND_ASSIGN", "T_XOR_ASSIGN",
  "T_OR_ASSIGN", "T_INC_OP", "T_DEC_OP", "T_PTR_OP", "T_AND_OP", "T_OR_OP",
  "T_LE_OP", "T_GE_OP", "T_EQ_OP", "T_NE_OP", "T_AND", "T_OR", "T_SUM",
  "T_AVG", "T_COUNT", "T_PRODUCT", "T_MAX", "T_MIN", "T_GRAPH", "T_GNN",
  "T_DIR_GRAPH", "T_NODE", "T_EDGE", "T_UPDATES", "T_CONTAINER",
  "T_NODEMAP", "T_VECTOR", "T_HASHMAP", "T_HASHSET", "T_NP", "T_EP",
  "T_LIST", "T_SET_NODES", "T_SET_EDGES", "T_FROM", "T_BFS", "T_REVERSE",
  "T_INCREMENTAL", "T_DECREMENTAL", "T_STATIC", "T_DYNAMIC", "T_BATCH",
  "T_ONADD", "T_ONDELETE", "ID", "INT_NUM", "FLOAT_NUM", "BOOL_VAL",
  "CHAR_VAL", "STRING_VAL", "return_func", "'?'", "':'", "'<'", "'>'",
  "'+'", "'-'", "'*'", "'/'", "'%'", "'('", "')'", "','", "';'", "'.'",
  "'{'", "'}'", "'='", "'!'", "'['", "']'", "$accept", "program",
  "function_def", "function_data", "paramList", "type", "param",
  "function_body", "statements", "statement", "blockstatements",
  "batch_blockstmt", "on_add_blockstmt", "on_delete_blockstmt",
  "block_begin", "block_end", "return_stmt", "declaration", "type1",
  "primitive", "type3", "graph", "gnn", "collections", "container",
  "vector", "nodemap", "hashmap", "hashset", "type2", "property",
  "assignment", "rhs", "expression", "indexExpr", "unary_expr",
  "proc_call", "val", "control_flow", "iteration_cf", "filterExpr",
  "boolean_expr", "selection_cf", "reduction", "reduce_op", "leftList",
  "rightList", "reductionCall", "reduction_calls", "leftSide", "arg_list",
  "bfs_abstraction", "reverse_abstraction", "oid", "tid", "id", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-357)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-176)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -357,   128,  -357,   -66,   -41,   -30,   -66,   -66,  -357,   -71,
    -357,   -20,   888,   888,    12,    17,  -357,  -357,  -357,  -357,
     888,  -357,  -357,  -357,  -357,  -357,  -357,  -357,  -357,  -357,
    -357,  -357,   -50,    41,    38,    54,    59,    70,    75,    78,
    -357,    93,    96,   -48,    92,   -66,  -357,  -357,  -357,  -357,
    -357,  -357,  -357,  -357,  -357,   -66,  -357,   116,   888,   888,
     587,   118,   -66,   872,   872,   872,   872,   872,   567,   899,
     -66,   -66,  -357,   888,  -357,   103,  -357,   124,   133,  -357,
     142,   151,  -357,  -357,  -357,   159,   162,    29,   -71,   227,
     170,   171,   193,   197,   198,   203,  -357,  -357,  -357,  -357,
      29,    29,  -357,    29,  -357,  -357,  -357,  -357,  -357,  -357,
     202,   204,   -66,   -66,   -66,   206,   407,    65,   208,   213,
    -357,  -357,  -357,  -357,   216,    50,  -357,  -357,  -357,   218,
    -357,   229,   230,  -357,  -357,  -357,   224,   232,   226,   235,
     236,   240,   243,   244,   247,   248,   249,   250,  -357,   -66,
    -357,  -357,   -66,    80,    29,    29,   782,   -40,  -357,  -357,
     251,   324,   254,   -66,    29,    29,   -66,   -66,   -66,   782,
     264,   -55,   686,   782,  -357,  -357,   253,   255,   256,  -357,
    -357,  -357,  -357,  -357,  -357,  -357,  -357,    29,    29,    29,
      29,    29,    29,    29,    29,    29,    29,    29,    29,    29,
      29,    29,    29,  -357,  -357,  -357,    29,    46,    29,   -66,
    -357,   261,  -357,   266,   872,  -357,  -357,  -357,  -357,  -357,
    -357,  -357,  -357,  -357,   263,   338,   -66,   339,   782,   268,
     269,   271,   -66,   343,   272,   182,   -76,   274,   275,   281,
     348,   349,   273,    29,  -357,    29,    29,    29,   432,   528,
     100,   100,   306,   306,   100,   100,   102,   102,     0,     0,
     442,   782,   279,  -357,   782,   280,  -357,  -357,  -357,  -357,
    -357,  -357,  -357,   282,   782,   -74,    29,    29,   294,  -357,
      29,   283,    29,   685,   -71,    29,   301,   -66,    29,    29,
      29,  -357,  -357,    29,   -66,   -66,   303,  -357,  -357,  -357,
    -357,  -357,    29,  -357,    29,   -66,   -66,     9,    79,  -357,
     152,    91,   293,    29,   181,   101,   304,   373,  -357,   308,
      29,   309,  -357,  -357,   755,   310,   327,   158,   311,   332,
    -357,   295,  -357,   872,  -357,   872,   -71,   -71,    29,   307,
     -71,   -71,    29,   685,   331,   764,    29,   -71,    29,    29,
     333,   144,  -357,  -357,   342,   345,  -357,  -357,     4,    57,
      29,  -357,  -357,     4,  -357,  -357,   -71,   365,  -357,   356,
     357,    29,  -357,  -357,   412,   358,   -66,   347,   359,  -357,
     -66,   369,   370,   361,   371,   -21,   364,   -71,    84,    29,
     -71,   366,   -71,   -71,    29,  -357,    29,    29,  -357,   773,
    -357,   362,  -357,  -357,  -357,  -357,   367,   -71,   -71,  -357,
    -357,   393,   -42,  -357,    29,  -357,   381,   -71,  -357
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,     0,     3,     0,
     176,     0,     0,     0,     0,     0,    37,     4,    18,    19,
       0,    49,    50,    51,    54,    52,    53,    56,    58,    57,
      77,    78,     0,     0,     0,     0,     0,     0,     0,     0,
      59,     0,     0,     0,    10,     0,    45,    46,    48,    47,
      63,    64,    65,    66,    67,     0,    79,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     7,     0,    15,    16,     8,     0,     0,    55,
       0,     0,   120,   119,   121,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   115,   116,   117,   118,
       0,     0,    38,     0,    20,    27,    30,    31,    32,    33,
       0,     0,     0,     0,     0,     0,     0,   107,   106,    89,
     104,    24,   123,   122,     0,   105,    26,   161,   162,   160,
       5,     0,     0,    12,    14,    13,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    11,     0,
       6,     9,     0,     0,     0,     0,    39,   107,   106,    89,
     105,     0,     0,     0,   164,   164,     0,     0,     0,     0,
       0,   105,     0,   102,    29,    21,    40,     0,    42,    22,
     143,   147,   144,   146,   145,   109,   110,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    28,    23,    25,   164,     0,     0,     0,
      62,    70,    74,    73,     0,    76,    84,    85,    80,    82,
      81,    83,    60,    61,     0,     0,     0,     0,   136,     0,
       0,     0,     0,     0,   168,   167,   105,     0,     0,     0,
       0,     0,     0,     0,   103,     0,     0,     0,    94,    95,
      96,    97,   100,   101,    98,    99,    90,    91,    92,    93,
       0,   142,   105,    87,    88,     0,   155,   156,   157,   158,
     159,    86,   139,     0,   141,   173,   164,   164,     0,    17,
       0,     0,     0,     0,     0,     0,     0,     0,   164,   164,
       0,   112,   113,     0,     0,     0,     0,   148,    41,    44,
      43,   108,   164,   111,   164,     0,     0,     0,     0,    75,
     107,   105,   160,     0,   107,   105,   160,   137,   125,     0,
       0,     0,   165,   166,     0,     0,     0,     0,     0,     0,
     175,     0,    69,     0,    72,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   111,   154,   174,     0,     0,   132,   128,    89,   160,
       0,   131,   129,    89,   138,   126,     0,    89,    34,    89,
      89,     0,    68,    71,     0,     0,     0,     0,     0,   124,
       0,     0,     0,   104,     0,   105,     0,     0,   173,     0,
       0,     0,     0,     0,     0,   140,     0,     0,   127,     0,
     130,   134,    35,    36,   150,   151,     0,     0,     0,   135,
     133,   170,     0,   169,     0,   171,     0,     0,   172
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -357,  -357,  -357,  -357,   155,   -35,  -357,  -357,  -357,  -269,
      -9,  -357,  -357,  -357,  -357,  -357,  -357,  -357,     5,   -57,
     -59,  -357,  -357,   192,  -357,  -357,  -357,  -357,  -357,     8,
    -357,   -56,  -199,   222,   -24,   -51,   -36,  -356,  -357,  -357,
    -344,  -148,  -357,  -357,   337,   220,  -305,   137,  -357,   -47,
    -157,  -357,  -357,  -357,  -357,    -1
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     1,     8,     9,    43,   132,    44,    17,    60,   104,
     105,   106,   107,   108,    19,   109,   110,   111,   133,    46,
     134,    47,    48,    49,    50,    51,    52,    53,    54,   135,
      56,   234,   271,   169,   157,   158,   159,   120,   121,   122,
     375,   229,   123,   124,   200,   170,   384,   272,   273,   160,
     237,   126,   413,   127,   128,   129
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      18,   113,    11,   263,   115,    14,    15,   230,   238,   118,
      10,   142,   144,   125,   317,   383,   206,    45,    45,   378,
      55,    55,   305,   290,   119,    45,    16,   306,    55,   136,
     137,   138,   139,   185,   186,    62,   117,   206,   383,   243,
     383,    82,    83,    84,    74,    72,   298,   299,   300,   265,
     414,    12,  -163,   171,    75,    16,   201,   408,    82,    83,
      84,   131,    13,    45,    45,   112,    55,    55,   114,   146,
     147,   206,    20,   396,   364,   180,   181,   182,    45,   161,
     183,    55,   184,    21,    22,    23,    24,    25,    26,   404,
     266,   405,   267,   268,   269,   270,   226,  -134,    91,    92,
     374,   199,   332,   333,    58,    10,    96,    97,    98,    59,
      99,   176,   177,   178,   339,    91,    92,   236,   236,   307,
     308,   101,    10,    96,    97,    98,    63,    99,     2,   103,
      64,   322,   323,   185,   186,   185,   186,   319,   101,    65,
     117,   117,   206,     3,    66,   328,   103,   329,   224,   207,
    -173,   225,   227,   376,   262,    67,    10,  -163,   306,   236,
      68,   201,   233,    69,   202,   239,   240,   241,    57,  -114,
    -114,  -114,   334,   335,  -114,    61,  -114,  -175,    70,   278,
     305,    71,   117,   206,   337,   306,    73,   195,   196,   197,
     198,   197,   198,   206,   341,   149,   171,     4,     5,     6,
       7,   199,   266,   199,   267,   268,   269,   270,   275,    76,
    -114,   130,   377,    77,    78,   185,   186,   150,   187,   188,
     189,   190,   191,   192,   113,   281,   151,   115,   148,   236,
     236,   286,   118,   311,   152,   315,   125,  -114,  -114,  -114,
    -114,   236,   236,   153,  -163,   336,  -114,   119,   201,   406,
     162,   154,   117,   117,   155,   236,   310,   236,   314,   117,
     143,   145,   163,   164,   117,   117,   416,   193,   194,   195,
     196,   197,   198,  -163,   340,   318,   289,   201,   117,   312,
     117,   316,   116,   199,   113,   165,   321,   115,   112,   166,
     167,   114,   118,   325,   326,   168,   125,   174,   354,   175,
     355,   179,   358,   203,   330,   331,   363,   119,   204,   156,
     367,   205,   369,   370,   209,   210,   211,   212,   213,   117,
     214,   215,   216,   172,   385,   173,   217,   356,   357,   218,
     219,   361,   362,   220,   221,   222,   223,   359,   368,   185,
     186,   359,   231,   206,   189,   190,   232,   385,   112,   385,
     242,   114,   245,   276,   246,   247,   279,   379,   277,   280,
     282,   283,   284,   285,   287,   293,   288,   291,   292,   294,
     295,   302,   296,   303,   304,   388,   228,   228,   398,   391,
     309,   400,   313,   402,   403,   320,   235,   235,   327,   338,
     343,   193,   194,   195,   196,   197,   198,   353,   410,   411,
     342,   344,   360,   415,   351,   346,   348,   199,   418,   248,
     249,   250,   251,   252,   253,   254,   255,   256,   257,   258,
     259,   260,   261,   349,   264,   352,   365,   371,   235,   264,
     274,   380,   180,   181,   182,   372,   386,   183,   373,   184,
     185,   186,   389,   187,   188,   189,   190,   191,   192,   381,
     382,   387,   390,   392,   393,   394,   397,   395,   374,   401,
     409,   412,   208,   297,   350,   185,   186,   264,   264,   264,
     189,   190,   191,   192,   417,   185,   186,     0,   187,   188,
     189,   190,   191,   192,     0,     0,     0,     0,     0,     0,
       0,     0,   193,   194,   195,   196,   197,   198,   235,   235,
       0,     0,     0,     0,     0,   116,     0,   228,   199,     0,
     235,   235,   264,     0,     0,   324,     0,   193,   194,   195,
     196,   197,   198,     0,   235,     0,   235,   193,   194,   195,
     196,   197,   198,   199,     0,   264,     0,     0,     0,     0,
       0,     0,   345,   199,   301,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   185,   186,     0,   187,   116,   189,   190,   191,   192,
      21,    22,    23,    24,    25,    26,     0,     0,     0,     0,
       0,     0,   228,     0,     0,     0,     0,     0,     0,     0,
      21,    22,    23,    24,    25,    26,    79,    80,    81,    82,
      83,    84,     0,    85,     0,    86,    87,    88,     0,    89,
       0,   399,     0,   193,   194,   195,   196,   197,   198,   228,
     140,   141,    32,    33,    34,    35,    36,    37,     0,   199,
      40,    41,    42,     0,     0,     0,   228,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     0,    90,     0,    91,    92,     0,     0,
      93,    94,    95,    10,    96,    97,    98,     0,    99,     0,
       0,     0,   100,     0,     0,     0,     0,     0,     0,   101,
       0,     0,     0,     0,    16,   102,     0,   103,    21,    22,
      23,    24,    25,    26,    79,    80,    81,    82,    83,    84,
       0,    85,     0,    86,    87,    88,     0,    89,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   185,
     186,     0,   187,   188,   189,   190,   191,   192,     0,     0,
       0,     0,     0,     0,     0,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,     0,    90,     0,    91,    92,     0,     0,    93,    94,
      95,    10,    96,    97,    98,     0,    99,     0,     0,     0,
     100,   193,   194,   195,   196,   197,   198,   101,     0,   244,
       0,     0,    16,     0,     0,   103,     0,   199,   185,   186,
       0,   187,   188,   189,   190,   191,   192,   185,   186,     0,
     187,   188,   189,   190,   191,   192,   185,   186,     0,   187,
     188,   189,   190,   191,   192,   185,   186,     0,   187,   188,
     189,   190,   191,   192,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     193,   194,   195,   196,   197,   198,     0,     0,   347,   193,
     194,   195,   196,   197,   198,     0,   199,   366,   193,   194,
     195,   196,   197,   198,     0,   199,   407,   193,   194,   195,
     196,   197,   198,     0,   199,    21,    22,    23,    24,    25,
      26,    79,     0,   199,     0,     0,     0,     0,     0,     0,
       0,    21,    22,    23,    24,    25,    26,     0,     0,     0,
       0,     0,    21,    22,    23,    24,    25,    26,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    32,    33,    34,    35,    36,    37,
       0,     0,    40,    41,    42
};

static const yytype_int16 yycheck[] =
{
       9,    60,     3,   202,    60,     6,     7,   155,   165,    60,
      76,    68,    69,    60,   283,   371,    92,    12,    13,   363,
      12,    13,    96,    99,    60,    20,    97,   101,    20,    64,
      65,    66,    67,    33,    34,    85,    60,    92,   394,    94,
     396,    12,    13,    14,    45,    93,   245,   246,   247,   206,
      92,    92,    92,   100,    55,    97,    96,   401,    12,    13,
      14,    62,    92,    58,    59,    60,    58,    59,    60,    70,
      71,    92,    92,    94,   343,    25,    26,    27,    73,    88,
      30,    73,    32,     3,     4,     5,     6,     7,     8,   394,
      44,   396,    46,    47,    48,    49,   153,    93,    69,    70,
      96,   101,    93,    94,    92,    76,    77,    78,    79,    92,
      81,   112,   113,   114,   313,    69,    70,   164,   165,   276,
     277,    92,    76,    77,    78,    79,    85,    81,     0,   100,
      92,   288,   289,    33,    34,    33,    34,   285,    92,    85,
     164,   165,    92,    15,    85,   302,   100,   304,   149,    99,
      93,   152,   153,    96,   201,    85,    76,    92,   101,   206,
      85,    96,   163,    85,    99,   166,   167,   168,    13,    25,
      26,    27,    93,    94,    30,    20,    32,    93,    85,   214,
      96,    85,   206,    92,    93,   101,    94,    87,    88,    89,
      90,    89,    90,    92,    93,    92,   243,    69,    70,    71,
      72,   101,    44,   101,    46,    47,    48,    49,   209,    93,
      66,    93,   360,    58,    59,    33,    34,    93,    36,    37,
      38,    39,    40,    41,   283,   226,    93,   283,    73,   276,
     277,   232,   283,   280,    92,   282,   283,    93,    94,    95,
      96,   288,   289,    92,    92,    93,   102,   283,    96,   397,
      23,    92,   276,   277,    92,   302,   280,   304,   282,   283,
      68,    69,    92,    92,   288,   289,   414,    85,    86,    87,
      88,    89,    90,    92,    93,   284,    94,    96,   302,   280,
     304,   282,    60,   101,   343,    92,   287,   343,   283,    92,
      92,   283,   343,   294,   295,    92,   343,    95,   333,    95,
     335,    95,   338,    95,   305,   306,   342,   343,    95,    87,
     346,    95,   348,   349,    96,    86,    86,    93,    86,   343,
      94,    86,    86,   101,   371,   103,    86,   336,   337,    86,
      86,   340,   341,    86,    86,    86,    86,   338,   347,    33,
      34,   342,    18,    92,    38,    39,    92,   394,   343,   396,
      86,   343,    99,    92,    99,    99,    93,   366,    92,    21,
      21,    93,    93,    92,    21,    84,    94,    93,    93,    21,
      21,    92,    99,    93,    92,   376,   154,   155,   387,   380,
      86,   390,    99,   392,   393,    84,   164,   165,    85,    96,
      17,    85,    86,    87,    88,    89,    90,   102,   407,   408,
      96,    93,    95,   412,    93,    96,    96,   101,   417,   187,
     188,   189,   190,   191,   192,   193,   194,   195,   196,   197,
     198,   199,   200,    96,   202,    93,    95,    94,   206,   207,
     208,    66,    25,    26,    27,    93,    24,    30,    93,    32,
      33,    34,    95,    36,    37,    38,    39,    40,    41,    93,
      93,    93,    93,    84,    84,    94,    92,    86,    96,    93,
      93,    68,   125,   243,   327,    33,    34,   245,   246,   247,
      38,    39,    40,    41,    93,    33,    34,    -1,    36,    37,
      38,    39,    40,    41,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    85,    86,    87,    88,    89,    90,   276,   277,
      -1,    -1,    -1,    -1,    -1,   283,    -1,   285,   101,    -1,
     288,   289,   290,    -1,    -1,   293,    -1,    85,    86,    87,
      88,    89,    90,    -1,   302,    -1,   304,    85,    86,    87,
      88,    89,    90,   101,    -1,   313,    -1,    -1,    -1,    -1,
      -1,    -1,   320,   101,   102,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    33,    34,    -1,    36,   343,    38,    39,    40,    41,
       3,     4,     5,     6,     7,     8,    -1,    -1,    -1,    -1,
      -1,    -1,   360,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    -1,    16,    -1,    18,    19,    20,    -1,    22,
      -1,   389,    -1,    85,    86,    87,    88,    89,    90,   397,
      53,    54,    55,    56,    57,    58,    59,    60,    -1,   101,
      63,    64,    65,    -1,    -1,    -1,   414,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    -1,    67,    -1,    69,    70,    -1,    -1,
      73,    74,    75,    76,    77,    78,    79,    -1,    81,    -1,
      -1,    -1,    85,    -1,    -1,    -1,    -1,    -1,    -1,    92,
      -1,    -1,    -1,    -1,    97,    98,    -1,   100,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      -1,    16,    -1,    18,    19,    20,    -1,    22,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    33,
      34,    -1,    36,    37,    38,    39,    40,    41,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    -1,    67,    -1,    69,    70,    -1,    -1,    73,    74,
      75,    76,    77,    78,    79,    -1,    81,    -1,    -1,    -1,
      85,    85,    86,    87,    88,    89,    90,    92,    -1,    93,
      -1,    -1,    97,    -1,    -1,   100,    -1,   101,    33,    34,
      -1,    36,    37,    38,    39,    40,    41,    33,    34,    -1,
      36,    37,    38,    39,    40,    41,    33,    34,    -1,    36,
      37,    38,    39,    40,    41,    33,    34,    -1,    36,    37,
      38,    39,    40,    41,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      85,    86,    87,    88,    89,    90,    -1,    -1,    93,    85,
      86,    87,    88,    89,    90,    -1,   101,    93,    85,    86,
      87,    88,    89,    90,    -1,   101,    93,    85,    86,    87,
      88,    89,    90,    -1,   101,     3,     4,     5,     6,     7,
       8,     9,    -1,   101,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,     5,     6,     7,     8,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,     6,     7,     8,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    55,    56,    57,    58,    59,    60,
      -1,    -1,    63,    64,    65
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,   104,     0,    15,    69,    70,    71,    72,   105,   106,
      76,   158,    92,    92,   158,   158,    97,   110,   113,   117,
      92,     3,     4,     5,     6,     7,     8,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,   107,   109,   121,   122,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   107,    92,    92,
     111,   107,    85,    85,    92,    85,    85,    85,    85,    85,
      85,    85,    93,    94,   158,   158,    93,   107,   107,     9,
      10,    11,    12,    13,    14,    16,    18,    19,    20,    22,
      67,    69,    70,    73,    74,    75,    77,    78,    79,    81,
      85,    92,    98,   100,   112,   113,   114,   115,   116,   118,
     119,   120,   121,   123,   132,   134,   136,   137,   138,   139,
     140,   141,   142,   145,   146,   152,   154,   156,   157,   158,
      93,   158,   108,   121,   123,   132,   108,   108,   108,   108,
      53,    54,   122,   126,   122,   126,   158,   158,   107,    92,
      93,    93,    92,    92,    92,    92,   136,   137,   138,   139,
     152,   113,    23,    92,    92,    92,    92,    92,    92,   136,
     148,   152,   136,   136,    95,    95,   158,   158,   158,    95,
      25,    26,    27,    30,    32,    33,    34,    36,    37,    38,
      39,    40,    41,    85,    86,    87,    88,    89,    90,   101,
     147,    96,    99,    95,    95,    95,    92,    99,   147,    96,
      86,    86,    93,    86,    94,    86,    86,    86,    86,    86,
      86,    86,    86,    86,   158,   158,   122,   158,   136,   144,
     144,    18,    92,   158,   134,   136,   152,   153,   153,   158,
     158,   158,    86,    94,    93,    99,    99,    99,   136,   136,
     136,   136,   136,   136,   136,   136,   136,   136,   136,   136,
     136,   136,   152,   135,   136,   153,    44,    46,    47,    48,
      49,   135,   150,   151,   136,   158,    92,    92,   108,    93,
      21,   158,    21,    93,    93,    92,   158,    21,    94,    94,
      99,    93,    93,    84,    21,    21,    99,   148,   135,   135,
     135,   102,    92,    93,    92,    96,   101,   153,   153,    86,
     137,   152,   158,    99,   137,   152,   158,   112,   113,   144,
      84,   158,   153,   153,   136,   158,   158,    85,   153,   153,
     158,   158,    93,    94,    93,    94,    93,    93,    96,   135,
      93,    93,    96,    17,    93,   136,    96,    93,    96,    96,
     150,    93,    93,   102,   108,   108,   113,   113,   139,   158,
      95,   113,   113,   139,   112,    95,    93,   139,   113,   139,
     139,    94,    93,    93,    96,   143,    96,   144,   143,   113,
      66,    93,    93,   140,   149,   152,    24,    93,   158,    95,
      93,   158,    84,    84,    94,    86,    94,    92,   113,   136,
     113,    93,   113,   113,   149,   149,   144,    93,   143,    93,
     113,   113,    68,   155,    92,   113,   144,    93,   113
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,   103,   104,   104,   105,   106,   106,   106,   106,   106,
     107,   107,   108,   108,   108,   109,   109,   109,   110,   111,
     111,   112,   112,   112,   112,   112,   112,   112,   112,   112,
     112,   112,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   120,   120,   120,   120,   121,   121,   121,   121,   122,
     122,   122,   122,   122,   122,   123,   124,   124,   125,   126,
     126,   126,   126,   126,   126,   126,   126,   126,   127,   127,
     127,   128,   128,   128,   129,   130,   131,   132,   132,   132,
     133,   133,   133,   133,   133,   133,   134,   134,   135,   136,
     136,   136,   136,   136,   136,   136,   136,   136,   136,   136,
     136,   136,   136,   136,   136,   136,   136,   136,   137,   138,
     138,   139,   139,   139,   139,   140,   140,   140,   140,   140,
     140,   140,   141,   141,   142,   142,   142,   142,   142,   142,
     142,   142,   142,   142,   143,   143,   144,   145,   145,   146,
     146,   146,   146,   147,   147,   147,   147,   147,   148,   148,
     149,   149,   149,   149,   150,   151,   151,   151,   151,   151,
     152,   152,   152,   152,   153,   153,   153,   153,   153,   154,
     154,   155,   155,   156,   156,   157,   158
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     2,     5,     5,     4,     4,     5,
       1,     3,     1,     1,     1,     2,     2,     5,     1,     0,
       2,     2,     2,     2,     1,     2,     1,     1,     2,     2,
       1,     1,     1,     3,     7,    10,    10,     1,     1,     2,
       2,     4,     2,     4,     4,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       4,     4,     4,     1,     1,     1,     1,     1,     9,     7,
       4,     9,     7,     4,     4,     6,     4,     1,     1,     1,
       4,     4,     4,     4,     4,     4,     3,     3,     1,     1,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     2,     3,     1,     1,     1,     1,     4,     2,
       2,     4,     4,     4,     6,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     8,     5,     7,    10,     7,     7,
      10,     7,     7,    12,     0,     5,     1,     5,     7,     3,
       9,     3,     3,     1,     1,     1,     1,     1,     3,     1,
       3,     3,     1,     1,     4,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     0,     3,     3,     1,     1,    13,
      12,     2,     5,     3,     6,     5,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
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






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
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
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
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
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 3: /* program: program function_def  */
#line 111 "lrparser.y"
                               {/* printf("LIST SIZE %d",frontEndContext.getFuncList().size())  ;*/ }
#line 1872 "y.tab.c"
    break;

  case 4: /* function_def: function_data function_body  */
#line 113 "lrparser.y"
                                            { 
	                                          Function* func=(Function*)(yyvsp[-1].node);
                                              blockStatement* block=(blockStatement*)(yyvsp[0].node);
                                              func->setBlockStatement(block);
											   Util::addFuncToList(func);
											}
#line 1883 "y.tab.c"
    break;

  case 5: /* function_data: T_FUNC id '(' paramList ')'  */
#line 120 "lrparser.y"
                                           { 
										   (yyval.node)=Util::createFuncNode((yyvsp[-3].node),(yyvsp[-1].pList)->PList);
                                           Util::setCurrentFuncType(GEN_FUNC);
										   Util::resetTemp(tempIds);
										   tempIds.clear();
	                                      }
#line 1894 "y.tab.c"
    break;

  case 6: /* function_data: T_STATIC id '(' paramList ')'  */
#line 126 "lrparser.y"
                                                           { 
										   (yyval.node)=Util::createStaticFuncNode((yyvsp[-3].node),(yyvsp[-1].pList)->PList);
                                            Util::setCurrentFuncType(STATIC_FUNC);
											Util::resetTemp(tempIds);
											tempIds.clear();
	                                      }
#line 1905 "y.tab.c"
    break;

  case 7: /* function_data: T_INCREMENTAL '(' paramList ')'  */
#line 132 "lrparser.y"
                                                     { 
										   (yyval.node)=Util::createIncrementalNode((yyvsp[-1].pList)->PList);
                                            Util::setCurrentFuncType(INCREMENTAL_FUNC);
											Util::resetTemp(tempIds);
											tempIds.clear();
	                                      }
#line 1916 "y.tab.c"
    break;

  case 8: /* function_data: T_DECREMENTAL '(' paramList ')'  */
#line 138 "lrparser.y"
                                                             { 
										   (yyval.node)=Util::createDecrementalNode((yyvsp[-1].pList)->PList);
                                            Util::setCurrentFuncType(DECREMENTAL_FUNC);
											Util::resetTemp(tempIds);
											tempIds.clear();
	                                      }
#line 1927 "y.tab.c"
    break;

  case 9: /* function_data: T_DYNAMIC id '(' paramList ')'  */
#line 144 "lrparser.y"
                                                        { (yyval.node)=Util::createDynamicFuncNode((yyvsp[-3].node),(yyvsp[-1].pList)->PList);
                                            Util::setCurrentFuncType(DYNAMIC_FUNC);
											Util::resetTemp(tempIds);
											tempIds.clear();
											}
#line 1937 "y.tab.c"
    break;

  case 10: /* paramList: param  */
#line 151 "lrparser.y"
                 {(yyval.pList)=Util::createPList((yyvsp[0].node));}
#line 1943 "y.tab.c"
    break;

  case 11: /* paramList: param ',' paramList  */
#line 152 "lrparser.y"
                                     {(yyval.pList)=Util::addToPList((yyvsp[0].pList),(yyvsp[-2].node)); 
			                           }
#line 1950 "y.tab.c"
    break;

  case 12: /* type: type1  */
#line 155 "lrparser.y"
            {(yyval.node) = (yyvsp[0].node);}
#line 1956 "y.tab.c"
    break;

  case 13: /* type: type2  */
#line 156 "lrparser.y"
            {(yyval.node) = (yyvsp[0].node);}
#line 1962 "y.tab.c"
    break;

  case 14: /* type: type3  */
#line 157 "lrparser.y"
                {(yyval.node) = (yyvsp[0].node);}
#line 1968 "y.tab.c"
    break;

  case 15: /* param: type1 id  */
#line 159 "lrparser.y"
                 {  //Identifier* id=(Identifier*)Util::createIdentifierNode($2);
                        Type* type=(Type*)(yyvsp[-1].node);
	                     Identifier* id=(Identifier*)(yyvsp[0].node);
						 
						 if(type->isGraphType())
						    {
							 tempIds.push_back(id);
						   
							}
						 if(type->isGNNType())
						    {
							 tempIds.push_back(id);
						   
							}
					printf("\n");
                    (yyval.node)=Util::createParamNode((yyvsp[-1].node),(yyvsp[0].node)); }
#line 1989 "y.tab.c"
    break;

  case 16: /* param: type2 id  */
#line 175 "lrparser.y"
                          { // Identifier* id=(Identifier*)Util::createIdentifierNode($2);
			  
					
                             (yyval.node)=Util::createParamNode((yyvsp[-1].node),(yyvsp[0].node));}
#line 1998 "y.tab.c"
    break;

  case 17: /* param: type2 id '(' id ')'  */
#line 179 "lrparser.y"
                                                 { // Identifier* id1=(Identifier*)Util::createIdentifierNode($4);
			                            //Identifier* id=(Identifier*)Util::createIdentifierNode($2);
				                        Type* tempType=(Type*)(yyvsp[-4].node);
			                            if(tempType->isNodeEdgeType())
										  tempType->addSourceGraph((yyvsp[-1].node));
				                         (yyval.node)=Util::createParamNode(tempType,(yyvsp[-3].node));
									 }
#line 2010 "y.tab.c"
    break;

  case 18: /* function_body: blockstatements  */
#line 188 "lrparser.y"
                                {(yyval.node)=(yyvsp[0].node);}
#line 2016 "y.tab.c"
    break;

  case 19: /* statements: %empty  */
#line 191 "lrparser.y"
              {}
#line 2022 "y.tab.c"
    break;

  case 20: /* statements: statements statement  */
#line 192 "lrparser.y"
                               {printf ("found one statement\n") ; Util::addToBlock((yyvsp[0].node)); }
#line 2028 "y.tab.c"
    break;

  case 21: /* statement: declaration ';'  */
#line 194 "lrparser.y"
                          {(yyval.node)=(yyvsp[-1].node);}
#line 2034 "y.tab.c"
    break;

  case 22: /* statement: assignment ';'  */
#line 195 "lrparser.y"
                       {printf ("found an assignment type statement" ); (yyval.node)=(yyvsp[-1].node);}
#line 2040 "y.tab.c"
    break;

  case 23: /* statement: proc_call ';'  */
#line 196 "lrparser.y"
                       {printf ("found an proc call type statement" );(yyval.node)=Util::createNodeForProcCallStmt((yyvsp[-1].node));}
#line 2046 "y.tab.c"
    break;

  case 24: /* statement: control_flow  */
#line 197 "lrparser.y"
                      {printf ("found an control flow type statement" );(yyval.node)=(yyvsp[0].node);}
#line 2052 "y.tab.c"
    break;

  case 25: /* statement: reduction ';'  */
#line 198 "lrparser.y"
                      {printf ("found an reduction type statement" );(yyval.node)=(yyvsp[-1].node);}
#line 2058 "y.tab.c"
    break;

  case 26: /* statement: bfs_abstraction  */
#line 199 "lrparser.y"
                          {printf ("found bfs\n") ;(yyval.node)=(yyvsp[0].node); }
#line 2064 "y.tab.c"
    break;

  case 27: /* statement: blockstatements  */
#line 200 "lrparser.y"
                          {printf ("found block\n") ;(yyval.node)=(yyvsp[0].node);}
#line 2070 "y.tab.c"
    break;

  case 28: /* statement: unary_expr ';'  */
#line 201 "lrparser.y"
                         {printf ("found unary\n") ;(yyval.node)=Util::createNodeForUnaryStatements((yyvsp[-1].node));}
#line 2076 "y.tab.c"
    break;

  case 29: /* statement: return_stmt ';'  */
#line 202 "lrparser.y"
                          {printf ("found return\n") ;(yyval.node) = (yyvsp[-1].node) ;}
#line 2082 "y.tab.c"
    break;

  case 30: /* statement: batch_blockstmt  */
#line 203 "lrparser.y"
                           {printf ("found batch\n") ;(yyval.node) = (yyvsp[0].node);}
#line 2088 "y.tab.c"
    break;

  case 31: /* statement: on_add_blockstmt  */
#line 204 "lrparser.y"
                           {printf ("found on add block\n") ;(yyval.node) = (yyvsp[0].node);}
#line 2094 "y.tab.c"
    break;

  case 32: /* statement: on_delete_blockstmt  */
#line 205 "lrparser.y"
                              {printf ("found delete block\n") ;(yyval.node) = (yyvsp[0].node);}
#line 2100 "y.tab.c"
    break;

  case 33: /* blockstatements: block_begin statements block_end  */
#line 208 "lrparser.y"
                                                   { (yyval.node)=Util::finishBlock();}
#line 2106 "y.tab.c"
    break;

  case 34: /* batch_blockstmt: T_BATCH '(' id ':' expression ')' blockstatements  */
#line 210 "lrparser.y"
                                                                    {(yyval.node) = Util::createBatchBlockStmt((yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node));}
#line 2112 "y.tab.c"
    break;

  case 35: /* on_add_blockstmt: T_ONADD '(' id T_IN id '.' proc_call ')' ':' blockstatements  */
#line 212 "lrparser.y"
                                                                                {(yyval.node) = Util::createOnAddBlock((yyvsp[-7].node), (yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[0].node));}
#line 2118 "y.tab.c"
    break;

  case 36: /* on_delete_blockstmt: T_ONDELETE '(' id T_IN id '.' proc_call ')' ':' blockstatements  */
#line 214 "lrparser.y"
                                                                                      {(yyval.node) = Util::createOnDeleteBlock((yyvsp[-7].node), (yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[0].node));}
#line 2124 "y.tab.c"
    break;

  case 37: /* block_begin: '{'  */
#line 216 "lrparser.y"
                { Util::createNewBlock(); }
#line 2130 "y.tab.c"
    break;

  case 39: /* return_stmt: T_RETURN expression  */
#line 220 "lrparser.y"
                                  {(yyval.node) = Util::createReturnStatementNode((yyvsp[0].node));}
#line 2136 "y.tab.c"
    break;

  case 40: /* declaration: type1 id  */
#line 223 "lrparser.y"
                         {
	                     Type* type=(Type*)(yyvsp[-1].node);
	                     Identifier* id=(Identifier*)(yyvsp[0].node);
						 
						 if(type->isGraphType())
						    Util::storeGraphId(id);


                         (yyval.node)=Util::createNormalDeclNode((yyvsp[-1].node),(yyvsp[0].node));}
#line 2150 "y.tab.c"
    break;

  case 41: /* declaration: type1 id '=' rhs  */
#line 232 "lrparser.y"
                            {//Identifier* id=(Identifier*)Util::createIdentifierNode($2);
	                    
	                    (yyval.node)=Util::createAssignedDeclNode((yyvsp[-3].node),(yyvsp[-2].node),(yyvsp[0].node));}
#line 2158 "y.tab.c"
    break;

  case 42: /* declaration: type2 id  */
#line 235 "lrparser.y"
                    {//Identifier* id=(Identifier*)Util::createIdentifierNode($2);
	            
                         (yyval.node)=Util::createNormalDeclNode((yyvsp[-1].node),(yyvsp[0].node)); }
#line 2166 "y.tab.c"
    break;

  case 43: /* declaration: type2 id '=' rhs  */
#line 238 "lrparser.y"
                           {//Identifier* id=(Identifier*)Util::createIdentifierNode($2);
	                   
	                    (yyval.node)=Util::createAssignedDeclNode((yyvsp[-3].node),(yyvsp[-2].node),(yyvsp[0].node));}
#line 2174 "y.tab.c"
    break;

  case 44: /* declaration: type3 id '=' rhs  */
#line 241 "lrparser.y"
                           {//Identifier* id=(Identifier*)Util::createIdentifierNode($2);
	                   
	                    (yyval.node)=Util::createAssignedDeclNode((yyvsp[-3].node),(yyvsp[-2].node),(yyvsp[0].node));}
#line 2182 "y.tab.c"
    break;

  case 45: /* type1: primitive  */
#line 245 "lrparser.y"
                 {(yyval.node)=(yyvsp[0].node); }
#line 2188 "y.tab.c"
    break;

  case 46: /* type1: graph  */
#line 246 "lrparser.y"
                {(yyval.node)=(yyvsp[0].node);}
#line 2194 "y.tab.c"
    break;

  case 47: /* type1: collections  */
#line 247 "lrparser.y"
                      { (yyval.node)=(yyvsp[0].node);}
#line 2200 "y.tab.c"
    break;

  case 48: /* type1: gnn  */
#line 248 "lrparser.y"
              {(yyval.node)=(yyvsp[0].node);}
#line 2206 "y.tab.c"
    break;

  case 49: /* primitive: T_INT  */
#line 251 "lrparser.y"
                 { (yyval.node)=Util::createPrimitiveTypeNode(TYPE_INT);}
#line 2212 "y.tab.c"
    break;

  case 50: /* primitive: T_FLOAT  */
#line 252 "lrparser.y"
                  { (yyval.node)=Util::createPrimitiveTypeNode(TYPE_FLOAT);}
#line 2218 "y.tab.c"
    break;

  case 51: /* primitive: T_BOOL  */
#line 253 "lrparser.y"
                 { (yyval.node)=Util::createPrimitiveTypeNode(TYPE_BOOL);}
#line 2224 "y.tab.c"
    break;

  case 52: /* primitive: T_DOUBLE  */
#line 254 "lrparser.y"
                   { (yyval.node)=Util::createPrimitiveTypeNode(TYPE_DOUBLE); }
#line 2230 "y.tab.c"
    break;

  case 53: /* primitive: T_LONG  */
#line 255 "lrparser.y"
             {(yyval.node)=(yyval.node)=Util::createPrimitiveTypeNode(TYPE_LONG);}
#line 2236 "y.tab.c"
    break;

  case 54: /* primitive: T_STRING  */
#line 256 "lrparser.y"
                   {(yyval.node)=(yyval.node)=Util::createPrimitiveTypeNode(TYPE_STRING);}
#line 2242 "y.tab.c"
    break;

  case 55: /* type3: T_AUTOREF  */
#line 258 "lrparser.y"
                 { (yyval.node)=Util::createPrimitiveTypeNode(TYPE_AUTOREF);}
#line 2248 "y.tab.c"
    break;

  case 56: /* graph: T_GRAPH  */
#line 260 "lrparser.y"
                { (yyval.node)=Util::createGraphTypeNode(TYPE_GRAPH,NULL);}
#line 2254 "y.tab.c"
    break;

  case 57: /* graph: T_DIR_GRAPH  */
#line 261 "lrparser.y"
                     {(yyval.node)=Util::createGraphTypeNode(TYPE_DIRGRAPH,NULL);}
#line 2260 "y.tab.c"
    break;

  case 58: /* gnn: T_GNN  */
#line 263 "lrparser.y"
            { (yyval.node)=Util::createGNNTypeNode(TYPE_GNN,NULL);}
#line 2266 "y.tab.c"
    break;

  case 59: /* collections: T_LIST  */
#line 265 "lrparser.y"
                     { (yyval.node)=Util::createCollectionTypeNode(TYPE_LIST,NULL);}
#line 2272 "y.tab.c"
    break;

  case 60: /* collections: T_SET_NODES '<' id '>'  */
#line 266 "lrparser.y"
                                         {//Identifier* id=(Identifier*)Util::createIdentifierNode($3);
			                     (yyval.node)=Util::createCollectionTypeNode(TYPE_SETN,(yyvsp[-1].node));}
#line 2279 "y.tab.c"
    break;

  case 61: /* collections: T_SET_EDGES '<' id '>'  */
#line 268 "lrparser.y"
                                 {// Identifier* id=(Identifier*)Util::createIdentifierNode($3);
					                    (yyval.node)=Util::createCollectionTypeNode(TYPE_SETE,(yyvsp[-1].node));}
#line 2286 "y.tab.c"
    break;

  case 62: /* collections: T_UPDATES '<' id '>'  */
#line 270 "lrparser.y"
                                         { (yyval.node)=Util::createCollectionTypeNode(TYPE_UPDATES,(yyvsp[-1].node));}
#line 2292 "y.tab.c"
    break;

  case 63: /* collections: container  */
#line 271 "lrparser.y"
                        {(yyval.node) = (yyvsp[0].node);}
#line 2298 "y.tab.c"
    break;

  case 64: /* collections: vector  */
#line 272 "lrparser.y"
               {(yyval.node) = (yyvsp[0].node);}
#line 2304 "y.tab.c"
    break;

  case 65: /* collections: nodemap  */
#line 273 "lrparser.y"
                            {(yyval.node) = (yyvsp[0].node);}
#line 2310 "y.tab.c"
    break;

  case 66: /* collections: hashmap  */
#line 274 "lrparser.y"
                          {(yyval.node) = (yyvsp[0].node);}
#line 2316 "y.tab.c"
    break;

  case 67: /* collections: hashset  */
#line 275 "lrparser.y"
                      {(yyval.node) = (yyvsp[0].node);}
#line 2322 "y.tab.c"
    break;

  case 68: /* container: T_CONTAINER '<' type '>' '(' arg_list ',' type ')'  */
#line 277 "lrparser.y"
                                                               {(yyval.node) = Util::createContainerTypeNode(TYPE_CONTAINER, (yyvsp[-6].node), (yyvsp[-3].aList)->AList, (yyvsp[-1].node));}
#line 2328 "y.tab.c"
    break;

  case 69: /* container: T_CONTAINER '<' type '>' '(' arg_list ')'  */
#line 278 "lrparser.y"
                                                      { (yyval.node) =  Util::createContainerTypeNode(TYPE_CONTAINER, (yyvsp[-4].node), (yyvsp[-1].aList)->AList, NULL);}
#line 2334 "y.tab.c"
    break;

  case 70: /* container: T_CONTAINER '<' type '>'  */
#line 279 "lrparser.y"
                                     { list<argument*> argList;
			                          (yyval.node) = Util::createContainerTypeNode(TYPE_CONTAINER, (yyvsp[-1].node), argList, NULL);}
#line 2341 "y.tab.c"
    break;

  case 71: /* vector: T_VECTOR '<' type '>' '(' arg_list ',' type ')'  */
#line 282 "lrparser.y"
                                                       {(yyval.node) = Util::createContainerTypeNode(TYPE_VECTOR, (yyvsp[-6].node), (yyvsp[-3].aList)->AList, (yyvsp[-1].node));}
#line 2347 "y.tab.c"
    break;

  case 72: /* vector: T_VECTOR '<' type '>' '(' arg_list ')'  */
#line 283 "lrparser.y"
                                                  { (yyval.node) =  Util::createContainerTypeNode(TYPE_VECTOR, (yyvsp[-4].node), (yyvsp[-1].aList)->AList, NULL);}
#line 2353 "y.tab.c"
    break;

  case 73: /* vector: T_VECTOR '<' type '>'  */
#line 284 "lrparser.y"
                                 { list<argument*> argList;
			                          (yyval.node) = Util::createContainerTypeNode(TYPE_VECTOR, (yyvsp[-1].node), argList, NULL);}
#line 2360 "y.tab.c"
    break;

  case 74: /* nodemap: T_NODEMAP '(' type ')'  */
#line 286 "lrparser.y"
                                 {(yyval.node) = Util::createNodeMapTypeNode(TYPE_NODEMAP, (yyvsp[-1].node));}
#line 2366 "y.tab.c"
    break;

  case 75: /* hashmap: T_HASHMAP '<' type ',' type '>'  */
#line 288 "lrparser.y"
                                          { list<argument*> argList;
			                          (yyval.node) = Util::createHashMapTypeNode(TYPE_HASHMAP, (yyvsp[-3].node), argList, (yyvsp[-1].node));}
#line 2373 "y.tab.c"
    break;

  case 76: /* hashset: T_HASHSET '<' type '>'  */
#line 291 "lrparser.y"
                                 { list<argument*> argList;
			                          (yyval.node) = Util::createHashSetTypeNode(TYPE_HASHSET, (yyvsp[-1].node), argList, NULL);}
#line 2380 "y.tab.c"
    break;

  case 77: /* type2: T_NODE  */
#line 294 "lrparser.y"
               {(yyval.node)=Util::createNodeEdgeTypeNode(TYPE_NODE) ;}
#line 2386 "y.tab.c"
    break;

  case 78: /* type2: T_EDGE  */
#line 295 "lrparser.y"
                {(yyval.node)=Util::createNodeEdgeTypeNode(TYPE_EDGE);}
#line 2392 "y.tab.c"
    break;

  case 79: /* type2: property  */
#line 296 "lrparser.y"
                      {(yyval.node)=(yyvsp[0].node);}
#line 2398 "y.tab.c"
    break;

  case 80: /* property: T_NP '<' primitive '>'  */
#line 298 "lrparser.y"
                                  { (yyval.node)=Util::createPropertyTypeNode(TYPE_PROPNODE,(yyvsp[-1].node)); }
#line 2404 "y.tab.c"
    break;

  case 81: /* property: T_EP '<' primitive '>'  */
#line 299 "lrparser.y"
                                       { (yyval.node)=Util::createPropertyTypeNode(TYPE_PROPEDGE,(yyvsp[-1].node)); }
#line 2410 "y.tab.c"
    break;

  case 82: /* property: T_NP '<' collections '>'  */
#line 300 "lrparser.y"
                                                    {  (yyval.node)=Util::createPropertyTypeNode(TYPE_PROPNODE,(yyvsp[-1].node)); }
#line 2416 "y.tab.c"
    break;

  case 83: /* property: T_EP '<' collections '>'  */
#line 301 "lrparser.y"
                                                     {(yyval.node)=Util::createPropertyTypeNode(TYPE_PROPEDGE,(yyvsp[-1].node));}
#line 2422 "y.tab.c"
    break;

  case 84: /* property: T_NP '<' T_NODE '>'  */
#line 302 "lrparser.y"
                                    {ASTNode* type = Util::createNodeEdgeTypeNode(TYPE_NODE);
			                         (yyval.node)=Util::createPropertyTypeNode(TYPE_PROPNODE, type); }
#line 2429 "y.tab.c"
    break;

  case 85: /* property: T_NP '<' T_EDGE '>'  */
#line 304 "lrparser.y"
                                                {ASTNode* type = Util::createNodeEdgeTypeNode(TYPE_EDGE);
			                         (yyval.node)=Util::createPropertyTypeNode(TYPE_PROPNODE, type); }
#line 2436 "y.tab.c"
    break;

  case 86: /* assignment: leftSide '=' rhs  */
#line 307 "lrparser.y"
                                { printf("testassign\n");(yyval.node)=Util::createAssignmentNode((yyvsp[-2].node),(yyvsp[0].node));}
#line 2442 "y.tab.c"
    break;

  case 87: /* assignment: indexExpr '=' rhs  */
#line 308 "lrparser.y"
                                  {printf ("called assign for count\n") ; (yyval.node)=Util::createAssignmentNode((yyvsp[-2].node) , (yyvsp[0].node));}
#line 2448 "y.tab.c"
    break;

  case 88: /* rhs: expression  */
#line 311 "lrparser.y"
                 { (yyval.node)=(yyvsp[0].node);}
#line 2454 "y.tab.c"
    break;

  case 89: /* expression: proc_call  */
#line 313 "lrparser.y"
                       { (yyval.node)=(yyvsp[0].node);}
#line 2460 "y.tab.c"
    break;

  case 90: /* expression: expression '+' expression  */
#line 314 "lrparser.y"
                                         { (yyval.node)=Util::createNodeForArithmeticExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_ADD);}
#line 2466 "y.tab.c"
    break;

  case 91: /* expression: expression '-' expression  */
#line 315 "lrparser.y"
                                             { (yyval.node)=Util::createNodeForArithmeticExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_SUB);}
#line 2472 "y.tab.c"
    break;

  case 92: /* expression: expression '*' expression  */
#line 316 "lrparser.y"
                                             {(yyval.node)=Util::createNodeForArithmeticExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_MUL);}
#line 2478 "y.tab.c"
    break;

  case 93: /* expression: expression '/' expression  */
#line 317 "lrparser.y"
                                           {(yyval.node)=Util::createNodeForArithmeticExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_DIV);}
#line 2484 "y.tab.c"
    break;

  case 94: /* expression: expression T_AND_OP expression  */
#line 318 "lrparser.y"
                                              {(yyval.node)=Util::createNodeForLogicalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_AND);}
#line 2490 "y.tab.c"
    break;

  case 95: /* expression: expression T_OR_OP expression  */
#line 319 "lrparser.y"
                                                  {(yyval.node)=Util::createNodeForLogicalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_OR);}
#line 2496 "y.tab.c"
    break;

  case 96: /* expression: expression T_LE_OP expression  */
#line 320 "lrparser.y"
                                                 {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_LE);}
#line 2502 "y.tab.c"
    break;

  case 97: /* expression: expression T_GE_OP expression  */
#line 321 "lrparser.y"
                                                {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_GE);}
#line 2508 "y.tab.c"
    break;

  case 98: /* expression: expression '<' expression  */
#line 322 "lrparser.y"
                                                    {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_LT);}
#line 2514 "y.tab.c"
    break;

  case 99: /* expression: expression '>' expression  */
#line 323 "lrparser.y"
                                                    {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_GT);}
#line 2520 "y.tab.c"
    break;

  case 100: /* expression: expression T_EQ_OP expression  */
#line 324 "lrparser.y"
                                                        {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_EQ);}
#line 2526 "y.tab.c"
    break;

  case 101: /* expression: expression T_NE_OP expression  */
#line 325 "lrparser.y"
                                            {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_NE);}
#line 2532 "y.tab.c"
    break;

  case 102: /* expression: '!' expression  */
#line 326 "lrparser.y"
                                         {(yyval.node)=Util::createNodeForUnaryExpr((yyvsp[0].node),OPERATOR_NOT);}
#line 2538 "y.tab.c"
    break;

  case 103: /* expression: '(' expression ')'  */
#line 327 "lrparser.y"
                                          { Expression* expr=(Expression*)(yyvsp[-1].node);
				                     expr->setEnclosedBrackets();
			                        (yyval.node)=expr;}
#line 2546 "y.tab.c"
    break;

  case 104: /* expression: val  */
#line 330 "lrparser.y"
                       {(yyval.node)=(yyvsp[0].node);}
#line 2552 "y.tab.c"
    break;

  case 105: /* expression: leftSide  */
#line 331 "lrparser.y"
                                    { (yyval.node)=Util::createNodeForId((yyvsp[0].node));}
#line 2558 "y.tab.c"
    break;

  case 106: /* expression: unary_expr  */
#line 332 "lrparser.y"
                                      {(yyval.node)=(yyvsp[0].node);}
#line 2564 "y.tab.c"
    break;

  case 107: /* expression: indexExpr  */
#line 333 "lrparser.y"
                                     {(yyval.node) = (yyvsp[0].node);}
#line 2570 "y.tab.c"
    break;

  case 108: /* indexExpr: expression '[' expression ']'  */
#line 335 "lrparser.y"
                                          {printf("first done this \n");(yyval.node) = Util::createNodeForIndexExpr((yyvsp[-3].node), (yyvsp[-1].node), OPERATOR_INDEX);}
#line 2576 "y.tab.c"
    break;

  case 109: /* unary_expr: expression T_INC_OP  */
#line 337 "lrparser.y"
                                   {(yyval.node)=Util::createNodeForUnaryExpr((yyvsp[-1].node),OPERATOR_INC);}
#line 2582 "y.tab.c"
    break;

  case 110: /* unary_expr: expression T_DEC_OP  */
#line 338 "lrparser.y"
                                                {(yyval.node)=Util::createNodeForUnaryExpr((yyvsp[-1].node),OPERATOR_DEC);}
#line 2588 "y.tab.c"
    break;

  case 111: /* proc_call: leftSide '(' arg_list ')'  */
#line 340 "lrparser.y"
                                      { 
                                       
                                       (yyval.node) = Util::createNodeForProcCall((yyvsp[-3].node),(yyvsp[-1].aList)->AList,NULL); 

									    }
#line 2598 "y.tab.c"
    break;

  case 112: /* proc_call: T_INCREMENTAL '(' arg_list ')'  */
#line 345 "lrparser.y"
                                                         { ASTNode* id = Util::createIdentifierNode("Incremental");
			                                   (yyval.node) = Util::createNodeForProcCall(id, (yyvsp[-1].aList)->AList,NULL); 

				                               }
#line 2607 "y.tab.c"
    break;

  case 113: /* proc_call: T_DECREMENTAL '(' arg_list ')'  */
#line 349 "lrparser.y"
                                                         { ASTNode* id = Util::createIdentifierNode("Decremental");
			                                   (yyval.node) = Util::createNodeForProcCall(id, (yyvsp[-1].aList)->AList,NULL); 

				                               }
#line 2616 "y.tab.c"
    break;

  case 114: /* proc_call: indexExpr '.' leftSide '(' arg_list ')'  */
#line 353 "lrparser.y"
                                                                  {
                                                   
													 Expression* expr = (Expression*)(yyvsp[-5].node);
                                                     (yyval.node) = Util::createNodeForProcCall((yyvsp[-3].node) , (yyvsp[-1].aList)->AList, expr); 

									                 }
#line 2627 "y.tab.c"
    break;

  case 115: /* val: INT_NUM  */
#line 364 "lrparser.y"
              { (yyval.node) = Util::createNodeForIval((yyvsp[0].ival)); }
#line 2633 "y.tab.c"
    break;

  case 116: /* val: FLOAT_NUM  */
#line 365 "lrparser.y"
                    {(yyval.node) = Util::createNodeForFval((yyvsp[0].fval));}
#line 2639 "y.tab.c"
    break;

  case 117: /* val: BOOL_VAL  */
#line 366 "lrparser.y"
                   { (yyval.node) = Util::createNodeForBval((yyvsp[0].bval));}
#line 2645 "y.tab.c"
    break;

  case 118: /* val: STRING_VAL  */
#line 367 "lrparser.y"
                     { (yyval.node) = Util::createNodeForSval((yyvsp[0].text));}
#line 2651 "y.tab.c"
    break;

  case 119: /* val: T_INF  */
#line 368 "lrparser.y"
                {(yyval.node)=Util::createNodeForINF(true);}
#line 2657 "y.tab.c"
    break;

  case 120: /* val: T_P_INF  */
#line 369 "lrparser.y"
                  {(yyval.node)=Util::createNodeForINF(true);}
#line 2663 "y.tab.c"
    break;

  case 121: /* val: T_N_INF  */
#line 370 "lrparser.y"
                  {(yyval.node)=Util::createNodeForINF(false);}
#line 2669 "y.tab.c"
    break;

  case 122: /* control_flow: selection_cf  */
#line 373 "lrparser.y"
                            { (yyval.node)=(yyvsp[0].node); }
#line 2675 "y.tab.c"
    break;

  case 123: /* control_flow: iteration_cf  */
#line 374 "lrparser.y"
                             { (yyval.node)=(yyvsp[0].node); }
#line 2681 "y.tab.c"
    break;

  case 124: /* iteration_cf: T_FIXEDPOINT T_UNTIL '(' id ':' expression ')' blockstatements  */
#line 376 "lrparser.y"
                                                                              { (yyval.node)=Util::createNodeForFixedPointStmt((yyvsp[-4].node),(yyvsp[-2].node),(yyvsp[0].node));}
#line 2687 "y.tab.c"
    break;

  case 125: /* iteration_cf: T_WHILE '(' boolean_expr ')' blockstatements  */
#line 377 "lrparser.y"
                                                                 {(yyval.node)=Util::createNodeForWhileStmt((yyvsp[-2].node),(yyvsp[0].node)); }
#line 2693 "y.tab.c"
    break;

  case 126: /* iteration_cf: T_DO blockstatements T_WHILE '(' boolean_expr ')' ';'  */
#line 378 "lrparser.y"
                                                                           {(yyval.node)=Util::createNodeForDoWhileStmt((yyvsp[-2].node),(yyvsp[-5].node));  }
#line 2699 "y.tab.c"
    break;

  case 127: /* iteration_cf: T_FORALL '(' id T_IN id '.' proc_call filterExpr ')' blockstatements  */
#line 379 "lrparser.y"
                                                                                       { 
																				(yyval.node)=Util::createNodeForForAllStmt((yyvsp[-7].node),(yyvsp[-5].node),(yyvsp[-3].node),(yyvsp[-2].node),(yyvsp[0].node),true);}
#line 2706 "y.tab.c"
    break;

  case 128: /* iteration_cf: T_FORALL '(' id T_IN leftSide ')' blockstatements  */
#line 381 "lrparser.y"
                                                                        { (yyval.node)=Util::createNodeForForStmt((yyvsp[-4].node),(yyvsp[-2].node),(yyvsp[0].node),true);}
#line 2712 "y.tab.c"
    break;

  case 129: /* iteration_cf: T_FOR '(' id T_IN leftSide ')' blockstatements  */
#line 382 "lrparser.y"
                                                                 { (yyval.node)=Util::createNodeForForStmt((yyvsp[-4].node),(yyvsp[-2].node),(yyvsp[0].node),false);}
#line 2718 "y.tab.c"
    break;

  case 130: /* iteration_cf: T_FOR '(' id T_IN id '.' proc_call filterExpr ')' blockstatements  */
#line 383 "lrparser.y"
                                                                                    {(yyval.node)=Util::createNodeForForAllStmt((yyvsp[-7].node),(yyvsp[-5].node),(yyvsp[-3].node),(yyvsp[-2].node),(yyvsp[0].node),false);}
#line 2724 "y.tab.c"
    break;

  case 131: /* iteration_cf: T_FOR '(' id T_IN indexExpr ')' blockstatements  */
#line 384 "lrparser.y"
                                                                  {(yyval.node) = Util::createNodeForForStmt((yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node), false);}
#line 2730 "y.tab.c"
    break;

  case 132: /* iteration_cf: T_FORALL '(' id T_IN indexExpr ')' blockstatements  */
#line 385 "lrparser.y"
                                                                     {(yyval.node) = Util::createNodeForForStmt((yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node), true);}
#line 2736 "y.tab.c"
    break;

  case 133: /* iteration_cf: T_FOR '(' primitive id '=' rhs ';' boolean_expr ';' expression ')' blockstatements  */
#line 387 "lrparser.y"
                                                                                                     {(yyval.node) = Util::createNodeForSimpleForStmt((yyvsp[-9].node), (yyvsp[-8].node), (yyvsp[-6].node), (yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2742 "y.tab.c"
    break;

  case 134: /* filterExpr: %empty  */
#line 389 "lrparser.y"
                      { (yyval.node)=NULL;}
#line 2748 "y.tab.c"
    break;

  case 135: /* filterExpr: '.' T_FILTER '(' boolean_expr ')'  */
#line 390 "lrparser.y"
                                              { (yyval.node)=(yyvsp[-1].node);}
#line 2754 "y.tab.c"
    break;

  case 136: /* boolean_expr: expression  */
#line 392 "lrparser.y"
                          { (yyval.node)=(yyvsp[0].node) ;}
#line 2760 "y.tab.c"
    break;

  case 137: /* selection_cf: T_IF '(' boolean_expr ')' statement  */
#line 394 "lrparser.y"
                                                   { (yyval.node)=Util::createNodeForIfStmt((yyvsp[-2].node),(yyvsp[0].node),NULL); }
#line 2766 "y.tab.c"
    break;

  case 138: /* selection_cf: T_IF '(' boolean_expr ')' statement T_ELSE statement  */
#line 395 "lrparser.y"
                                                                           {(yyval.node)=Util::createNodeForIfStmt((yyvsp[-4].node),(yyvsp[-2].node),(yyvsp[0].node)); }
#line 2772 "y.tab.c"
    break;

  case 139: /* reduction: leftSide '=' reductionCall  */
#line 398 "lrparser.y"
                                       { (yyval.node)=Util::createNodeForReductionStmt((yyvsp[-2].node),(yyvsp[0].node)) ;}
#line 2778 "y.tab.c"
    break;

  case 140: /* reduction: '<' leftList '>' '=' '<' reductionCall ',' rightList '>'  */
#line 399 "lrparser.y"
                                                                              { reductionCall* reduc=(reductionCall*)(yyvsp[-3].node);
		                                                               (yyval.node)=Util::createNodeForReductionStmtList((yyvsp[-7].nodeList)->ASTNList,reduc,(yyvsp[-1].nodeList)->ASTNList);}
#line 2785 "y.tab.c"
    break;

  case 141: /* reduction: leftSide reduce_op expression  */
#line 401 "lrparser.y"
                                                   {(yyval.node)=Util::createNodeForReductionOpStmt((yyvsp[-2].node),(yyvsp[-1].ival),(yyvsp[0].node));}
#line 2791 "y.tab.c"
    break;

  case 142: /* reduction: expression reduce_op expression  */
#line 402 "lrparser.y"
                                         {printf ("here calling creation for red op\n") ;(yyval.node)=Util::createNodeForReductionOpStmt ((yyvsp[-2].node),(yyvsp[-1].ival),(yyvsp[0].node));}
#line 2797 "y.tab.c"
    break;

  case 143: /* reduce_op: T_ADD_ASSIGN  */
#line 405 "lrparser.y"
                         {(yyval.ival)=OPERATOR_ADDASSIGN;}
#line 2803 "y.tab.c"
    break;

  case 144: /* reduce_op: T_MUL_ASSIGN  */
#line 406 "lrparser.y"
                         {(yyval.ival)=OPERATOR_MULASSIGN;}
#line 2809 "y.tab.c"
    break;

  case 145: /* reduce_op: T_OR_ASSIGN  */
#line 407 "lrparser.y"
                                 {(yyval.ival)=OPERATOR_ORASSIGN;}
#line 2815 "y.tab.c"
    break;

  case 146: /* reduce_op: T_AND_ASSIGN  */
#line 408 "lrparser.y"
                                 {(yyval.ival)=OPERATOR_ANDASSIGN;}
#line 2821 "y.tab.c"
    break;

  case 147: /* reduce_op: T_SUB_ASSIGN  */
#line 409 "lrparser.y"
                                 {(yyval.ival)=OPERATOR_SUBASSIGN;}
#line 2827 "y.tab.c"
    break;

  case 148: /* leftList: leftSide ',' leftList  */
#line 411 "lrparser.y"
                                  { (yyval.nodeList)=Util::addToNList((yyvsp[0].nodeList),(yyvsp[-2].node));
                                         }
#line 2834 "y.tab.c"
    break;

  case 149: /* leftList: leftSide  */
#line 413 "lrparser.y"
                           { (yyval.nodeList)=Util::createNList((yyvsp[0].node));;}
#line 2840 "y.tab.c"
    break;

  case 150: /* rightList: val ',' rightList  */
#line 415 "lrparser.y"
                              { (yyval.nodeList)=Util::addToNList((yyvsp[0].nodeList),(yyvsp[-2].node));}
#line 2846 "y.tab.c"
    break;

  case 151: /* rightList: leftSide ',' rightList  */
#line 416 "lrparser.y"
                                   { ASTNode* node = Util::createNodeForId((yyvsp[-2].node));
			                         (yyval.nodeList)=Util::addToNList((yyvsp[0].nodeList),node);}
#line 2853 "y.tab.c"
    break;

  case 152: /* rightList: val  */
#line 418 "lrparser.y"
                   { (yyval.nodeList)=Util::createNList((yyvsp[0].node));}
#line 2859 "y.tab.c"
    break;

  case 153: /* rightList: leftSide  */
#line 419 "lrparser.y"
                              { ASTNode* node = Util::createNodeForId((yyvsp[0].node));
			            (yyval.nodeList)=Util::createNList(node);}
#line 2866 "y.tab.c"
    break;

  case 154: /* reductionCall: reduction_calls '(' arg_list ')'  */
#line 428 "lrparser.y"
                                                 {(yyval.node)=Util::createNodeforReductionCall((yyvsp[-3].ival),(yyvsp[-1].aList)->AList);}
#line 2872 "y.tab.c"
    break;

  case 155: /* reduction_calls: T_SUM  */
#line 430 "lrparser.y"
                        { (yyval.ival)=REDUCE_SUM;}
#line 2878 "y.tab.c"
    break;

  case 156: /* reduction_calls: T_COUNT  */
#line 431 "lrparser.y"
                           {(yyval.ival)=REDUCE_COUNT;}
#line 2884 "y.tab.c"
    break;

  case 157: /* reduction_calls: T_PRODUCT  */
#line 432 "lrparser.y"
                             {(yyval.ival)=REDUCE_PRODUCT;}
#line 2890 "y.tab.c"
    break;

  case 158: /* reduction_calls: T_MAX  */
#line 433 "lrparser.y"
                         {(yyval.ival)=REDUCE_MAX;}
#line 2896 "y.tab.c"
    break;

  case 159: /* reduction_calls: T_MIN  */
#line 434 "lrparser.y"
                         {(yyval.ival)=REDUCE_MIN;}
#line 2902 "y.tab.c"
    break;

  case 160: /* leftSide: id  */
#line 436 "lrparser.y"
              { (yyval.node)=(yyvsp[0].node); }
#line 2908 "y.tab.c"
    break;

  case 161: /* leftSide: oid  */
#line 437 "lrparser.y"
               { printf("Here hello \n"); (yyval.node)=(yyvsp[0].node); }
#line 2914 "y.tab.c"
    break;

  case 162: /* leftSide: tid  */
#line 438 "lrparser.y"
               {(yyval.node) = (yyvsp[0].node); }
#line 2920 "y.tab.c"
    break;

  case 163: /* leftSide: indexExpr  */
#line 439 "lrparser.y"
                    {(yyval.node)=(yyvsp[0].node);}
#line 2926 "y.tab.c"
    break;

  case 164: /* arg_list: %empty  */
#line 442 "lrparser.y"
              {
                 argList* aList=new argList();
				 (yyval.aList)=aList;  }
#line 2934 "y.tab.c"
    break;

  case 165: /* arg_list: assignment ',' arg_list  */
#line 446 "lrparser.y"
                                         {argument* a1=new argument();
		                          assignment* assign=(assignment*)(yyvsp[-2].node);
		                     a1->setAssign(assign);
							 a1->setAssignFlag();
		                 //a1->assignExpr=(assignment*)$1;
						 // a1->assign=true;
						  (yyval.aList)=Util::addToAList((yyvsp[0].aList),a1);
						  /*
						  for(argument* arg:$$->AList)
						  {
							  printf("VALUE OF ARG %d",arg->getAssignExpr()); //rm for warnings
						  }
						  */ 
						  
                          }
#line 2954 "y.tab.c"
    break;

  case 166: /* arg_list: expression ',' arg_list  */
#line 463 "lrparser.y"
                                             {argument* a1=new argument();
		                                Expression* expr=(Expression*)(yyvsp[-2].node);
										a1->setExpression(expr);
										a1->setExpressionFlag();
						               // a1->expressionflag=true;
										 (yyval.aList)=Util::addToAList((yyvsp[0].aList),a1);
						                }
#line 2966 "y.tab.c"
    break;

  case 167: /* arg_list: expression  */
#line 470 "lrparser.y"
                            {argument* a1=new argument();
		                 Expression* expr=(Expression*)(yyvsp[0].node);
						 a1->setExpression(expr);
						a1->setExpressionFlag();
						  (yyval.aList)=Util::createAList(a1); }
#line 2976 "y.tab.c"
    break;

  case 168: /* arg_list: assignment  */
#line 475 "lrparser.y"
                            { argument* a1=new argument();
		                   assignment* assign=(assignment*)(yyvsp[0].node);
		                     a1->setAssign(assign);
							 a1->setAssignFlag();
						   (yyval.aList)=Util::createAList(a1);
						   }
#line 2987 "y.tab.c"
    break;

  case 169: /* bfs_abstraction: T_BFS '(' id T_IN id '.' proc_call T_FROM id ')' filterExpr blockstatements reverse_abstraction  */
#line 483 "lrparser.y"
                                                                                                                 {(yyval.node)=Util::createIterateInBFSNode((yyvsp[-10].node),(yyvsp[-8].node),(yyvsp[-6].node),(yyvsp[-4].node),(yyvsp[-2].node),(yyvsp[-1].node),(yyvsp[0].node)) ;}
#line 2993 "y.tab.c"
    break;

  case 170: /* bfs_abstraction: T_BFS '(' id T_IN id '.' proc_call T_FROM id ')' filterExpr blockstatements  */
#line 484 "lrparser.y"
                                                                                                      {(yyval.node)=Util::createIterateInBFSNode((yyvsp[-9].node),(yyvsp[-7].node),(yyvsp[-5].node),(yyvsp[-3].node),(yyvsp[-1].node),(yyvsp[0].node),NULL) ; }
#line 2999 "y.tab.c"
    break;

  case 171: /* reverse_abstraction: T_REVERSE blockstatements  */
#line 488 "lrparser.y"
                                                 {(yyval.node)=Util::createIterateInReverseBFSNode(NULL,(yyvsp[0].node));}
#line 3005 "y.tab.c"
    break;

  case 172: /* reverse_abstraction: T_REVERSE '(' boolean_expr ')' blockstatements  */
#line 489 "lrparser.y"
                                                                       {(yyval.node)=Util::createIterateInReverseBFSNode((yyvsp[-2].node),(yyvsp[0].node));}
#line 3011 "y.tab.c"
    break;

  case 173: /* oid: id '.' id  */
#line 492 "lrparser.y"
                 { //Identifier* id1=(Identifier*)Util::createIdentifierNode($1);
                  // Identifier* id2=(Identifier*)Util::createIdentifierNode($1);
				   (yyval.node) = Util::createPropIdNode((yyvsp[-2].node),(yyvsp[0].node));
				    }
#line 3020 "y.tab.c"
    break;

  case 174: /* oid: id '.' id '[' id ']'  */
#line 496 "lrparser.y"
                                { ASTNode* expr1 = Util::createNodeForId((yyvsp[-3].node));
	                          ASTNode* expr2 = Util::createNodeForId((yyvsp[-1].node));
							  ASTNode* indexexpr =  Util::createNodeForIndexExpr(expr1, expr2, OPERATOR_INDEX);
	                          (yyval.node) = Util::createPropIdNode((yyvsp[-5].node) , indexexpr);}
#line 3029 "y.tab.c"
    break;

  case 175: /* tid: id '.' id '.' id  */
#line 503 "lrparser.y"
                       {// Identifier* id1=(Identifier*)Util::createIdentifierNode($1);
                  // Identifier* id2=(Identifier*)Util::createIdentifierNode($1);
				   (yyval.node)=Util::createPropIdNode((yyvsp[-4].node),(yyvsp[-2].node));
				    }
#line 3038 "y.tab.c"
    break;

  case 176: /* id: ID  */
#line 507 "lrparser.y"
          { 
	         (yyval.node)=Util::createIdentifierNode((yyvsp[0].text));  

            
            }
#line 3048 "y.tab.c"
    break;


#line 3052 "y.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
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

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
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
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 515 "lrparser.y"


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
