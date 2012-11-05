/* Driver template for the LEMON parser generator.
** The author disclaims copyright to this source code.
*/
/* First off, code is included that follows the "include" declaration
** in the input grammar file. */
#include "ekTypes.h"
#include <stdio.h>
#line 22 "/home/joe/private/work/eureka/eureka/core/ekParser.y"

    #include "ekCompiler.h"
    #include "ekLexer.h"
    #include "ekParser.h"
    #include "ekSyntax.h"
    #include "ekContext.h"

    #include <string.h>
    #include <stdlib.h>

    #undef assert
    #define assert(ignoring_this_function)
    
#ifdef EUREKA_TRACE_PARSER
    #undef NDEBUG
#endif
#line 26 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
/* Next is all token values, in a form suitable for use by makeheaders.
** This section will be null unless lemon is run with the -m switch.
*/
/* 
** These constants (all generated automatically by the parser generator)
** specify the various kinds of tokens (terminals) that the parser
** understands. 
**
** Each symbol here is a terminal symbol in the grammar.
*/
/* Make sure the INTERFACE macro is defined.
*/
#ifndef INTERFACE
# define INTERFACE 1
#endif
/* The next thing included is series of defines which control
** various aspects of the generated parser.
**    YYCODETYPE         is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 terminals
**                       and nonterminals.  "int" is used otherwise.
**    YYNOCODE           is a number of type YYCODETYPE which corresponds
**                       to no legal terminal or nonterminal number.  This
**                       number is used to fill in empty slots of the hash 
**                       table.
**    YYFALLBACK         If defined, this indicates that one or more tokens
**                       have fall-back values which should be used if the
**                       original value of the token will not parse.
**    YYACTIONTYPE       is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 rules and
**                       states combined.  "int" is used otherwise.
**    ekParseTOKENTYPE     is the data type used for minor tokens given 
**                       directly to the parser from the tokenizer.
**    YYMINORTYPE        is the data type used for all minor tokens.
**                       This is typically a union of many types, one of
**                       which is ekParseTOKENTYPE.  The entry in the union
**                       for base tokens is called "yy0".
**    YYSTACKDEPTH       is the maximum depth of the parser's stack.  If
**                       zero the stack is dynamically sized using realloc()
**    ekParseARG_SDECL     A static variable declaration for the %extra_argument
**    ekParseARG_PDECL     A parameter declaration for the %extra_argument
**    ekParseARG_STORE     Code to store %extra_argument into yypParser
**    ekParseARG_FETCH     Code to extract %extra_argument from yypParser
**    YYNSTATE           the combined number of states.
**    YYNRULE            the number of rules in the grammar
**    YYERRORSYMBOL      is the code number of the error symbol.  If not
**                       defined, then do no error processing.
*/
#define YYCODETYPE unsigned char
#define YYNOCODE 87
#define YYACTIONTYPE unsigned char
#define ekParseTOKENTYPE  ekToken 
typedef union {
  int yyinit;
  ekParseTOKENTYPE yy0;
  ekSyntax* yy22;
  ekToken yy122;
  int yy173;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define ekParseARG_SDECL  ekCompiler *C ;
#define ekParseARG_PDECL , ekCompiler *C 
#define ekParseARG_FETCH  ekCompiler *C  = yypParser->C 
#define ekParseARG_STORE yypParser->C  = C 
#define YYNSTATE 166
#define YYNRULE 82
#define YYERRORSYMBOL 74
#define YYERRSYMDT yy173
#define YYFALLBACK 1
#define YY_NO_ACTION      (YYNSTATE+YYNRULE+2)
#define YY_ACCEPT_ACTION  (YYNSTATE+YYNRULE+1)
#define YY_ERROR_ACTION   (YYNSTATE+YYNRULE)

/* The yyzerominor constant is used to initialize instances of
** YYMINORTYPE objects to zero. */
static const YYMINORTYPE yyzerominor = { 0 };

/* Define the yytestcase() macro to be a no-op if is not already defined
** otherwise.
**
** Applications can choose to define yytestcase() in the %include section
** to a macro that can assist in verifying code coverage.  For production
** code the yytestcase() macro should be turned off.  But it is useful
** for testing.
*/
#ifndef yytestcase
# define yytestcase(X)
#endif


/* Next are the tables used to determine what action to take based on the
** current state and lookahead token.  These tables are used to implement
** functions that take a state number and lookahead value and return an
** action integer.  
**
** Suppose the action integer is N.  Then the action is determined as
** follows
**
**   0 <= N < YYNSTATE                  Shift N.  That is, push the lookahead
**                                      token onto the stack and goto state N.
**
**   YYNSTATE <= N < YYNSTATE+YYNRULE   Reduce by rule N-YYNSTATE.
**
**   N == YYNSTATE+YYNRULE              A syntax error has occurred.
**
**   N == YYNSTATE+YYNRULE+1            The parser accepts its input.
**
**   N == YYNSTATE+YYNRULE+2            No such action.  Denotes unused
**                                      slots in the yy_action[] table.
**
** The action table is constructed as a single large table named yy_action[].
** Given state S and lookahead X, the action is computed as
**
**      yy_action[ yy_shift_ofst[S] + X ]
**
** If the index value yy_shift_ofst[S]+X is out of range or if the value
** yy_lookahead[yy_shift_ofst[S]+X] is not equal to X or if yy_shift_ofst[S]
** is equal to YY_SHIFT_USE_DFLT, it means that the action is not in the table
** and that yy_default[S] should be used instead.  
**
** The formula above is for computing the action when the lookahead is
** a terminal symbol.  If the lookahead is a non-terminal (as occurs after
** a reduce action) then the yy_reduce_ofst[] array is used in place of
** the yy_shift_ofst[] array and YY_REDUCE_USE_DFLT is used in place of
** YY_SHIFT_USE_DFLT.
**
** The following are the tables generated in this section:
**
**  yy_action[]        A single table containing all actions.
**  yy_lookahead[]     A table containing the lookahead for each entry in
**                     yy_action.  Used to detect hash collisions.
**  yy_shift_ofst[]    For each state, the offset into yy_action for
**                     shifting terminals.
**  yy_reduce_ofst[]   For each state, the offset into yy_action for
**                     shifting non-terminals after a reduce.
**  yy_default[]       Default action for each state.
*/
static const YYACTIONTYPE yy_action[] = {
 /*     0 */    17,   59,    8,    4,  102,   43,  112,  121,   96,  124,
 /*    10 */    21,   15,   66,  110,   57,  108,  149,  122,   50,   52,
 /*    20 */    51,   54,   53,   45,   49,   47,   48,  164,   24,   13,
 /*    30 */   163,  110,   57,  115,  121,   22,   20,   34,   25,   35,
 /*    40 */   130,  131,   28,   29,   27,   32,   33,   30,   31,   40,
 /*    50 */    38,   37,   19,   23,   24,   13,   44,  117,  153,   12,
 /*    60 */    16,  103,  150,  144,  116,  146,   18,   39,  129,  132,
 /*    70 */   133,  134,  105,   21,   63,  110,   57,   41,   42,  120,
 /*    80 */   249,    9,  156,  128,   98,   21,   18,   39,   66,   89,
 /*    90 */    57,   21,   95,  110,   57,  127,   21,  142,   26,   36,
 /*   100 */    34,   25,   35,   18,   39,   28,   29,   27,   32,   33,
 /*   110 */    30,   31,   40,   38,   37,   19,   23,   24,   13,   21,
 /*   120 */    81,  110,   57,   41,   40,   38,   37,   19,   23,   24,
 /*   130 */    13,   38,   37,   19,   23,   24,   13,   37,   19,   23,
 /*   140 */    24,   13,  104,  147,   26,   36,   34,   25,   35,   13,
 /*   150 */   106,   28,   29,   27,   32,   33,   30,   31,   40,   38,
 /*   160 */    37,   19,   23,   24,   13,   21,  126,  148,   97,   41,
 /*   170 */    18,   39,   66,  110,   57,  145,  120,    7,  117,  137,
 /*   180 */   113,   98,   43,   55,   21,   66,   89,   57,   21,    3,
 /*   190 */    26,   36,   34,   25,   35,   56,    5,   28,   29,   27,
 /*   200 */    32,   33,   30,   31,   40,   38,   37,   19,   23,   24,
 /*   210 */    13,   21,  100,  110,   57,   41,  143,   29,   27,   32,
 /*   220 */    33,   30,   31,   40,   38,   37,   19,   23,   24,   13,
 /*   230 */    19,   23,   24,   13,    6,  123,   26,   36,   34,   25,
 /*   240 */    35,  107,   58,   28,   29,   27,   32,   33,   30,   31,
 /*   250 */    40,   38,   37,   19,   23,   24,   13,   17,  136,    8,
 /*   260 */     4,  102,  250,   21,  146,    1,   18,   39,   15,   66,
 /*   270 */   110,   57,  108,  149,   17,  151,    8,  120,  102,  152,
 /*   280 */   137,  139,   98,  147,  155,   15,   66,   89,   57,  108,
 /*   290 */   149,  158,   22,   20,  159,   28,   29,   27,   32,   33,
 /*   300 */    30,   31,   40,   38,   37,   19,   23,   24,   13,   22,
 /*   310 */    20,  165,  250,   14,  250,  153,   12,   16,  103,  150,
 /*   320 */   250,  116,   82,  110,   57,  129,  132,  133,  134,  105,
 /*   330 */   250,  160,  153,   12,   16,  103,  150,   17,  116,    8,
 /*   340 */    21,  102,  129,  132,  133,  134,  105,  250,   15,  101,
 /*   350 */   110,   57,  108,  149,  166,   17,    2,    8,  250,  102,
 /*   360 */    66,  110,   57,  118,  119,  250,   15,   46,  250,  250,
 /*   370 */   108,  149,   22,   20,   27,   32,   33,   30,   31,   40,
 /*   380 */    38,   37,   19,   23,   24,   13,  250,   21,  250,  250,
 /*   390 */    22,   20,  250,  250,  157,  153,   12,   16,  103,  150,
 /*   400 */   250,  116,  109,  110,   57,  129,  132,  133,  134,  105,
 /*   410 */    80,  110,   57,  153,   12,   16,  103,  150,   17,  116,
 /*   420 */     8,   21,  102,  129,  132,  133,  134,  105,  250,   15,
 /*   430 */    23,   24,   13,  108,  149,   17,   99,    8,  250,  102,
 /*   440 */    66,  110,   57,   77,  110,   57,   15,   86,  110,   57,
 /*   450 */   108,  149,   55,   22,   20,  250,   32,   33,   30,   31,
 /*   460 */    40,   38,   37,   19,   23,   24,   13,   84,  110,   57,
 /*   470 */    22,   20,   23,   24,   13,  135,  153,   12,   16,  103,
 /*   480 */   150,  250,  116,   21,  250,  250,  129,  132,  133,  134,
 /*   490 */   105,  250,  154,  153,   12,   16,  103,  150,  250,  116,
 /*   500 */   250,  250,  250,  129,  132,  133,  134,  105,   26,   36,
 /*   510 */    34,   25,   35,  114,   21,   28,   29,   27,   32,   33,
 /*   520 */    30,   31,   40,   38,   37,   19,   23,   24,   13,   17,
 /*   530 */   250,    8,  250,  102,   85,  110,   57,  250,   21,  250,
 /*   540 */    15,  250,   25,   35,  108,  149,   28,   29,   27,   32,
 /*   550 */    33,   30,   31,   40,   38,   37,   19,   23,   24,   13,
 /*   560 */   250,   90,  110,   57,   22,   20,   21,   35,  250,  250,
 /*   570 */    28,   29,   27,   32,   33,   30,   31,   40,   38,   37,
 /*   580 */    19,   23,   24,   13,  250,  250,  250,  153,   12,   16,
 /*   590 */   103,  150,  250,  116,   21,  250,  250,  129,  132,  133,
 /*   600 */   134,  105,   33,   30,   31,   40,   38,   37,   19,   23,
 /*   610 */    24,   13,   91,  110,   57,  250,  250,   87,  110,   57,
 /*   620 */    36,   34,   25,   35,  250,  250,   28,   29,   27,   32,
 /*   630 */    33,   30,   31,   40,   38,   37,   19,   23,   24,   13,
 /*   640 */    17,   14,   21,  250,  102,   88,  110,   57,  250,  250,
 /*   650 */   120,  250,  250,  137,  161,   98,  149,  250,   17,   66,
 /*   660 */    89,   57,  102,   79,  110,   57,   83,  110,   57,  250,
 /*   670 */   250,  250,  250,  250,  149,   22,   20,  250,  250,   30,
 /*   680 */    31,   40,   38,   37,   19,   23,   24,   13,   78,  110,
 /*   690 */    57,  250,  250,   22,   20,   94,  110,   57,  250,  250,
 /*   700 */    21,  111,  150,   93,  110,   57,  250,  250,  129,  132,
 /*   710 */   133,  134,  105,   64,  110,   57,   92,  110,   57,  111,
 /*   720 */   150,  162,  250,   60,  110,   57,  129,  132,  133,  134,
 /*   730 */   105,   17,  250,  250,  250,  102,  250,  250,   31,   40,
 /*   740 */    38,   37,   19,   23,   24,   13,  250,  149,  250,  120,
 /*   750 */   250,   10,  156,  250,   98,  250,  250,  250,   66,   89,
 /*   760 */    57,  250,  250,   67,  110,   57,   22,   20,  250,  120,
 /*   770 */   250,  250,  137,  138,   98,  250,  250,  250,   66,   89,
 /*   780 */    57,   65,  110,   57,  250,  120,  250,  250,  137,  140,
 /*   790 */    98,  250,  111,  150,   66,   89,   57,  250,  250,  129,
 /*   800 */   132,  133,  134,  105,  250,  120,  250,  250,  137,  141,
 /*   810 */    98,   61,  110,   57,   66,   89,   57,  120,  250,   11,
 /*   820 */   156,  250,   98,   68,  110,   57,   66,   89,   57,  120,
 /*   830 */   250,  250,  125,  250,   98,   62,  110,   57,   66,   89,
 /*   840 */    57,  250,   69,  110,   57,  250,  250,   70,  110,   57,
 /*   850 */    71,  110,   57,   72,  110,   57,   73,  110,   57,  250,
 /*   860 */   250,  250,   74,  110,   57,   75,  110,   57,  250,   76,
 /*   870 */   110,   57,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */     1,    1,    3,    4,    5,    9,   81,   82,   79,   80,
 /*    10 */     6,   12,   83,   84,   85,   16,   17,   15,   22,   23,
 /*    20 */    24,   25,   26,   27,   28,   29,   30,   80,   50,   51,
 /*    30 */    83,   84,   85,   81,   82,   36,   37,   33,   34,   35,
 /*    40 */    69,   70,   38,   39,   40,   41,   42,   43,   44,   45,
 /*    50 */    46,   47,   48,   49,   50,   51,   66,   67,   59,   60,
 /*    60 */    61,   62,   63,   63,   65,   63,   67,   68,   69,   70,
 /*    70 */    71,   72,   73,    6,   83,   84,   85,   10,   11,   74,
 /*    80 */    75,   76,   77,   64,   79,    6,   67,   68,   83,   84,
 /*    90 */    85,    6,   83,   84,   85,   59,    6,   80,   31,   32,
 /*   100 */    33,   34,   35,   67,   68,   38,   39,   40,   41,   42,
 /*   110 */    43,   44,   45,   46,   47,   48,   49,   50,   51,    6,
 /*   120 */    83,   84,   85,   10,   45,   46,   47,   48,   49,   50,
 /*   130 */    51,   46,   47,   48,   49,   50,   51,   47,   48,   49,
 /*   140 */    50,   51,   82,   63,   31,   32,   33,   34,   35,   51,
 /*   150 */    82,   38,   39,   40,   41,   42,   43,   44,   45,   46,
 /*   160 */    47,   48,   49,   50,   51,    6,   59,   64,   79,   10,
 /*   170 */    67,   68,   83,   84,   85,   64,   74,   64,   67,   77,
 /*   180 */    78,   79,    9,    2,    6,   83,   84,   85,    6,   64,
 /*   190 */    31,   32,   33,   34,   35,    2,   13,   38,   39,   40,
 /*   200 */    41,   42,   43,   44,   45,   46,   47,   48,   49,   50,
 /*   210 */    51,    6,   83,   84,   85,   10,   57,   39,   40,   41,
 /*   220 */    42,   43,   44,   45,   46,   47,   48,   49,   50,   51,
 /*   230 */    48,   49,   50,   51,   64,   15,   31,   32,   33,   34,
 /*   240 */    35,   67,    2,   38,   39,   40,   41,   42,   43,   44,
 /*   250 */    45,   46,   47,   48,   49,   50,   51,    1,   59,    3,
 /*   260 */     4,    5,   86,    6,   63,   79,   67,   68,   12,   83,
 /*   270 */    84,   85,   16,   17,    1,   63,    3,   74,    5,   63,
 /*   280 */    77,   78,   79,   63,   59,   12,   83,   84,   85,   16,
 /*   290 */    17,   63,   36,   37,   63,   38,   39,   40,   41,   42,
 /*   300 */    43,   44,   45,   46,   47,   48,   49,   50,   51,   36,
 /*   310 */    37,   59,   86,    2,   86,   59,   60,   61,   62,   63,
 /*   320 */    86,   65,   83,   84,   85,   69,   70,   71,   72,   73,
 /*   330 */    86,   58,   59,   60,   61,   62,   63,    1,   65,    3,
 /*   340 */     6,    5,   69,   70,   71,   72,   73,   86,   12,   83,
 /*   350 */    84,   85,   16,   17,    0,    1,   79,    3,   86,    5,
 /*   360 */    83,   84,   85,   52,   53,   86,   12,   56,   86,   86,
 /*   370 */    16,   17,   36,   37,   40,   41,   42,   43,   44,   45,
 /*   380 */    46,   47,   48,   49,   50,   51,   86,    6,   86,   86,
 /*   390 */    36,   37,   86,   86,   58,   59,   60,   61,   62,   63,
 /*   400 */    86,   65,   83,   84,   85,   69,   70,   71,   72,   73,
 /*   410 */    83,   84,   85,   59,   60,   61,   62,   63,    1,   65,
 /*   420 */     3,    6,    5,   69,   70,   71,   72,   73,   86,   12,
 /*   430 */    49,   50,   51,   16,   17,    1,   79,    3,   86,    5,
 /*   440 */    83,   84,   85,   83,   84,   85,   12,   83,   84,   85,
 /*   450 */    16,   17,    2,   36,   37,   86,   41,   42,   43,   44,
 /*   460 */    45,   46,   47,   48,   49,   50,   51,   83,   84,   85,
 /*   470 */    36,   37,   49,   50,   51,   58,   59,   60,   61,   62,
 /*   480 */    63,   86,   65,    6,   86,   86,   69,   70,   71,   72,
 /*   490 */    73,   86,   58,   59,   60,   61,   62,   63,   86,   65,
 /*   500 */    86,   86,   86,   69,   70,   71,   72,   73,   31,   32,
 /*   510 */    33,   34,   35,   63,    6,   38,   39,   40,   41,   42,
 /*   520 */    43,   44,   45,   46,   47,   48,   49,   50,   51,    1,
 /*   530 */    86,    3,   86,    5,   83,   84,   85,   86,    6,   86,
 /*   540 */    12,   86,   34,   35,   16,   17,   38,   39,   40,   41,
 /*   550 */    42,   43,   44,   45,   46,   47,   48,   49,   50,   51,
 /*   560 */    86,   83,   84,   85,   36,   37,    6,   35,   86,   86,
 /*   570 */    38,   39,   40,   41,   42,   43,   44,   45,   46,   47,
 /*   580 */    48,   49,   50,   51,   86,   86,   86,   59,   60,   61,
 /*   590 */    62,   63,   86,   65,    6,   86,   86,   69,   70,   71,
 /*   600 */    72,   73,   42,   43,   44,   45,   46,   47,   48,   49,
 /*   610 */    50,   51,   83,   84,   85,   86,   86,   83,   84,   85,
 /*   620 */    32,   33,   34,   35,   86,   86,   38,   39,   40,   41,
 /*   630 */    42,   43,   44,   45,   46,   47,   48,   49,   50,   51,
 /*   640 */     1,    2,    6,   86,    5,   83,   84,   85,   86,   86,
 /*   650 */    74,   86,   86,   77,   78,   79,   17,   86,    1,   83,
 /*   660 */    84,   85,    5,   83,   84,   85,   83,   84,   85,   86,
 /*   670 */    86,   86,   86,   86,   17,   36,   37,   86,   86,   43,
 /*   680 */    44,   45,   46,   47,   48,   49,   50,   51,   83,   84,
 /*   690 */    85,   86,   86,   36,   37,   83,   84,   85,   86,   86,
 /*   700 */     6,   62,   63,   83,   84,   85,   86,   86,   69,   70,
 /*   710 */    71,   72,   73,   83,   84,   85,   83,   84,   85,   62,
 /*   720 */    63,   64,   86,   83,   84,   85,   69,   70,   71,   72,
 /*   730 */    73,    1,   86,   86,   86,    5,   86,   86,   44,   45,
 /*   740 */    46,   47,   48,   49,   50,   51,   86,   17,   86,   74,
 /*   750 */    86,   76,   77,   86,   79,   86,   86,   86,   83,   84,
 /*   760 */    85,   86,   86,   83,   84,   85,   36,   37,   86,   74,
 /*   770 */    86,   86,   77,   78,   79,   86,   86,   86,   83,   84,
 /*   780 */    85,   83,   84,   85,   86,   74,   86,   86,   77,   78,
 /*   790 */    79,   86,   62,   63,   83,   84,   85,   86,   86,   69,
 /*   800 */    70,   71,   72,   73,   86,   74,   86,   86,   77,   78,
 /*   810 */    79,   83,   84,   85,   83,   84,   85,   74,   86,   76,
 /*   820 */    77,   86,   79,   83,   84,   85,   83,   84,   85,   74,
 /*   830 */    86,   86,   77,   86,   79,   83,   84,   85,   83,   84,
 /*   840 */    85,   86,   83,   84,   85,   86,   86,   83,   84,   85,
 /*   850 */    83,   84,   85,   83,   84,   85,   83,   84,   85,   86,
 /*   860 */    86,   86,   83,   84,   85,   83,   84,   85,   86,   83,
 /*   870 */    84,   85,
};
#define YY_SHIFT_USE_DFLT (-30)
#define YY_SHIFT_MAX 124
static const short yy_shift_ofst[] = {
 /*     0 */   528,   -1,   -1,  256,  273,  256,  256,  256,  336,  354,
 /*    10 */   417,  434,  639,  639,  657,  730,  730,  730,  730,  730,
 /*    20 */   730,  730,  730,  730,  730,  730,  730,  730,  730,  730,
 /*    30 */   730,  730,  730,  730,  730,  730,  730,  730,  730,  730,
 /*    40 */   730,  730,  730,  730,  730,  730,  730,  730,  730,  730,
 /*    50 */   730,  730,  730,  730,  730,  220,  220,  311,   80,   80,
 /*    60 */    67,  113,  159,  205,  205,  205,  205,  477,  477,  477,
 /*    70 */   477,  477,  477,  477,  477,  477,  477,  588,    4,  508,
 /*    80 */   532,  257,  257,  257,  178,  334,  415,  560,  636,   -4,
 /*    90 */   694,   79,   85,   90,  182,  381,   36,   19,  199,  103,
 /*   100 */   423,  -22,  -29,  450,  -10,    0,  111,    2,  107,   98,
 /*   110 */   173,  181,  125,  183,  193,  170,  240,  201,  212,  216,
 /*   120 */   225,  174,  228,  231,  252,
};
#define YY_REDUCE_USE_DFLT (-76)
#define YY_REDUCE_MAX 59
static const short yy_reduce_ofst[] = {
 /*     0 */     5,  102,  203,  576,  675,  695,  711,  731,  743,  755,
 /*    10 */   755,  755,  -71,  -53,   89,  186,  277,  357,   -9,    9,
 /*    20 */    37,  129,  239,  266,  319,  327,  360,  364,  384,  451,
 /*    30 */   478,  529,  534,  562,  580,  583,  605,  612,  620,  630,
 /*    40 */   633,  640,  680,  698,  728,  740,  752,  759,  764,  767,
 /*    50 */   770,  773,  779,  782,  786,  -75,  -48,   17,   60,   68,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   248,  248,  248,  248,  248,  248,  248,  248,  248,  248,
 /*    10 */   248,  248,  248,  248,  248,  248,  248,  248,  248,  248,
 /*    20 */   248,  248,  248,  248,  248,  248,  248,  248,  248,  248,
 /*    30 */   248,  248,  248,  248,  248,  248,  248,  248,  248,  248,
 /*    40 */   248,  248,  248,  248,  248,  248,  248,  248,  248,  248,
 /*    50 */   248,  248,  248,  248,  248,  244,  244,  232,  244,  244,
 /*    60 */   248,  248,  248,  196,  197,  221,  198,  231,  181,  182,
 /*    70 */   183,  184,  185,  186,  187,  188,  189,  206,  216,  214,
 /*    80 */   205,  199,  200,  215,  208,  209,  207,  212,  213,  223,
 /*    90 */   210,  211,  222,  218,  217,  201,  248,  248,  248,  248,
 /*   100 */   202,  203,  248,  248,  248,  248,  248,  248,  248,  204,
 /*   110 */   223,  248,  248,  177,  248,  248,  248,  248,  248,  248,
 /*   120 */   248,  245,  248,  248,  248,  167,  172,  173,  194,  224,
 /*   130 */   225,  227,  226,  228,  229,  170,  175,  171,  176,  178,
 /*   140 */   179,  180,  237,  238,  233,  234,  242,  243,  235,  236,
 /*   150 */   241,  239,  240,  190,  192,  193,  168,  191,  246,  247,
 /*   160 */   169,  230,  195,  220,  219,  174,
};
#define YY_SZ_ACTTAB (int)(sizeof(yy_action)/sizeof(yy_action[0]))

/* The next table maps tokens into fallback tokens.  If a construct
** like the following:
** 
**      %fallback ID X Y Z.
**
** appears in the grammar, then ID becomes a fallback token for X, Y,
** and Z.  Whenever one of the tokens X, Y, or Z is input to the parser
** but it does not parse, the type of the token is changed to ID and
** the parse is retried before an error is thrown.
*/
#ifdef YYFALLBACK
static const YYCODETYPE yyFallback[] = {
    0,  /*          $ => nothing */
    0,  /* GROUPLEFTPAREN => nothing */
    1,  /*  LEFTPAREN => GROUPLEFTPAREN */
    0,  /* SCOPESTARTBLOCK => nothing */
    3,  /* STARTBLOCK => SCOPESTARTBLOCK */
    0,  /*   NEGATIVE => nothing */
    5,  /*       DASH => NEGATIVE */
};
#endif /* YYFALLBACK */

/* The following structure represents a single element of the
** parser's stack.  Information stored includes:
**
**   +  The state number for the parser at this level of the stack.
**
**   +  The value of the token stored at this level of the stack.
**      (In other words, the "major" token.)
**
**   +  The semantic value stored at this level of the stack.  This is
**      the information used by the action routines in the grammar.
**      It is sometimes called the "minor" token.
*/
struct yyStackEntry {
  YYACTIONTYPE stateno;  /* The state-number */
  YYCODETYPE major;      /* The major token value.  This is the code
                         ** number for the token at this stack level */
  YYMINORTYPE minor;     /* The user-supplied minor token value.  This
                         ** is the value of the token  */
};
typedef struct yyStackEntry yyStackEntry;

/* The state of the parser is completely contained in an instance of
** the following structure */
struct yyParser {
  int yyidx;                    /* Index of top element in stack */
#ifdef YYTRACKMAXSTACKDEPTH
  int yyidxMax;                 /* Maximum value of yyidx */
#endif
  int yyerrcnt;                 /* Shifts left before out of the error */
  ekParseARG_SDECL                /* A place to hold %extra_argument */
#if YYSTACKDEPTH<=0
  int yystksz;                  /* Current side of the stack */
  yyStackEntry *yystack;        /* The parser's stack */
#else
  yyStackEntry yystack[YYSTACKDEPTH];  /* The parser's stack */
#endif
};
typedef struct yyParser yyParser;

#ifndef NDEBUG
#include <stdio.h>
static FILE *yyTraceFILE = 0;
static char *yyTracePrompt = 0;
#endif /* NDEBUG */

#ifndef NDEBUG
/* 
** Turn parser tracing on by giving a stream to which to write the trace
** and a prompt to preface each trace message.  Tracing is turned off
** by making either argument NULL 
**
** Inputs:
** <ul>
** <li> A FILE* to which trace output should be written.
**      If NULL, then tracing is turned off.
** <li> A prefix string written at the beginning of every
**      line of trace output.  If NULL, then tracing is
**      turned off.
** </ul>
**
** Outputs:
** None.
*/
void ekParseTrace(FILE *TraceFILE, char *zTracePrompt){
  yyTraceFILE = TraceFILE;
  yyTracePrompt = zTracePrompt;
  if( yyTraceFILE==0 ) yyTracePrompt = 0;
  else if( yyTracePrompt==0 ) yyTraceFILE = 0;
}
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing shifts, the names of all terminals and nonterminals
** are required.  The following table supplies these names */
static const char *const yyTokenName[] = { 
  "$",             "GROUPLEFTPAREN",  "LEFTPAREN",     "SCOPESTARTBLOCK",
  "STARTBLOCK",    "NEGATIVE",      "DASH",          "NEWLINE",     
  "SEMI",          "ASSIGN",        "QUESTIONMARK",  "COLON",       
  "IF",            "ELSE",          "HEREDOC",       "ELLIPSIS",    
  "BREAK",         "THIS",          "UNKNOWN",       "COMMENT",     
  "SPACE",         "EOF",           "BITWISE_OREQUALS",  "BITWISE_XOREQUALS",
  "BITWISE_ANDEQUALS",  "SHIFTRIGHTEQUALS",  "SHIFTLEFTEQUALS",  "PLUSEQUALS",  
  "SLASHEQUALS",   "DASHEQUALS",    "STAREQUALS",    "OR",          
  "BITWISE_OR",    "BITWISE_XOR",   "AND",           "BITWISE_AND", 
  "BITWISE_NOT",   "NOT",           "EQUALS",        "NOTEQUALS",   
  "CMP",           "LESSTHAN",      "LESSTHANOREQUAL",  "GREATERTHAN", 
  "GREATERTHANOREQUAL",  "INHERITS",      "SHIFTRIGHT",    "SHIFTLEFT",   
  "PLUS",          "STAR",          "SLASH",         "MOD",         
  "PERIOD",        "COLONCOLON",    "OPENBRACE",     "CLOSEBRACE",  
  "OPENBRACKET",   "CLOSEBRACKET",  "ENDBLOCK",      "ENDSTATEMENT",
  "RETURN",        "WHILE",         "FUNCTION",      "IDENTIFIER",  
  "RIGHTPAREN",    "FOR",           "IN",            "COMMA",       
  "FATCOMMA",      "INTEGER",       "FLOATNUM",      "LITERALSTRING",
  "NULL",          "VAR",           "error",         "chunk",       
  "statement_list",  "statement",     "statement_block",  "expr_list",   
  "paren_expr_list",  "func_args",     "ident_list",    "expression",  
  "lvalue",        "lvalue_indexable",
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *const yyRuleName[] = {
 /*   0 */ "chunk ::= statement_list",
 /*   1 */ "statement_list ::= statement_list statement",
 /*   2 */ "statement_list ::= statement",
 /*   3 */ "statement_block ::= STARTBLOCK ENDBLOCK",
 /*   4 */ "statement_block ::= STARTBLOCK statement_list ENDBLOCK",
 /*   5 */ "statement_block ::= statement",
 /*   6 */ "statement ::= BREAK ENDSTATEMENT",
 /*   7 */ "statement ::= RETURN expr_list ENDSTATEMENT",
 /*   8 */ "statement ::= RETURN paren_expr_list ENDSTATEMENT",
 /*   9 */ "statement ::= expr_list ENDSTATEMENT",
 /*  10 */ "statement ::= IF expr_list statement_block ELSE statement_block",
 /*  11 */ "statement ::= IF expr_list statement_block",
 /*  12 */ "statement ::= WHILE expr_list statement_block",
 /*  13 */ "statement ::= FUNCTION IDENTIFIER LEFTPAREN func_args RIGHTPAREN statement_block",
 /*  14 */ "statement ::= FOR LEFTPAREN ident_list IN expression RIGHTPAREN statement_block",
 /*  15 */ "statement ::= lvalue PLUSEQUALS expression",
 /*  16 */ "statement ::= lvalue DASHEQUALS expression",
 /*  17 */ "statement ::= lvalue STAREQUALS expression",
 /*  18 */ "statement ::= lvalue SLASHEQUALS expression",
 /*  19 */ "statement ::= lvalue BITWISE_OREQUALS expression",
 /*  20 */ "statement ::= lvalue BITWISE_ANDEQUALS expression",
 /*  21 */ "statement ::= lvalue BITWISE_XOREQUALS expression",
 /*  22 */ "statement ::= lvalue SHIFTLEFTEQUALS expression",
 /*  23 */ "statement ::= lvalue SHIFTRIGHTEQUALS expression",
 /*  24 */ "statement ::= ENDSTATEMENT",
 /*  25 */ "statement ::= SCOPESTARTBLOCK ENDBLOCK",
 /*  26 */ "statement ::= SCOPESTARTBLOCK statement_list ENDBLOCK",
 /*  27 */ "statement ::= error ENDSTATEMENT",
 /*  28 */ "paren_expr_list ::= LEFTPAREN expr_list RIGHTPAREN",
 /*  29 */ "paren_expr_list ::= LEFTPAREN RIGHTPAREN",
 /*  30 */ "expr_list ::= expr_list COMMA expression",
 /*  31 */ "expr_list ::= expr_list FATCOMMA expression",
 /*  32 */ "expr_list ::= expression",
 /*  33 */ "expression ::= NOT expression",
 /*  34 */ "expression ::= BITWISE_NOT expression",
 /*  35 */ "expression ::= expression PLUS expression",
 /*  36 */ "expression ::= expression DASH expression",
 /*  37 */ "expression ::= expression STAR expression",
 /*  38 */ "expression ::= expression SLASH expression",
 /*  39 */ "expression ::= expression AND expression",
 /*  40 */ "expression ::= expression OR expression",
 /*  41 */ "expression ::= expression CMP expression",
 /*  42 */ "expression ::= expression EQUALS expression",
 /*  43 */ "expression ::= expression NOTEQUALS expression",
 /*  44 */ "expression ::= expression GREATERTHAN expression",
 /*  45 */ "expression ::= expression GREATERTHANOREQUAL expression",
 /*  46 */ "expression ::= expression LESSTHAN expression",
 /*  47 */ "expression ::= expression LESSTHANOREQUAL expression",
 /*  48 */ "expression ::= expression BITWISE_XOR expression",
 /*  49 */ "expression ::= expression BITWISE_AND expression",
 /*  50 */ "expression ::= expression BITWISE_OR expression",
 /*  51 */ "expression ::= expression SHIFTLEFT expression",
 /*  52 */ "expression ::= expression SHIFTRIGHT expression",
 /*  53 */ "expression ::= expression MOD paren_expr_list",
 /*  54 */ "expression ::= expression MOD expression",
 /*  55 */ "expression ::= lvalue ASSIGN expression",
 /*  56 */ "expression ::= expression INHERITS expression",
 /*  57 */ "expression ::= lvalue",
 /*  58 */ "expression ::= INTEGER",
 /*  59 */ "expression ::= NEGATIVE INTEGER",
 /*  60 */ "expression ::= FLOATNUM",
 /*  61 */ "expression ::= NEGATIVE FLOATNUM",
 /*  62 */ "expression ::= LITERALSTRING",
 /*  63 */ "expression ::= NULL",
 /*  64 */ "expression ::= FUNCTION LEFTPAREN func_args RIGHTPAREN statement_block",
 /*  65 */ "expression ::= expression QUESTIONMARK expression COLON expression",
 /*  66 */ "lvalue ::= lvalue_indexable",
 /*  67 */ "lvalue ::= VAR IDENTIFIER",
 /*  68 */ "lvalue ::= VAR GROUPLEFTPAREN ident_list RIGHTPAREN",
 /*  69 */ "lvalue ::= GROUPLEFTPAREN expr_list RIGHTPAREN",
 /*  70 */ "lvalue_indexable ::= THIS",
 /*  71 */ "lvalue_indexable ::= lvalue_indexable paren_expr_list",
 /*  72 */ "lvalue_indexable ::= lvalue_indexable OPENBRACKET expression CLOSEBRACKET",
 /*  73 */ "lvalue_indexable ::= lvalue_indexable PERIOD IDENTIFIER",
 /*  74 */ "lvalue_indexable ::= lvalue_indexable COLONCOLON IDENTIFIER",
 /*  75 */ "lvalue_indexable ::= IDENTIFIER",
 /*  76 */ "ident_list ::= ident_list COMMA IDENTIFIER",
 /*  77 */ "ident_list ::= IDENTIFIER",
 /*  78 */ "ident_list ::=",
 /*  79 */ "func_args ::= ident_list",
 /*  80 */ "func_args ::= ident_list COMMA ELLIPSIS IDENTIFIER",
 /*  81 */ "func_args ::= ELLIPSIS IDENTIFIER",
};
#endif /* NDEBUG */


#if YYSTACKDEPTH<=0
/*
** Try to increase the size of the parser stack.
*/
static void yyGrowStack(yyParser *p){
  int newSize;
  yyStackEntry *pNew;

  newSize = p->yystksz*2 + 100;
  pNew = ekRealloc(p->yystack, newSize*sizeof(pNew[0]));
  if( pNew ){
    p->yystack = pNew;
    p->yystksz = newSize;
#ifndef NDEBUG
    if( yyTraceFILE ){
      fprintf(yyTraceFILE,"%sStack grows to %d entries!\n",
              yyTracePrompt, p->yystksz);
    }
#endif
  }
}
#endif

/* 
** This function allocates a new parser.
** The only argument is a pointer to a function which works like
** malloc.
**
** Inputs:
** A pointer to the function used to allocate memory.
**
** Outputs:
** A pointer to a parser.  This pointer is used in subsequent calls
** to ekParse and ekParseFree.
*/
void *ekParseAlloc(struct ekContext *Y){
  yyParser *pParser;
  pParser = (yyParser*)ekAlloc( sizeof(yyParser) );
  if( pParser ){
    pParser->yyidx = -1;
#ifdef YYTRACKMAXSTACKDEPTH
    pParser->yyidxMax = 0;
#endif
#if YYSTACKDEPTH<=0
    pParser->yystack = NULL;
    pParser->yystksz = 0;
    yyGrowStack(pParser);
#endif
  }
  return pParser;
}

/* The following function deletes the value associated with a
** symbol.  The symbol can be either a terminal or nonterminal.
** "yymajor" is the symbol code, and "yypminor" is a pointer to
** the value.
*/
static void yy_destructor(
  yyParser *yypParser,    /* The parser */
  YYCODETYPE yymajor,     /* Type code for object to destroy */
  YYMINORTYPE *yypminor   /* The object to be destroyed */
){
  ekParseARG_FETCH;
  switch( yymajor ){
    /* Here is inserted the actions which take place when a
    ** terminal or non-terminal is destroyed.  This can happen
    ** when the symbol is popped from the stack during a
    ** reduce or during error processing or when a parser is 
    ** being destroyed before it is finished parsing.
    **
    ** Note: during a reduce, the only symbols destroyed are those
    ** which appear on the RHS of the rule, but which are not used
    ** inside the C code.
    */
    case 76: /* statement_list */
    case 77: /* statement */
    case 78: /* statement_block */
    case 79: /* expr_list */
    case 80: /* paren_expr_list */
    case 82: /* ident_list */
    case 83: /* expression */
    case 84: /* lvalue */
    case 85: /* lvalue_indexable */
{
#line 130 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
 ekSyntaxDestroy(C->Y, (yypminor->yy22)); 
#line 693 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
}
      break;
    default:  break;   /* If no destructor action specified: do nothing */
  }
}

/*
** Pop the parser's stack once.
**
** If there is a destructor routine associated with the token which
** is popped from the stack, then call it.
**
** Return the major token number for the symbol popped.
*/
static int yy_pop_parser_stack(yyParser *pParser){
  YYCODETYPE yymajor;
  yyStackEntry *yytos = &pParser->yystack[pParser->yyidx];

  if( pParser->yyidx<0 ) return 0;
#ifndef NDEBUG
  if( yyTraceFILE && pParser->yyidx>=0 ){
    fprintf(yyTraceFILE,"%sPopping %s\n",
      yyTracePrompt,
      yyTokenName[yytos->major]);
  }
#endif
  yymajor = yytos->major;
  yy_destructor(pParser, yymajor, &yytos->minor);
  pParser->yyidx--;
  return yymajor;
}

/* 
** Deallocate and destroy a parser.  Destructors are all called for
** all stack elements before shutting the parser down.
**
** Inputs:
** <ul>
** <li>  A pointer to the parser.  This should be a pointer
**       obtained from ekParseAlloc.
** <li>  A pointer to a function used to reclaim memory obtained
**       from malloc.
** </ul>
*/
void ekParseFree(struct ekContext *Y,
  void *p                    /* The parser to be deleted */
){
  yyParser *pParser = (yyParser*)p;
  if( pParser==0 ) return;
  while( pParser->yyidx>=0 ) yy_pop_parser_stack(pParser);
#if YYSTACKDEPTH<=0
  ekFree(pParser->yystack);
#endif
  ekFree((void*)pParser);
}

/*
** Return the peak depth of the stack for a parser.
*/
#ifdef YYTRACKMAXSTACKDEPTH
int ekParseStackPeak(void *p){
  yyParser *pParser = (yyParser*)p;
  return pParser->yyidxMax;
}
#endif

/*
** Find the appropriate action for a parser given the terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_shift_action(
  yyParser *pParser,        /* The parser */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
  int stateno = pParser->yystack[pParser->yyidx].stateno;
 
  if( stateno>YY_SHIFT_MAX || (i = yy_shift_ofst[stateno])==YY_SHIFT_USE_DFLT ){
    return yy_default[stateno];
  }
  assert( iLookAhead!=YYNOCODE );
  i += iLookAhead;
  if( i<0 || i>=YY_SZ_ACTTAB || yy_lookahead[i]!=iLookAhead ){
    if( iLookAhead>0 ){
#ifdef YYFALLBACK
      YYCODETYPE iFallback;            /* Fallback token */
      if( iLookAhead<sizeof(yyFallback)/sizeof(yyFallback[0])
             && (iFallback = yyFallback[iLookAhead])!=0 ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE, "%sFALLBACK %s => %s\n",
             yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[iFallback]);
        }
#endif
        return yy_find_shift_action(pParser, iFallback);
      }
#endif
#ifdef YYWILDCARD
      {
        int j = i - iLookAhead + YYWILDCARD;
        if( j>=0 && j<YY_SZ_ACTTAB && yy_lookahead[j]==YYWILDCARD ){
#ifndef NDEBUG
          if( yyTraceFILE ){
            fprintf(yyTraceFILE, "%sWILDCARD %s => %s\n",
               yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[YYWILDCARD]);
          }
#endif /* NDEBUG */
          return yy_action[j];
        }
      }
#endif /* YYWILDCARD */
    }
    return yy_default[stateno];
  }else{
    return yy_action[i];
  }
}

