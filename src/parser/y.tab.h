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
/* Line 1529 of yacc.c.  */
#line 225 "y.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

