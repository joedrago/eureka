/* Driver template for the LEMON parser generator.
** The author disclaims copyright to this source code.
*/
/* First off, code is included that follows the "include" declaration
** in the input grammar file. */
#include "ekTypes.h"
#include <stdio.h>
#line 22 "/Users/jdrago/w2/eureka/lib/ekParser.y"

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
#line 26 "/Users/jdrago/w2/eureka/lib/ekParser.c"
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
#define YYNOCODE 93
#define YYACTIONTYPE unsigned short int
#define ekParseTOKENTYPE  ekToken 
typedef union {
  int yyinit;
  ekParseTOKENTYPE yy0;
  ekToken yy94;
  ekSyntax* yy114;
  int yy185;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define ekParseARG_SDECL  ekCompiler *C ;
#define ekParseARG_PDECL , ekCompiler *C 
#define ekParseARG_FETCH  ekCompiler *C  = yypParser->C 
#define ekParseARG_STORE yypParser->C  = C 
#define YYNSTATE 185
#define YYNRULE 97
#define YYERRORSYMBOL 80
#define YYERRSYMDT yy185
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
 /*     0 */    20,  175,    6,    5,   13,  113,   52,   16,   21,   56,
 /*    10 */    42,   52,   45,  170,   18,   92,  118,   60,   67,  167,
 /*    20 */   117,   50,   47,   48,   34,   41,   51,   54,   57,   44,
 /*    30 */    86,  118,   60,   36,   30,   27,   35,   29,   39,   24,
 /*    40 */    38,   31,   37,   25,   26,   22,   23,   43,   55,   53,
 /*    50 */    28,   32,   33,   15,   53,   28,   32,   33,   15,  179,
 /*    60 */    17,  144,   12,   19,  114,  168,  169,   65,  164,   52,
 /*    70 */    59,  137,  134,  133,  132,  130,  129,  184,  109,   20,
 /*    80 */    66,    6,    5,   13,  113,  125,   16,  180,  176,  178,
 /*    90 */    62,   52,   21,   18,   72,   95,   60,   67,  167,   35,
 /*   100 */    29,   49,  116,   38,   31,   37,   25,   26,   22,   23,
 /*   110 */    43,   55,   53,   28,   32,   33,   15,   39,   24,   30,
 /*   120 */    27,   35,   29,  127,  128,   38,   31,   37,   25,   26,
 /*   130 */    22,   23,   43,   55,   53,   28,   32,   33,   15,   17,
 /*   140 */   144,   12,   19,  114,  168,  160,   65,   52,   56,   42,
 /*   150 */   137,  134,  133,  132,  130,  129,  184,  109,   20,  152,
 /*   160 */     6,    5,   13,  113,  140,   16,  102,  118,   60,   52,
 /*   170 */   162,  172,   18,  116,   56,   42,   67,  167,   29,   56,
 /*   180 */    42,   38,   31,   37,   25,   26,   22,   23,   43,   55,
 /*   190 */    53,   28,   32,   33,   15,  111,   39,   24,   27,   35,
 /*   200 */    29,  136,  135,   38,   31,   37,   25,   26,   22,   23,
 /*   210 */    43,   55,   53,   28,   32,   33,   15,  138,   17,  144,
 /*   220 */    12,   19,  114,  168,   52,   65,   73,  118,   60,  137,
 /*   230 */   134,  133,  132,  130,  129,  184,  109,   20,  142,    6,
 /*   240 */   163,   13,  113,  125,   16,   52,  176,  123,   62,   56,
 /*   250 */    42,   18,   72,   95,   60,   67,  167,  151,   38,   31,
 /*   260 */    37,   25,   26,   22,   23,   43,   55,   53,   28,   32,
 /*   270 */    33,   15,  164,   61,   64,   39,   24,   72,  118,   60,
 /*   280 */    31,   37,   25,   26,   22,   23,   43,   55,   53,   28,
 /*   290 */    32,   33,   15,   52,  126,  128,  149,   17,  144,   12,
 /*   300 */    19,  114,  168,  165,   65,   52,   33,   15,  137,  134,
 /*   310 */   133,  132,  130,  129,  184,  109,  185,   20,  171,    6,
 /*   320 */    52,   13,  113,  125,   16,   63,  176,  154,   62,   52,
 /*   330 */   150,   18,   72,   95,   60,   67,  167,   28,   32,   33,
 /*   340 */    15,   37,   25,   26,   22,   23,   43,   55,   53,   28,
 /*   350 */    32,   33,   15,  173,  174,   39,   24,   25,   26,   22,
 /*   360 */    23,   43,   55,   53,   28,   32,   33,   15,   22,   23,
 /*   370 */    43,   55,   53,   28,   32,   33,   15,   17,  144,   12,
 /*   380 */    19,  114,  168,  161,   65,   52,    7,  147,  137,  134,
 /*   390 */   133,  132,  130,  129,  184,  109,   20,   52,    6,    8,
 /*   400 */    13,  113,  166,   16,   52,   56,   42,   58,  125,    4,
 /*   410 */    18,  176,  183,   62,   67,  167,  110,   72,   95,   60,
 /*   420 */    32,   33,   15,   26,   22,   23,   43,   55,   53,   28,
 /*   430 */    32,   33,   15,   15,   39,   24,  284,   23,   43,   55,
 /*   440 */    53,   28,   32,   33,   15,   43,   55,   53,   28,   32,
 /*   450 */    33,   15,   89,  118,   60,  146,   17,  144,   12,   19,
 /*   460 */   114,  168,  284,   65,   80,  118,   60,  137,  134,  133,
 /*   470 */   132,  130,  129,  184,  109,   20,  284,    6,  284,   13,
 /*   480 */   113,  125,   16,    9,  148,  115,   62,  284,  284,   18,
 /*   490 */    72,   95,   60,   67,  167,  125,  283,   10,  148,  284,
 /*   500 */    62,   71,  118,   60,   72,   95,   60,  125,  284,  284,
 /*   510 */   176,  181,   62,   39,   24,  284,   72,   95,   60,  125,
 /*   520 */   284,  108,  176,  131,   62,   72,  118,   60,   72,   95,
 /*   530 */    60,  284,   52,  163,  182,   17,  144,   12,   19,  114,
 /*   540 */   168,  284,   65,   97,  118,   60,  137,  134,  133,  132,
 /*   550 */   130,  129,  184,  109,   20,  284,    6,  284,   13,  113,
 /*   560 */   125,   16,   11,  148,   52,   62,  284,  284,   18,   72,
 /*   570 */    95,   60,   67,  167,   55,   53,   28,   32,   33,   15,
 /*   580 */   143,  284,   59,   56,   42,  125,  284,  284,  159,  284,
 /*   590 */    62,  284,   39,   24,   72,   95,   60,  157,  145,  105,
 /*   600 */   156,  118,   60,   72,  118,   60,   56,   42,  284,   32,
 /*   610 */    33,   15,  284,  153,   17,  144,   12,   19,  114,  168,
 /*   620 */   284,   65,   98,  118,   60,  137,  134,  133,  132,  130,
 /*   630 */   129,  184,  109,   20,  284,    6,  284,   13,  113,    2,
 /*   640 */    16,  284,  284,   72,  118,   60,  124,   18,  284,   20,
 /*   650 */   284,   67,  167,   13,  113,    3,   16,  284,  103,   72,
 /*   660 */   118,   60,   72,  118,   60,   88,  118,   60,  167,  284,
 /*   670 */   284,   39,   24,  284,  106,   14,  284,  284,   72,  118,
 /*   680 */    60,  284,   40,  284,  284,  104,  284,   39,   24,   72,
 /*   690 */   118,   60,  284,   17,  144,   12,   19,  114,  168,  284,
 /*   700 */    65,   94,  118,   60,  137,  134,  133,  132,  130,  129,
 /*   710 */   184,  109,   52,  119,  168,  155,  284,  284,   45,  284,
 /*   720 */   137,  134,  133,  132,  130,  129,  184,  109,  120,  122,
 /*   730 */    96,  118,   60,  100,  118,   60,  284,  284,  284,   36,
 /*   740 */    30,   27,   35,   29,  284,  284,   38,   31,   37,   25,
 /*   750 */    26,   22,   23,   43,   55,   53,   28,   32,   33,   15,
 /*   760 */    52,   81,  118,   60,  158,  284,   45,   46,  101,  118,
 /*   770 */    60,  284,  107,  118,   60,   74,  118,   60,   82,  118,
 /*   780 */    60,    1,  118,   60,   83,  118,   60,   36,   30,   27,
 /*   790 */    35,   29,  284,  284,   38,   31,   37,   25,   26,   22,
 /*   800 */    23,   43,   55,   53,   28,   32,   33,   15,   52,  284,
 /*   810 */    75,  118,   60,  284,   45,   76,  118,   60,   68,  118,
 /*   820 */    60,   79,  118,   60,   99,  118,   60,  284,  284,   70,
 /*   830 */   118,   60,   78,  118,   60,   36,   30,   27,   35,   29,
 /*   840 */   284,  284,   38,   31,   37,   25,   26,   22,   23,   43,
 /*   850 */    55,   53,   28,   32,   33,   15,   52,   69,  118,   60,
 /*   860 */    90,  118,   60,   91,  118,   60,   93,  118,   60,   84,
 /*   870 */   118,   60,   87,  118,   60,   77,  118,   60,  121,  118,
 /*   880 */    60,  284,  284,   36,   30,   27,   35,   29,  284,  284,
 /*   890 */    38,   31,   37,   25,   26,   22,   23,   43,   55,   53,
 /*   900 */    28,   32,   33,   15,   20,   14,  284,  284,   13,  113,
 /*   910 */   284,   16,  112,  118,   60,   85,  118,   60,  284,  284,
 /*   920 */   284,  284,  284,  167,  284,   20,   14,  284,  284,   13,
 /*   930 */   113,  284,   16,  284,  284,  284,  284,  284,  284,  284,
 /*   940 */   284,  284,   39,   24,  167,  284,  284,  284,  284,  284,
 /*   950 */   284,  284,  284,  284,  284,  284,  284,  284,  284,  284,
 /*   960 */   284,  284,  284,   39,   24,  177,   20,  284,  119,  168,
 /*   970 */    13,  113,  284,   16,  284,  137,  134,  133,  132,  130,
 /*   980 */   129,  184,  109,  284,  284,  167,  284,   20,  284,  119,
 /*   990 */   168,   13,  113,  284,   16,  284,  137,  134,  133,  132,
 /*  1000 */   130,  129,  184,  109,   39,   24,  167,  284,  284,  284,
 /*  1010 */   284,  284,  284,  284,  284,  284,  284,  284,  284,  284,
 /*  1020 */   284,  284,  284,  284,  141,   39,   24,  284,   20,  284,
 /*  1030 */   119,  168,   13,  113,  284,   16,  284,  137,  134,  133,
 /*  1040 */   132,  130,  129,  184,  109,  284,  139,  167,  284,  284,
 /*  1050 */   284,  119,  168,  284,  284,  284,  284,  284,  137,  134,
 /*  1060 */   133,  132,  130,  129,  184,  109,   39,   24,  284,  284,
 /*  1070 */   284,  284,  284,  284,  284,  284,  284,  284,  284,  284,
 /*  1080 */   284,  284,  284,  284,  284,  284,  284,  284,  284,  284,
 /*  1090 */   284,  284,  119,  168,  284,  284,  284,  284,  284,  137,
 /*  1100 */   134,  133,  132,  130,  129,  184,  109,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */     1,   62,    3,    4,    5,    6,    7,    8,   12,   70,
 /*    10 */    71,    7,   13,   66,   15,   89,   90,   91,   19,   20,
 /*    20 */    18,   25,   26,   27,   28,   29,   30,   31,   32,   33,
 /*    30 */    89,   90,   91,   34,   35,   36,   37,   38,   39,   40,
 /*    40 */    41,   42,   43,   44,   45,   46,   47,   48,   49,   50,
 /*    50 */    51,   52,   53,   54,   50,   51,   52,   53,   54,   80,
 /*    60 */    61,   62,   63,   64,   65,   66,   66,   68,   66,    7,
 /*    70 */     2,   72,   73,   74,   75,   76,   77,   78,   79,    1,
 /*    80 */     1,    3,    4,    5,    6,   80,    8,   62,   83,   84,
 /*    90 */    85,    7,   12,   15,   89,   90,   91,   19,   20,   37,
 /*   100 */    38,   69,   70,   41,   42,   43,   44,   45,   46,   47,
 /*   110 */    48,   49,   50,   51,   52,   53,   54,   39,   40,   35,
 /*   120 */    36,   37,   38,   87,   88,   41,   42,   43,   44,   45,
 /*   130 */    46,   47,   48,   49,   50,   51,   52,   53,   54,   61,
 /*   140 */    62,   63,   64,   65,   66,   66,   68,    7,   70,   71,
 /*   150 */    72,   73,   74,   75,   76,   77,   78,   79,    1,   66,
 /*   160 */     3,    4,    5,    6,   60,    8,   89,   90,   91,    7,
 /*   170 */    67,   62,   15,   70,   70,   71,   19,   20,   38,   70,
 /*   180 */    71,   41,   42,   43,   44,   45,   46,   47,   48,   49,
 /*   190 */    50,   51,   52,   53,   54,   88,   39,   40,   36,   37,
 /*   200 */    38,   72,   73,   41,   42,   43,   44,   45,   46,   47,
 /*   210 */    48,   49,   50,   51,   52,   53,   54,   80,   61,   62,
 /*   220 */    63,   64,   65,   66,    7,   68,   89,   90,   91,   72,
 /*   230 */    73,   74,   75,   76,   77,   78,   79,    1,   59,    3,
 /*   240 */    66,    5,    6,   80,    8,    7,   83,   84,   85,   70,
 /*   250 */    71,   15,   89,   90,   91,   19,   20,   66,   41,   42,
 /*   260 */    43,   44,   45,   46,   47,   48,   49,   50,   51,   52,
 /*   270 */    53,   54,   66,   85,   86,   39,   40,   89,   90,   91,
 /*   280 */    42,   43,   44,   45,   46,   47,   48,   49,   50,   51,
 /*   290 */    52,   53,   54,    7,   87,   88,   60,   61,   62,   63,
 /*   300 */    64,   65,   66,   80,   68,    7,   53,   54,   72,   73,
 /*   310 */    74,   75,   76,   77,   78,   79,    0,    1,   62,    3,
 /*   320 */     7,    5,    6,   80,    8,   88,   83,   84,   85,    7,
 /*   330 */    80,   15,   89,   90,   91,   19,   20,   51,   52,   53,
 /*   340 */    54,   43,   44,   45,   46,   47,   48,   49,   50,   51,
 /*   350 */    52,   53,   54,   80,   80,   39,   40,   44,   45,   46,
 /*   360 */    47,   48,   49,   50,   51,   52,   53,   54,   46,   47,
 /*   370 */    48,   49,   50,   51,   52,   53,   54,   61,   62,   63,
 /*   380 */    64,   65,   66,   86,   68,    7,   67,   62,   72,   73,
 /*   390 */    74,   75,   76,   77,   78,   79,    1,    7,    3,   67,
 /*   400 */     5,    6,   67,    8,    7,   70,   71,    2,   80,   16,
 /*   410 */    15,   83,   84,   85,   19,   20,   70,   89,   90,   91,
 /*   420 */    52,   53,   54,   45,   46,   47,   48,   49,   50,   51,
 /*   430 */    52,   53,   54,   54,   39,   40,   92,   47,   48,   49,
 /*   440 */    50,   51,   52,   53,   54,   48,   49,   50,   51,   52,
 /*   450 */    53,   54,   89,   90,   91,   60,   61,   62,   63,   64,
 /*   460 */    65,   66,   92,   68,   89,   90,   91,   72,   73,   74,
 /*   470 */    75,   76,   77,   78,   79,    1,   92,    3,   92,    5,
 /*   480 */     6,   80,    8,   82,   83,   18,   85,   92,   92,   15,
 /*   490 */    89,   90,   91,   19,   20,   80,   81,   82,   83,   92,
 /*   500 */    85,   89,   90,   91,   89,   90,   91,   80,   92,   92,
 /*   510 */    83,   84,   85,   39,   40,   92,   89,   90,   91,   80,
 /*   520 */    92,   85,   83,   84,   85,   89,   90,   91,   89,   90,
 /*   530 */    91,   92,    7,   66,   60,   61,   62,   63,   64,   65,
 /*   540 */    66,   92,   68,   89,   90,   91,   72,   73,   74,   75,
 /*   550 */    76,   77,   78,   79,    1,   92,    3,   92,    5,    6,
 /*   560 */    80,    8,   82,   83,    7,   85,   92,   92,   15,   89,
 /*   570 */    90,   91,   19,   20,   49,   50,   51,   52,   53,   54,
 /*   580 */    67,   92,    2,   70,   71,   80,   92,   92,   83,   92,
 /*   590 */    85,   92,   39,   40,   89,   90,   91,   86,   62,   85,
 /*   600 */    89,   90,   91,   89,   90,   91,   70,   71,   92,   52,
 /*   610 */    53,   54,   92,   60,   61,   62,   63,   64,   65,   66,
 /*   620 */    92,   68,   89,   90,   91,   72,   73,   74,   75,   76,
 /*   630 */    77,   78,   79,    1,   92,    3,   92,    5,    6,   85,
 /*   640 */     8,   92,   92,   89,   90,   91,   66,   15,   92,    1,
 /*   650 */    92,   19,   20,    5,    6,   85,    8,   92,   85,   89,
 /*   660 */    90,   91,   89,   90,   91,   89,   90,   91,   20,   92,
 /*   670 */    92,   39,   40,   92,   85,    2,   92,   92,   89,   90,
 /*   680 */    91,   92,    9,   92,   92,   85,   92,   39,   40,   89,
 /*   690 */    90,   91,   92,   61,   62,   63,   64,   65,   66,   92,
 /*   700 */    68,   89,   90,   91,   72,   73,   74,   75,   76,   77,
 /*   710 */    78,   79,    7,   65,   66,   67,   92,   92,   13,   92,
 /*   720 */    72,   73,   74,   75,   76,   77,   78,   79,   55,   56,
 /*   730 */    89,   90,   91,   89,   90,   91,   92,   92,   92,   34,
 /*   740 */    35,   36,   37,   38,   92,   92,   41,   42,   43,   44,
 /*   750 */    45,   46,   47,   48,   49,   50,   51,   52,   53,   54,
 /*   760 */     7,   89,   90,   91,   59,   92,   13,   14,   89,   90,
 /*   770 */    91,   92,   89,   90,   91,   89,   90,   91,   89,   90,
 /*   780 */    91,   89,   90,   91,   89,   90,   91,   34,   35,   36,
 /*   790 */    37,   38,   92,   92,   41,   42,   43,   44,   45,   46,
 /*   800 */    47,   48,   49,   50,   51,   52,   53,   54,    7,   92,
 /*   810 */    89,   90,   91,   92,   13,   89,   90,   91,   89,   90,
 /*   820 */    91,   89,   90,   91,   89,   90,   91,   92,   92,   89,
 /*   830 */    90,   91,   89,   90,   91,   34,   35,   36,   37,   38,
 /*   840 */    92,   92,   41,   42,   43,   44,   45,   46,   47,   48,
 /*   850 */    49,   50,   51,   52,   53,   54,    7,   89,   90,   91,
 /*   860 */    89,   90,   91,   89,   90,   91,   89,   90,   91,   89,
 /*   870 */    90,   91,   89,   90,   91,   89,   90,   91,   89,   90,
 /*   880 */    91,   92,   92,   34,   35,   36,   37,   38,   92,   92,
 /*   890 */    41,   42,   43,   44,   45,   46,   47,   48,   49,   50,
 /*   900 */    51,   52,   53,   54,    1,    2,   92,   92,    5,    6,
 /*   910 */    92,    8,   89,   90,   91,   89,   90,   91,   92,   92,
 /*   920 */    92,   92,   92,   20,   92,    1,    2,   92,   92,    5,
 /*   930 */     6,   92,    8,   92,   92,   92,   92,   92,   92,   92,
 /*   940 */    92,   92,   39,   40,   20,   92,   92,   92,   92,   92,
 /*   950 */    92,   92,   92,   92,   92,   92,   92,   92,   92,   92,
 /*   960 */    92,   92,   92,   39,   40,   62,    1,   92,   65,   66,
 /*   970 */     5,    6,   92,    8,   92,   72,   73,   74,   75,   76,
 /*   980 */    77,   78,   79,   92,   92,   20,   92,    1,   92,   65,
 /*   990 */    66,    5,    6,   92,    8,   92,   72,   73,   74,   75,
 /*  1000 */    76,   77,   78,   79,   39,   40,   20,   92,   92,   92,
 /*  1010 */    92,   92,   92,   92,   92,   92,   92,   92,   92,   92,
 /*  1020 */    92,   92,   92,   92,   59,   39,   40,   92,    1,   92,
 /*  1030 */    65,   66,    5,    6,   92,    8,   92,   72,   73,   74,
 /*  1040 */    75,   76,   77,   78,   79,   92,   60,   20,   92,   92,
 /*  1050 */    92,   65,   66,   92,   92,   92,   92,   92,   72,   73,
 /*  1060 */    74,   75,   76,   77,   78,   79,   39,   40,   92,   92,
 /*  1070 */    92,   92,   92,   92,   92,   92,   92,   92,   92,   92,
 /*  1080 */    92,   92,   92,   92,   92,   92,   92,   92,   92,   92,
 /*  1090 */    92,   92,   65,   66,   92,   92,   92,   92,   92,   72,
 /*  1100 */    73,   74,   75,   76,   77,   78,   79,
};
#define YY_SHIFT_USE_DFLT (-62)
#define YY_SHIFT_MAX 128
static const short yy_shift_ofst[] = {
 /*     0 */   632,   -1,   78,   78,  157,  553,  236,  157,  157,  395,
 /*    10 */   316,  474,  903,  986,  648,  924,  965, 1027, 1027, 1027,
 /*    20 */  1027, 1027, 1027, 1027, 1027, 1027, 1027, 1027, 1027, 1027,
 /*    30 */  1027, 1027, 1027, 1027, 1027, 1027, 1027, 1027, 1027, 1027,
 /*    40 */  1027, 1027, 1027, 1027, 1027, 1027, 1027, 1027, 1027, 1027,
 /*    50 */  1027, 1027, 1027, 1027, 1027, 1027, 1027, 1027,    2,    2,
 /*    60 */   673,  -61,  109,   32,  256,  206,  206,   25,  753,  705,
 /*    70 */   801,  801,  801,  801,  849,  849,  849,  849,  849,  849,
 /*    80 */   849,  849,  849,  849,   84,  162,   62,  140,  217,  217,
 /*    90 */   217,  238,  298,  313,  378,   -4,  322,  390,  397,  525,
 /*   100 */     4,  286,  557,  335,  513,  536,  104,  368,  179,   79,
 /*   110 */   467,  103,  253,  129,  580,  191,  174,   93,   80,   68,
 /*   120 */     0,  379,  -53,  393,  405,  325,  319,  332,  346,
};
#define YY_REDUCE_USE_DFLT (-75)
#define YY_REDUCE_MAX 67
static const short yy_reduce_ofst[] = {
 /*     0 */   415,  439,  163,  427,    5,  480,  401,  328,  243,  505,
 /*    10 */   505,  505,  188,  589,  600,  511,  436,  514,  554,  570,
 /*    20 */   573,  137,  454,  533,  576,  612,  641,  -59,   77,  363,
 /*    30 */   826,  -74,  823,  789,  786,  783,  780,  777,  774,  771,
 /*    40 */   768,  743,  740,  735,  732,  729,  726,  721,  695,  692,
 /*    50 */   689,  686,  683,  679,  672,  644,  412,  375,  207,   36,
 /*    60 */   297,  274,  273,  250,  223,  237,  107,  -21,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   282,  282,  282,  282,  282,  282,  282,  282,  282,  282,
 /*    10 */   282,  282,  282,  282,  282,  282,  282,  282,  282,  282,
 /*    20 */   282,  282,  282,  282,  282,  282,  282,  282,  282,  282,
 /*    30 */   282,  282,  282,  282,  282,  282,  282,  282,  282,  282,
 /*    40 */   282,  282,  282,  282,  282,  282,  282,  282,  282,  282,
 /*    50 */   282,  282,  282,  282,  282,  282,  282,  282,  278,  278,
 /*    60 */   266,  282,  282,  282,  282,  278,  278,  282,  282,  282,
 /*    70 */   223,  222,  224,  251,  207,  213,  265,  215,  214,  209,
 /*    80 */   208,  210,  211,  212,  232,  242,  240,  231,  225,  241,
 /*    90 */   226,  234,  235,  233,  238,  254,  239,  236,  237,  253,
 /*   100 */   244,  243,  227,  282,  282,  282,  282,  228,  282,  282,
 /*   110 */   282,  282,  229,  282,  282,  282,  282,  282,  254,  282,
 /*   120 */   282,  230,  282,  202,  282,  282,  282,  282,  279,  262,
 /*   130 */   261,  205,  260,  259,  257,  258,  256,  255,  252,  249,
 /*   140 */   250,  247,  248,  220,  216,  191,  218,  219,  187,  217,
 /*   150 */   206,  280,  281,  188,  264,  221,  246,  245,  272,  186,
 /*   160 */   267,  271,  268,  276,  277,  198,  269,  270,  275,  273,
 /*   170 */   274,  197,  199,  200,  196,  195,  190,  194,  201,  193,
 /*   180 */   192,  203,  189,  204,  263,
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
  "ENDBLOCK",      "IMPORT",        "ENDSTATEMENT",  "RETURN",      
  "WHILE",         "FUNCTION",      "IDENTIFIER",    "RIGHTPAREN",  
  "FOR",           "IN",            "COMMA",         "FATCOMMA",    
  "INTEGER",       "FLOATNUM",      "LITERALSTRING",  "REGEXSTRING", 
  "TRUE",          "FALSE",         "NULL",          "VAR",         
  "error",         "chunk",         "statement_list",  "statement",   
  "statement_block",  "expr_list",     "paren_expr_list",  "func_args",   
  "ident_list",    "expression",    "lvalue",        "lvalue_indexable",
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
 /*   6 */ "statement ::= IMPORT expr_list ENDSTATEMENT",
 /*   7 */ "statement ::= BREAK ENDSTATEMENT",
 /*   8 */ "statement ::= BREAK error",
 /*   9 */ "statement ::= RETURN ENDSTATEMENT",
 /*  10 */ "statement ::= RETURN expr_list ENDSTATEMENT",
 /*  11 */ "statement ::= RETURN expr_list error",
 /*  12 */ "statement ::= RETURN paren_expr_list ENDSTATEMENT",
 /*  13 */ "statement ::= RETURN paren_expr_list error",
 /*  14 */ "statement ::= expr_list ENDSTATEMENT",
 /*  15 */ "statement ::= expr_list error",
 /*  16 */ "statement ::= IF expr_list statement_block ELSE statement_block",
 /*  17 */ "statement ::= IF expr_list statement_block",
 /*  18 */ "statement ::= WHILE expr_list statement_block",
 /*  19 */ "statement ::= FUNCTION IDENTIFIER LEFTPAREN func_args RIGHTPAREN statement_block",
 /*  20 */ "statement ::= FOR ident_list IN expression statement_block",
 /*  21 */ "statement ::= FOR ident_list error",
 /*  22 */ "statement ::= lvalue PLUSEQUALS expression",
 /*  23 */ "statement ::= lvalue DASHEQUALS expression",
 /*  24 */ "statement ::= lvalue STAREQUALS expression",
 /*  25 */ "statement ::= lvalue SLASHEQUALS expression",
 /*  26 */ "statement ::= lvalue BITWISE_OREQUALS expression",
 /*  27 */ "statement ::= lvalue BITWISE_ANDEQUALS expression",
 /*  28 */ "statement ::= lvalue BITWISE_XOREQUALS expression",
 /*  29 */ "statement ::= lvalue SHIFTLEFTEQUALS expression",
 /*  30 */ "statement ::= lvalue SHIFTRIGHTEQUALS expression",
 /*  31 */ "statement ::= ENDSTATEMENT",
 /*  32 */ "statement ::= SCOPESTARTBLOCK ENDBLOCK",
 /*  33 */ "statement ::= SCOPESTARTBLOCK statement_list ENDBLOCK",
 /*  34 */ "statement ::= error ENDSTATEMENT",
 /*  35 */ "paren_expr_list ::= LEFTPAREN expr_list RIGHTPAREN",
 /*  36 */ "paren_expr_list ::= LEFTPAREN RIGHTPAREN",
 /*  37 */ "expr_list ::= expr_list COMMA expression",
 /*  38 */ "expr_list ::= expr_list FATCOMMA expression",
 /*  39 */ "expr_list ::= expression",
 /*  40 */ "expression ::= NOT expression",
 /*  41 */ "expression ::= BITWISE_NOT expression",
 /*  42 */ "expression ::= expression PLUS expression",
 /*  43 */ "expression ::= expression DASH expression",
 /*  44 */ "expression ::= expression STAR expression",
 /*  45 */ "expression ::= expression SLASH expression",
 /*  46 */ "expression ::= expression AND expression",
 /*  47 */ "expression ::= expression OR expression",
 /*  48 */ "expression ::= expression CMP expression",
 /*  49 */ "expression ::= expression EQUALS expression",
 /*  50 */ "expression ::= expression NOTEQUALS expression",
 /*  51 */ "expression ::= expression GREATERTHAN expression",
 /*  52 */ "expression ::= expression GREATERTHANOREQUAL expression",
 /*  53 */ "expression ::= expression LESSTHAN expression",
 /*  54 */ "expression ::= expression LESSTHANOREQUAL expression",
 /*  55 */ "expression ::= expression BITWISE_XOR expression",
 /*  56 */ "expression ::= expression BITWISE_AND expression",
 /*  57 */ "expression ::= expression BITWISE_OR expression",
 /*  58 */ "expression ::= expression SHIFTLEFT expression",
 /*  59 */ "expression ::= expression SHIFTRIGHT expression",
 /*  60 */ "expression ::= expression MOD paren_expr_list",
 /*  61 */ "expression ::= expression MOD expression",
 /*  62 */ "expression ::= ARRAYOPENBRACKET CLOSEBRACKET",
 /*  63 */ "expression ::= ARRAYOPENBRACKET expr_list CLOSEBRACKET",
 /*  64 */ "expression ::= MAPSTARTBLOCK ENDBLOCK",
 /*  65 */ "expression ::= MAPSTARTBLOCK expr_list ENDBLOCK",
 /*  66 */ "expression ::= lvalue ASSIGN expression",
 /*  67 */ "expression ::= lvalue ASSIGN error",
 /*  68 */ "expression ::= expression INHERITS expression",
 /*  69 */ "expression ::= lvalue",
 /*  70 */ "expression ::= INTEGER",
 /*  71 */ "expression ::= NEGATIVE INTEGER",
 /*  72 */ "expression ::= FLOATNUM",
 /*  73 */ "expression ::= NEGATIVE FLOATNUM",
 /*  74 */ "expression ::= LITERALSTRING",
 /*  75 */ "expression ::= REGEXSTRING",
 /*  76 */ "expression ::= TRUE",
 /*  77 */ "expression ::= FALSE",
 /*  78 */ "expression ::= NULL",
 /*  79 */ "expression ::= FUNCTION LEFTPAREN func_args RIGHTPAREN statement_block",
 /*  80 */ "expression ::= expression QUESTIONMARK expression COLON expression",
 /*  81 */ "lvalue ::= lvalue_indexable",
 /*  82 */ "lvalue ::= VAR IDENTIFIER",
 /*  83 */ "lvalue ::= VAR GROUPLEFTPAREN ident_list RIGHTPAREN",
 /*  84 */ "lvalue ::= GROUPLEFTPAREN expr_list RIGHTPAREN",
 /*  85 */ "lvalue_indexable ::= THIS",
 /*  86 */ "lvalue_indexable ::= lvalue_indexable paren_expr_list",
 /*  87 */ "lvalue_indexable ::= lvalue_indexable OPENBRACKET expression CLOSEBRACKET",
 /*  88 */ "lvalue_indexable ::= lvalue_indexable PERIOD IDENTIFIER",
 /*  89 */ "lvalue_indexable ::= lvalue_indexable COLONCOLON IDENTIFIER",
 /*  90 */ "lvalue_indexable ::= IDENTIFIER",
 /*  91 */ "ident_list ::= ident_list COMMA IDENTIFIER",
 /*  92 */ "ident_list ::= IDENTIFIER",
 /*  93 */ "ident_list ::=",
 /*  94 */ "func_args ::= ident_list",
 /*  95 */ "func_args ::= ident_list COMMA ELLIPSIS IDENTIFIER",
 /*  96 */ "func_args ::= ELLIPSIS IDENTIFIER",
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
    case 82: /* statement_list */
    case 83: /* statement */
    case 84: /* statement_block */
    case 85: /* expr_list */
    case 86: /* paren_expr_list */
    case 88: /* ident_list */
    case 89: /* expression */
    case 90: /* lvalue */
    case 91: /* lvalue_indexable */
{
#line 132 "/Users/jdrago/w2/eureka/lib/ekParser.y"
 ekSyntaxDestroy(C->E, (yypminor->yy114)); 
#line 761 "/Users/jdrago/w2/eureka/lib/ekParser.c"
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
  { 81, 1 },
  { 82, 2 },
  { 82, 1 },
  { 84, 2 },
  { 84, 3 },
  { 84, 1 },
  { 83, 3 },
  { 83, 2 },
  { 83, 2 },
  { 83, 2 },
  { 83, 3 },
  { 83, 3 },
  { 83, 3 },
  { 83, 3 },
  { 83, 2 },
  { 83, 2 },
  { 83, 5 },
  { 83, 3 },
  { 83, 3 },
  { 83, 6 },
  { 83, 5 },
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
  { 83, 2 },
  { 83, 3 },
  { 83, 2 },
  { 86, 3 },
  { 86, 2 },
  { 85, 3 },
  { 85, 3 },
  { 85, 1 },
  { 89, 2 },
  { 89, 2 },
  { 89, 3 },
  { 89, 3 },
  { 89, 3 },
  { 89, 3 },
  { 89, 3 },
  { 89, 3 },
  { 89, 3 },
  { 89, 3 },
  { 89, 3 },
  { 89, 3 },
  { 89, 3 },
  { 89, 3 },
  { 89, 3 },
  { 89, 3 },
  { 89, 3 },
  { 89, 3 },
  { 89, 3 },
  { 89, 3 },
  { 89, 3 },
  { 89, 3 },
  { 89, 2 },
  { 89, 3 },
  { 89, 2 },
  { 89, 3 },
  { 89, 3 },
  { 89, 3 },
  { 89, 3 },
  { 89, 1 },
  { 89, 1 },
  { 89, 2 },
  { 89, 1 },
  { 89, 2 },
  { 89, 1 },
  { 89, 1 },
  { 89, 1 },
  { 89, 1 },
  { 89, 1 },
  { 89, 5 },
  { 89, 5 },
  { 90, 1 },
  { 90, 2 },
  { 90, 4 },
  { 90, 3 },
  { 91, 1 },
  { 91, 2 },
  { 91, 4 },
  { 91, 3 },
  { 91, 3 },
  { 91, 1 },
  { 88, 3 },
  { 88, 1 },
  { 88, 0 },
  { 87, 1 },
  { 87, 4 },
  { 87, 2 },
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
#line 123 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ C->root = yymsp[0].minor.yy114; }
#line 1143 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 1: /* statement_list ::= statement_list statement */
#line 135 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxListAppend(C->E, yymsp[-1].minor.yy114, yymsp[0].minor.yy114, 0); }
#line 1148 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 2: /* statement_list ::= statement */
      case 5: /* statement_block ::= statement */ yytestcase(yyruleno==5);
#line 138 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateList(C->E, EST_STATEMENTLIST, yymsp[0].minor.yy114); }
#line 1154 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 3: /* statement_block ::= STARTBLOCK ENDBLOCK */
      case 31: /* statement ::= ENDSTATEMENT */ yytestcase(yyruleno==31);
      case 32: /* statement ::= SCOPESTARTBLOCK ENDBLOCK */ yytestcase(yyruleno==32);
#line 150 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateList(C->E, EST_STATEMENTLIST, NULL); }
#line 1161 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 4: /* statement_block ::= STARTBLOCK statement_list ENDBLOCK */
      case 35: /* paren_expr_list ::= LEFTPAREN expr_list RIGHTPAREN */ yytestcase(yyruleno==35);
      case 84: /* lvalue ::= GROUPLEFTPAREN expr_list RIGHTPAREN */ yytestcase(yyruleno==84);
#line 153 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = yymsp[-1].minor.yy114; }
#line 1168 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 6: /* statement ::= IMPORT expr_list ENDSTATEMENT */
#line 168 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateImport(C->E, yymsp[-1].minor.yy114); }
#line 1173 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 7: /* statement ::= BREAK ENDSTATEMENT */
#line 171 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateBreak(C->E, yymsp[-1].minor.yy0.line); }
#line 1178 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 8: /* statement ::= BREAK error */
      case 11: /* statement ::= RETURN expr_list error */ yytestcase(yyruleno==11);
      case 13: /* statement ::= RETURN paren_expr_list error */ yytestcase(yyruleno==13);
      case 15: /* statement ::= expr_list error */ yytestcase(yyruleno==15);
#line 174 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ ekCompileExplainError(C, "expected ;"); }
#line 1186 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 9: /* statement ::= RETURN ENDSTATEMENT */
#line 177 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateReturn(C->E, yymsp[-1].minor.yy0.line, NULL); }
#line 1191 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 10: /* statement ::= RETURN expr_list ENDSTATEMENT */
      case 12: /* statement ::= RETURN paren_expr_list ENDSTATEMENT */ yytestcase(yyruleno==12);
#line 180 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateReturn(C->E, yymsp[-1].minor.yy114->line, yymsp[-1].minor.yy114); }
#line 1197 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 14: /* statement ::= expr_list ENDSTATEMENT */
#line 192 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateStatementExpr(C->E, yymsp[-1].minor.yy114); }
#line 1202 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 16: /* statement ::= IF expr_list statement_block ELSE statement_block */
#line 198 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateIfElse(C->E, yymsp[-3].minor.yy114, yymsp[-2].minor.yy114, yymsp[0].minor.yy114, ekFalse); }
#line 1207 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 17: /* statement ::= IF expr_list statement_block */
#line 201 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateIfElse(C->E, yymsp[-1].minor.yy114, yymsp[0].minor.yy114, NULL, ekFalse); }
#line 1212 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 18: /* statement ::= WHILE expr_list statement_block */
#line 204 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateWhile(C->E, yymsp[-1].minor.yy114, yymsp[0].minor.yy114); }
#line 1217 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 19: /* statement ::= FUNCTION IDENTIFIER LEFTPAREN func_args RIGHTPAREN statement_block */
#line 207 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateFunctionDecl(C->E, &yymsp[-4].minor.yy0, yymsp[-2].minor.yy114, yymsp[0].minor.yy114, yymsp[-5].minor.yy0.line); }
#line 1222 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 20: /* statement ::= FOR ident_list IN expression statement_block */
#line 210 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateFor(C->E, yymsp[-3].minor.yy114, yymsp[-1].minor.yy114, yymsp[0].minor.yy114); }
#line 1227 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 21: /* statement ::= FOR ident_list error */
#line 213 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ ekCompileExplainError(C, "expected 'in'"); }
#line 1232 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 22: /* statement ::= lvalue PLUSEQUALS expression */
#line 216 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateBinary(C->E, EST_ADD, yymsp[-2].minor.yy114, yymsp[0].minor.yy114, ekTrue); }
#line 1237 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 23: /* statement ::= lvalue DASHEQUALS expression */
#line 219 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateBinary(C->E, EST_SUB, yymsp[-2].minor.yy114, yymsp[0].minor.yy114, ekTrue); }
#line 1242 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 24: /* statement ::= lvalue STAREQUALS expression */
#line 222 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateBinary(C->E, EST_MUL, yymsp[-2].minor.yy114, yymsp[0].minor.yy114, ekTrue); }
#line 1247 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 25: /* statement ::= lvalue SLASHEQUALS expression */
#line 225 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateBinary(C->E, EST_DIV, yymsp[-2].minor.yy114, yymsp[0].minor.yy114, ekTrue); }
#line 1252 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 26: /* statement ::= lvalue BITWISE_OREQUALS expression */
#line 228 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateBinary(C->E, EST_BITWISE_OR, yymsp[-2].minor.yy114, yymsp[0].minor.yy114, ekTrue); }
#line 1257 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 27: /* statement ::= lvalue BITWISE_ANDEQUALS expression */
#line 231 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateBinary(C->E, EST_BITWISE_AND, yymsp[-2].minor.yy114, yymsp[0].minor.yy114, ekTrue); }
#line 1262 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 28: /* statement ::= lvalue BITWISE_XOREQUALS expression */
#line 234 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateBinary(C->E, EST_BITWISE_XOR, yymsp[-2].minor.yy114, yymsp[0].minor.yy114, ekTrue); }
#line 1267 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 29: /* statement ::= lvalue SHIFTLEFTEQUALS expression */
#line 237 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateBinary(C->E, EST_SHIFTLEFT, yymsp[-2].minor.yy114, yymsp[0].minor.yy114, ekTrue); }
#line 1272 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 30: /* statement ::= lvalue SHIFTRIGHTEQUALS expression */
#line 240 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateBinary(C->E, EST_SHIFTRIGHT, yymsp[-2].minor.yy114, yymsp[0].minor.yy114, ekTrue); }
#line 1277 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 33: /* statement ::= SCOPESTARTBLOCK statement_list ENDBLOCK */
#line 249 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateScope(C->E, yymsp[-1].minor.yy114); }
#line 1282 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 36: /* paren_expr_list ::= LEFTPAREN RIGHTPAREN */
#line 266 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateList(C->E, EST_EXPRESSIONLIST, NULL); }
#line 1287 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 37: /* expr_list ::= expr_list COMMA expression */
#line 278 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxListAppend(C->E, yymsp[-2].minor.yy114, yymsp[0].minor.yy114, 0); }
#line 1292 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 38: /* expr_list ::= expr_list FATCOMMA expression */
#line 281 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxListAppend(C->E, yymsp[-2].minor.yy114, yymsp[0].minor.yy114, ESLF_AUTOLITERAL); }
#line 1297 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 39: /* expr_list ::= expression */
#line 284 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateList(C->E, EST_EXPRESSIONLIST, yymsp[0].minor.yy114); }
#line 1302 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 40: /* expression ::= NOT expression */
#line 296 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateUnary(C->E, EST_NOT, yymsp[0].minor.yy114, yymsp[0].minor.yy114->line); }
#line 1307 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 41: /* expression ::= BITWISE_NOT expression */
#line 299 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateUnary(C->E, EST_BITWISE_NOT, yymsp[0].minor.yy114, yymsp[0].minor.yy114->line); }
#line 1312 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 42: /* expression ::= expression PLUS expression */
#line 302 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateBinary(C->E, EST_ADD, yymsp[-2].minor.yy114, yymsp[0].minor.yy114, ekFalse); }
#line 1317 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 43: /* expression ::= expression DASH expression */
#line 305 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateBinary(C->E, EST_SUB, yymsp[-2].minor.yy114, yymsp[0].minor.yy114, ekFalse); }
#line 1322 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 44: /* expression ::= expression STAR expression */
#line 308 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateBinary(C->E, EST_MUL, yymsp[-2].minor.yy114, yymsp[0].minor.yy114, ekFalse); }
#line 1327 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 45: /* expression ::= expression SLASH expression */
#line 311 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateBinary(C->E, EST_DIV, yymsp[-2].minor.yy114, yymsp[0].minor.yy114, ekFalse); }
#line 1332 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 46: /* expression ::= expression AND expression */
#line 314 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateBinary(C->E, EST_AND, yymsp[-2].minor.yy114, yymsp[0].minor.yy114, ekFalse); }
#line 1337 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 47: /* expression ::= expression OR expression */
#line 317 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateBinary(C->E, EST_OR, yymsp[-2].minor.yy114, yymsp[0].minor.yy114, ekFalse); }
#line 1342 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 48: /* expression ::= expression CMP expression */
#line 320 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateBinary(C->E, EST_CMP, yymsp[-2].minor.yy114, yymsp[0].minor.yy114, ekFalse); }
#line 1347 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 49: /* expression ::= expression EQUALS expression */
#line 323 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateBinary(C->E, EST_EQUALS, yymsp[-2].minor.yy114, yymsp[0].minor.yy114, ekFalse); }
#line 1352 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 50: /* expression ::= expression NOTEQUALS expression */
#line 326 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateBinary(C->E, EST_NOTEQUALS, yymsp[-2].minor.yy114, yymsp[0].minor.yy114, ekFalse); }
#line 1357 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 51: /* expression ::= expression GREATERTHAN expression */
#line 329 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateBinary(C->E, EST_GREATERTHAN, yymsp[-2].minor.yy114, yymsp[0].minor.yy114, ekFalse); }
#line 1362 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 52: /* expression ::= expression GREATERTHANOREQUAL expression */
#line 332 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateBinary(C->E, EST_GREATERTHANOREQUAL, yymsp[-2].minor.yy114, yymsp[0].minor.yy114, ekFalse); }
#line 1367 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 53: /* expression ::= expression LESSTHAN expression */
#line 335 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateBinary(C->E, EST_LESSTHAN, yymsp[-2].minor.yy114, yymsp[0].minor.yy114, ekFalse); }
#line 1372 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 54: /* expression ::= expression LESSTHANOREQUAL expression */
#line 338 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateBinary(C->E, EST_LESSTHANOREQUAL, yymsp[-2].minor.yy114, yymsp[0].minor.yy114, ekFalse); }
#line 1377 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 55: /* expression ::= expression BITWISE_XOR expression */
#line 341 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateBinary(C->E, EST_BITWISE_XOR, yymsp[-2].minor.yy114, yymsp[0].minor.yy114, ekFalse); }
#line 1382 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 56: /* expression ::= expression BITWISE_AND expression */
#line 344 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateBinary(C->E, EST_BITWISE_AND, yymsp[-2].minor.yy114, yymsp[0].minor.yy114, ekFalse); }
#line 1387 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 57: /* expression ::= expression BITWISE_OR expression */
#line 347 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateBinary(C->E, EST_BITWISE_OR, yymsp[-2].minor.yy114, yymsp[0].minor.yy114, ekFalse); }
#line 1392 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 58: /* expression ::= expression SHIFTLEFT expression */
#line 350 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateBinary(C->E, EST_SHIFTLEFT, yymsp[-2].minor.yy114, yymsp[0].minor.yy114, ekFalse); }
#line 1397 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 59: /* expression ::= expression SHIFTRIGHT expression */
#line 353 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateBinary(C->E, EST_SHIFTRIGHT, yymsp[-2].minor.yy114, yymsp[0].minor.yy114, ekFalse); }
#line 1402 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 60: /* expression ::= expression MOD paren_expr_list */
      case 61: /* expression ::= expression MOD expression */ yytestcase(yyruleno==61);
#line 356 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateStringFormat(C->E, yymsp[-2].minor.yy114, yymsp[0].minor.yy114); }
#line 1408 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 62: /* expression ::= ARRAYOPENBRACKET CLOSEBRACKET */
#line 362 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateUnary(C->E, EST_ARRAY, NULL, yymsp[-1].minor.yy0.line); }
#line 1413 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 63: /* expression ::= ARRAYOPENBRACKET expr_list CLOSEBRACKET */
#line 365 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateUnary(C->E, EST_ARRAY, yymsp[-1].minor.yy114, yymsp[-1].minor.yy114->line); }
#line 1418 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 64: /* expression ::= MAPSTARTBLOCK ENDBLOCK */
#line 368 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateUnary(C->E, EST_MAP, NULL, yymsp[-1].minor.yy0.line); }
#line 1423 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 65: /* expression ::= MAPSTARTBLOCK expr_list ENDBLOCK */
#line 371 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateUnary(C->E, EST_MAP, yymsp[-1].minor.yy114, yymsp[-1].minor.yy114->line); }
#line 1428 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 66: /* expression ::= lvalue ASSIGN expression */
#line 374 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateAssignment(C->E, yymsp[-2].minor.yy114, yymsp[0].minor.yy114); }
#line 1433 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 67: /* expression ::= lvalue ASSIGN error */
#line 377 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ ekCompileExplainError(C, "assignment expected expression"); }
#line 1438 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 68: /* expression ::= expression INHERITS expression */
#line 380 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateInherits(C->E, yymsp[-2].minor.yy114, yymsp[0].minor.yy114); }
#line 1443 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 69: /* expression ::= lvalue */
      case 81: /* lvalue ::= lvalue_indexable */ yytestcase(yyruleno==81);
#line 383 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = yymsp[0].minor.yy114; }
#line 1449 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 70: /* expression ::= INTEGER */
#line 386 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateKInt(C->E, &yymsp[0].minor.yy0, 0); }
#line 1454 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 71: /* expression ::= NEGATIVE INTEGER */
#line 389 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateKInt(C->E, &yymsp[0].minor.yy0, CKO_NEGATIVE); }
#line 1459 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 72: /* expression ::= FLOATNUM */
#line 392 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateKFloat(C->E, &yymsp[0].minor.yy0, 0); }
#line 1464 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 73: /* expression ::= NEGATIVE FLOATNUM */
#line 395 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateKFloat(C->E, &yymsp[0].minor.yy0, CKO_NEGATIVE); }
#line 1469 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 74: /* expression ::= LITERALSTRING */
#line 398 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateKString(C->E, &yymsp[0].minor.yy0, 0); }
#line 1474 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 75: /* expression ::= REGEXSTRING */
#line 401 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateKString(C->E, &yymsp[0].minor.yy0, 1); }
#line 1479 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 76: /* expression ::= TRUE */
#line 404 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateBool(C->E, &yymsp[0].minor.yy0, ekTrue); }
#line 1484 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 77: /* expression ::= FALSE */
#line 407 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateBool(C->E, &yymsp[0].minor.yy0, ekFalse); }
#line 1489 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 78: /* expression ::= NULL */
#line 410 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateNull(C->E, yymsp[0].minor.yy0.line); }
#line 1494 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 79: /* expression ::= FUNCTION LEFTPAREN func_args RIGHTPAREN statement_block */
#line 413 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateFunctionDecl(C->E, NULL, yymsp[-2].minor.yy114, yymsp[0].minor.yy114, yymsp[-4].minor.yy0.line); }
#line 1499 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 80: /* expression ::= expression QUESTIONMARK expression COLON expression */
#line 416 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateIfElse(C->E, yymsp[-4].minor.yy114, yymsp[-2].minor.yy114, yymsp[0].minor.yy114, ekTrue); }
#line 1504 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 82: /* lvalue ::= VAR IDENTIFIER */
#line 432 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxMarkVar(C->E, ekSyntaxCreateList(C->E, EST_IDENTIFIERLIST, ekSyntaxCreateIdentifier(C->E, &yymsp[0].minor.yy0))); }
#line 1509 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 83: /* lvalue ::= VAR GROUPLEFTPAREN ident_list RIGHTPAREN */
#line 435 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxMarkVar(C->E, yymsp[-1].minor.yy114); }
#line 1514 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 85: /* lvalue_indexable ::= THIS */
#line 450 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateThis(C->E, yymsp[0].minor.yy0.line); }
#line 1519 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 86: /* lvalue_indexable ::= lvalue_indexable paren_expr_list */
#line 453 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateCall(C->E, yymsp[-1].minor.yy114, yymsp[0].minor.yy114); }
#line 1524 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 87: /* lvalue_indexable ::= lvalue_indexable OPENBRACKET expression CLOSEBRACKET */
#line 456 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateIndex(C->E, yymsp[-3].minor.yy114, yymsp[-1].minor.yy114, ekFalse); }
#line 1529 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 88: /* lvalue_indexable ::= lvalue_indexable PERIOD IDENTIFIER */
#line 459 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateIndex(C->E, yymsp[-2].minor.yy114, ekSyntaxCreateKString(C->E, &yymsp[0].minor.yy0, 0), ekFalse); }
#line 1534 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 89: /* lvalue_indexable ::= lvalue_indexable COLONCOLON IDENTIFIER */
#line 462 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateIndex(C->E, yymsp[-2].minor.yy114, ekSyntaxCreateKString(C->E, &yymsp[0].minor.yy0, 0), ekTrue); }
#line 1539 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 90: /* lvalue_indexable ::= IDENTIFIER */
#line 465 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateIdentifier(C->E, &yymsp[0].minor.yy0); }
#line 1544 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 91: /* ident_list ::= ident_list COMMA IDENTIFIER */
#line 477 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxListAppend(C->E, yymsp[-2].minor.yy114, ekSyntaxCreateIdentifier(C->E, &yymsp[0].minor.yy0), 0); }
#line 1549 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 92: /* ident_list ::= IDENTIFIER */
#line 480 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateList(C->E, EST_IDENTIFIERLIST, ekSyntaxCreateIdentifier(C->E, &yymsp[0].minor.yy0)); }
#line 1554 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 93: /* ident_list ::= */
#line 483 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateList(C->E, EST_IDENTIFIERLIST, NULL); }
#line 1559 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 94: /* func_args ::= ident_list */
#line 492 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateFunctionArgs(C->E, yymsp[0].minor.yy114, NULL); }
#line 1564 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 95: /* func_args ::= ident_list COMMA ELLIPSIS IDENTIFIER */
#line 495 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateFunctionArgs(C->E, yymsp[-3].minor.yy114, &yymsp[0].minor.yy0); }
#line 1569 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      case 96: /* func_args ::= ELLIPSIS IDENTIFIER */
#line 498 "/Users/jdrago/w2/eureka/lib/ekParser.y"
{ yygotominor.yy114 = ekSyntaxCreateFunctionArgs(C->E, ekSyntaxCreateList(C->E, EST_IDENTIFIERLIST, NULL), &yymsp[0].minor.yy0); }
#line 1574 "/Users/jdrago/w2/eureka/lib/ekParser.c"
        break;
      default:
      /* (34) statement ::= error ENDSTATEMENT */ yytestcase(yyruleno==34);
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
#line 117 "/Users/jdrago/w2/eureka/lib/ekParser.y"
 ekCompileSyntaxError(C, &TOKEN, "Syntax Error"); 
#line 1639 "/Users/jdrago/w2/eureka/lib/ekParser.c"
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