/*
** Find the appropriate action for a parser given the non-terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_reduce_action(
  int stateno,              /* Current state number */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
#ifdef YYERRORSYMBOL
  if( stateno>YY_REDUCE_MAX ){
    return yy_default[stateno];
  }
#else
  assert( stateno<=YY_REDUCE_MAX );
#endif
  i = yy_reduce_ofst[stateno];
  assert( i!=YY_REDUCE_USE_DFLT );
  assert( iLookAhead!=YYNOCODE );
  i += iLookAhead;
#ifdef YYERRORSYMBOL
  if( i<0 || i>=YY_SZ_ACTTAB || yy_lookahead[i]!=iLookAhead ){
    return yy_default[stateno];
  }
#else
  assert( i>=0 && i<YY_SZ_ACTTAB );
  assert( yy_lookahead[i]==iLookAhead );
#endif
  return yy_action[i];
}

/*
** The following routine is called if the stack overflows.
*/
static void yyStackOverflow(yyParser *yypParser, YYMINORTYPE *yypMinor){
   ekParseARG_FETCH;
   yypParser->yyidx--;
#ifndef NDEBUG
   if( yyTraceFILE ){
     fprintf(yyTraceFILE,"%sStack Overflow!\n",yyTracePrompt);
   }
#endif
   while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
   /* Here code is inserted which will execute if the parser
   ** stack every overflows */
   ekParseARG_STORE; /* Suppress warning about unused %extra_argument var */
}

