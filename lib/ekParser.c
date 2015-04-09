/* Driver template for the LEMON parser generator.
** The author disclaims copyright to this source code.
*/
/* First off, code is included that follows the "include" declaration
** in the input grammar file. */
#include "ekTypes.h"
#include <stdio.h>
#line 22 "/Users/jdrago/work/eureka/lib/ekParser.y"

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
#line 26 "/Users/jdrago/work/eureka/lib/ekParser.c"
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
#define YYNOCODE 92
#define YYACTIONTYPE unsigned short int
#define ekParseTOKENTYPE  ekToken 
typedef union {
  int yyinit;
  ekParseTOKENTYPE yy0;
  ekSyntax* yy94;
  ekToken yy170;
  int yy183;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define ekParseARG_SDECL  ekCompiler *C ;
#define ekParseARG_PDECL , ekCompiler *C 
#define ekParseARG_FETCH  ekCompiler *C  = yypParser->C 
#define ekParseARG_STORE yypParser->C  = C 
#define YYNSTATE 182
#define YYNRULE 94
#define YYERRORSYMBOL 79
#define YYERRSYMDT yy183
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
 /*     0 */    18,  127,    6,    4,   14,  109,  126,   15,   19,  173,
 /*    10 */   162,  104,   46,  128,   17,   70,   93,   59,   61,  129,
 /*    20 */    64,   52,   54,   53,   56,   55,   47,   51,   49,   50,
 /*    30 */    60,   63,  149,  147,   70,  117,   59,  158,   26,   24,
 /*    40 */    18,  180,    6,    4,   14,  109,  126,   15,  179,  173,
 /*    50 */   167,  104,   25,   19,   17,   70,   93,   59,   61,  129,
 /*    60 */   144,   12,   20,  113,  130,  132,  120,   57,   22,   42,
 /*    70 */   151,  145,  143,  141,  139,  138,  137,  110,   26,   24,
 /*    80 */    21,   38,   29,   39,  175,   25,   32,   33,   31,   36,
 /*    90 */    37,   34,   35,   43,   41,   40,   23,   27,   28,   13,
 /*   100 */   144,   12,   20,  113,  130,   25,  120,  125,  121,   25,
 /*   110 */   151,  145,  143,  141,  139,  138,  137,  110,  182,   18,
 /*   120 */   179,    6,  177,   14,  109,  128,   15,   41,   40,   23,
 /*   130 */    27,   28,   13,   17,   38,   29,   39,   61,  129,   32,
 /*   140 */    33,   31,   36,   37,   34,   35,   43,   41,   40,   23,
 /*   150 */    27,   28,   13,   23,   27,   28,   13,   26,   24,   18,
 /*   160 */   131,    6,    8,   14,  109,  126,   15,    9,  152,   25,
 /*   170 */   104,   58,   62,   17,   70,   93,   59,   61,  129,  144,
 /*   180 */    12,   20,  113,  130,  150,  120,   27,   28,   13,  151,
 /*   190 */   145,  143,  141,  139,  138,  137,  110,   26,   24,   29,
 /*   200 */    39,  119,  121,   32,   33,   31,   36,   37,   34,   35,
 /*   210 */    43,   41,   40,   23,   27,   28,   13,  108,  160,  144,
 /*   220 */    12,   20,  113,  130,  111,  120,   25,   28,   13,  151,
 /*   230 */   145,  143,  141,  139,  138,  137,  110,   18,  164,    6,
 /*   240 */   155,   14,  109,  126,   15,   25,  173,  172,  104,   22,
 /*   250 */    42,   17,   70,   93,   59,   61,  129,   39,  124,    5,
 /*   260 */    32,   33,   31,   36,   37,   34,   35,   43,   41,   40,
 /*   270 */    23,   27,   28,   13,  159,   26,   24,   22,   42,   32,
 /*   280 */    33,   31,   36,   37,   34,   35,   43,   41,   40,   23,
 /*   290 */    27,   28,   13,   88,  117,   59,  169,  144,   12,   20,
 /*   300 */   113,  130,  142,  120,   25,  178,  178,  151,  145,  143,
 /*   310 */   141,  139,  138,  137,  110,   18,  140,    6,  170,   14,
 /*   320 */   109,  126,   15,   25,  173,  168,  104,  153,    3,   17,
 /*   330 */    70,   93,   59,   61,  129,  166,   69,  117,   59,   33,
 /*   340 */    31,   36,   37,   34,   35,   43,   41,   40,   23,   27,
 /*   350 */    28,   13,  101,   26,   24,   58,   70,  117,   59,   31,
 /*   360 */    36,   37,   34,   35,   43,   41,   40,   23,   27,   28,
 /*   370 */    13,   99,  117,   59,  148,  144,   12,   20,  113,  130,
 /*   380 */   107,  120,   25,   13,  171,  151,  145,  143,  141,  139,
 /*   390 */   138,  137,  110,   18,  278,    6,   25,   14,  109,  278,
 /*   400 */    15,   98,  117,   59,   25,  126,  278,   17,  173,  122,
 /*   410 */   104,   61,  129,  278,   70,   93,   59,  278,  116,   36,
 /*   420 */    37,   34,   35,   43,   41,   40,   23,   27,   28,   13,
 /*   430 */   278,   26,   24,  278,   37,   34,   35,   43,   41,   40,
 /*   440 */    23,   27,   28,   13,   35,   43,   41,   40,   23,   27,
 /*   450 */    28,   13,  134,  144,   12,   20,  113,  130,   25,  120,
 /*   460 */    71,  117,   59,  151,  145,  143,  141,  139,  138,  137,
 /*   470 */   110,   18,   25,    6,   16,   14,  109,  278,   15,  136,
 /*   480 */   278,   48,  135,  117,   59,   17,  278,   18,  278,   61,
 /*   490 */   129,   14,  109,  278,   15,  278,   97,  117,   59,   43,
 /*   500 */    41,   40,   23,   27,   28,   13,  129,  278,  278,   26,
 /*   510 */    24,   34,   35,   43,   41,   40,   23,   27,   28,   13,
 /*   520 */   181,  165,   25,   22,   42,   26,   24,  115,  118,   22,
 /*   530 */    42,  144,   12,   20,  113,  130,  278,  120,   65,  117,
 /*   540 */    59,  151,  145,  143,  141,  139,  138,  137,  110,   25,
 /*   550 */   114,  130,  133,  278,  278,   44,   45,  151,  145,  143,
 /*   560 */   141,  139,  138,  137,  110,   40,   23,   27,   28,   13,
 /*   570 */   278,  157,   25,   72,  117,   59,   30,   21,   38,   29,
 /*   580 */    39,   22,   42,   32,   33,   31,   36,   37,   34,   35,
 /*   590 */    43,   41,   40,   23,   27,   28,   13,   25,  278,  126,
 /*   600 */   277,   10,  152,   44,  104,   66,  117,   59,   70,   93,
 /*   610 */    59,  126,  278,   11,  152,  278,  104,   27,   28,   13,
 /*   620 */    70,   93,   59,  278,   30,   21,   38,   29,   39,  278,
 /*   630 */   278,   32,   33,   31,   36,   37,   34,   35,   43,   41,
 /*   640 */    40,   23,   27,   28,   13,   25,  278,  278,  102,  174,
 /*   650 */   278,   44,   70,  117,   59,  278,  126,  278,  278,  173,
 /*   660 */   163,  104,  278,  278,  106,   70,   93,   59,   70,  117,
 /*   670 */    59,  278,   30,   21,   38,   29,   39,  278,  278,   32,
 /*   680 */    33,   31,   36,   37,   34,   35,   43,   41,   40,   23,
 /*   690 */    27,   28,   13,   25,   81,  117,   59,  278,  278,   44,
 /*   700 */   126,  278,  278,  146,    7,  104,   67,  117,   59,   70,
 /*   710 */    93,   59,    1,   80,  117,   59,   70,  117,   59,  278,
 /*   720 */    30,   21,   38,   29,   39,  278,  278,   32,   33,   31,
 /*   730 */    36,   37,   34,   35,   43,   41,   40,   23,   27,   28,
 /*   740 */    13,   25,  278,  278,  103,   79,  117,   59,   70,  117,
 /*   750 */    59,    2,  278,  278,  278,   70,  117,   59,   78,  117,
 /*   760 */    59,   77,  117,   59,   76,  117,   59,  278,   30,   21,
 /*   770 */    38,   29,   39,  278,  278,   32,   33,   31,   36,   37,
 /*   780 */    34,   35,   43,   41,   40,   23,   27,   28,   13,   18,
 /*   790 */    16,  278,  278,   14,  109,  278,   15,   75,  117,   59,
 /*   800 */    74,  117,   59,   73,  117,   59,  278,  278,  129,  112,
 /*   810 */   117,   59,   86,  117,   59,  278,  105,  117,   59,   87,
 /*   820 */   117,   59,  278,  100,  117,   59,  278,   26,   24,   18,
 /*   830 */    68,  117,   59,   14,  109,  278,   15,   83,  117,   59,
 /*   840 */   123,  117,   59,  278,   85,  117,   59,  278,  129,  161,
 /*   850 */    18,  278,  114,  130,   14,  109,  278,   15,  278,  151,
 /*   860 */   145,  143,  141,  139,  138,  137,  110,   26,   24,  129,
 /*   870 */   176,   82,  117,   59,   91,  117,   59,  278,   22,   42,
 /*   880 */   278,  278,   89,  117,   59,  278,  278,  156,   26,   24,
 /*   890 */   278,  278,  114,  130,   90,  117,   59,  278,  278,  151,
 /*   900 */   145,  143,  141,  139,  138,  137,  110,  278,  278,  154,
 /*   910 */   278,   18,   16,  114,  130,   14,  109,  278,   15,  278,
 /*   920 */   151,  145,  143,  141,  139,  138,  137,  110,   18,  278,
 /*   930 */   129,  278,   14,  109,  278,   15,  278,  278,   95,  117,
 /*   940 */    59,   96,  117,   59,   92,  117,   59,  129,  278,   26,
 /*   950 */    24,   94,  117,   59,   84,  117,   59,  278,  278,  278,
 /*   960 */   278,  278,  278,  278,  278,  278,   26,   24,  278,  278,
 /*   970 */   278,  278,  278,  278,  114,  130,  278,  278,  278,  278,
 /*   980 */   278,  151,  145,  143,  141,  139,  138,  137,  110,  278,
 /*   990 */   278,  114,  130,  278,  278,  278,  278,  278,  151,  145,
 /*  1000 */   143,  141,  139,  138,  137,  110,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */     1,   18,    3,    4,    5,    6,   79,    8,   12,   82,
 /*    10 */    83,   84,   68,   69,   15,   88,   89,   90,   19,   20,
 /*    20 */     1,   25,   26,   27,   28,   29,   30,   31,   32,   33,
 /*    30 */    84,   85,   71,   72,   88,   89,   90,   65,   39,   40,
 /*    40 */     1,   61,    3,    4,    5,    6,   79,    8,   65,   82,
 /*    50 */    83,   84,    7,   12,   15,   88,   89,   90,   19,   20,
 /*    60 */    61,   62,   63,   64,   65,   65,   67,    2,   69,   70,
 /*    70 */    71,   72,   73,   74,   75,   76,   77,   78,   39,   40,
 /*    80 */    35,   36,   37,   38,   65,    7,   41,   42,   43,   44,
 /*    90 */    45,   46,   47,   48,   49,   50,   51,   52,   53,   54,
 /*   100 */    61,   62,   63,   64,   65,    7,   67,   86,   87,    7,
 /*   110 */    71,   72,   73,   74,   75,   76,   77,   78,    0,    1,
 /*   120 */    65,    3,   66,    5,    6,   69,    8,   49,   50,   51,
 /*   130 */    52,   53,   54,   15,   36,   37,   38,   19,   20,   41,
 /*   140 */    42,   43,   44,   45,   46,   47,   48,   49,   50,   51,
 /*   150 */    52,   53,   54,   51,   52,   53,   54,   39,   40,    1,
 /*   160 */    65,    3,   66,    5,    6,   79,    8,   81,   82,    7,
 /*   170 */    84,    2,    2,   15,   88,   89,   90,   19,   20,   61,
 /*   180 */    62,   63,   64,   65,   61,   67,   52,   53,   54,   71,
 /*   190 */    72,   73,   74,   75,   76,   77,   78,   39,   40,   37,
 /*   200 */    38,   86,   87,   41,   42,   43,   44,   45,   46,   47,
 /*   210 */    48,   49,   50,   51,   52,   53,   54,   69,   60,   61,
 /*   220 */    62,   63,   64,   65,   87,   67,    7,   53,   54,   71,
 /*   230 */    72,   73,   74,   75,   76,   77,   78,    1,   65,    3,
 /*   240 */    60,    5,    6,   79,    8,    7,   82,   83,   84,   69,
 /*   250 */    70,   15,   88,   89,   90,   19,   20,   38,   18,   66,
 /*   260 */    41,   42,   43,   44,   45,   46,   47,   48,   49,   50,
 /*   270 */    51,   52,   53,   54,   66,   39,   40,   69,   70,   41,
 /*   280 */    42,   43,   44,   45,   46,   47,   48,   49,   50,   51,
 /*   290 */    52,   53,   54,   88,   89,   90,   60,   61,   62,   63,
 /*   300 */    64,   65,   79,   67,    7,   65,   65,   71,   72,   73,
 /*   310 */    74,   75,   76,   77,   78,    1,   61,    3,   79,    5,
 /*   320 */     6,   79,    8,    7,   82,   83,   84,   79,   16,   15,
 /*   330 */    88,   89,   90,   19,   20,   79,   88,   89,   90,   42,
 /*   340 */    43,   44,   45,   46,   47,   48,   49,   50,   51,   52,
 /*   350 */    53,   54,   84,   39,   40,    2,   88,   89,   90,   43,
 /*   360 */    44,   45,   46,   47,   48,   49,   50,   51,   52,   53,
 /*   370 */    54,   88,   89,   90,   60,   61,   62,   63,   64,   65,
 /*   380 */    87,   67,    7,   54,   85,   71,   72,   73,   74,   75,
 /*   390 */    76,   77,   78,    1,   91,    3,    7,    5,    6,   91,
 /*   400 */     8,   88,   89,   90,    7,   79,   91,   15,   82,   83,
 /*   410 */    84,   19,   20,   91,   88,   89,   90,   91,   65,   44,
 /*   420 */    45,   46,   47,   48,   49,   50,   51,   52,   53,   54,
 /*   430 */    91,   39,   40,   91,   45,   46,   47,   48,   49,   50,
 /*   440 */    51,   52,   53,   54,   47,   48,   49,   50,   51,   52,
 /*   450 */    53,   54,   60,   61,   62,   63,   64,   65,    7,   67,
 /*   460 */    88,   89,   90,   71,   72,   73,   74,   75,   76,   77,
 /*   470 */    78,    1,    7,    3,    2,    5,    6,   91,    8,   85,
 /*   480 */    91,    9,   88,   89,   90,   15,   91,    1,   91,   19,
 /*   490 */    20,    5,    6,   91,    8,   91,   88,   89,   90,   48,
 /*   500 */    49,   50,   51,   52,   53,   54,   20,   91,   91,   39,
 /*   510 */    40,   46,   47,   48,   49,   50,   51,   52,   53,   54,
 /*   520 */    66,   61,    7,   69,   70,   39,   40,   55,   56,   69,
 /*   530 */    70,   61,   62,   63,   64,   65,   91,   67,   88,   89,
 /*   540 */    90,   71,   72,   73,   74,   75,   76,   77,   78,    7,
 /*   550 */    64,   65,   66,   91,   91,   13,   14,   71,   72,   73,
 /*   560 */    74,   75,   76,   77,   78,   50,   51,   52,   53,   54,
 /*   570 */    91,   59,    7,   88,   89,   90,   34,   35,   36,   37,
 /*   580 */    38,   69,   70,   41,   42,   43,   44,   45,   46,   47,
 /*   590 */    48,   49,   50,   51,   52,   53,   54,    7,   91,   79,
 /*   600 */    80,   81,   82,   13,   84,   88,   89,   90,   88,   89,
 /*   610 */    90,   79,   91,   81,   82,   91,   84,   52,   53,   54,
 /*   620 */    88,   89,   90,   91,   34,   35,   36,   37,   38,   91,
 /*   630 */    91,   41,   42,   43,   44,   45,   46,   47,   48,   49,
 /*   640 */    50,   51,   52,   53,   54,    7,   91,   91,   84,   59,
 /*   650 */    91,   13,   88,   89,   90,   91,   79,   91,   91,   82,
 /*   660 */    83,   84,   91,   91,   84,   88,   89,   90,   88,   89,
 /*   670 */    90,   91,   34,   35,   36,   37,   38,   91,   91,   41,
 /*   680 */    42,   43,   44,   45,   46,   47,   48,   49,   50,   51,
 /*   690 */    52,   53,   54,    7,   88,   89,   90,   91,   91,   13,
 /*   700 */    79,   91,   91,   82,   66,   84,   88,   89,   90,   88,
 /*   710 */    89,   90,   84,   88,   89,   90,   88,   89,   90,   91,
 /*   720 */    34,   35,   36,   37,   38,   91,   91,   41,   42,   43,
 /*   730 */    44,   45,   46,   47,   48,   49,   50,   51,   52,   53,
 /*   740 */    54,    7,   91,   91,   84,   88,   89,   90,   88,   89,
 /*   750 */    90,   84,   91,   91,   91,   88,   89,   90,   88,   89,
 /*   760 */    90,   88,   89,   90,   88,   89,   90,   91,   34,   35,
 /*   770 */    36,   37,   38,   91,   91,   41,   42,   43,   44,   45,
 /*   780 */    46,   47,   48,   49,   50,   51,   52,   53,   54,    1,
 /*   790 */     2,   91,   91,    5,    6,   91,    8,   88,   89,   90,
 /*   800 */    88,   89,   90,   88,   89,   90,   91,   91,   20,   88,
 /*   810 */    89,   90,   88,   89,   90,   91,   88,   89,   90,   88,
 /*   820 */    89,   90,   91,   88,   89,   90,   91,   39,   40,    1,
 /*   830 */    88,   89,   90,    5,    6,   91,    8,   88,   89,   90,
 /*   840 */    88,   89,   90,   91,   88,   89,   90,   91,   20,   61,
 /*   850 */     1,   91,   64,   65,    5,    6,   91,    8,   91,   71,
 /*   860 */    72,   73,   74,   75,   76,   77,   78,   39,   40,   20,
 /*   870 */    61,   88,   89,   90,   88,   89,   90,   91,   69,   70,
 /*   880 */    91,   91,   88,   89,   90,   91,   91,   59,   39,   40,
 /*   890 */    91,   91,   64,   65,   88,   89,   90,   91,   91,   71,
 /*   900 */    72,   73,   74,   75,   76,   77,   78,   91,   91,   60,
 /*   910 */    91,    1,    2,   64,   65,    5,    6,   91,    8,   91,
 /*   920 */    71,   72,   73,   74,   75,   76,   77,   78,    1,   91,
 /*   930 */    20,   91,    5,    6,   91,    8,   91,   91,   88,   89,
 /*   940 */    90,   88,   89,   90,   88,   89,   90,   20,   91,   39,
 /*   950 */    40,   88,   89,   90,   88,   89,   90,   91,   91,   91,
 /*   960 */    91,   91,   91,   91,   91,   91,   39,   40,   91,   91,
 /*   970 */    91,   91,   91,   91,   64,   65,   91,   91,   91,   91,
 /*   980 */    91,   71,   72,   73,   74,   75,   76,   77,   78,   91,
 /*   990 */    91,   64,   65,   91,   91,   91,   91,   91,   71,   72,
 /*  1000 */    73,   74,   75,   76,   77,   78,
};
#define YY_SHIFT_USE_DFLT (-57)
#define YY_SHIFT_MAX 128
static const short yy_shift_ofst[] = {
 /*     0 */   470,   -1,   -1,   39,  236,   39,  158,   39,   39,  314,
 /*    10 */   118,  392,  788,  910,  849,  828,  486,  927,  927,  927,
 /*    20 */   927,  927,  927,  927,  927,  927,  927,  927,  927,  927,
 /*    30 */   927,  927,  927,  927,  927,  927,  927,  927,  927,  927,
 /*    40 */   927,  927,  927,  927,  927,  927,  927,  927,  927,  927,
 /*    50 */   927,  927,  927,  927,  927,  927,  927,  -17,  -17,  472,
 /*    60 */   460,  -20,   55,  255,   55,  542,  638,  590,  686,  686,
 /*    70 */   686,  686,  734,  734,  734,  734,  734,  734,  734,  734,
 /*    80 */   734,  734,   45,   98,  162,  219,  238,  238,  238,  297,
 /*    90 */   316,  375,  389,   -4,  465,  397,  451,   78,  515,  102,
 /*   100 */   565,  454,  512,  180,  809,  134,  208,  -56,  240,  -39,
 /*   110 */    19,   56,  174,  353,  169,   95,   65,   41,    0,   96,
 /*   120 */   170,  148,  312,  329,  -28,  193,  123,  173,  241,
};
#define YY_REDUCE_USE_DFLT (-74)
#define YY_REDUCE_MAX 64
static const short yy_reduce_ofst[] = {
 /*     0 */   520,  326,  577,  242,  532,  164,   86,  -33,  -73,  621,
 /*    10 */   621,  621,  -54,  394,  660,  564,  580,  628,  268,  248,
 /*    20 */   667,  749,  742,  735,  731,  728,  724,  721,  752,  756,
 /*    30 */   783,  786,  794,  806,  850,  853,  856,  863,  866,  205,
 /*    40 */   283,  313,  372,  408,  450,  485,  517,  606,  618,  625,
 /*    50 */   657,  670,  673,  676,  709,  712,  715,  115,   21,  299,
 /*    60 */   239,  256,  293,  223,  137,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   276,  276,  276,  276,  276,  276,  276,  276,  276,  276,
 /*    10 */   276,  276,  276,  276,  276,  276,  276,  276,  276,  276,
 /*    20 */   276,  276,  276,  276,  276,  276,  276,  276,  276,  276,
 /*    30 */   276,  276,  276,  276,  276,  276,  276,  276,  276,  276,
 /*    40 */   276,  276,  276,  276,  276,  276,  276,  276,  276,  276,
 /*    50 */   276,  276,  276,  276,  276,  276,  276,  272,  272,  260,
 /*    60 */   276,  276,  272,  276,  272,  276,  276,  276,  216,  245,
 /*    70 */   218,  217,  259,  209,  208,  207,  206,  205,  204,  203,
 /*    80 */   202,  201,  226,  236,  234,  225,  220,  219,  235,  228,
 /*    90 */   229,  227,  232,  248,  233,  230,  231,  247,  238,  237,
 /*   100 */   221,  276,  276,  276,  276,  222,  276,  276,  276,  276,
 /*   110 */   276,  276,  223,  276,  276,  276,  276,  248,  276,  276,
 /*   120 */   276,  273,  197,  224,  276,  276,  276,  276,  276,  264,
 /*   130 */   269,  267,  268,  215,  186,  240,  239,  257,  256,  255,
 /*   140 */   193,  254,  194,  253,  210,  251,  183,  252,  212,  250,
 /*   150 */   213,  249,  184,  246,  243,  244,  241,  242,  274,  214,
 /*   160 */   211,  190,  199,  198,  275,  191,  189,  200,  196,  185,
 /*   170 */   192,  265,  258,  187,  266,  261,  195,  262,  270,  271,
 /*   180 */   188,  263,
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
    5,  /* SCOPESTARTBLOCK => MAPSTARTBLOCK */
    3,  /* STARTBLOCK => SCOPESTARTBLOCK */
    0,  /* MAPSTARTBLOCK => nothing */
    0,  /*   NEGATIVE => nothing */
    6,  /*       DASH => NEGATIVE */
    0,  /* ARRAYOPENBRACKET => nothing */
    8,  /* OPENBRACKET => ARRAYOPENBRACKET */
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
  "STARTBLOCK",    "MAPSTARTBLOCK",  "NEGATIVE",      "DASH",        
  "ARRAYOPENBRACKET",  "OPENBRACKET",   "NEWLINE",       "SEMI",        
  "ASSIGN",        "QUESTIONMARK",  "COLON",         "IF",          
  "ELSE",          "HEREDOC",       "ELLIPSIS",      "BREAK",       
  "THIS",          "UNKNOWN",       "COMMENT",       "SPACE",       
  "EOF",           "BITWISE_OREQUALS",  "BITWISE_XOREQUALS",  "BITWISE_ANDEQUALS",
  "SHIFTRIGHTEQUALS",  "SHIFTLEFTEQUALS",  "PLUSEQUALS",    "SLASHEQUALS", 
  "DASHEQUALS",    "STAREQUALS",    "OR",            "BITWISE_OR",  
  "BITWISE_XOR",   "AND",           "BITWISE_AND",   "BITWISE_NOT", 
  "NOT",           "EQUALS",        "NOTEQUALS",     "CMP",         
  "LESSTHAN",      "LESSTHANOREQUAL",  "GREATERTHAN",   "GREATERTHANOREQUAL",
  "INHERITS",      "SHIFTRIGHT",    "SHIFTLEFT",     "PLUS",        
  "STAR",          "SLASH",         "MOD",           "PERIOD",      
  "COLONCOLON",    "OPENBRACE",     "CLOSEBRACE",    "CLOSEBRACKET",
  "ENDBLOCK",      "ENDSTATEMENT",  "RETURN",        "WHILE",       
  "FUNCTION",      "IDENTIFIER",    "RIGHTPAREN",    "FOR",         
  "IN",            "COMMA",         "FATCOMMA",      "INTEGER",     
  "FLOATNUM",      "LITERALSTRING",  "REGEXSTRING",   "TRUE",        
  "FALSE",         "NULL",          "VAR",           "error",       
  "chunk",         "statement_list",  "statement",     "statement_block",
  "expr_list",     "paren_expr_list",  "func_args",     "ident_list",  
  "expression",    "lvalue",        "lvalue_indexable",
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
 /*   7 */ "statement ::= BREAK error",
 /*   8 */ "statement ::= RETURN ENDSTATEMENT",
 /*   9 */ "statement ::= RETURN expr_list ENDSTATEMENT",
 /*  10 */ "statement ::= RETURN expr_list error",
 /*  11 */ "statement ::= RETURN paren_expr_list ENDSTATEMENT",
 /*  12 */ "statement ::= RETURN paren_expr_list error",
 /*  13 */ "statement ::= expr_list ENDSTATEMENT",
 /*  14 */ "statement ::= IF expr_list statement_block ELSE statement_block",
 /*  15 */ "statement ::= IF expr_list statement_block",
 /*  16 */ "statement ::= WHILE expr_list statement_block",
 /*  17 */ "statement ::= FUNCTION IDENTIFIER LEFTPAREN func_args RIGHTPAREN statement_block",
 /*  18 */ "statement ::= FOR LEFTPAREN ident_list IN expression RIGHTPAREN statement_block",
 /*  19 */ "statement ::= lvalue PLUSEQUALS expression",
 /*  20 */ "statement ::= lvalue DASHEQUALS expression",
 /*  21 */ "statement ::= lvalue STAREQUALS expression",
 /*  22 */ "statement ::= lvalue SLASHEQUALS expression",
 /*  23 */ "statement ::= lvalue BITWISE_OREQUALS expression",
 /*  24 */ "statement ::= lvalue BITWISE_ANDEQUALS expression",
 /*  25 */ "statement ::= lvalue BITWISE_XOREQUALS expression",
 /*  26 */ "statement ::= lvalue SHIFTLEFTEQUALS expression",
 /*  27 */ "statement ::= lvalue SHIFTRIGHTEQUALS expression",
 /*  28 */ "statement ::= ENDSTATEMENT",
 /*  29 */ "statement ::= SCOPESTARTBLOCK ENDBLOCK",
 /*  30 */ "statement ::= SCOPESTARTBLOCK statement_list ENDBLOCK",
 /*  31 */ "statement ::= error ENDSTATEMENT",
 /*  32 */ "paren_expr_list ::= LEFTPAREN expr_list RIGHTPAREN",
 /*  33 */ "paren_expr_list ::= LEFTPAREN RIGHTPAREN",
 /*  34 */ "expr_list ::= expr_list COMMA expression",
 /*  35 */ "expr_list ::= expr_list FATCOMMA expression",
 /*  36 */ "expr_list ::= expression",
 /*  37 */ "expression ::= NOT expression",
 /*  38 */ "expression ::= BITWISE_NOT expression",
 /*  39 */ "expression ::= expression PLUS expression",
 /*  40 */ "expression ::= expression DASH expression",
 /*  41 */ "expression ::= expression STAR expression",
 /*  42 */ "expression ::= expression SLASH expression",
 /*  43 */ "expression ::= expression AND expression",
 /*  44 */ "expression ::= expression OR expression",
 /*  45 */ "expression ::= expression CMP expression",
 /*  46 */ "expression ::= expression EQUALS expression",
 /*  47 */ "expression ::= expression NOTEQUALS expression",
 /*  48 */ "expression ::= expression GREATERTHAN expression",
 /*  49 */ "expression ::= expression GREATERTHANOREQUAL expression",
 /*  50 */ "expression ::= expression LESSTHAN expression",
 /*  51 */ "expression ::= expression LESSTHANOREQUAL expression",
 /*  52 */ "expression ::= expression BITWISE_XOR expression",
 /*  53 */ "expression ::= expression BITWISE_AND expression",
 /*  54 */ "expression ::= expression BITWISE_OR expression",
 /*  55 */ "expression ::= expression SHIFTLEFT expression",
 /*  56 */ "expression ::= expression SHIFTRIGHT expression",
 /*  57 */ "expression ::= expression MOD paren_expr_list",
 /*  58 */ "expression ::= expression MOD expression",
 /*  59 */ "expression ::= ARRAYOPENBRACKET CLOSEBRACKET",
 /*  60 */ "expression ::= ARRAYOPENBRACKET expr_list CLOSEBRACKET",
 /*  61 */ "expression ::= MAPSTARTBLOCK ENDBLOCK",
 /*  62 */ "expression ::= MAPSTARTBLOCK expr_list ENDBLOCK",
 /*  63 */ "expression ::= lvalue ASSIGN expression",
 /*  64 */ "expression ::= lvalue ASSIGN error",
 /*  65 */ "expression ::= expression INHERITS expression",
 /*  66 */ "expression ::= lvalue",
 /*  67 */ "expression ::= INTEGER",
 /*  68 */ "expression ::= NEGATIVE INTEGER",
 /*  69 */ "expression ::= FLOATNUM",
 /*  70 */ "expression ::= NEGATIVE FLOATNUM",
 /*  71 */ "expression ::= LITERALSTRING",
 /*  72 */ "expression ::= REGEXSTRING",
 /*  73 */ "expression ::= TRUE",
 /*  74 */ "expression ::= FALSE",
 /*  75 */ "expression ::= NULL",
 /*  76 */ "expression ::= FUNCTION LEFTPAREN func_args RIGHTPAREN statement_block",
 /*  77 */ "expression ::= expression QUESTIONMARK expression COLON expression",
 /*  78 */ "lvalue ::= lvalue_indexable",
 /*  79 */ "lvalue ::= VAR IDENTIFIER",
 /*  80 */ "lvalue ::= VAR GROUPLEFTPAREN ident_list RIGHTPAREN",
 /*  81 */ "lvalue ::= GROUPLEFTPAREN expr_list RIGHTPAREN",
 /*  82 */ "lvalue_indexable ::= THIS",
 /*  83 */ "lvalue_indexable ::= lvalue_indexable paren_expr_list",
 /*  84 */ "lvalue_indexable ::= lvalue_indexable OPENBRACKET expression CLOSEBRACKET",
 /*  85 */ "lvalue_indexable ::= lvalue_indexable PERIOD IDENTIFIER",
 /*  86 */ "lvalue_indexable ::= lvalue_indexable COLONCOLON IDENTIFIER",
 /*  87 */ "lvalue_indexable ::= IDENTIFIER",
 /*  88 */ "ident_list ::= ident_list COMMA IDENTIFIER",
 /*  89 */ "ident_list ::= IDENTIFIER",
 /*  90 */ "ident_list ::=",
 /*  91 */ "func_args ::= ident_list",
 /*  92 */ "func_args ::= ident_list COMMA ELLIPSIS IDENTIFIER",
 /*  93 */ "func_args ::= ELLIPSIS IDENTIFIER",
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
void *ekParseAlloc(struct ekContext *E){
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
    case 81: /* statement_list */
    case 82: /* statement */
    case 83: /* statement_block */
    case 84: /* expr_list */
    case 85: /* paren_expr_list */
    case 87: /* ident_list */
    case 88: /* expression */
    case 89: /* lvalue */
    case 90: /* lvalue_indexable */
{
#line 132 "/Users/jdrago/work/eureka/lib/ekParser.y"
 ekSyntaxDestroy(C->E, (yypminor->yy94)); 
#line 738 "/Users/jdrago/work/eureka/lib/ekParser.c"
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
void ekParseFree(struct ekContext *E,
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
  { 80, 1 },
  { 81, 2 },
  { 81, 1 },
  { 83, 2 },
  { 83, 3 },
  { 83, 1 },
  { 82, 2 },
  { 82, 2 },
  { 82, 2 },
  { 82, 3 },
  { 82, 3 },
  { 82, 3 },
  { 82, 3 },
  { 82, 2 },
  { 82, 5 },
  { 82, 3 },
  { 82, 3 },
  { 82, 6 },
  { 82, 7 },
  { 82, 3 },
  { 82, 3 },
  { 82, 3 },
  { 82, 3 },
  { 82, 3 },
  { 82, 3 },
  { 82, 3 },
  { 82, 3 },
  { 82, 3 },
  { 82, 1 },
  { 82, 2 },
  { 82, 3 },
  { 82, 2 },
  { 85, 3 },
  { 85, 2 },
  { 84, 3 },
  { 84, 3 },
  { 84, 1 },
  { 88, 2 },
  { 88, 2 },
  { 88, 3 },
  { 88, 3 },
  { 88, 3 },
  { 88, 3 },
  { 88, 3 },
  { 88, 3 },
  { 88, 3 },
  { 88, 3 },
  { 88, 3 },
  { 88, 3 },
  { 88, 3 },
  { 88, 3 },
  { 88, 3 },
  { 88, 3 },
  { 88, 3 },
  { 88, 3 },
  { 88, 3 },
  { 88, 3 },
  { 88, 3 },
  { 88, 3 },
  { 88, 2 },
  { 88, 3 },
  { 88, 2 },
  { 88, 3 },
  { 88, 3 },
  { 88, 3 },
  { 88, 3 },
  { 88, 1 },
  { 88, 1 },
  { 88, 2 },
  { 88, 1 },
  { 88, 2 },
  { 88, 1 },
  { 88, 1 },
  { 88, 1 },
  { 88, 1 },
  { 88, 1 },
  { 88, 5 },
  { 88, 5 },
  { 89, 1 },
  { 89, 2 },
  { 89, 4 },
  { 89, 3 },
  { 90, 1 },
  { 90, 2 },
  { 90, 4 },
  { 90, 3 },
  { 90, 3 },
  { 90, 1 },
  { 87, 3 },
  { 87, 1 },
  { 87, 0 },
  { 86, 1 },
  { 86, 4 },
  { 86, 2 },
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
#line 123 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ C->root = yymsp[0].minor.yy94; }
#line 1117 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 1: /* statement_list ::= statement_list statement */
#line 135 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxListAppend(C->E, yymsp[-1].minor.yy94, yymsp[0].minor.yy94, 0); }
#line 1122 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 2: /* statement_list ::= statement */
      case 5: /* statement_block ::= statement */ yytestcase(yyruleno==5);
#line 138 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateList(C->E, EST_STATEMENTLIST, yymsp[0].minor.yy94); }
#line 1128 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 3: /* statement_block ::= STARTBLOCK ENDBLOCK */
      case 28: /* statement ::= ENDSTATEMENT */ yytestcase(yyruleno==28);
      case 29: /* statement ::= SCOPESTARTBLOCK ENDBLOCK */ yytestcase(yyruleno==29);
#line 150 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateList(C->E, EST_STATEMENTLIST, NULL); }
#line 1135 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 4: /* statement_block ::= STARTBLOCK statement_list ENDBLOCK */
      case 32: /* paren_expr_list ::= LEFTPAREN expr_list RIGHTPAREN */ yytestcase(yyruleno==32);
      case 81: /* lvalue ::= GROUPLEFTPAREN expr_list RIGHTPAREN */ yytestcase(yyruleno==81);
#line 153 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = yymsp[-1].minor.yy94; }
#line 1142 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 6: /* statement ::= BREAK ENDSTATEMENT */
#line 168 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBreak(C->E, yymsp[-1].minor.yy0.line); }
#line 1147 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 7: /* statement ::= BREAK error */
      case 10: /* statement ::= RETURN expr_list error */ yytestcase(yyruleno==10);
      case 12: /* statement ::= RETURN paren_expr_list error */ yytestcase(yyruleno==12);
#line 171 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ ekCompileExplainError(C, "expected ;"); }
#line 1154 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 8: /* statement ::= RETURN ENDSTATEMENT */
#line 174 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateReturn(C->E, yymsp[-1].minor.yy0.line, NULL); }
#line 1159 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 9: /* statement ::= RETURN expr_list ENDSTATEMENT */
      case 11: /* statement ::= RETURN paren_expr_list ENDSTATEMENT */ yytestcase(yyruleno==11);
#line 177 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateReturn(C->E, yymsp[-1].minor.yy94->line, yymsp[-1].minor.yy94); }
#line 1165 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 13: /* statement ::= expr_list ENDSTATEMENT */
#line 189 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateStatementExpr(C->E, yymsp[-1].minor.yy94); }
#line 1170 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 14: /* statement ::= IF expr_list statement_block ELSE statement_block */
#line 192 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateIfElse(C->E, yymsp[-3].minor.yy94, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1175 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 15: /* statement ::= IF expr_list statement_block */
#line 195 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateIfElse(C->E, yymsp[-1].minor.yy94, yymsp[0].minor.yy94, NULL, ekFalse); }
#line 1180 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 16: /* statement ::= WHILE expr_list statement_block */
#line 198 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateWhile(C->E, yymsp[-1].minor.yy94, yymsp[0].minor.yy94); }
#line 1185 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 17: /* statement ::= FUNCTION IDENTIFIER LEFTPAREN func_args RIGHTPAREN statement_block */
#line 201 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateFunctionDecl(C->E, &yymsp[-4].minor.yy0, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, yymsp[-5].minor.yy0.line); }
#line 1190 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 18: /* statement ::= FOR LEFTPAREN ident_list IN expression RIGHTPAREN statement_block */
#line 204 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateFor(C->E, yymsp[-4].minor.yy94, yymsp[-2].minor.yy94, yymsp[0].minor.yy94); }
#line 1195 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 19: /* statement ::= lvalue PLUSEQUALS expression */
#line 207 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_ADD, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1200 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 20: /* statement ::= lvalue DASHEQUALS expression */
#line 210 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_SUB, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1205 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 21: /* statement ::= lvalue STAREQUALS expression */
#line 213 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_MUL, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1210 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 22: /* statement ::= lvalue SLASHEQUALS expression */
#line 216 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_DIV, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1215 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 23: /* statement ::= lvalue BITWISE_OREQUALS expression */
#line 219 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_BITWISE_OR, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1220 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 24: /* statement ::= lvalue BITWISE_ANDEQUALS expression */
#line 222 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_BITWISE_AND, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1225 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 25: /* statement ::= lvalue BITWISE_XOREQUALS expression */
#line 225 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_BITWISE_XOR, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1230 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 26: /* statement ::= lvalue SHIFTLEFTEQUALS expression */
#line 228 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_SHIFTLEFT, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1235 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 27: /* statement ::= lvalue SHIFTRIGHTEQUALS expression */
#line 231 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_SHIFTRIGHT, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1240 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 30: /* statement ::= SCOPESTARTBLOCK statement_list ENDBLOCK */
#line 240 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateScope(C->E, yymsp[-1].minor.yy94); }
#line 1245 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 33: /* paren_expr_list ::= LEFTPAREN RIGHTPAREN */
#line 257 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateList(C->E, EST_EXPRESSIONLIST, NULL); }
#line 1250 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 34: /* expr_list ::= expr_list COMMA expression */
#line 269 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxListAppend(C->E, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, 0); }
#line 1255 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 35: /* expr_list ::= expr_list FATCOMMA expression */
#line 272 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxListAppend(C->E, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ESLF_AUTOLITERAL); }
#line 1260 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 36: /* expr_list ::= expression */
#line 275 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateList(C->E, EST_EXPRESSIONLIST, yymsp[0].minor.yy94); }
#line 1265 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 37: /* expression ::= NOT expression */
#line 287 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateUnary(C->E, EST_NOT, yymsp[0].minor.yy94, yymsp[0].minor.yy94->line); }
#line 1270 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 38: /* expression ::= BITWISE_NOT expression */
#line 290 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateUnary(C->E, EST_BITWISE_NOT, yymsp[0].minor.yy94, yymsp[0].minor.yy94->line); }
#line 1275 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 39: /* expression ::= expression PLUS expression */
#line 293 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_ADD, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1280 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 40: /* expression ::= expression DASH expression */
#line 296 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_SUB, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1285 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 41: /* expression ::= expression STAR expression */
#line 299 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_MUL, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1290 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 42: /* expression ::= expression SLASH expression */
#line 302 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_DIV, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1295 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 43: /* expression ::= expression AND expression */
#line 305 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_AND, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1300 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 44: /* expression ::= expression OR expression */
#line 308 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_OR, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1305 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 45: /* expression ::= expression CMP expression */
#line 311 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_CMP, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1310 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 46: /* expression ::= expression EQUALS expression */
#line 314 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_EQUALS, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1315 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 47: /* expression ::= expression NOTEQUALS expression */
#line 317 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_NOTEQUALS, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1320 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 48: /* expression ::= expression GREATERTHAN expression */
#line 320 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_GREATERTHAN, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1325 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 49: /* expression ::= expression GREATERTHANOREQUAL expression */
#line 323 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_GREATERTHANOREQUAL, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1330 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 50: /* expression ::= expression LESSTHAN expression */
#line 326 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_LESSTHAN, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1335 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 51: /* expression ::= expression LESSTHANOREQUAL expression */
#line 329 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_LESSTHANOREQUAL, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1340 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 52: /* expression ::= expression BITWISE_XOR expression */
#line 332 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_BITWISE_XOR, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1345 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 53: /* expression ::= expression BITWISE_AND expression */
#line 335 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_BITWISE_AND, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1350 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 54: /* expression ::= expression BITWISE_OR expression */
#line 338 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_BITWISE_OR, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1355 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 55: /* expression ::= expression SHIFTLEFT expression */
#line 341 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_SHIFTLEFT, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1360 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 56: /* expression ::= expression SHIFTRIGHT expression */
#line 344 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_SHIFTRIGHT, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1365 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 57: /* expression ::= expression MOD paren_expr_list */
      case 58: /* expression ::= expression MOD expression */ yytestcase(yyruleno==58);
#line 347 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateStringFormat(C->E, yymsp[-2].minor.yy94, yymsp[0].minor.yy94); }
#line 1371 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 59: /* expression ::= ARRAYOPENBRACKET CLOSEBRACKET */
#line 353 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateUnary(C->E, EST_ARRAY, NULL, yymsp[-1].minor.yy0.line); }
#line 1376 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 60: /* expression ::= ARRAYOPENBRACKET expr_list CLOSEBRACKET */
#line 356 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateUnary(C->E, EST_ARRAY, yymsp[-1].minor.yy94, yymsp[-1].minor.yy94->line); }
#line 1381 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 61: /* expression ::= MAPSTARTBLOCK ENDBLOCK */
#line 359 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateUnary(C->E, EST_MAP, NULL, yymsp[-1].minor.yy0.line); }
#line 1386 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 62: /* expression ::= MAPSTARTBLOCK expr_list ENDBLOCK */
#line 362 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateUnary(C->E, EST_MAP, yymsp[-1].minor.yy94, yymsp[-1].minor.yy94->line); }
#line 1391 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 63: /* expression ::= lvalue ASSIGN expression */
#line 365 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateAssignment(C->E, yymsp[-2].minor.yy94, yymsp[0].minor.yy94); }
#line 1396 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 64: /* expression ::= lvalue ASSIGN error */
#line 368 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ ekCompileExplainError(C, "assignment expected expression"); }
#line 1401 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 65: /* expression ::= expression INHERITS expression */
#line 371 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateInherits(C->E, yymsp[-2].minor.yy94, yymsp[0].minor.yy94); }
#line 1406 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 66: /* expression ::= lvalue */
      case 78: /* lvalue ::= lvalue_indexable */ yytestcase(yyruleno==78);
#line 374 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = yymsp[0].minor.yy94; }
#line 1412 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 67: /* expression ::= INTEGER */
#line 377 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateKInt(C->E, &yymsp[0].minor.yy0, 0); }
#line 1417 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 68: /* expression ::= NEGATIVE INTEGER */
#line 380 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateKInt(C->E, &yymsp[0].minor.yy0, CKO_NEGATIVE); }
#line 1422 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 69: /* expression ::= FLOATNUM */
#line 383 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateKFloat(C->E, &yymsp[0].minor.yy0, 0); }
#line 1427 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 70: /* expression ::= NEGATIVE FLOATNUM */
#line 386 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateKFloat(C->E, &yymsp[0].minor.yy0, CKO_NEGATIVE); }
#line 1432 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 71: /* expression ::= LITERALSTRING */
#line 389 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateKString(C->E, &yymsp[0].minor.yy0, 0); }
#line 1437 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 72: /* expression ::= REGEXSTRING */
#line 392 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateKString(C->E, &yymsp[0].minor.yy0, 1); }
#line 1442 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 73: /* expression ::= TRUE */
#line 395 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBool(C->E, &yymsp[0].minor.yy0, ekTrue); }
#line 1447 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 74: /* expression ::= FALSE */
#line 398 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBool(C->E, &yymsp[0].minor.yy0, ekFalse); }
#line 1452 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 75: /* expression ::= NULL */
#line 401 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateNull(C->E, yymsp[0].minor.yy0.line); }
#line 1457 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 76: /* expression ::= FUNCTION LEFTPAREN func_args RIGHTPAREN statement_block */
#line 404 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateFunctionDecl(C->E, NULL, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, yymsp[-4].minor.yy0.line); }
#line 1462 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 77: /* expression ::= expression QUESTIONMARK expression COLON expression */
#line 407 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateIfElse(C->E, yymsp[-4].minor.yy94, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1467 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 79: /* lvalue ::= VAR IDENTIFIER */
#line 423 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxMarkVar(C->E, ekSyntaxCreateList(C->E, EST_IDENTIFIERLIST, ekSyntaxCreateIdentifier(C->E, &yymsp[0].minor.yy0))); }
#line 1472 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 80: /* lvalue ::= VAR GROUPLEFTPAREN ident_list RIGHTPAREN */
#line 426 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxMarkVar(C->E, yymsp[-1].minor.yy94); }
#line 1477 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 82: /* lvalue_indexable ::= THIS */
#line 441 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateThis(C->E, yymsp[0].minor.yy0.line); }
#line 1482 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 83: /* lvalue_indexable ::= lvalue_indexable paren_expr_list */
#line 444 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateCall(C->E, yymsp[-1].minor.yy94, yymsp[0].minor.yy94); }
#line 1487 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 84: /* lvalue_indexable ::= lvalue_indexable OPENBRACKET expression CLOSEBRACKET */
#line 447 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateIndex(C->E, yymsp[-3].minor.yy94, yymsp[-1].minor.yy94, ekFalse); }
#line 1492 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 85: /* lvalue_indexable ::= lvalue_indexable PERIOD IDENTIFIER */
#line 450 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateIndex(C->E, yymsp[-2].minor.yy94, ekSyntaxCreateKString(C->E, &yymsp[0].minor.yy0, 0), ekFalse); }
#line 1497 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 86: /* lvalue_indexable ::= lvalue_indexable COLONCOLON IDENTIFIER */
#line 453 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateIndex(C->E, yymsp[-2].minor.yy94, ekSyntaxCreateKString(C->E, &yymsp[0].minor.yy0, 0), ekTrue); }
#line 1502 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 87: /* lvalue_indexable ::= IDENTIFIER */
#line 456 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateIdentifier(C->E, &yymsp[0].minor.yy0); }
#line 1507 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 88: /* ident_list ::= ident_list COMMA IDENTIFIER */
#line 468 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxListAppend(C->E, yymsp[-2].minor.yy94, ekSyntaxCreateIdentifier(C->E, &yymsp[0].minor.yy0), 0); }
#line 1512 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 89: /* ident_list ::= IDENTIFIER */
#line 471 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateList(C->E, EST_IDENTIFIERLIST, ekSyntaxCreateIdentifier(C->E, &yymsp[0].minor.yy0)); }
#line 1517 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 90: /* ident_list ::= */
#line 474 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateList(C->E, EST_IDENTIFIERLIST, NULL); }
#line 1522 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 91: /* func_args ::= ident_list */
#line 483 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateFunctionArgs(C->E, yymsp[0].minor.yy94, NULL); }
#line 1527 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 92: /* func_args ::= ident_list COMMA ELLIPSIS IDENTIFIER */
#line 486 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateFunctionArgs(C->E, yymsp[-3].minor.yy94, &yymsp[0].minor.yy0); }
#line 1532 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 93: /* func_args ::= ELLIPSIS IDENTIFIER */
#line 489 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateFunctionArgs(C->E, ekSyntaxCreateList(C->E, EST_IDENTIFIERLIST, NULL), &yymsp[0].minor.yy0); }
#line 1537 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      default:
      /* (31) statement ::= error ENDSTATEMENT */ yytestcase(yyruleno==31);
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
#line 117 "/Users/jdrago/work/eureka/lib/ekParser.y"
 ekCompileSyntaxError(C, &TOKEN, "Syntax Error"); 
#line 1602 "/Users/jdrago/work/eureka/lib/ekParser.c"
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
