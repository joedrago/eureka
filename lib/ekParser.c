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
#define YYNSTATE 178
#define YYNRULE 90
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
 /*     0 */    19,   60,    8,    7,   13,  110,  116,   16,   18,  150,
 /*    10 */   151,  101,  127,  121,   17,   68,   91,   59,  118,  162,
 /*    20 */   115,   28,   44,   24,   23,   27,   50,   29,   56,   26,
 /*    30 */   100,  128,   42,   14,   68,  126,   59,  108,   21,   36,
 /*    40 */    19,    3,    8,  136,   13,  110,  116,   16,   10,  169,
 /*    50 */    22,  101,   49,   39,   17,   68,   91,   59,  118,  162,
 /*    60 */   166,   12,   20,  107,  163,  157,  125,  160,   49,   39,
 /*    70 */   139,  142,  143,  144,  145,  146,  147,  105,   21,   36,
 /*    80 */    46,   30,    4,   22,   52,   32,   38,   41,   47,   54,
 /*    90 */    48,   40,   53,   51,   35,   37,   42,   14,  172,  173,
 /*   100 */   166,   12,   20,  107,  163,   22,  125,  165,   22,  119,
 /*   110 */   139,  142,  143,  144,  145,  146,  147,  105,  178,   19,
 /*   120 */   168,    8,  158,   13,  110,  112,   16,   35,   37,   42,
 /*   130 */    14,  133,  164,   17,   49,   39,   30,  118,  162,   52,
 /*   140 */    32,   38,   41,   47,   54,   48,   40,   53,   51,   35,
 /*   150 */    37,   42,   14,   37,   42,   14,  159,   21,   36,   19,
 /*   160 */   131,    8,  171,   13,  110,  116,   16,   22,  150,  154,
 /*   170 */   101,  140,  141,   17,   68,   91,   59,  118,  162,  166,
 /*   180 */    12,   20,  107,  163,   61,  125,   37,   42,   14,  139,
 /*   190 */   142,  143,  144,  145,  146,  147,  105,   21,   36,  124,
 /*   200 */   121,   52,   32,   38,   41,   47,   54,   48,   40,   53,
 /*   210 */    51,   35,   37,   42,   14,  122,  126,   59,  148,  166,
 /*   220 */    12,   20,  107,  163,   57,  125,   22,   55,  112,  139,
 /*   230 */   142,  143,  144,  145,  146,  147,  105,   19,    6,    8,
 /*   240 */     7,   13,  110,  116,   16,  129,  150,  174,  101,  138,
 /*   250 */   109,   17,   68,   91,   59,  118,  162,   18,   65,  126,
 /*   260 */    59,   32,   38,   41,   47,   54,   48,   40,   53,   51,
 /*   270 */    35,   37,   42,   14,  159,   21,   36,   19,  155,    8,
 /*   280 */   160,   13,  110,  116,   16,   22,  150,  152,  101,  111,
 /*   290 */    58,   17,   68,   91,   59,  118,  162,  166,   12,   20,
 /*   300 */   107,  163,   14,  125,   97,  126,   59,  139,  142,  143,
 /*   310 */   144,  145,  146,  147,  105,   21,   36,  270,   66,  126,
 /*   320 */    59,   38,   41,   47,   54,   48,   40,   53,   51,   35,
 /*   330 */    37,   42,   14,   22,  270,  270,  167,  166,   12,   20,
 /*   340 */   107,  163,  270,  125,   22,  270,  270,  139,  142,  143,
 /*   350 */   144,  145,  146,  147,  105,   19,  132,    8,   22,   13,
 /*   360 */   110,  270,   16,  270,   49,   39,   22,  177,  270,   17,
 /*   370 */   176,  126,   59,  118,  162,   53,   51,   35,   37,   42,
 /*   380 */    14,   41,   47,   54,   48,   40,   53,   51,   35,   37,
 /*   390 */    42,   14,  270,   21,   36,  270,   47,   54,   48,   40,
 /*   400 */    53,   51,   35,   37,   42,   14,   48,   40,   53,   51,
 /*   410 */    35,   37,   42,   14,  170,  166,   12,   20,  107,  163,
 /*   420 */   270,  125,   22,  270,  270,  139,  142,  143,  144,  145,
 /*   430 */   146,  147,  105,   19,  270,    8,  270,   13,  110,  116,
 /*   440 */    16,    9,  169,  270,  101,   22,  270,   17,   68,   91,
 /*   450 */    59,  118,  162,  270,  102,   63,  126,   59,   68,  126,
 /*   460 */    59,   54,   48,   40,   53,   51,   35,   37,   42,   14,
 /*   470 */   270,   21,   36,   31,   25,   46,   30,  270,  270,   52,
 /*   480 */    32,   38,   41,   47,   54,   48,   40,   53,   51,   35,
 /*   490 */    37,   42,   14,  166,   12,   20,  107,  163,  270,  125,
 /*   500 */    77,  126,   59,  139,  142,  143,  144,  145,  146,  147,
 /*   510 */   105,   22,  270,  116,  269,   11,  169,   34,  101,   62,
 /*   520 */   126,   59,   68,   91,   59,  116,   22,    1,  150,  114,
 /*   530 */   101,   68,  126,   59,   68,   91,   59,  270,   45,   31,
 /*   540 */    25,   46,   30,  270,  270,   52,   32,   38,   41,   47,
 /*   550 */    54,   48,   40,   53,   51,   35,   37,   42,   14,   22,
 /*   560 */    96,  126,   59,  156,  270,   34,   43,   40,   53,   51,
 /*   570 */    35,   37,   42,   14,  116,   22,   99,  150,  153,  101,
 /*   580 */    68,  126,   59,   68,   91,   59,   45,   31,   25,   46,
 /*   590 */    30,  270,  270,   52,   32,   38,   41,   47,   54,   48,
 /*   600 */    40,   53,   51,   35,   37,   42,   14,   22,  270,  270,
 /*   610 */   104,  270,  270,   34,   68,  126,   59,   57,   51,   35,
 /*   620 */    37,   42,   14,  103,  270,  270,    2,   68,  126,   59,
 /*   630 */    68,  126,   59,  270,   45,   31,   25,   46,   30,  270,
 /*   640 */   270,   52,   32,   38,   41,   47,   54,   48,   40,   53,
 /*   650 */    51,   35,   37,   42,   14,   22,   15,  270,  270,  116,
 /*   660 */   270,   34,  130,   33,  101,  270,    5,  270,   68,   91,
 /*   670 */    59,   87,  126,   59,   86,  126,   59,   80,  126,   59,
 /*   680 */   113,  270,   45,   31,   25,   46,   30,  270,  270,   52,
 /*   690 */    32,   38,   41,   47,   54,   48,   40,   53,   51,   35,
 /*   700 */    37,   42,   14,   22,   95,  126,   59,  270,  270,  117,
 /*   710 */   123,   85,  126,   59,  270,   92,  126,   59,   69,  126,
 /*   720 */    59,   64,  126,   59,   70,  126,   59,   75,  126,   59,
 /*   730 */    45,   31,   25,   46,   30,  270,  270,   52,   32,   38,
 /*   740 */    41,   47,   54,   48,   40,   53,   51,   35,   37,   42,
 /*   750 */    14,   19,   15,  270,  149,   13,  110,  134,   16,   73,
 /*   760 */   126,   59,   49,   39,   89,  126,   59,   49,   39,  270,
 /*   770 */   162,  270,   19,   78,  126,   59,   13,  110,  270,   16,
 /*   780 */    76,  126,   59,  161,  270,  270,   49,   39,  270,   21,
 /*   790 */    36,  162,  270,   19,   81,  126,   59,   13,  110,  270,
 /*   800 */    16,   94,  126,   59,   71,  126,   59,   72,  126,   59,
 /*   810 */    21,   36,  162,  270,  120,  163,   74,  126,   59,  270,
 /*   820 */   270,  139,  142,  143,  144,  145,  146,  147,  105,  270,
 /*   830 */   135,   21,   36,   19,  270,  120,  163,   13,  110,  270,
 /*   840 */    16,  270,  139,  142,  143,  144,  145,  146,  147,  105,
 /*   850 */   270,  270,  162,   67,  126,   59,  120,  163,  175,  270,
 /*   860 */   270,  270,  270,  139,  142,  143,  144,  145,  146,  147,
 /*   870 */   105,   21,   36,  270,  270,   98,  126,   59,   79,  126,
 /*   880 */    59,  270,   84,  126,   59,   88,  126,   59,  270,   82,
 /*   890 */   126,   59,  137,  106,  126,   59,  120,  163,  270,  270,
 /*   900 */    22,  270,  270,  139,  142,  143,  144,  145,  146,  147,
 /*   910 */   105,   90,  126,   59,   83,  126,   59,   93,  126,   59,
 /*   920 */   270,  270,  270,  270,  270,  270,  270,  270,  270,   25,
 /*   930 */    46,   30,  270,  270,   52,   32,   38,   41,   47,   54,
 /*   940 */    48,   40,   53,   51,   35,   37,   42,   14,   19,  270,
 /*   950 */   270,  270,   13,  110,  270,   16,  270,  270,  270,  270,
 /*   960 */   270,  270,  270,  270,  270,  270,  270,  162,  270,  270,
 /*   970 */   270,  270,  270,  270,  270,  270,  270,  270,  270,  270,
 /*   980 */   270,  270,  270,  270,  270,  270,   21,   36,  270,  270,
 /*   990 */   270,  270,  270,  270,  270,  270,  270,  270,  270,  270,
 /*  1000 */   270,  270,  270,  270,  270,  270,  270,  270,  270,  270,
 /*  1010 */   270,  120,  163,  270,  270,  270,  270,  270,  139,  142,
 /*  1020 */   143,  144,  145,  146,  147,  105,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */     1,    1,    3,    4,    5,    6,   79,    8,   12,   82,
 /*    10 */    83,   84,   86,   87,   15,   88,   89,   90,   19,   20,
 /*    20 */    18,   25,   26,   27,   28,   29,   30,   31,   32,   33,
 /*    30 */    84,   85,   53,   54,   88,   89,   90,   87,   39,   40,
 /*    40 */     1,   66,    3,   60,    5,    6,   79,    8,   81,   82,
 /*    50 */     7,   84,   69,   70,   15,   88,   89,   90,   19,   20,
 /*    60 */    61,   62,   63,   64,   65,   65,   67,   65,   69,   70,
 /*    70 */    71,   72,   73,   74,   75,   76,   77,   78,   39,   40,
 /*    80 */    37,   38,   16,    7,   41,   42,   43,   44,   45,   46,
 /*    90 */    47,   48,   49,   50,   51,   52,   53,   54,   65,   60,
 /*   100 */    61,   62,   63,   64,   65,    7,   67,   65,    7,   18,
 /*   110 */    71,   72,   73,   74,   75,   76,   77,   78,    0,    1,
 /*   120 */    61,    3,   66,    5,    6,   69,    8,   51,   52,   53,
 /*   130 */    54,   66,   65,   15,   69,   70,   38,   19,   20,   41,
 /*   140 */    42,   43,   44,   45,   46,   47,   48,   49,   50,   51,
 /*   150 */    52,   53,   54,   52,   53,   54,   65,   39,   40,    1,
 /*   160 */    61,    3,   65,    5,    6,   79,    8,    7,   82,   83,
 /*   170 */    84,   71,   72,   15,   88,   89,   90,   19,   20,   61,
 /*   180 */    62,   63,   64,   65,    2,   67,   52,   53,   54,   71,
 /*   190 */    72,   73,   74,   75,   76,   77,   78,   39,   40,   86,
 /*   200 */    87,   41,   42,   43,   44,   45,   46,   47,   48,   49,
 /*   210 */    50,   51,   52,   53,   54,   88,   89,   90,   60,   61,
 /*   220 */    62,   63,   64,   65,    2,   67,    7,   68,   69,   71,
 /*   230 */    72,   73,   74,   75,   76,   77,   78,    1,   66,    3,
 /*   240 */     4,    5,    6,   79,    8,   61,   82,   83,   84,   79,
 /*   250 */    69,   15,   88,   89,   90,   19,   20,   12,   88,   89,
 /*   260 */    90,   42,   43,   44,   45,   46,   47,   48,   49,   50,
 /*   270 */    51,   52,   53,   54,   65,   39,   40,    1,   85,    3,
 /*   280 */    65,    5,    6,   79,    8,    7,   82,   83,   84,   87,
 /*   290 */     2,   15,   88,   89,   90,   19,   20,   61,   62,   63,
 /*   300 */    64,   65,   54,   67,   88,   89,   90,   71,   72,   73,
 /*   310 */    74,   75,   76,   77,   78,   39,   40,   91,   88,   89,
 /*   320 */    90,   43,   44,   45,   46,   47,   48,   49,   50,   51,
 /*   330 */    52,   53,   54,    7,   91,   91,   60,   61,   62,   63,
 /*   340 */    64,   65,   91,   67,    7,   91,   91,   71,   72,   73,
 /*   350 */    74,   75,   76,   77,   78,    1,   61,    3,    7,    5,
 /*   360 */     6,   91,    8,   91,   69,   70,    7,   85,   91,   15,
 /*   370 */    88,   89,   90,   19,   20,   49,   50,   51,   52,   53,
 /*   380 */    54,   44,   45,   46,   47,   48,   49,   50,   51,   52,
 /*   390 */    53,   54,   91,   39,   40,   91,   45,   46,   47,   48,
 /*   400 */    49,   50,   51,   52,   53,   54,   47,   48,   49,   50,
 /*   410 */    51,   52,   53,   54,   60,   61,   62,   63,   64,   65,
 /*   420 */    91,   67,    7,   91,   91,   71,   72,   73,   74,   75,
 /*   430 */    76,   77,   78,    1,   91,    3,   91,    5,    6,   79,
 /*   440 */     8,   81,   82,   91,   84,    7,   91,   15,   88,   89,
 /*   450 */    90,   19,   20,   91,   84,   88,   89,   90,   88,   89,
 /*   460 */    90,   46,   47,   48,   49,   50,   51,   52,   53,   54,
 /*   470 */    91,   39,   40,   35,   36,   37,   38,   91,   91,   41,
 /*   480 */    42,   43,   44,   45,   46,   47,   48,   49,   50,   51,
 /*   490 */    52,   53,   54,   61,   62,   63,   64,   65,   91,   67,
 /*   500 */    88,   89,   90,   71,   72,   73,   74,   75,   76,   77,
 /*   510 */    78,    7,   91,   79,   80,   81,   82,   13,   84,   88,
 /*   520 */    89,   90,   88,   89,   90,   79,    7,   84,   82,   83,
 /*   530 */    84,   88,   89,   90,   88,   89,   90,   91,   34,   35,
 /*   540 */    36,   37,   38,   91,   91,   41,   42,   43,   44,   45,
 /*   550 */    46,   47,   48,   49,   50,   51,   52,   53,   54,    7,
 /*   560 */    88,   89,   90,   59,   91,   13,   14,   48,   49,   50,
 /*   570 */    51,   52,   53,   54,   79,    7,   84,   82,   83,   84,
 /*   580 */    88,   89,   90,   88,   89,   90,   34,   35,   36,   37,
 /*   590 */    38,   91,   91,   41,   42,   43,   44,   45,   46,   47,
 /*   600 */    48,   49,   50,   51,   52,   53,   54,    7,   91,   91,
 /*   610 */    84,   91,   91,   13,   88,   89,   90,    2,   50,   51,
 /*   620 */    52,   53,   54,   84,   91,   91,   84,   88,   89,   90,
 /*   630 */    88,   89,   90,   91,   34,   35,   36,   37,   38,   91,
 /*   640 */    91,   41,   42,   43,   44,   45,   46,   47,   48,   49,
 /*   650 */    50,   51,   52,   53,   54,    7,    2,   91,   91,   79,
 /*   660 */    91,   13,   82,    9,   84,   91,   66,   91,   88,   89,
 /*   670 */    90,   88,   89,   90,   88,   89,   90,   88,   89,   90,
 /*   680 */    65,   91,   34,   35,   36,   37,   38,   91,   91,   41,
 /*   690 */    42,   43,   44,   45,   46,   47,   48,   49,   50,   51,
 /*   700 */    52,   53,   54,    7,   88,   89,   90,   91,   91,   55,
 /*   710 */    56,   88,   89,   90,   91,   88,   89,   90,   88,   89,
 /*   720 */    90,   88,   89,   90,   88,   89,   90,   88,   89,   90,
 /*   730 */    34,   35,   36,   37,   38,   91,   91,   41,   42,   43,
 /*   740 */    44,   45,   46,   47,   48,   49,   50,   51,   52,   53,
 /*   750 */    54,    1,    2,   91,   61,    5,    6,   59,    8,   88,
 /*   760 */    89,   90,   69,   70,   88,   89,   90,   69,   70,   91,
 /*   770 */    20,   91,    1,   88,   89,   90,    5,    6,   91,    8,
 /*   780 */    88,   89,   90,   66,   91,   91,   69,   70,   91,   39,
 /*   790 */    40,   20,   91,    1,   88,   89,   90,    5,    6,   91,
 /*   800 */     8,   88,   89,   90,   88,   89,   90,   88,   89,   90,
 /*   810 */    39,   40,   20,   91,   64,   65,   88,   89,   90,   91,
 /*   820 */    91,   71,   72,   73,   74,   75,   76,   77,   78,   91,
 /*   830 */    59,   39,   40,    1,   91,   64,   65,    5,    6,   91,
 /*   840 */     8,   91,   71,   72,   73,   74,   75,   76,   77,   78,
 /*   850 */    91,   91,   20,   88,   89,   90,   64,   65,   66,   91,
 /*   860 */    91,   91,   91,   71,   72,   73,   74,   75,   76,   77,
 /*   870 */    78,   39,   40,   91,   91,   88,   89,   90,   88,   89,
 /*   880 */    90,   91,   88,   89,   90,   88,   89,   90,   91,   88,
 /*   890 */    89,   90,   60,   88,   89,   90,   64,   65,   91,   91,
 /*   900 */     7,   91,   91,   71,   72,   73,   74,   75,   76,   77,
 /*   910 */    78,   88,   89,   90,   88,   89,   90,   88,   89,   90,
 /*   920 */    91,   91,   91,   91,   91,   91,   91,   91,   91,   36,
 /*   930 */    37,   38,   91,   91,   41,   42,   43,   44,   45,   46,
 /*   940 */    47,   48,   49,   50,   51,   52,   53,   54,    1,   91,
 /*   950 */    91,   91,    5,    6,   91,    8,   91,   91,   91,   91,
 /*   960 */    91,   91,   91,   91,   91,   91,   91,   20,   91,   91,
 /*   970 */    91,   91,   91,   91,   91,   91,   91,   91,   91,   91,
 /*   980 */    91,   91,   91,   91,   91,   91,   39,   40,   91,   91,
 /*   990 */    91,   91,   91,   91,   91,   91,   91,   91,   91,   91,
 /*  1000 */    91,   91,   91,   91,   91,   91,   91,   91,   91,   91,
 /*  1010 */    91,   64,   65,   91,   91,   91,   91,   91,   71,   72,
 /*  1020 */    73,   74,   75,   76,   77,   78,
};
#define YY_SHIFT_USE_DFLT (-26)
#define YY_SHIFT_MAX 128
static const short yy_shift_ofst[] = {
 /*     0 */   432,   -1,   -1,  236,  236,  236,  236,   39,  354,  276,
 /*    10 */   158,  118,  750,  832,  750,  792,  771,  947,  947,  947,
 /*    20 */   947,  947,  947,  947,  947,  947,  947,  947,  947,  947,
 /*    30 */   947,  947,  947,  947,  947,  947,  947,  947,  947,  947,
 /*    40 */   947,  947,  947,  947,  947,  947,  947,  947,  947,  947,
 /*    50 */   947,  947,  947,  947,  947,  947,  947,    2,    2,  654,
 /*    60 */   215,  215,  504,  552,  600,  648,  648,  648,  648,  696,
 /*    70 */   696,  696,  696,  696,  696,  696,  696,  696,  696,  438,
 /*    80 */   893,   43,   98,  160,  160,  160,  219,  278,  337,  351,
 /*    90 */   415,   -4,  359,  519,  326,  568,   76,  101,  134,  -17,
 /*   100 */   295,  693,  698,   65,  717,    0,  -21,  615,  159,   91,
 /*   110 */   100,   56,  209,  288,   66,   33,   59,   67,   99,   97,
 /*   120 */   222,  181,  248,   42,  172,  182,  245,  -25,  184,
};
#define YY_REDUCE_USE_DFLT (-75)
#define YY_REDUCE_MAX 61
static const short yy_reduce_ofst[] = {
 /*     0 */   434,  204,  446,  495,  -73,   86,  164,  -33,  360,  580,
 /*    10 */   580,  580,  -54,  492,  282,  539,  370,  542,  170,  526,
 /*    20 */   443,  794,  787,  728,  716,  706,  685,  671,  636,  630,
 /*    30 */   623,  589,  583,  431,  367,  216,  826,  805,  797,  765,
 /*    40 */   713,  676,  127,  692,  719,  790,  801,  823,  829,  230,
 /*    50 */   412,  472,  586,  616,  627,  633,  639,  113,  -74,  193,
 /*    60 */   202,  -50,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   268,  268,  268,  268,  268,  268,  268,  268,  268,  268,
 /*    10 */   268,  268,  268,  268,  268,  268,  268,  268,  268,  268,
 /*    20 */   268,  268,  268,  268,  268,  268,  268,  268,  268,  268,
 /*    30 */   268,  268,  268,  268,  268,  268,  268,  268,  268,  268,
 /*    40 */   268,  268,  268,  268,  268,  268,  268,  268,  268,  268,
 /*    50 */   268,  268,  268,  268,  268,  268,  268,  264,  264,  252,
 /*    60 */   264,  264,  268,  268,  268,  237,  208,  209,  210,  196,
 /*    70 */   197,  198,  199,  200,  201,  194,  251,  193,  195,  218,
 /*    80 */   228,  226,  217,  211,  212,  227,  220,  221,  219,  224,
 /*    90 */   225,  240,  222,  223,  239,  230,  229,  213,  214,  268,
 /*   100 */   268,  268,  268,  268,  268,  268,  215,  268,  268,  268,
 /*   110 */   268,  268,  268,  268,  189,  268,  268,  268,  268,  268,
 /*   120 */   268,  265,  216,  268,  268,  268,  240,  268,  268,  186,
 /*   130 */   179,  184,  185,  206,  234,  233,  236,  235,  238,  241,
 /*   140 */   242,  244,  243,  245,  246,  247,  248,  249,  182,  187,
 /*   150 */   183,  188,  190,  191,  192,  257,  258,  253,  254,  262,
 /*   160 */   263,  255,  256,  261,  259,  260,  202,  204,  205,  180,
 /*   170 */   203,  266,  267,  181,  250,  207,  232,  231,
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
 /*  55 */ "expression ::= ARRAYOPENBRACKET CLOSEBRACKET",
 /*  56 */ "expression ::= ARRAYOPENBRACKET expr_list CLOSEBRACKET",
 /*  57 */ "expression ::= MAPSTARTBLOCK ENDBLOCK",
 /*  58 */ "expression ::= MAPSTARTBLOCK expr_list ENDBLOCK",
 /*  59 */ "expression ::= lvalue ASSIGN expression",
 /*  60 */ "expression ::= lvalue ASSIGN error",
 /*  61 */ "expression ::= expression INHERITS expression",
 /*  62 */ "expression ::= lvalue",
 /*  63 */ "expression ::= INTEGER",
 /*  64 */ "expression ::= NEGATIVE INTEGER",
 /*  65 */ "expression ::= FLOATNUM",
 /*  66 */ "expression ::= NEGATIVE FLOATNUM",
 /*  67 */ "expression ::= LITERALSTRING",
 /*  68 */ "expression ::= REGEXSTRING",
 /*  69 */ "expression ::= TRUE",
 /*  70 */ "expression ::= FALSE",
 /*  71 */ "expression ::= NULL",
 /*  72 */ "expression ::= FUNCTION LEFTPAREN func_args RIGHTPAREN statement_block",
 /*  73 */ "expression ::= expression QUESTIONMARK expression COLON expression",
 /*  74 */ "lvalue ::= lvalue_indexable",
 /*  75 */ "lvalue ::= VAR IDENTIFIER",
 /*  76 */ "lvalue ::= VAR GROUPLEFTPAREN ident_list RIGHTPAREN",
 /*  77 */ "lvalue ::= GROUPLEFTPAREN expr_list RIGHTPAREN",
 /*  78 */ "lvalue_indexable ::= THIS",
 /*  79 */ "lvalue_indexable ::= lvalue_indexable paren_expr_list",
 /*  80 */ "lvalue_indexable ::= lvalue_indexable OPENBRACKET expression CLOSEBRACKET",
 /*  81 */ "lvalue_indexable ::= lvalue_indexable PERIOD IDENTIFIER",
 /*  82 */ "lvalue_indexable ::= lvalue_indexable COLONCOLON IDENTIFIER",
 /*  83 */ "lvalue_indexable ::= IDENTIFIER",
 /*  84 */ "ident_list ::= ident_list COMMA IDENTIFIER",
 /*  85 */ "ident_list ::= IDENTIFIER",
 /*  86 */ "ident_list ::=",
 /*  87 */ "func_args ::= ident_list",
 /*  88 */ "func_args ::= ident_list COMMA ELLIPSIS IDENTIFIER",
 /*  89 */ "func_args ::= ELLIPSIS IDENTIFIER",
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
#line 737 "/Users/jdrago/work/eureka/lib/ekParser.c"
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
#line 1112 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 1: /* statement_list ::= statement_list statement */
#line 135 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxListAppend(C->E, yymsp[-1].minor.yy94, yymsp[0].minor.yy94, 0); }
#line 1117 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 2: /* statement_list ::= statement */
      case 5: /* statement_block ::= statement */ yytestcase(yyruleno==5);
#line 138 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateList(C->E, EST_STATEMENTLIST, yymsp[0].minor.yy94); }
#line 1123 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 3: /* statement_block ::= STARTBLOCK ENDBLOCK */
      case 24: /* statement ::= ENDSTATEMENT */ yytestcase(yyruleno==24);
      case 25: /* statement ::= SCOPESTARTBLOCK ENDBLOCK */ yytestcase(yyruleno==25);
#line 150 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateList(C->E, EST_STATEMENTLIST, NULL); }
#line 1130 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 4: /* statement_block ::= STARTBLOCK statement_list ENDBLOCK */
      case 28: /* paren_expr_list ::= LEFTPAREN expr_list RIGHTPAREN */ yytestcase(yyruleno==28);
      case 77: /* lvalue ::= GROUPLEFTPAREN expr_list RIGHTPAREN */ yytestcase(yyruleno==77);
#line 153 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = yymsp[-1].minor.yy94; }
#line 1137 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 6: /* statement ::= BREAK ENDSTATEMENT */
#line 168 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBreak(C->E, yymsp[-1].minor.yy0.line); }
#line 1142 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 7: /* statement ::= RETURN expr_list ENDSTATEMENT */
      case 8: /* statement ::= RETURN paren_expr_list ENDSTATEMENT */ yytestcase(yyruleno==8);
#line 171 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateReturn(C->E, yymsp[-1].minor.yy94); }
#line 1148 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 9: /* statement ::= expr_list ENDSTATEMENT */
#line 177 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateStatementExpr(C->E, yymsp[-1].minor.yy94); }
#line 1153 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 10: /* statement ::= IF expr_list statement_block ELSE statement_block */
#line 180 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateIfElse(C->E, yymsp[-3].minor.yy94, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1158 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 11: /* statement ::= IF expr_list statement_block */
#line 183 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateIfElse(C->E, yymsp[-1].minor.yy94, yymsp[0].minor.yy94, NULL, ekFalse); }
#line 1163 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 12: /* statement ::= WHILE expr_list statement_block */
#line 186 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateWhile(C->E, yymsp[-1].minor.yy94, yymsp[0].minor.yy94); }
#line 1168 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 13: /* statement ::= FUNCTION IDENTIFIER LEFTPAREN func_args RIGHTPAREN statement_block */
#line 189 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateFunctionDecl(C->E, &yymsp[-4].minor.yy0, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, yymsp[-5].minor.yy0.line); }
#line 1173 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 14: /* statement ::= FOR LEFTPAREN ident_list IN expression RIGHTPAREN statement_block */
#line 192 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateFor(C->E, yymsp[-4].minor.yy94, yymsp[-2].minor.yy94, yymsp[0].minor.yy94); }
#line 1178 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 15: /* statement ::= lvalue PLUSEQUALS expression */
#line 195 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_ADD, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1183 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 16: /* statement ::= lvalue DASHEQUALS expression */
#line 198 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_SUB, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1188 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 17: /* statement ::= lvalue STAREQUALS expression */
#line 201 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_MUL, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1193 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 18: /* statement ::= lvalue SLASHEQUALS expression */
#line 204 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_DIV, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1198 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 19: /* statement ::= lvalue BITWISE_OREQUALS expression */
#line 207 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_BITWISE_OR, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1203 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 20: /* statement ::= lvalue BITWISE_ANDEQUALS expression */
#line 210 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_BITWISE_AND, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1208 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 21: /* statement ::= lvalue BITWISE_XOREQUALS expression */
#line 213 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_BITWISE_XOR, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1213 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 22: /* statement ::= lvalue SHIFTLEFTEQUALS expression */
#line 216 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_SHIFTLEFT, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1218 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 23: /* statement ::= lvalue SHIFTRIGHTEQUALS expression */
#line 219 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_SHIFTRIGHT, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1223 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 26: /* statement ::= SCOPESTARTBLOCK statement_list ENDBLOCK */
#line 228 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateScope(C->E, yymsp[-1].minor.yy94); }
#line 1228 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 29: /* paren_expr_list ::= LEFTPAREN RIGHTPAREN */
#line 245 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateList(C->E, EST_EXPRESSIONLIST, NULL); }
#line 1233 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 30: /* expr_list ::= expr_list COMMA expression */
#line 257 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxListAppend(C->E, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, 0); }
#line 1238 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 31: /* expr_list ::= expr_list FATCOMMA expression */
#line 260 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxListAppend(C->E, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ESLF_AUTOLITERAL); }
#line 1243 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 32: /* expr_list ::= expression */
#line 263 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateList(C->E, EST_EXPRESSIONLIST, yymsp[0].minor.yy94); }
#line 1248 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 33: /* expression ::= NOT expression */
#line 275 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateUnary(C->E, EST_NOT, yymsp[0].minor.yy94, yymsp[0].minor.yy94->line); }
#line 1253 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 34: /* expression ::= BITWISE_NOT expression */
#line 278 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateUnary(C->E, EST_BITWISE_NOT, yymsp[0].minor.yy94, yymsp[0].minor.yy94->line); }
#line 1258 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 35: /* expression ::= expression PLUS expression */
#line 281 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_ADD, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1263 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 36: /* expression ::= expression DASH expression */
#line 284 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_SUB, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1268 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 37: /* expression ::= expression STAR expression */
#line 287 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_MUL, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1273 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 38: /* expression ::= expression SLASH expression */
#line 290 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_DIV, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1278 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 39: /* expression ::= expression AND expression */
#line 293 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_AND, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1283 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 40: /* expression ::= expression OR expression */
#line 296 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_OR, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1288 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 41: /* expression ::= expression CMP expression */
#line 299 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_CMP, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1293 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 42: /* expression ::= expression EQUALS expression */
#line 302 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_EQUALS, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1298 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 43: /* expression ::= expression NOTEQUALS expression */
#line 305 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_NOTEQUALS, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1303 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 44: /* expression ::= expression GREATERTHAN expression */
#line 308 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_GREATERTHAN, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1308 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 45: /* expression ::= expression GREATERTHANOREQUAL expression */
#line 311 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_GREATERTHANOREQUAL, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1313 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 46: /* expression ::= expression LESSTHAN expression */
#line 314 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_LESSTHAN, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1318 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 47: /* expression ::= expression LESSTHANOREQUAL expression */
#line 317 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_LESSTHANOREQUAL, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1323 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 48: /* expression ::= expression BITWISE_XOR expression */
#line 320 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_BITWISE_XOR, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1328 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 49: /* expression ::= expression BITWISE_AND expression */
#line 323 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_BITWISE_AND, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1333 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 50: /* expression ::= expression BITWISE_OR expression */
#line 326 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_BITWISE_OR, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1338 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 51: /* expression ::= expression SHIFTLEFT expression */
#line 329 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_SHIFTLEFT, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1343 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 52: /* expression ::= expression SHIFTRIGHT expression */
#line 332 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_SHIFTRIGHT, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1348 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 53: /* expression ::= expression MOD paren_expr_list */
      case 54: /* expression ::= expression MOD expression */ yytestcase(yyruleno==54);
#line 335 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateStringFormat(C->E, yymsp[-2].minor.yy94, yymsp[0].minor.yy94); }
#line 1354 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 55: /* expression ::= ARRAYOPENBRACKET CLOSEBRACKET */
#line 341 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateUnary(C->E, EST_ARRAY, NULL, yymsp[-1].minor.yy0.line); }
#line 1359 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 56: /* expression ::= ARRAYOPENBRACKET expr_list CLOSEBRACKET */
#line 344 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateUnary(C->E, EST_ARRAY, yymsp[-1].minor.yy94, yymsp[-1].minor.yy94->line); }
#line 1364 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 57: /* expression ::= MAPSTARTBLOCK ENDBLOCK */
#line 347 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateUnary(C->E, EST_MAP, NULL, yymsp[-1].minor.yy0.line); }
#line 1369 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 58: /* expression ::= MAPSTARTBLOCK expr_list ENDBLOCK */
#line 350 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateUnary(C->E, EST_MAP, yymsp[-1].minor.yy94, yymsp[-1].minor.yy94->line); }
#line 1374 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 59: /* expression ::= lvalue ASSIGN expression */
#line 353 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateAssignment(C->E, yymsp[-2].minor.yy94, yymsp[0].minor.yy94); }
#line 1379 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 60: /* expression ::= lvalue ASSIGN error */
#line 356 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ ekCompileExplainError(C, "assignment expected expression"); }
#line 1384 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 61: /* expression ::= expression INHERITS expression */
#line 359 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateInherits(C->E, yymsp[-2].minor.yy94, yymsp[0].minor.yy94); }
#line 1389 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 62: /* expression ::= lvalue */
      case 74: /* lvalue ::= lvalue_indexable */ yytestcase(yyruleno==74);
#line 362 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = yymsp[0].minor.yy94; }
#line 1395 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 63: /* expression ::= INTEGER */
#line 365 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateKInt(C->E, &yymsp[0].minor.yy0, 0); }
#line 1400 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 64: /* expression ::= NEGATIVE INTEGER */
#line 368 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateKInt(C->E, &yymsp[0].minor.yy0, CKO_NEGATIVE); }
#line 1405 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 65: /* expression ::= FLOATNUM */
#line 371 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateKFloat(C->E, &yymsp[0].minor.yy0, 0); }
#line 1410 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 66: /* expression ::= NEGATIVE FLOATNUM */
#line 374 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateKFloat(C->E, &yymsp[0].minor.yy0, CKO_NEGATIVE); }
#line 1415 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 67: /* expression ::= LITERALSTRING */
#line 377 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateKString(C->E, &yymsp[0].minor.yy0, 0); }
#line 1420 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 68: /* expression ::= REGEXSTRING */
#line 380 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateKString(C->E, &yymsp[0].minor.yy0, 1); }
#line 1425 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 69: /* expression ::= TRUE */
#line 383 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBool(C->E, &yymsp[0].minor.yy0, ekTrue); }
#line 1430 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 70: /* expression ::= FALSE */
#line 386 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBool(C->E, &yymsp[0].minor.yy0, ekFalse); }
#line 1435 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 71: /* expression ::= NULL */
#line 389 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateNull(C->E, yymsp[0].minor.yy0.line); }
#line 1440 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 72: /* expression ::= FUNCTION LEFTPAREN func_args RIGHTPAREN statement_block */
#line 392 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateFunctionDecl(C->E, NULL, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, yymsp[-4].minor.yy0.line); }
#line 1445 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 73: /* expression ::= expression QUESTIONMARK expression COLON expression */
#line 395 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateIfElse(C->E, yymsp[-4].minor.yy94, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1450 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 75: /* lvalue ::= VAR IDENTIFIER */
#line 411 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxMarkVar(C->E, ekSyntaxCreateList(C->E, EST_IDENTIFIERLIST, ekSyntaxCreateIdentifier(C->E, &yymsp[0].minor.yy0))); }
#line 1455 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 76: /* lvalue ::= VAR GROUPLEFTPAREN ident_list RIGHTPAREN */
#line 414 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxMarkVar(C->E, yymsp[-1].minor.yy94); }
#line 1460 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 78: /* lvalue_indexable ::= THIS */
#line 429 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateThis(C->E, yymsp[0].minor.yy0.line); }
#line 1465 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 79: /* lvalue_indexable ::= lvalue_indexable paren_expr_list */
#line 432 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateCall(C->E, yymsp[-1].minor.yy94, yymsp[0].minor.yy94); }
#line 1470 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 80: /* lvalue_indexable ::= lvalue_indexable OPENBRACKET expression CLOSEBRACKET */
#line 435 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateIndex(C->E, yymsp[-3].minor.yy94, yymsp[-1].minor.yy94, ekFalse); }
#line 1475 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 81: /* lvalue_indexable ::= lvalue_indexable PERIOD IDENTIFIER */
#line 438 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateIndex(C->E, yymsp[-2].minor.yy94, ekSyntaxCreateKString(C->E, &yymsp[0].minor.yy0, 0), ekFalse); }
#line 1480 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 82: /* lvalue_indexable ::= lvalue_indexable COLONCOLON IDENTIFIER */
#line 441 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateIndex(C->E, yymsp[-2].minor.yy94, ekSyntaxCreateKString(C->E, &yymsp[0].minor.yy0, 0), ekTrue); }
#line 1485 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 83: /* lvalue_indexable ::= IDENTIFIER */
#line 444 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateIdentifier(C->E, &yymsp[0].minor.yy0); }
#line 1490 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 84: /* ident_list ::= ident_list COMMA IDENTIFIER */
#line 456 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxListAppend(C->E, yymsp[-2].minor.yy94, ekSyntaxCreateIdentifier(C->E, &yymsp[0].minor.yy0), 0); }
#line 1495 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 85: /* ident_list ::= IDENTIFIER */
#line 459 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateList(C->E, EST_IDENTIFIERLIST, ekSyntaxCreateIdentifier(C->E, &yymsp[0].minor.yy0)); }
#line 1500 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 86: /* ident_list ::= */
#line 462 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateList(C->E, EST_IDENTIFIERLIST, NULL); }
#line 1505 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 87: /* func_args ::= ident_list */
#line 471 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateFunctionArgs(C->E, yymsp[0].minor.yy94, NULL); }
#line 1510 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 88: /* func_args ::= ident_list COMMA ELLIPSIS IDENTIFIER */
#line 474 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateFunctionArgs(C->E, yymsp[-3].minor.yy94, &yymsp[0].minor.yy0); }
#line 1515 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 89: /* func_args ::= ELLIPSIS IDENTIFIER */
#line 477 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateFunctionArgs(C->E, ekSyntaxCreateList(C->E, EST_IDENTIFIERLIST, NULL), &yymsp[0].minor.yy0); }
#line 1520 "/Users/jdrago/work/eureka/lib/ekParser.c"
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
#line 117 "/Users/jdrago/work/eureka/lib/ekParser.y"
 ekCompileSyntaxError(C, &TOKEN, "Syntax Error"); 
#line 1585 "/Users/jdrago/work/eureka/lib/ekParser.c"
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