/*
** Perform a shift action.
*/
static void yy_shift(
  yyParser *yypParser,          /* The parser to be shifted */
  int yyNewState,               /* The new state to shift in */
  int yyMajor,                  /* The major token to shift in */
  YYMINORTYPE *yypMinor         /* Pointer to the minor token to shift in */
){
  yyStackEntry *yytos;
  yypParser->yyidx++;
#ifdef YYTRACKMAXSTACKDEPTH
  if( yypParser->yyidx>yypParser->yyidxMax ){
    yypParser->yyidxMax = yypParser->yyidx;
  }
#endif
#if YYSTACKDEPTH>0 
  if( yypParser->yyidx>=YYSTACKDEPTH ){
    yyStackOverflow(yypParser, yypMinor);
    return;
  }
#else
  if( yypParser->yyidx>=yypParser->yystksz ){
    yyGrowStack(yypParser);
    if( yypParser->yyidx>=yypParser->yystksz ){
      yyStackOverflow(yypParser, yypMinor);
      return;
    }
  }
#endif
  yytos = &yypParser->yystack[yypParser->yyidx];
  yytos->stateno = (YYACTIONTYPE)yyNewState;
  yytos->major = (YYCODETYPE)yyMajor;
  yytos->minor = *yypMinor;
#ifndef NDEBUG
  if( yyTraceFILE && yypParser->yyidx>0 ){
    int i;
    fprintf(yyTraceFILE,"%sShift %d\n",yyTracePrompt,yyNewState);
    fprintf(yyTraceFILE,"%sStack:",yyTracePrompt);
    for(i=1; i<=yypParser->yyidx; i++)
      fprintf(yyTraceFILE," %s",yyTokenName[yypParser->yystack[i].major]);
    fprintf(yyTraceFILE,"\n");
  }
#endif
}

