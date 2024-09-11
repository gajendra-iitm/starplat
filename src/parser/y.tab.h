/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

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
     

#line 246 "y.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
