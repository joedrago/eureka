/* Driver template for the LEMON parser generator.
** The author disclaims copyright to this source code.
*/
/* First off, code is included that follows the "include" declaration
** in the input grammar file. */
#include "ekTypes.h"
#include <stdio.h>
#line 22 "/home/joe/private/work/eureka/lib/ekParser.y"

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
#line 26 "/home/joe/private/work/eureka/lib/ekParser.c"
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
#define YYNSTATE 177
#define YYNRULE 89
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
 /*     0 */    19,  127,    8,    4,   16,  106,  124,   15,   45,  148,
 /*    10 */   117,  102,  116,  125,   17,   67,   91,   59,  112,  160,
 /*    20 */   126,   52,   54,   53,   56,   55,   47,   51,   49,   50,
 /*    30 */    98,  128,   26,   13,   67,  114,   59,  153,   24,   22,
 /*    40 */    19,   61,    8,    4,   16,  106,  124,   15,  158,  148,
 /*    50 */   150,  102,  119,  125,   17,   67,   91,   59,  112,  160,
 /*    60 */   164,   12,   18,  107,  161,  158,  120,  157,   20,   41,
 /*    70 */   137,  140,  141,  142,  143,  144,  145,  109,   24,   22,
 /*    80 */    19,  108,    8,  135,   16,  106,  124,   15,   10,  167,
 /*    90 */    23,  102,   20,   41,   17,   67,   91,   59,  112,  160,
 /*   100 */   164,   12,   18,  107,  161,  155,  120,   65,  114,   59,
 /*   110 */   137,  140,  141,  142,  143,  144,  145,  109,   24,   22,
 /*   120 */    27,   37,  138,  139,   30,   31,   29,   34,   35,   32,
 /*   130 */    33,   42,   40,   39,   21,   25,   26,   13,  130,  171,
 /*   140 */   164,   12,   18,  107,  161,  110,  120,   23,   46,  121,
 /*   150 */   137,  140,  141,  142,  143,  144,  145,  109,   19,   13,
 /*   160 */     8,   45,   16,  106,  124,   15,   23,  148,  172,  102,
 /*   170 */    57,  131,   17,   67,   91,   59,  112,  160,   37,   20,
 /*   180 */    41,   30,   31,   29,   34,   35,   32,   33,   42,   40,
 /*   190 */    39,   21,   25,   26,   13,  132,   24,   22,   20,   41,
 /*   200 */    30,   31,   29,   34,   35,   32,   33,   42,   40,   39,
 /*   210 */    21,   25,   26,   13,   97,  114,   59,  168,  164,   12,
 /*   220 */    18,  107,  161,    5,  120,   23,    3,   57,  137,  140,
 /*   230 */   141,  142,  143,  144,  145,  109,  177,   19,    6,    8,
 /*   240 */    58,   16,  106,  124,   15,   60,  148,  149,  102,  147,
 /*   250 */   157,   17,   67,   91,   59,  112,  160,   20,   41,   23,
 /*   260 */    31,   29,   34,   35,   32,   33,   42,   40,   39,   21,
 /*   270 */    25,   26,   13,   23,  162,   24,   22,   19,  163,    8,
 /*   280 */   166,   16,  106,  124,   15,   23,  148,  151,  102,  111,
 /*   290 */   118,   17,   67,   91,   59,  112,  160,  164,   12,   18,
 /*   300 */   107,  161,  169,  120,   25,   26,   13,  137,  140,  141,
 /*   310 */   142,  143,  144,  145,  109,   24,   22,   21,   25,   26,
 /*   320 */    13,   29,   34,   35,   32,   33,   42,   40,   39,   21,
 /*   330 */    25,   26,   13,   83,  114,   59,  146,  164,   12,   18,
 /*   340 */   107,  161,  170,  120,   23,  176,  268,  137,  140,  141,
 /*   350 */   142,  143,  144,  145,  109,   19,  268,    8,   23,   16,
 /*   360 */   106,  159,   15,  268,   20,   41,   23,  124,  268,   17,
 /*   370 */   148,  152,  102,  112,  160,  268,   67,   91,   59,  268,
 /*   380 */   268,   34,   35,   32,   33,   42,   40,   39,   21,   25,
 /*   390 */    26,   13,  156,   24,   22,  121,   35,   32,   33,   42,
 /*   400 */    40,   39,   21,   25,   26,   13,   33,   42,   40,   39,
 /*   410 */    21,   25,   26,   13,  165,  164,   12,   18,  107,  161,
 /*   420 */   268,  120,   23,  268,  268,  137,  140,  141,  142,  143,
 /*   430 */   144,  145,  109,   19,  268,    8,  268,   16,  106,  124,
 /*   440 */    15,   11,  167,  268,  102,   23,  268,   17,   67,   91,
 /*   450 */    59,  112,  160,  175,  268,  268,  174,  114,   59,  268,
 /*   460 */   268,   32,   33,   42,   40,   39,   21,   25,   26,   13,
 /*   470 */   268,   24,   22,   38,   36,   27,   37,  268,  268,   30,
 /*   480 */    31,   29,   34,   35,   32,   33,   42,   40,   39,   21,
 /*   490 */    25,   26,   13,  164,   12,   18,  107,  161,  268,  120,
 /*   500 */   104,  114,   59,  137,  140,  141,  142,  143,  144,  145,
 /*   510 */   109,   23,  268,  268,   84,  114,   59,   43,   44,  268,
 /*   520 */   124,  267,    9,  167,  268,  102,   23,  268,  268,   67,
 /*   530 */    91,   59,  268,   23,  105,  114,   59,  268,   28,   38,
 /*   540 */    36,   27,   37,  268,  268,   30,   31,   29,   34,   35,
 /*   550 */    32,   33,   42,   40,   39,   21,   25,   26,   13,   23,
 /*   560 */   113,  114,   59,  268,  268,   43,  268,   42,   40,   39,
 /*   570 */    21,   25,   26,   13,  268,   40,   39,   21,   25,   26,
 /*   580 */    13,   23,   82,  114,   59,  268,   28,   38,   36,   27,
 /*   590 */    37,  268,  268,   30,   31,   29,   34,   35,   32,   33,
 /*   600 */    42,   40,   39,   21,   25,   26,   13,   23,   25,   26,
 /*   610 */    13,  124,  268,   43,  129,  268,  102,  268,    7,  268,
 /*   620 */    67,   91,   59,  268,   39,   21,   25,   26,   13,   79,
 /*   630 */   114,   59,  268,  268,   28,   38,   36,   27,   37,  268,
 /*   640 */   268,   30,   31,   29,   34,   35,   32,   33,   42,   40,
 /*   650 */    39,   21,   25,   26,   13,   23,   88,  114,   59,  154,
 /*   660 */    99,   43,  133,  100,   67,  114,   59,   67,  114,   59,
 /*   670 */   268,  101,   20,   41,    1,   67,  114,   59,   67,  114,
 /*   680 */    59,  268,   28,   38,   36,   27,   37,  268,  268,   30,
 /*   690 */    31,   29,   34,   35,   32,   33,   42,   40,   39,   21,
 /*   700 */    25,   26,   13,   23,  268,    2,   14,  268,  268,   67,
 /*   710 */   114,   59,  103,   48,  268,  268,   67,  114,   59,   86,
 /*   720 */   114,   59,  268,   87,  114,   59,   92,  114,   59,  268,
 /*   730 */    28,   38,   36,   27,   37,  268,  268,   30,   31,   29,
 /*   740 */    34,   35,   32,   33,   42,   40,   39,   21,   25,   26,
 /*   750 */    13,   19,   14,  268,  268,   16,  106,  268,   15,  122,
 /*   760 */   123,  268,  268,   93,  114,   59,   89,  114,   59,  268,
 /*   770 */   160,  268,   19,   90,  114,   59,   16,  106,  268,   15,
 /*   780 */    81,  114,   59,   85,  114,   59,   80,  114,   59,   24,
 /*   790 */    22,  160,  268,   19,   96,  114,   59,   16,  106,  268,
 /*   800 */    15,   95,  114,   59,  268,   66,  114,   59,  268,  268,
 /*   810 */    24,   22,  160,  268,  115,  161,   94,  114,   59,  268,
 /*   820 */   268,  137,  140,  141,  142,  143,  144,  145,  109,  268,
 /*   830 */   268,   24,   22,  268,   19,  115,  161,  173,   16,  106,
 /*   840 */   268,   15,  137,  140,  141,  142,  143,  144,  145,  109,
 /*   850 */   268,  134,  268,  160,  268,  268,  115,  161,  268,  268,
 /*   860 */   268,  268,  268,  137,  140,  141,  142,  143,  144,  145,
 /*   870 */   109,  268,   24,   22,   62,  114,   59,   69,  114,   59,
 /*   880 */   268,  268,   68,  114,   59,   63,  114,   59,  268,   70,
 /*   890 */   114,   59,  268,  136,   64,  114,   59,  115,  161,  268,
 /*   900 */   268,   23,  268,  268,  137,  140,  141,  142,  143,  144,
 /*   910 */   145,  109,   71,  114,   59,   72,  114,   59,  268,  268,
 /*   920 */    73,  114,   59,   74,  114,   59,   75,  114,   59,  268,
 /*   930 */    36,   27,   37,  268,  268,   30,   31,   29,   34,   35,
 /*   940 */    32,   33,   42,   40,   39,   21,   25,   26,   13,   19,
 /*   950 */   268,  268,  268,   16,  106,  268,   15,   76,  114,   59,
 /*   960 */   268,   77,  114,   59,   78,  114,   59,  268,  160,  268,
 /*   970 */   268,  268,  268,  268,  268,  268,  268,  268,  268,  268,
 /*   980 */   268,  268,  268,  268,  268,  268,  268,   24,   22,  268,
 /*   990 */   268,  268,  268,  268,  268,  268,  268,  268,  268,  268,
 /*  1000 */   268,  268,  268,  268,  268,  268,  268,  268,  268,  268,
 /*  1010 */   268,  268,  115,  161,  268,  268,  268,  268,  268,  137,
 /*  1020 */   140,  141,  142,  143,  144,  145,  109,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */     1,   18,    3,    4,    5,    6,   79,    8,   12,   82,
 /*    10 */    83,   84,   86,   87,   15,   88,   89,   90,   19,   20,
 /*    20 */    18,   25,   26,   27,   28,   29,   30,   31,   32,   33,
 /*    30 */    84,   85,   53,   54,   88,   89,   90,   85,   39,   40,
 /*    40 */     1,    1,    3,    4,    5,    6,   79,    8,   65,   82,
 /*    50 */    83,   84,   86,   87,   15,   88,   89,   90,   19,   20,
 /*    60 */    61,   62,   63,   64,   65,   65,   67,   65,   69,   70,
 /*    70 */    71,   72,   73,   74,   75,   76,   77,   78,   39,   40,
 /*    80 */     1,   87,    3,   60,    5,    6,   79,    8,   81,   82,
 /*    90 */     7,   84,   69,   70,   15,   88,   89,   90,   19,   20,
 /*   100 */    61,   62,   63,   64,   65,   65,   67,   88,   89,   90,
 /*   110 */    71,   72,   73,   74,   75,   76,   77,   78,   39,   40,
 /*   120 */    37,   38,   71,   72,   41,   42,   43,   44,   45,   46,
 /*   130 */    47,   48,   49,   50,   51,   52,   53,   54,   61,   60,
 /*   140 */    61,   62,   63,   64,   65,   87,   67,    7,   68,   69,
 /*   150 */    71,   72,   73,   74,   75,   76,   77,   78,    1,   54,
 /*   160 */     3,   12,    5,    6,   79,    8,    7,   82,   83,   84,
 /*   170 */     2,   61,   15,   88,   89,   90,   19,   20,   38,   69,
 /*   180 */    70,   41,   42,   43,   44,   45,   46,   47,   48,   49,
 /*   190 */    50,   51,   52,   53,   54,   66,   39,   40,   69,   70,
 /*   200 */    41,   42,   43,   44,   45,   46,   47,   48,   49,   50,
 /*   210 */    51,   52,   53,   54,   88,   89,   90,   60,   61,   62,
 /*   220 */    63,   64,   65,   16,   67,    7,   66,    2,   71,   72,
 /*   230 */    73,   74,   75,   76,   77,   78,    0,    1,   66,    3,
 /*   240 */     2,    5,    6,   79,    8,    2,   82,   83,   84,   61,
 /*   250 */    65,   15,   88,   89,   90,   19,   20,   69,   70,    7,
 /*   260 */    42,   43,   44,   45,   46,   47,   48,   49,   50,   51,
 /*   270 */    52,   53,   54,    7,   65,   39,   40,    1,   65,    3,
 /*   280 */    61,    5,    6,   79,    8,    7,   82,   83,   84,   69,
 /*   290 */    65,   15,   88,   89,   90,   19,   20,   61,   62,   63,
 /*   300 */    64,   65,   65,   67,   52,   53,   54,   71,   72,   73,
 /*   310 */    74,   75,   76,   77,   78,   39,   40,   51,   52,   53,
 /*   320 */    54,   43,   44,   45,   46,   47,   48,   49,   50,   51,
 /*   330 */    52,   53,   54,   88,   89,   90,   60,   61,   62,   63,
 /*   340 */    64,   65,   65,   67,    7,   61,   91,   71,   72,   73,
 /*   350 */    74,   75,   76,   77,   78,    1,   91,    3,    7,    5,
 /*   360 */     6,   66,    8,   91,   69,   70,    7,   79,   91,   15,
 /*   370 */    82,   83,   84,   19,   20,   91,   88,   89,   90,   91,
 /*   380 */    91,   44,   45,   46,   47,   48,   49,   50,   51,   52,
 /*   390 */    53,   54,   66,   39,   40,   69,   45,   46,   47,   48,
 /*   400 */    49,   50,   51,   52,   53,   54,   47,   48,   49,   50,
 /*   410 */    51,   52,   53,   54,   60,   61,   62,   63,   64,   65,
 /*   420 */    91,   67,    7,   91,   91,   71,   72,   73,   74,   75,
 /*   430 */    76,   77,   78,    1,   91,    3,   91,    5,    6,   79,
 /*   440 */     8,   81,   82,   91,   84,    7,   91,   15,   88,   89,
 /*   450 */    90,   19,   20,   85,   91,   91,   88,   89,   90,   91,
 /*   460 */    91,   46,   47,   48,   49,   50,   51,   52,   53,   54,
 /*   470 */    91,   39,   40,   35,   36,   37,   38,   91,   91,   41,
 /*   480 */    42,   43,   44,   45,   46,   47,   48,   49,   50,   51,
 /*   490 */    52,   53,   54,   61,   62,   63,   64,   65,   91,   67,
 /*   500 */    88,   89,   90,   71,   72,   73,   74,   75,   76,   77,
 /*   510 */    78,    7,   91,   91,   88,   89,   90,   13,   14,   91,
 /*   520 */    79,   80,   81,   82,   91,   84,    7,   91,   91,   88,
 /*   530 */    89,   90,   91,    7,   88,   89,   90,   91,   34,   35,
 /*   540 */    36,   37,   38,   91,   91,   41,   42,   43,   44,   45,
 /*   550 */    46,   47,   48,   49,   50,   51,   52,   53,   54,    7,
 /*   560 */    88,   89,   90,   91,   91,   13,   91,   48,   49,   50,
 /*   570 */    51,   52,   53,   54,   91,   49,   50,   51,   52,   53,
 /*   580 */    54,    7,   88,   89,   90,   91,   34,   35,   36,   37,
 /*   590 */    38,   91,   91,   41,   42,   43,   44,   45,   46,   47,
 /*   600 */    48,   49,   50,   51,   52,   53,   54,    7,   52,   53,
 /*   610 */    54,   79,   91,   13,   82,   91,   84,   91,   66,   91,
 /*   620 */    88,   89,   90,   91,   50,   51,   52,   53,   54,   88,
 /*   630 */    89,   90,   91,   91,   34,   35,   36,   37,   38,   91,
 /*   640 */    91,   41,   42,   43,   44,   45,   46,   47,   48,   49,
 /*   650 */    50,   51,   52,   53,   54,    7,   88,   89,   90,   59,
 /*   660 */    84,   13,   59,   84,   88,   89,   90,   88,   89,   90,
 /*   670 */    91,   84,   69,   70,   84,   88,   89,   90,   88,   89,
 /*   680 */    90,   91,   34,   35,   36,   37,   38,   91,   91,   41,
 /*   690 */    42,   43,   44,   45,   46,   47,   48,   49,   50,   51,
 /*   700 */    52,   53,   54,    7,   91,   84,    2,   91,   91,   88,
 /*   710 */    89,   90,   84,    9,   91,   91,   88,   89,   90,   88,
 /*   720 */    89,   90,   91,   88,   89,   90,   88,   89,   90,   91,
 /*   730 */    34,   35,   36,   37,   38,   91,   91,   41,   42,   43,
 /*   740 */    44,   45,   46,   47,   48,   49,   50,   51,   52,   53,
 /*   750 */    54,    1,    2,   91,   91,    5,    6,   91,    8,   55,
 /*   760 */    56,   91,   91,   88,   89,   90,   88,   89,   90,   91,
 /*   770 */    20,   91,    1,   88,   89,   90,    5,    6,   91,    8,
 /*   780 */    88,   89,   90,   88,   89,   90,   88,   89,   90,   39,
 /*   790 */    40,   20,   91,    1,   88,   89,   90,    5,    6,   91,
 /*   800 */     8,   88,   89,   90,   91,   88,   89,   90,   91,   91,
 /*   810 */    39,   40,   20,   91,   64,   65,   88,   89,   90,   91,
 /*   820 */    91,   71,   72,   73,   74,   75,   76,   77,   78,   91,
 /*   830 */    91,   39,   40,   91,    1,   64,   65,   66,    5,    6,
 /*   840 */    91,    8,   71,   72,   73,   74,   75,   76,   77,   78,
 /*   850 */    91,   59,   91,   20,   91,   91,   64,   65,   91,   91,
 /*   860 */    91,   91,   91,   71,   72,   73,   74,   75,   76,   77,
 /*   870 */    78,   91,   39,   40,   88,   89,   90,   88,   89,   90,
 /*   880 */    91,   91,   88,   89,   90,   88,   89,   90,   91,   88,
 /*   890 */    89,   90,   91,   60,   88,   89,   90,   64,   65,   91,
 /*   900 */    91,    7,   91,   91,   71,   72,   73,   74,   75,   76,
 /*   910 */    77,   78,   88,   89,   90,   88,   89,   90,   91,   91,
 /*   920 */    88,   89,   90,   88,   89,   90,   88,   89,   90,   91,
 /*   930 */    36,   37,   38,   91,   91,   41,   42,   43,   44,   45,
 /*   940 */    46,   47,   48,   49,   50,   51,   52,   53,   54,    1,
 /*   950 */    91,   91,   91,    5,    6,   91,    8,   88,   89,   90,
 /*   960 */    91,   88,   89,   90,   88,   89,   90,   91,   20,   91,
 /*   970 */    91,   91,   91,   91,   91,   91,   91,   91,   91,   91,
 /*   980 */    91,   91,   91,   91,   91,   91,   91,   39,   40,   91,
 /*   990 */    91,   91,   91,   91,   91,   91,   91,   91,   91,   91,
 /*  1000 */    91,   91,   91,   91,   91,   91,   91,   91,   91,   91,
 /*  1010 */    91,   91,   64,   65,   91,   91,   91,   91,   91,   71,
 /*  1020 */    72,   73,   74,   75,   76,   77,   78,
};
#define YY_SHIFT_USE_DFLT (-22)
#define YY_SHIFT_MAX 128
static const short yy_shift_ofst[] = {
 /*     0 */   432,   -1,   -1,   39,   79,   39,   39,   39,  157,  236,
 /*    10 */   276,  354,  750,  750,  771,  792,  833,  948,  948,  948,
 /*    20 */   948,  948,  948,  948,  948,  948,  948,  948,  948,  948,
 /*    30 */   948,  948,  948,  948,  948,  948,  948,  948,  948,  948,
 /*    40 */   948,  948,  948,  948,  948,  948,  948,  948,  948,  948,
 /*    50 */   948,  948,  948,  948,  948,  948,  948,  -17,  -17,  704,
 /*    60 */     0,    0,  504,  552,  600,  648,  648,  648,  648,  696,
 /*    70 */   696,  696,  696,  696,  696,  696,  696,  696,  696,  438,
 /*    80 */   894,   83,  140,  159,  159,  159,  218,  278,  337,  351,
 /*    90 */   415,   -4,  359,  519,  526,  574,  266,  252,  110,  129,
 /*   100 */   603,   23,  188,  295,  556,  -21,   51,  225,   80,   40,
 /*   110 */   326,    2,   77,  105,  149,  168,  160,  207,  238,  172,
 /*   120 */   243,  185,  209,  213,  219,  220,  237,  277,  284,
};
#define YY_REDUCE_USE_DFLT (-75)
#define YY_REDUCE_MAX 61
static const short yy_reduce_ofst[] = {
 /*     0 */   441,  -73,  -33,   85,    7,  164,  204,  288,  360,  532,
 /*    10 */   532,  532,  -54,  368,  576,  579,  587,  590,  621,  628,
 /*    20 */    19,  126,  245,  412,  426,  446,  472,  494,  541,  568,
 /*    30 */   631,  635,  638,  675,  678,  685,  692,  695,  698,  706,
 /*    40 */   713,  717,  728,  786,  789,  794,  797,  801,  806,  824,
 /*    50 */   827,  832,  835,  838,  869,  873,  876,  -74,  -34,  -48,
 /*    60 */    -6,   58,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   266,  266,  266,  266,  266,  266,  266,  266,  266,  266,
 /*    10 */   266,  266,  266,  266,  266,  266,  266,  266,  266,  266,
 /*    20 */   266,  266,  266,  266,  266,  266,  266,  266,  266,  266,
 /*    30 */   266,  266,  266,  266,  266,  266,  266,  266,  266,  266,
 /*    40 */   266,  266,  266,  266,  266,  266,  266,  266,  266,  266,
 /*    50 */   266,  266,  266,  266,  266,  266,  266,  262,  262,  250,
 /*    60 */   262,  262,  266,  266,  266,  207,  208,  209,  236,  249,
 /*    70 */   192,  193,  194,  195,  196,  197,  198,  199,  200,  217,
 /*    80 */   227,  225,  216,  210,  211,  226,  219,  220,  218,  223,
 /*    90 */   224,  238,  221,  222,  237,  229,  228,  212,  266,  266,
 /*   100 */   266,  266,  266,  266,  213,  214,  266,  266,  266,  266,
 /*   110 */   266,  266,  266,  215,  238,  266,  266,  188,  266,  266,
 /*   120 */   266,  266,  266,  266,  266,  263,  266,  266,  266,  178,
 /*   130 */   183,  184,  205,  233,  232,  235,  234,  239,  240,  242,
 /*   140 */   241,  243,  244,  245,  246,  247,  181,  186,  182,  187,
 /*   150 */   189,  190,  191,  255,  256,  251,  252,  260,  261,  253,
 /*   160 */   254,  259,  257,  258,  201,  203,  204,  179,  202,  264,
 /*   170 */   265,  180,  248,  206,  231,  230,  185,
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
 /*  60 */ "expression ::= expression INHERITS expression",
 /*  61 */ "expression ::= lvalue",
 /*  62 */ "expression ::= INTEGER",
 /*  63 */ "expression ::= NEGATIVE INTEGER",
 /*  64 */ "expression ::= FLOATNUM",
 /*  65 */ "expression ::= NEGATIVE FLOATNUM",
 /*  66 */ "expression ::= LITERALSTRING",
 /*  67 */ "expression ::= REGEXSTRING",
 /*  68 */ "expression ::= TRUE",
 /*  69 */ "expression ::= FALSE",
 /*  70 */ "expression ::= NULL",
 /*  71 */ "expression ::= FUNCTION LEFTPAREN func_args RIGHTPAREN statement_block",
 /*  72 */ "expression ::= expression QUESTIONMARK expression COLON expression",
 /*  73 */ "lvalue ::= lvalue_indexable",
 /*  74 */ "lvalue ::= VAR IDENTIFIER",
 /*  75 */ "lvalue ::= VAR GROUPLEFTPAREN ident_list RIGHTPAREN",
 /*  76 */ "lvalue ::= GROUPLEFTPAREN expr_list RIGHTPAREN",
 /*  77 */ "lvalue_indexable ::= THIS",
 /*  78 */ "lvalue_indexable ::= lvalue_indexable paren_expr_list",
 /*  79 */ "lvalue_indexable ::= lvalue_indexable OPENBRACKET expression CLOSEBRACKET",
 /*  80 */ "lvalue_indexable ::= lvalue_indexable PERIOD IDENTIFIER",
 /*  81 */ "lvalue_indexable ::= lvalue_indexable COLONCOLON IDENTIFIER",
 /*  82 */ "lvalue_indexable ::= IDENTIFIER",
 /*  83 */ "ident_list ::= ident_list COMMA IDENTIFIER",
 /*  84 */ "ident_list ::= IDENTIFIER",
 /*  85 */ "ident_list ::=",
 /*  86 */ "func_args ::= ident_list",
 /*  87 */ "func_args ::= ident_list COMMA ELLIPSIS IDENTIFIER",
 /*  88 */ "func_args ::= ELLIPSIS IDENTIFIER",
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
#line 132 "/home/joe/private/work/eureka/lib/ekParser.y"
 ekSyntaxDestroy(C->E, (yypminor->yy94)); 
#line 736 "/home/joe/private/work/eureka/lib/ekParser.c"
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
#line 123 "/home/joe/private/work/eureka/lib/ekParser.y"
{ C->root = yymsp[0].minor.yy94; }
#line 1110 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 1: /* statement_list ::= statement_list statement */
#line 135 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxListAppend(C->E, yymsp[-1].minor.yy94, yymsp[0].minor.yy94, 0); }
#line 1115 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 2: /* statement_list ::= statement */
      case 5: /* statement_block ::= statement */ yytestcase(yyruleno==5);
#line 138 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateList(C->E, EST_STATEMENTLIST, yymsp[0].minor.yy94); }
#line 1121 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 3: /* statement_block ::= STARTBLOCK ENDBLOCK */
      case 24: /* statement ::= ENDSTATEMENT */ yytestcase(yyruleno==24);
      case 25: /* statement ::= SCOPESTARTBLOCK ENDBLOCK */ yytestcase(yyruleno==25);
#line 150 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateList(C->E, EST_STATEMENTLIST, NULL); }
#line 1128 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 4: /* statement_block ::= STARTBLOCK statement_list ENDBLOCK */
      case 28: /* paren_expr_list ::= LEFTPAREN expr_list RIGHTPAREN */ yytestcase(yyruleno==28);
      case 76: /* lvalue ::= GROUPLEFTPAREN expr_list RIGHTPAREN */ yytestcase(yyruleno==76);
#line 153 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = yymsp[-1].minor.yy94; }
#line 1135 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 6: /* statement ::= BREAK ENDSTATEMENT */
#line 168 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBreak(C->E, yymsp[-1].minor.yy0.line); }
#line 1140 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 7: /* statement ::= RETURN expr_list ENDSTATEMENT */
      case 8: /* statement ::= RETURN paren_expr_list ENDSTATEMENT */ yytestcase(yyruleno==8);
#line 171 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateReturn(C->E, yymsp[-1].minor.yy94); }
#line 1146 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 9: /* statement ::= expr_list ENDSTATEMENT */
#line 177 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateStatementExpr(C->E, yymsp[-1].minor.yy94); }
#line 1151 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 10: /* statement ::= IF expr_list statement_block ELSE statement_block */
#line 180 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateIfElse(C->E, yymsp[-3].minor.yy94, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1156 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 11: /* statement ::= IF expr_list statement_block */
#line 183 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateIfElse(C->E, yymsp[-1].minor.yy94, yymsp[0].minor.yy94, NULL, ekFalse); }
#line 1161 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 12: /* statement ::= WHILE expr_list statement_block */
#line 186 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateWhile(C->E, yymsp[-1].minor.yy94, yymsp[0].minor.yy94); }
#line 1166 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 13: /* statement ::= FUNCTION IDENTIFIER LEFTPAREN func_args RIGHTPAREN statement_block */
#line 189 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateFunctionDecl(C->E, &yymsp[-4].minor.yy0, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, yymsp[-5].minor.yy0.line); }
#line 1171 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 14: /* statement ::= FOR LEFTPAREN ident_list IN expression RIGHTPAREN statement_block */
#line 192 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateFor(C->E, yymsp[-4].minor.yy94, yymsp[-2].minor.yy94, yymsp[0].minor.yy94); }
#line 1176 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 15: /* statement ::= lvalue PLUSEQUALS expression */
#line 195 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_ADD, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1181 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 16: /* statement ::= lvalue DASHEQUALS expression */
#line 198 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_SUB, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1186 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 17: /* statement ::= lvalue STAREQUALS expression */
#line 201 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_MUL, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1191 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 18: /* statement ::= lvalue SLASHEQUALS expression */
#line 204 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_DIV, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1196 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 19: /* statement ::= lvalue BITWISE_OREQUALS expression */
#line 207 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_BITWISE_OR, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1201 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 20: /* statement ::= lvalue BITWISE_ANDEQUALS expression */
#line 210 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_BITWISE_AND, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1206 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 21: /* statement ::= lvalue BITWISE_XOREQUALS expression */
#line 213 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_BITWISE_XOR, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1211 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 22: /* statement ::= lvalue SHIFTLEFTEQUALS expression */
#line 216 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_SHIFTLEFT, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1216 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 23: /* statement ::= lvalue SHIFTRIGHTEQUALS expression */
#line 219 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_SHIFTRIGHT, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1221 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 26: /* statement ::= SCOPESTARTBLOCK statement_list ENDBLOCK */
#line 228 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateScope(C->E, yymsp[-1].minor.yy94); }
#line 1226 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 29: /* paren_expr_list ::= LEFTPAREN RIGHTPAREN */
#line 245 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateList(C->E, EST_EXPRESSIONLIST, NULL); }
#line 1231 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 30: /* expr_list ::= expr_list COMMA expression */
#line 257 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxListAppend(C->E, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, 0); }
#line 1236 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 31: /* expr_list ::= expr_list FATCOMMA expression */
#line 260 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxListAppend(C->E, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ESLF_AUTOLITERAL); }
#line 1241 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 32: /* expr_list ::= expression */
#line 263 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateList(C->E, EST_EXPRESSIONLIST, yymsp[0].minor.yy94); }
#line 1246 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 33: /* expression ::= NOT expression */
#line 275 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateUnary(C->E, EST_NOT, yymsp[0].minor.yy94, yymsp[0].minor.yy94->line); }
#line 1251 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 34: /* expression ::= BITWISE_NOT expression */
#line 278 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateUnary(C->E, EST_BITWISE_NOT, yymsp[0].minor.yy94, yymsp[0].minor.yy94->line); }
#line 1256 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 35: /* expression ::= expression PLUS expression */
#line 281 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_ADD, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1261 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 36: /* expression ::= expression DASH expression */
#line 284 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_SUB, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1266 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 37: /* expression ::= expression STAR expression */
#line 287 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_MUL, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1271 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 38: /* expression ::= expression SLASH expression */
#line 290 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_DIV, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1276 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 39: /* expression ::= expression AND expression */
#line 293 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_AND, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1281 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 40: /* expression ::= expression OR expression */
#line 296 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_OR, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1286 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 41: /* expression ::= expression CMP expression */
#line 299 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_CMP, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1291 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 42: /* expression ::= expression EQUALS expression */
#line 302 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_EQUALS, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1296 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 43: /* expression ::= expression NOTEQUALS expression */
#line 305 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_NOTEQUALS, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1301 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 44: /* expression ::= expression GREATERTHAN expression */
#line 308 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_GREATERTHAN, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1306 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 45: /* expression ::= expression GREATERTHANOREQUAL expression */
#line 311 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_GREATERTHANOREQUAL, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1311 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 46: /* expression ::= expression LESSTHAN expression */
#line 314 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_LESSTHAN, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1316 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 47: /* expression ::= expression LESSTHANOREQUAL expression */
#line 317 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_LESSTHANOREQUAL, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1321 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 48: /* expression ::= expression BITWISE_XOR expression */
#line 320 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_BITWISE_XOR, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1326 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 49: /* expression ::= expression BITWISE_AND expression */
#line 323 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_BITWISE_AND, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1331 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 50: /* expression ::= expression BITWISE_OR expression */
#line 326 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_BITWISE_OR, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1336 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 51: /* expression ::= expression SHIFTLEFT expression */
#line 329 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_SHIFTLEFT, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1341 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 52: /* expression ::= expression SHIFTRIGHT expression */
#line 332 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_SHIFTRIGHT, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1346 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 53: /* expression ::= expression MOD paren_expr_list */
      case 54: /* expression ::= expression MOD expression */ yytestcase(yyruleno==54);
#line 335 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateStringFormat(C->E, yymsp[-2].minor.yy94, yymsp[0].minor.yy94); }
#line 1352 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 55: /* expression ::= ARRAYOPENBRACKET CLOSEBRACKET */
#line 341 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateUnary(C->E, EST_ARRAY, NULL, yymsp[-1].minor.yy0.line); }
#line 1357 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 56: /* expression ::= ARRAYOPENBRACKET expr_list CLOSEBRACKET */
#line 344 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateUnary(C->E, EST_ARRAY, yymsp[-1].minor.yy94, yymsp[-1].minor.yy94->line); }
#line 1362 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 57: /* expression ::= MAPSTARTBLOCK ENDBLOCK */
#line 347 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateUnary(C->E, EST_MAP, NULL, yymsp[-1].minor.yy0.line); }
#line 1367 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 58: /* expression ::= MAPSTARTBLOCK expr_list ENDBLOCK */
#line 350 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateUnary(C->E, EST_MAP, yymsp[-1].minor.yy94, yymsp[-1].minor.yy94->line); }
#line 1372 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 59: /* expression ::= lvalue ASSIGN expression */
#line 353 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateAssignment(C->E, yymsp[-2].minor.yy94, yymsp[0].minor.yy94); }
#line 1377 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 60: /* expression ::= expression INHERITS expression */
#line 356 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateInherits(C->E, yymsp[-2].minor.yy94, yymsp[0].minor.yy94); }
#line 1382 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 61: /* expression ::= lvalue */
      case 73: /* lvalue ::= lvalue_indexable */ yytestcase(yyruleno==73);
#line 359 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = yymsp[0].minor.yy94; }
#line 1388 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 62: /* expression ::= INTEGER */
#line 362 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateKInt(C->E, &yymsp[0].minor.yy0, 0); }
#line 1393 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 63: /* expression ::= NEGATIVE INTEGER */
#line 365 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateKInt(C->E, &yymsp[0].minor.yy0, CKO_NEGATIVE); }
#line 1398 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 64: /* expression ::= FLOATNUM */
#line 368 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateKFloat(C->E, &yymsp[0].minor.yy0, 0); }
#line 1403 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 65: /* expression ::= NEGATIVE FLOATNUM */
#line 371 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateKFloat(C->E, &yymsp[0].minor.yy0, CKO_NEGATIVE); }
#line 1408 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 66: /* expression ::= LITERALSTRING */
#line 374 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateKString(C->E, &yymsp[0].minor.yy0, 0); }
#line 1413 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 67: /* expression ::= REGEXSTRING */
#line 377 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateKString(C->E, &yymsp[0].minor.yy0, 1); }
#line 1418 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 68: /* expression ::= TRUE */
#line 380 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBool(C->E, &yymsp[0].minor.yy0, ekTrue); }
#line 1423 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 69: /* expression ::= FALSE */
#line 383 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBool(C->E, &yymsp[0].minor.yy0, ekFalse); }
#line 1428 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 70: /* expression ::= NULL */
#line 386 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateNull(C->E, yymsp[0].minor.yy0.line); }
#line 1433 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 71: /* expression ::= FUNCTION LEFTPAREN func_args RIGHTPAREN statement_block */
#line 389 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateFunctionDecl(C->E, NULL, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, yymsp[-4].minor.yy0.line); }
#line 1438 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 72: /* expression ::= expression QUESTIONMARK expression COLON expression */
#line 392 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateIfElse(C->E, yymsp[-4].minor.yy94, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1443 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 74: /* lvalue ::= VAR IDENTIFIER */
#line 408 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxMarkVar(C->E, ekSyntaxCreateList(C->E, EST_IDENTIFIERLIST, ekSyntaxCreateIdentifier(C->E, &yymsp[0].minor.yy0))); }
#line 1448 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 75: /* lvalue ::= VAR GROUPLEFTPAREN ident_list RIGHTPAREN */
#line 411 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxMarkVar(C->E, yymsp[-1].minor.yy94); }
#line 1453 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 77: /* lvalue_indexable ::= THIS */
#line 426 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateThis(C->E, yymsp[0].minor.yy0.line); }
#line 1458 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 78: /* lvalue_indexable ::= lvalue_indexable paren_expr_list */
#line 429 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateCall(C->E, yymsp[-1].minor.yy94, yymsp[0].minor.yy94); }
#line 1463 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 79: /* lvalue_indexable ::= lvalue_indexable OPENBRACKET expression CLOSEBRACKET */
#line 432 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateIndex(C->E, yymsp[-3].minor.yy94, yymsp[-1].minor.yy94, ekFalse); }
#line 1468 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 80: /* lvalue_indexable ::= lvalue_indexable PERIOD IDENTIFIER */
#line 435 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateIndex(C->E, yymsp[-2].minor.yy94, ekSyntaxCreateKString(C->E, &yymsp[0].minor.yy0, 0), ekFalse); }
#line 1473 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 81: /* lvalue_indexable ::= lvalue_indexable COLONCOLON IDENTIFIER */
#line 438 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateIndex(C->E, yymsp[-2].minor.yy94, ekSyntaxCreateKString(C->E, &yymsp[0].minor.yy0, 0), ekTrue); }
#line 1478 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 82: /* lvalue_indexable ::= IDENTIFIER */
#line 441 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateIdentifier(C->E, &yymsp[0].minor.yy0); }
#line 1483 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 83: /* ident_list ::= ident_list COMMA IDENTIFIER */
#line 453 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxListAppend(C->E, yymsp[-2].minor.yy94, ekSyntaxCreateIdentifier(C->E, &yymsp[0].minor.yy0), 0); }
#line 1488 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 84: /* ident_list ::= IDENTIFIER */
#line 456 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateList(C->E, EST_IDENTIFIERLIST, ekSyntaxCreateIdentifier(C->E, &yymsp[0].minor.yy0)); }
#line 1493 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 85: /* ident_list ::= */
#line 459 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateList(C->E, EST_IDENTIFIERLIST, NULL); }
#line 1498 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 86: /* func_args ::= ident_list */
#line 468 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateFunctionArgs(C->E, yymsp[0].minor.yy94, NULL); }
#line 1503 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 87: /* func_args ::= ident_list COMMA ELLIPSIS IDENTIFIER */
#line 471 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateFunctionArgs(C->E, yymsp[-3].minor.yy94, &yymsp[0].minor.yy0); }
#line 1508 "/home/joe/private/work/eureka/lib/ekParser.c"
        break;
      case 88: /* func_args ::= ELLIPSIS IDENTIFIER */
#line 474 "/home/joe/private/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateFunctionArgs(C->E, ekSyntaxCreateList(C->E, EST_IDENTIFIERLIST, NULL), &yymsp[0].minor.yy0); }
#line 1513 "/home/joe/private/work/eureka/lib/ekParser.c"
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
#line 117 "/home/joe/private/work/eureka/lib/ekParser.y"
 ekCompileSyntaxError(C, &TOKEN); 
#line 1578 "/home/joe/private/work/eureka/lib/ekParser.c"
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