/* The following table contains information about every rule that
** is used during the reduce.
*/
static const struct {
  YYCODETYPE lhs;         /* Symbol on the left-hand side of the rule */
  unsigned char nrhs;     /* Number of right-hand side symbols in the rule */
} yyRuleInfo[] = {
  { 75, 1 },
  { 76, 2 },
  { 76, 1 },
  { 78, 2 },
  { 78, 3 },
  { 78, 1 },
  { 77, 2 },
  { 77, 3 },
  { 77, 3 },
  { 77, 2 },
  { 77, 5 },
  { 77, 3 },
  { 77, 3 },
  { 77, 6 },
  { 77, 7 },
  { 77, 3 },
  { 77, 3 },
  { 77, 3 },
  { 77, 3 },
  { 77, 3 },
  { 77, 3 },
  { 77, 3 },
  { 77, 3 },
  { 77, 3 },
  { 77, 1 },
  { 77, 2 },
  { 77, 3 },
  { 77, 2 },
  { 80, 3 },
  { 80, 2 },
  { 79, 3 },
  { 79, 3 },
  { 79, 1 },
  { 83, 2 },
  { 83, 2 },
  { 83, 3 },
  { 83, 3 },
  { 83, 3 },
  { 83, 3 },
  { 83, 3 },
  { 83, 3 },
  { 83, 3 },
  { 83, 3 },
  { 83, 3 },
  { 83, 3 },
  { 83, 3 },
  { 83, 3 },
  { 83, 3 },
  { 83, 3 },
  { 83, 3 },
  { 83, 3 },
  { 83, 3 },
  { 83, 3 },
  { 83, 3 },
  { 83, 3 },
  { 83, 3 },
  { 83, 3 },
  { 83, 1 },
  { 83, 1 },
  { 83, 2 },
  { 83, 1 },
  { 83, 2 },
  { 83, 1 },
  { 83, 1 },
  { 83, 5 },
  { 83, 5 },
  { 84, 1 },
  { 84, 2 },
  { 84, 4 },
  { 84, 3 },
  { 85, 1 },
  { 85, 2 },
  { 85, 4 },
  { 85, 3 },
  { 85, 3 },
  { 85, 1 },
  { 82, 3 },
  { 82, 1 },
  { 82, 0 },
  { 81, 1 },
  { 81, 4 },
  { 81, 2 },
};

static void yy_accept(yyParser*);  /* Forward Declaration */

/*
** Perform a reduce action and the shift that must immediately
** follow the reduce.
*/
static void yy_reduce(
  yyParser *yypParser,         /* The parser */
  int yyruleno                 /* Number of the rule by which to reduce */
){
  int yygoto;                     /* The next state */
  int yyact;                      /* The next action */
  YYMINORTYPE yygotominor;        /* The LHS of the rule reduced */
  yyStackEntry *yymsp;            /* The top of the parser's stack */
  int yysize;                     /* Amount to pop the stack */
  ekParseARG_FETCH;
  yymsp = &yypParser->yystack[yypParser->yyidx];
#ifndef NDEBUG
  if( yyTraceFILE && yyruleno>=0 
        && yyruleno<(int)(sizeof(yyRuleName)/sizeof(yyRuleName[0])) ){
    fprintf(yyTraceFILE, "%sReduce [%s].\n", yyTracePrompt,
      yyRuleName[yyruleno]);
  }
#endif /* NDEBUG */

  /* Silence complaints from purify about yygotominor being uninitialized
  ** in some cases when it is copied into the stack after the following
  ** switch.  yygotominor is uninitialized when a rule reduces that does
  ** not set the value of its left-hand side nonterminal.  Leaving the
  ** value of the nonterminal uninitialized is utterly harmless as long
  ** as the value is never used.  So really the only thing this code
  ** accomplishes is to quieten purify.  
  **
  ** 2007-01-16:  The wireshark project (www.wireshark.org) reports that
  ** without this code, their parser segfaults.  I'm not sure what there
  ** parser is doing to make this happen.  This is the second bug report
  ** from wireshark this week.  Clearly they are stressing Lemon in ways
  ** that it has not been previously stressed...  (SQLite ticket #2172)
  */
  /*memset(&yygotominor, 0, sizeof(yygotominor));*/
  yygotominor = yyzerominor;


  switch( yyruleno ){
  /* Beginning here are the reduction cases.  A typical example
  ** follows:
  **   case 0:
  **  #line <lineno> <grammarfile>
  **     { ... }           // User supplied code
  **  #line <lineno> <thisfile>
  **     break;
  */
      case 0: /* chunk ::= statement_list */
#line 121 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ C->root = yymsp[0].minor.yy22; }
#line 1060 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 1: /* statement_list ::= statement_list statement */
#line 133 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxListAppend(C->Y, yymsp[-1].minor.yy22, yymsp[0].minor.yy22, 0); }
#line 1065 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 2: /* statement_list ::= statement */
      case 5: /* statement_block ::= statement */ yytestcase(yyruleno==5);
#line 136 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateList(C->Y, YST_STATEMENTLIST, yymsp[0].minor.yy22); }
#line 1071 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 3: /* statement_block ::= STARTBLOCK ENDBLOCK */
      case 24: /* statement ::= ENDSTATEMENT */ yytestcase(yyruleno==24);
      case 25: /* statement ::= SCOPESTARTBLOCK ENDBLOCK */ yytestcase(yyruleno==25);
#line 148 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateList(C->Y, YST_STATEMENTLIST, NULL); }
#line 1078 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 4: /* statement_block ::= STARTBLOCK statement_list ENDBLOCK */
      case 28: /* paren_expr_list ::= LEFTPAREN expr_list RIGHTPAREN */ yytestcase(yyruleno==28);
      case 69: /* lvalue ::= GROUPLEFTPAREN expr_list RIGHTPAREN */ yytestcase(yyruleno==69);
#line 151 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = yymsp[-1].minor.yy22; }
#line 1085 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 6: /* statement ::= BREAK ENDSTATEMENT */
#line 166 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateBreak(C->Y, yymsp[-1].minor.yy0.line); }
#line 1090 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 7: /* statement ::= RETURN expr_list ENDSTATEMENT */
      case 8: /* statement ::= RETURN paren_expr_list ENDSTATEMENT */ yytestcase(yyruleno==8);
#line 169 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateReturn(C->Y, yymsp[-1].minor.yy22); }
#line 1096 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 9: /* statement ::= expr_list ENDSTATEMENT */
#line 175 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateStatementExpr(C->Y, yymsp[-1].minor.yy22); }
#line 1101 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 10: /* statement ::= IF expr_list statement_block ELSE statement_block */
#line 178 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateIfElse(C->Y, yymsp[-3].minor.yy22, yymsp[-2].minor.yy22, yymsp[0].minor.yy22, ekFalse); }
#line 1106 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 11: /* statement ::= IF expr_list statement_block */
#line 181 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateIfElse(C->Y, yymsp[-1].minor.yy22, yymsp[0].minor.yy22, NULL, ekFalse); }
#line 1111 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 12: /* statement ::= WHILE expr_list statement_block */
#line 184 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateWhile(C->Y, yymsp[-1].minor.yy22, yymsp[0].minor.yy22); }
#line 1116 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 13: /* statement ::= FUNCTION IDENTIFIER LEFTPAREN func_args RIGHTPAREN statement_block */
#line 187 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateFunctionDecl(C->Y, &yymsp[-4].minor.yy0, yymsp[-2].minor.yy22, yymsp[0].minor.yy22, yymsp[-5].minor.yy0.line); }
#line 1121 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 14: /* statement ::= FOR LEFTPAREN ident_list IN expression RIGHTPAREN statement_block */
#line 190 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateFor(C->Y, yymsp[-4].minor.yy22, yymsp[-2].minor.yy22, yymsp[0].minor.yy22); }
#line 1126 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 15: /* statement ::= lvalue PLUSEQUALS expression */
#line 193 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateBinary(C->Y, YST_ADD, yymsp[-2].minor.yy22, yymsp[0].minor.yy22, ekTrue); }
#line 1131 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 16: /* statement ::= lvalue DASHEQUALS expression */
#line 196 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateBinary(C->Y, YST_SUB, yymsp[-2].minor.yy22, yymsp[0].minor.yy22, ekTrue); }
#line 1136 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 17: /* statement ::= lvalue STAREQUALS expression */
#line 199 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateBinary(C->Y, YST_MUL, yymsp[-2].minor.yy22, yymsp[0].minor.yy22, ekTrue); }
#line 1141 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 18: /* statement ::= lvalue SLASHEQUALS expression */
#line 202 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateBinary(C->Y, YST_DIV, yymsp[-2].minor.yy22, yymsp[0].minor.yy22, ekTrue); }
#line 1146 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 19: /* statement ::= lvalue BITWISE_OREQUALS expression */
#line 205 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateBinary(C->Y, YST_BITWISE_OR, yymsp[-2].minor.yy22, yymsp[0].minor.yy22, ekTrue); }
#line 1151 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 20: /* statement ::= lvalue BITWISE_ANDEQUALS expression */
#line 208 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateBinary(C->Y, YST_BITWISE_AND, yymsp[-2].minor.yy22, yymsp[0].minor.yy22, ekTrue); }
#line 1156 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 21: /* statement ::= lvalue BITWISE_XOREQUALS expression */
#line 211 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateBinary(C->Y, YST_BITWISE_XOR, yymsp[-2].minor.yy22, yymsp[0].minor.yy22, ekTrue); }
#line 1161 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 22: /* statement ::= lvalue SHIFTLEFTEQUALS expression */
#line 214 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateBinary(C->Y, YST_SHIFTLEFT, yymsp[-2].minor.yy22, yymsp[0].minor.yy22, ekTrue); }
#line 1166 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 23: /* statement ::= lvalue SHIFTRIGHTEQUALS expression */
#line 217 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateBinary(C->Y, YST_SHIFTRIGHT, yymsp[-2].minor.yy22, yymsp[0].minor.yy22, ekTrue); }
#line 1171 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 26: /* statement ::= SCOPESTARTBLOCK statement_list ENDBLOCK */
#line 226 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateScope(C->Y, yymsp[-1].minor.yy22); }
#line 1176 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 29: /* paren_expr_list ::= LEFTPAREN RIGHTPAREN */
#line 243 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateList(C->Y, YST_EXPRESSIONLIST, NULL); }
#line 1181 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 30: /* expr_list ::= expr_list COMMA expression */
#line 255 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxListAppend(C->Y, yymsp[-2].minor.yy22, yymsp[0].minor.yy22, 0); }
#line 1186 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 31: /* expr_list ::= expr_list FATCOMMA expression */
#line 258 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxListAppend(C->Y, yymsp[-2].minor.yy22, yymsp[0].minor.yy22, YSLF_AUTOLITERAL); }
#line 1191 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 32: /* expr_list ::= expression */
#line 261 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateList(C->Y, YST_EXPRESSIONLIST, yymsp[0].minor.yy22); }
#line 1196 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 33: /* expression ::= NOT expression */
#line 273 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateUnary(C->Y, YST_NOT, yymsp[0].minor.yy22); }
#line 1201 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 34: /* expression ::= BITWISE_NOT expression */
#line 276 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateUnary(C->Y, YST_BITWISE_NOT, yymsp[0].minor.yy22); }
#line 1206 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 35: /* expression ::= expression PLUS expression */
#line 279 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateBinary(C->Y, YST_ADD, yymsp[-2].minor.yy22, yymsp[0].minor.yy22, ekFalse); }
#line 1211 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 36: /* expression ::= expression DASH expression */
#line 282 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateBinary(C->Y, YST_SUB, yymsp[-2].minor.yy22, yymsp[0].minor.yy22, ekFalse); }
#line 1216 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 37: /* expression ::= expression STAR expression */
#line 285 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateBinary(C->Y, YST_MUL, yymsp[-2].minor.yy22, yymsp[0].minor.yy22, ekFalse); }
#line 1221 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 38: /* expression ::= expression SLASH expression */
#line 288 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateBinary(C->Y, YST_DIV, yymsp[-2].minor.yy22, yymsp[0].minor.yy22, ekFalse); }
#line 1226 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 39: /* expression ::= expression AND expression */
#line 291 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateBinary(C->Y, YST_AND, yymsp[-2].minor.yy22, yymsp[0].minor.yy22, ekFalse); }
#line 1231 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 40: /* expression ::= expression OR expression */
#line 294 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateBinary(C->Y, YST_OR, yymsp[-2].minor.yy22, yymsp[0].minor.yy22, ekFalse); }
#line 1236 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 41: /* expression ::= expression CMP expression */
#line 297 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateBinary(C->Y, YST_CMP, yymsp[-2].minor.yy22, yymsp[0].minor.yy22, ekFalse); }
#line 1241 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 42: /* expression ::= expression EQUALS expression */
#line 300 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateBinary(C->Y, YST_EQUALS, yymsp[-2].minor.yy22, yymsp[0].minor.yy22, ekFalse); }
#line 1246 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 43: /* expression ::= expression NOTEQUALS expression */
#line 303 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateBinary(C->Y, YST_NOTEQUALS, yymsp[-2].minor.yy22, yymsp[0].minor.yy22, ekFalse); }
#line 1251 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 44: /* expression ::= expression GREATERTHAN expression */
#line 306 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateBinary(C->Y, YST_GREATERTHAN, yymsp[-2].minor.yy22, yymsp[0].minor.yy22, ekFalse); }
#line 1256 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 45: /* expression ::= expression GREATERTHANOREQUAL expression */
#line 309 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateBinary(C->Y, YST_GREATERTHANOREQUAL, yymsp[-2].minor.yy22, yymsp[0].minor.yy22, ekFalse); }
#line 1261 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 46: /* expression ::= expression LESSTHAN expression */
#line 312 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateBinary(C->Y, YST_LESSTHAN, yymsp[-2].minor.yy22, yymsp[0].minor.yy22, ekFalse); }
#line 1266 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 47: /* expression ::= expression LESSTHANOREQUAL expression */
#line 315 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateBinary(C->Y, YST_LESSTHANOREQUAL, yymsp[-2].minor.yy22, yymsp[0].minor.yy22, ekFalse); }
#line 1271 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 48: /* expression ::= expression BITWISE_XOR expression */
#line 318 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateBinary(C->Y, YST_BITWISE_XOR, yymsp[-2].minor.yy22, yymsp[0].minor.yy22, ekFalse); }
#line 1276 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 49: /* expression ::= expression BITWISE_AND expression */
#line 321 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateBinary(C->Y, YST_BITWISE_AND, yymsp[-2].minor.yy22, yymsp[0].minor.yy22, ekFalse); }
#line 1281 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 50: /* expression ::= expression BITWISE_OR expression */
#line 324 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateBinary(C->Y, YST_BITWISE_OR, yymsp[-2].minor.yy22, yymsp[0].minor.yy22, ekFalse); }
#line 1286 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 51: /* expression ::= expression SHIFTLEFT expression */
#line 327 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateBinary(C->Y, YST_SHIFTLEFT, yymsp[-2].minor.yy22, yymsp[0].minor.yy22, ekFalse); }
#line 1291 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 52: /* expression ::= expression SHIFTRIGHT expression */
#line 330 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateBinary(C->Y, YST_SHIFTRIGHT, yymsp[-2].minor.yy22, yymsp[0].minor.yy22, ekFalse); }
#line 1296 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 53: /* expression ::= expression MOD paren_expr_list */
      case 54: /* expression ::= expression MOD expression */ yytestcase(yyruleno==54);
#line 333 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateStringFormat(C->Y, yymsp[-2].minor.yy22, yymsp[0].minor.yy22); }
#line 1302 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 55: /* expression ::= lvalue ASSIGN expression */
#line 339 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateAssignment(C->Y, yymsp[-2].minor.yy22, yymsp[0].minor.yy22); }
#line 1307 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 56: /* expression ::= expression INHERITS expression */
#line 342 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateInherits(C->Y, yymsp[-2].minor.yy22, yymsp[0].minor.yy22); }
#line 1312 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 57: /* expression ::= lvalue */
      case 66: /* lvalue ::= lvalue_indexable */ yytestcase(yyruleno==66);
#line 345 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = yymsp[0].minor.yy22; }
#line 1318 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 58: /* expression ::= INTEGER */
#line 348 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateKInt(C->Y, &yymsp[0].minor.yy0, 0); }
#line 1323 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 59: /* expression ::= NEGATIVE INTEGER */
#line 351 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateKInt(C->Y, &yymsp[0].minor.yy0, CKO_NEGATIVE); }
#line 1328 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 60: /* expression ::= FLOATNUM */
#line 354 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateKFloat(C->Y, &yymsp[0].minor.yy0, 0); }
#line 1333 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 61: /* expression ::= NEGATIVE FLOATNUM */
#line 357 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateKFloat(C->Y, &yymsp[0].minor.yy0, CKO_NEGATIVE); }
#line 1338 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 62: /* expression ::= LITERALSTRING */
#line 360 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateKString(C->Y, &yymsp[0].minor.yy0); }
#line 1343 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 63: /* expression ::= NULL */
#line 363 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateNull(C->Y, yymsp[0].minor.yy0.line); }
#line 1348 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 64: /* expression ::= FUNCTION LEFTPAREN func_args RIGHTPAREN statement_block */
#line 366 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateFunctionDecl(C->Y, NULL, yymsp[-2].minor.yy22, yymsp[0].minor.yy22, yymsp[-4].minor.yy0.line); }
#line 1353 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 65: /* expression ::= expression QUESTIONMARK expression COLON expression */
#line 369 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateIfElse(C->Y, yymsp[-4].minor.yy22, yymsp[-2].minor.yy22, yymsp[0].minor.yy22, ekTrue); }
#line 1358 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 67: /* lvalue ::= VAR IDENTIFIER */
#line 385 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxMarkVar(C->Y, ekSyntaxCreateList(C->Y, YST_IDENTIFIERLIST, ekSyntaxCreateIdentifier(C->Y, &yymsp[0].minor.yy0))); }
#line 1363 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 68: /* lvalue ::= VAR GROUPLEFTPAREN ident_list RIGHTPAREN */
#line 388 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxMarkVar(C->Y, yymsp[-1].minor.yy22); }
#line 1368 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 70: /* lvalue_indexable ::= THIS */
#line 403 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateThis(C->Y, yymsp[0].minor.yy0.line); }
#line 1373 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 71: /* lvalue_indexable ::= lvalue_indexable paren_expr_list */
#line 406 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateCall(C->Y, yymsp[-1].minor.yy22, yymsp[0].minor.yy22); }
#line 1378 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 72: /* lvalue_indexable ::= lvalue_indexable OPENBRACKET expression CLOSEBRACKET */
#line 409 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateIndex(C->Y, yymsp[-3].minor.yy22, yymsp[-1].minor.yy22, ekFalse); }
#line 1383 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 73: /* lvalue_indexable ::= lvalue_indexable PERIOD IDENTIFIER */
#line 412 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateIndex(C->Y, yymsp[-2].minor.yy22, ekSyntaxCreateKString(C->Y, &yymsp[0].minor.yy0), ekFalse); }
#line 1388 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 74: /* lvalue_indexable ::= lvalue_indexable COLONCOLON IDENTIFIER */
#line 415 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateIndex(C->Y, yymsp[-2].minor.yy22, ekSyntaxCreateKString(C->Y, &yymsp[0].minor.yy0), ekTrue); }
#line 1393 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 75: /* lvalue_indexable ::= IDENTIFIER */
#line 418 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateIdentifier(C->Y, &yymsp[0].minor.yy0); }
#line 1398 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 76: /* ident_list ::= ident_list COMMA IDENTIFIER */
#line 430 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxListAppend(C->Y, yymsp[-2].minor.yy22, ekSyntaxCreateIdentifier(C->Y, &yymsp[0].minor.yy0), 0); }
#line 1403 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 77: /* ident_list ::= IDENTIFIER */
#line 433 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateList(C->Y, YST_IDENTIFIERLIST, ekSyntaxCreateIdentifier(C->Y, &yymsp[0].minor.yy0)); }
#line 1408 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 78: /* ident_list ::= */
#line 436 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateList(C->Y, YST_IDENTIFIERLIST, NULL); }
#line 1413 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 79: /* func_args ::= ident_list */
#line 445 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateFunctionArgs(C->Y, yymsp[0].minor.yy22, NULL); }
#line 1418 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 80: /* func_args ::= ident_list COMMA ELLIPSIS IDENTIFIER */
#line 448 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateFunctionArgs(C->Y, yymsp[-3].minor.yy22, &yymsp[0].minor.yy0); }
#line 1423 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      case 81: /* func_args ::= ELLIPSIS IDENTIFIER */
#line 451 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
{ yygotominor.yy22 = ekSyntaxCreateFunctionArgs(C->Y, ekSyntaxCreateList(C->Y, YST_IDENTIFIERLIST, NULL), &yymsp[0].minor.yy0); }
#line 1428 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
        break;
      default:
      /* (27) statement ::= error ENDSTATEMENT */ yytestcase(yyruleno==27);
        break;
  };
  yygoto = yyRuleInfo[yyruleno].lhs;
  yysize = yyRuleInfo[yyruleno].nrhs;
  yypParser->yyidx -= yysize;
  yyact = yy_find_reduce_action(yymsp[-yysize].stateno,(YYCODETYPE)yygoto);
  if( yyact < YYNSTATE ){
#ifdef NDEBUG
    /* If we are not debugging and the reduce action popped at least
    ** one element off the stack, then we can push the new element back
    ** onto the stack here, and skip the stack overflow test in yy_shift().
    ** That gives a significant speed improvement. */
    if( yysize ){
      yypParser->yyidx++;
      yymsp -= yysize-1;
      yymsp->stateno = (YYACTIONTYPE)yyact;
      yymsp->major = (YYCODETYPE)yygoto;
      yymsp->minor = yygotominor;
    }else
#endif
    {
      yy_shift(yypParser,yyact,yygoto,&yygotominor);
    }
  }else{
    assert( yyact == YYNSTATE + YYNRULE + 1 );
    yy_accept(yypParser);
  }
}

/*
** The following code executes when the parse fails
*/
#ifndef YYNOERRORRECOVERY
static void yy_parse_failed(
  yyParser *yypParser           /* The parser */
){
  ekParseARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sFail!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser fails */
  ekParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}
#endif /* YYNOERRORRECOVERY */

/*
** The following code executes when a syntax error first occurs.
*/
static void yy_syntax_error(
  yyParser *yypParser,           /* The parser */
  int yymajor,                   /* The major type of the error token */
  YYMINORTYPE yyminor            /* The minor type of the error token */
){
  ekParseARG_FETCH;
#define TOKEN (yyminor.yy0)
#line 115 "/home/joe/private/work/eureka/eureka/core/ekParser.y"
 ekCompileSyntaxError(C, &TOKEN); 
#line 1493 "/home/joe/private/work/eureka/eureka/core/ekParser.c"
  ekParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following is executed when the parser accepts
*/
static void yy_accept(
  yyParser *yypParser           /* The parser */
){
  ekParseARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sAccept!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser accepts */
  ekParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/* The main parser program.
** The first argument is a pointer to a structure obtained from
** "ekParseAlloc" which describes the current state of the parser.
** The second argument is the major token number.  The third is
** the minor token.  The fourth optional argument is whatever the
** user wants (and specified in the grammar) and is available for
** use by the action routines.
**
** Inputs:
** <ul>
** <li> A pointer to the parser (an opaque structure.)
** <li> The major token number.
** <li> The minor token number.
** <li> An option argument of a grammar-specified type.
** </ul>
**
** Outputs:
** None.
*/
void ekParse(
  void *yyp,                   /* The parser */
  int yymajor,                 /* The major token code number */
  ekParseTOKENTYPE yyminor       /* The value for the token */
  ekParseARG_PDECL               /* Optional %extra_argument parameter */
){
  YYMINORTYPE yyminorunion;
  int yyact;            /* The parser action. */
  int yyendofinput;     /* True if we are at the end of input */
#ifdef YYERRORSYMBOL
  int yyerrorhit = 0;   /* True if yymajor has invoked an error */
#endif
  yyParser *yypParser;  /* The parser */

  /* (re)initialize the parser, if necessary */
  yypParser = (yyParser*)yyp;
  if( yypParser->yyidx<0 ){
#if YYSTACKDEPTH<=0
    if( yypParser->yystksz <=0 ){
      /*memset(&yyminorunion, 0, sizeof(yyminorunion));*/
      yyminorunion = yyzerominor;
      yyStackOverflow(yypParser, &yyminorunion);
      return;
    }
#endif
    yypParser->yyidx = 0;
    yypParser->yyerrcnt = -1;
    yypParser->yystack[0].stateno = 0;
    yypParser->yystack[0].major = 0;
  }
  yyminorunion.yy0 = yyminor;
  yyendofinput = (yymajor==0);
  ekParseARG_STORE;

#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sInput %s\n",yyTracePrompt,yyTokenName[yymajor]);
  }
#endif

  do{
    yyact = yy_find_shift_action(yypParser,(YYCODETYPE)yymajor);
    if( yyact<YYNSTATE ){
      assert( !yyendofinput );  /* Impossible to shift the $ token */
      yy_shift(yypParser,yyact,yymajor,&yyminorunion);
      yypParser->yyerrcnt--;
      yymajor = YYNOCODE;
    }else if( yyact < YYNSTATE + YYNRULE ){
      yy_reduce(yypParser,yyact-YYNSTATE);
    }else{
#ifdef YYERRORSYMBOL
      int yymx;
#endif
      assert( yyact == YY_ERROR_ACTION );
#ifndef NDEBUG
      if( yyTraceFILE ){
        fprintf(yyTraceFILE,"%sSyntax Error!\n",yyTracePrompt);
      }
#endif
#ifdef YYERRORSYMBOL
      /* A syntax error has occurred.
      ** The response to an error depends upon whether or not the
      ** grammar defines an error token "ERROR".  
      **
      ** This is what we do if the grammar does define ERROR:
      **
      **  * Call the %syntax_error function.
      **
      **  * Begin popping the stack until we enter a state where
      **    it is legal to shift the error symbol, then shift
      **    the error symbol.
      **
      **  * Set the error count to three.
      **
      **  * Begin accepting and shifting new tokens.  No new error
      **    processing will occur until three tokens have been
      **    shifted successfully.
      **
      */
      if( yypParser->yyerrcnt<0 ){
        yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yymx = yypParser->yystack[yypParser->yyidx].major;
      if( yymx==YYERRORSYMBOL || yyerrorhit ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE,"%sDiscard input token %s\n",
             yyTracePrompt,yyTokenName[yymajor]);
        }
#endif
        yy_destructor(yypParser, (YYCODETYPE)yymajor,&yyminorunion);
        yymajor = YYNOCODE;
      }else{
         while(
          yypParser->yyidx >= 0 &&
          yymx != YYERRORSYMBOL &&
          (yyact = yy_find_reduce_action(
                        yypParser->yystack[yypParser->yyidx].stateno,
                        YYERRORSYMBOL)) >= YYNSTATE
        ){
          yy_pop_parser_stack(yypParser);
        }
        if( yypParser->yyidx < 0 || yymajor==0 ){
          yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
          yy_parse_failed(yypParser);
          yymajor = YYNOCODE;
        }else if( yymx!=YYERRORSYMBOL ){
          YYMINORTYPE u2;
          u2.YYERRSYMDT = 0;
          yy_shift(yypParser,yyact,YYERRORSYMBOL,&u2);
        }
      }
      yypParser->yyerrcnt = 3;
      yyerrorhit = 1;
#elif defined(YYNOERRORRECOVERY)
      /* If the YYNOERRORRECOVERY macro is defined, then do not attempt to
      ** do any kind of error recovery.  Instead, simply invoke the syntax
      ** error routine and continue going as if nothing had happened.
      **
      ** Applications can set this macro (for example inside %include) if
      ** they intend to abandon the parse upon the first syntax error seen.
      */
      yy_syntax_error(yypParser,yymajor,yyminorunion);
      yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      yymajor = YYNOCODE;
      
#else  /* YYERRORSYMBOL is not defined */
      /* This is what we do if the grammar does not define ERROR:
      **
      **  * Report an error message, and throw away the input token.
      **
      **  * If the input token is $, then fail the parse.
      **
      ** As before, subsequent error messages are suppressed until
      ** three input tokens have been successfully shifted.
      */
      if( yypParser->yyerrcnt<=0 ){
        yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yypParser->yyerrcnt = 3;
      yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      if( yyendofinput ){
        yy_parse_failed(yypParser);
      }
      yymajor = YYNOCODE;
#endif
    }
  }while( yymajor!=YYNOCODE && yypParser->yyidx>=0 );
  return;
}
