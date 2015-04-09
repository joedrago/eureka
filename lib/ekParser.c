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
#define YYNSTATE 181
#define YYNRULE 95
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
 /*     0 */    18,  148,    7,    4,   13,  107,   25,   16,   20,   22,
 /*    10 */    42,   25,   44,   20,   17,   82,  113,   59,   62,  135,
 /*    20 */   115,   52,   54,   53,   56,   55,   47,   51,   49,   50,
 /*    30 */    85,  113,   59,   30,   21,   38,   29,   39,   26,   24,
 /*    40 */    32,   33,   31,   36,   37,   34,   35,   43,   41,   40,
 /*    50 */    23,   27,   28,   15,   40,   23,   27,   28,   15,  178,
 /*    60 */   157,   12,   19,  108,  136,   58,   63,  130,   25,  150,
 /*    70 */   172,  168,  166,  164,  162,  160,  158,  109,   18,   64,
 /*    80 */     7,    4,   13,  107,  125,   16,    9,  167,  152,   61,
 /*    90 */   132,  129,   17,   68,   94,   59,   62,  135,   29,   39,
 /*   100 */    22,   42,   32,   33,   31,   36,   37,   34,   35,   43,
 /*   110 */    41,   40,   23,   27,   28,   15,   26,   24,   18,  114,
 /*   120 */     7,    4,   13,  107,  125,   16,   11,  167,  120,   61,
 /*   130 */   170,  169,   17,   68,   94,   59,   62,  135,  157,   12,
 /*   140 */    19,  108,  136,  179,   63,  133,   22,   42,  172,  168,
 /*   150 */   166,  164,  162,  160,  158,  109,   26,   24,   18,  131,
 /*   160 */     7,  106,   13,  107,  125,   16,  131,  159,   25,   61,
 /*   170 */   117,  126,   17,   68,   94,   59,   62,  135,  157,   12,
 /*   180 */    19,  108,  136,  111,   63,  118,  113,   59,  172,  168,
 /*   190 */   166,  164,  162,  160,  158,  109,   26,   24,  128,   39,
 /*   200 */   155,  122,   32,   33,   31,   36,   37,   34,   35,   43,
 /*   210 */    41,   40,   23,   27,   28,   15,  153,  156,  157,   12,
 /*   220 */    19,  108,  136,   25,   63,  121,  126,   25,  172,  168,
 /*   230 */   166,  164,  162,  160,  158,  109,  181,   18,  142,    7,
 /*   240 */   173,   13,  107,  125,   16,   58,  151,  176,   61,   28,
 /*   250 */    15,   17,   68,   94,   59,   62,  135,   32,   33,   31,
 /*   260 */    36,   37,   34,   35,   43,   41,   40,   23,   27,   28,
 /*   270 */    15,   23,   27,   28,   15,   26,   24,   18,    5,    7,
 /*   280 */    15,   13,  107,  125,   16,   25,  151,  144,   61,   46,
 /*   290 */   122,   17,   68,   94,   59,   62,  135,  157,   12,   19,
 /*   300 */   108,  136,    6,   63,   27,   28,   15,  172,  168,  166,
 /*   310 */   164,  162,  160,  158,  109,   26,   24,   57,   25,    8,
 /*   320 */    33,   31,   36,   37,   34,   35,   43,   41,   40,   23,
 /*   330 */    27,   28,   15,  112,  113,   59,  127,  157,   12,   19,
 /*   340 */   108,  136,  130,   63,   25,  138,  140,  172,  168,  166,
 /*   350 */   164,  162,  160,  158,  109,   18,  165,    7,   25,   13,
 /*   360 */   107,  110,   16,   27,   28,   15,   25,  125,  278,   17,
 /*   370 */   151,  180,   61,   62,  135,  278,   68,   94,   59,  278,
 /*   380 */    31,   36,   37,   34,   35,   43,   41,   40,   23,   27,
 /*   390 */    28,   15,  278,   26,   24,   36,   37,   34,   35,   43,
 /*   400 */    41,   40,   23,   27,   28,   15,   35,   43,   41,   40,
 /*   410 */    23,   27,   28,   15,  171,  157,   12,   19,  108,  136,
 /*   420 */   278,   63,   25,  278,  278,  172,  168,  166,  164,  162,
 /*   430 */   160,  158,  109,   18,   25,    7,  278,   13,  107,  125,
 /*   440 */    16,   25,  151,  147,   61,  278,  154,   17,   68,   94,
 /*   450 */    59,   62,  135,  145,   22,   42,  143,  113,   59,  278,
 /*   460 */    37,   34,   35,   43,   41,   40,   23,   27,   28,   15,
 /*   470 */   278,   26,   24,   34,   35,   43,   41,   40,   23,   27,
 /*   480 */    28,   15,   43,   41,   40,   23,   27,   28,   15,   88,
 /*   490 */   113,   59,  161,  157,   12,   19,  108,  136,  278,   63,
 /*   500 */   278,   25,  278,  172,  168,  166,  164,  162,  160,  158,
 /*   510 */   109,   18,  278,    7,  278,   13,  107,  125,   16,  278,
 /*   520 */   151,  119,   61,   25,  278,   17,   68,   94,   59,   62,
 /*   530 */   135,  125,  277,   10,  167,  278,   61,  104,  113,   59,
 /*   540 */    68,   94,   59,   41,   40,   23,   27,   28,   15,   26,
 /*   550 */    24,   21,   38,   29,   39,  278,  278,   32,   33,   31,
 /*   560 */    36,   37,   34,   35,   43,   41,   40,   23,   27,   28,
 /*   570 */    15,  157,   12,   19,  108,  136,  278,   63,   87,  113,
 /*   580 */    59,  172,  168,  166,  164,  162,  160,  158,  109,   25,
 /*   590 */   100,  113,   59,  278,  278,   44,   45,  125,  278,  278,
 /*   600 */   151,  149,   61,  174,  278,  102,   68,   94,   59,   68,
 /*   610 */   113,   59,   70,  113,   59,  278,   30,   21,   38,   29,
 /*   620 */    39,  278,  278,   32,   33,   31,   36,   37,   34,   35,
 /*   630 */    43,   41,   40,   23,   27,   28,   15,   25,  278,   14,
 /*   640 */    71,  113,   59,   44,   60,   65,   48,  101,   68,  113,
 /*   650 */    59,   68,  113,   59,    3,  278,  278,    2,   68,  113,
 /*   660 */    59,   68,  113,   59,   30,   21,   38,   29,   39,  278,
 /*   670 */   278,   32,   33,   31,   36,   37,   34,   35,   43,   41,
 /*   680 */    40,   23,   27,   28,   15,   25,   83,  113,   59,  177,
 /*   690 */   105,   44,  123,  124,   68,  113,   59,  103,   97,  113,
 /*   700 */    59,   68,  113,   59,  134,  278,  139,   22,   42,   22,
 /*   710 */    42,  278,   30,   21,   38,   29,   39,  278,  278,   32,
 /*   720 */    33,   31,   36,   37,   34,   35,   43,   41,   40,   23,
 /*   730 */    27,   28,   15,   25,   69,  113,   59,   79,  113,   59,
 /*   740 */    98,  113,   59,    1,  113,   59,   99,  113,   59,   77,
 /*   750 */   113,   59,   86,  113,   59,   67,  113,   59,  278,  278,
 /*   760 */    30,   21,   38,   29,   39,  278,  278,   32,   33,   31,
 /*   770 */    36,   37,   34,   35,   43,   41,   40,   23,   27,   28,
 /*   780 */    15,   18,   14,  278,  278,   13,  107,  278,   16,   84,
 /*   790 */   113,   59,   80,  113,   59,   93,  113,   59,  278,  278,
 /*   800 */   135,   76,  113,   59,   92,  113,   59,  278,   81,  113,
 /*   810 */    59,   96,  113,   59,  278,   78,  113,   59,  278,   26,
 /*   820 */    24,   18,   95,  113,   59,   13,  107,  278,   16,   75,
 /*   830 */   113,   59,   90,  113,   59,   72,  113,   59,  278,  278,
 /*   840 */   135,  146,   18,  278,  116,  136,   13,  107,  278,   16,
 /*   850 */   278,  172,  168,  166,  164,  162,  160,  158,  109,   26,
 /*   860 */    24,  135,  278,   18,   14,  278,  278,   13,  107,  278,
 /*   870 */    16,   89,  113,   59,   74,  113,   59,  278,  278,  137,
 /*   880 */    26,   24,  135,  278,  116,  136,   91,  113,   59,  163,
 /*   890 */   278,  172,  168,  166,  164,  162,  160,  158,  109,   22,
 /*   900 */    42,   26,   24,  278,   18,  116,  136,  141,   13,  107,
 /*   910 */   278,   16,  172,  168,  166,  164,  162,  160,  158,  109,
 /*   920 */    73,  113,   59,  135,  278,  278,  116,  136,  278,  278,
 /*   930 */    66,  113,   59,  172,  168,  166,  164,  162,  160,  158,
 /*   940 */   109,  278,   26,   24,  278,  278,  278,  278,  278,  278,
 /*   950 */   278,  278,  278,  278,  278,  278,  278,  278,  278,  278,
 /*   960 */   278,  278,  278,  175,  278,  278,  278,  116,  136,  278,
 /*   970 */   278,   25,  278,  278,  172,  168,  166,  164,  162,  160,
 /*   980 */   158,  109,  278,  278,  278,  278,  278,  278,  278,  278,
 /*   990 */   278,  278,  278,  278,  278,  278,  278,  278,  278,  278,
 /*  1000 */    38,   29,   39,  278,  278,   32,   33,   31,   36,   37,
 /*  1010 */    34,   35,   43,   41,   40,   23,   27,   28,   15,   18,
 /*  1020 */   278,  278,  278,   13,  107,  278,   16,  278,  278,  278,
 /*  1030 */   278,  278,  278,  278,  278,  278,  278,  278,  135,  278,
 /*  1040 */   278,  278,  278,  278,  278,  278,  278,  278,  278,  278,
 /*  1050 */   278,  278,  278,  278,  278,  278,  278,   26,   24,  278,
 /*  1060 */   278,  278,  278,  278,  278,  278,  278,  278,  278,  278,
 /*  1070 */   278,  278,  278,  278,  278,  278,  278,  278,  278,  278,
 /*  1080 */   278,  278,  116,  136,  278,  278,  278,  278,  278,  172,
 /*  1090 */   168,  166,  164,  162,  160,  158,  109,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */     1,   61,    3,    4,    5,    6,    7,    8,   12,   69,
 /*    10 */    70,    7,   13,   12,   15,   88,   89,   90,   19,   20,
 /*    20 */    18,   25,   26,   27,   28,   29,   30,   31,   32,   33,
 /*    30 */    88,   89,   90,   34,   35,   36,   37,   38,   39,   40,
 /*    40 */    41,   42,   43,   44,   45,   46,   47,   48,   49,   50,
 /*    50 */    51,   52,   53,   54,   50,   51,   52,   53,   54,   85,
 /*    60 */    61,   62,   63,   64,   65,    2,   67,   65,    7,   79,
 /*    70 */    71,   72,   73,   74,   75,   76,   77,   78,    1,    1,
 /*    80 */     3,    4,    5,    6,   79,    8,   81,   82,   79,   84,
 /*    90 */    61,   60,   15,   88,   89,   90,   19,   20,   37,   38,
 /*   100 */    69,   70,   41,   42,   43,   44,   45,   46,   47,   48,
 /*   110 */    49,   50,   51,   52,   53,   54,   39,   40,    1,   18,
 /*   120 */     3,    4,    5,    6,   79,    8,   81,   82,   65,   84,
 /*   130 */    71,   72,   15,   88,   89,   90,   19,   20,   61,   62,
 /*   140 */    63,   64,   65,   65,   67,   79,   69,   70,   71,   72,
 /*   150 */    73,   74,   75,   76,   77,   78,   39,   40,    1,   65,
 /*   160 */     3,   87,    5,    6,   79,    8,   65,   82,    7,   84,
 /*   170 */    86,   87,   15,   88,   89,   90,   19,   20,   61,   62,
 /*   180 */    63,   64,   65,   87,   67,   88,   89,   90,   71,   72,
 /*   190 */    73,   74,   75,   76,   77,   78,   39,   40,   66,   38,
 /*   200 */    79,   69,   41,   42,   43,   44,   45,   46,   47,   48,
 /*   210 */    49,   50,   51,   52,   53,   54,   61,   60,   61,   62,
 /*   220 */    63,   64,   65,    7,   67,   86,   87,    7,   71,   72,
 /*   230 */    73,   74,   75,   76,   77,   78,    0,    1,   65,    3,
 /*   240 */    65,    5,    6,   79,    8,    2,   82,   83,   84,   53,
 /*   250 */    54,   15,   88,   89,   90,   19,   20,   41,   42,   43,
 /*   260 */    44,   45,   46,   47,   48,   49,   50,   51,   52,   53,
 /*   270 */    54,   51,   52,   53,   54,   39,   40,    1,   66,    3,
 /*   280 */    54,    5,    6,   79,    8,    7,   82,   83,   84,   68,
 /*   290 */    69,   15,   88,   89,   90,   19,   20,   61,   62,   63,
 /*   300 */    64,   65,   16,   67,   52,   53,   54,   71,   72,   73,
 /*   310 */    74,   75,   76,   77,   78,   39,   40,    2,    7,   66,
 /*   320 */    42,   43,   44,   45,   46,   47,   48,   49,   50,   51,
 /*   330 */    52,   53,   54,   88,   89,   90,   60,   61,   62,   63,
 /*   340 */    64,   65,   65,   67,    7,   65,   65,   71,   72,   73,
 /*   350 */    74,   75,   76,   77,   78,    1,   61,    3,    7,    5,
 /*   360 */     6,   69,    8,   52,   53,   54,    7,   79,   91,   15,
 /*   370 */    82,   83,   84,   19,   20,   91,   88,   89,   90,   91,
 /*   380 */    43,   44,   45,   46,   47,   48,   49,   50,   51,   52,
 /*   390 */    53,   54,   91,   39,   40,   44,   45,   46,   47,   48,
 /*   400 */    49,   50,   51,   52,   53,   54,   47,   48,   49,   50,
 /*   410 */    51,   52,   53,   54,   60,   61,   62,   63,   64,   65,
 /*   420 */    91,   67,    7,   91,   91,   71,   72,   73,   74,   75,
 /*   430 */    76,   77,   78,    1,    7,    3,   91,    5,    6,   79,
 /*   440 */     8,    7,   82,   83,   84,   91,   61,   15,   88,   89,
 /*   450 */    90,   19,   20,   85,   69,   70,   88,   89,   90,   91,
 /*   460 */    45,   46,   47,   48,   49,   50,   51,   52,   53,   54,
 /*   470 */    91,   39,   40,   46,   47,   48,   49,   50,   51,   52,
 /*   480 */    53,   54,   48,   49,   50,   51,   52,   53,   54,   88,
 /*   490 */    89,   90,   60,   61,   62,   63,   64,   65,   91,   67,
 /*   500 */    91,    7,   91,   71,   72,   73,   74,   75,   76,   77,
 /*   510 */    78,    1,   91,    3,   91,    5,    6,   79,    8,   91,
 /*   520 */    82,   83,   84,    7,   91,   15,   88,   89,   90,   19,
 /*   530 */    20,   79,   80,   81,   82,   91,   84,   88,   89,   90,
 /*   540 */    88,   89,   90,   49,   50,   51,   52,   53,   54,   39,
 /*   550 */    40,   35,   36,   37,   38,   91,   91,   41,   42,   43,
 /*   560 */    44,   45,   46,   47,   48,   49,   50,   51,   52,   53,
 /*   570 */    54,   61,   62,   63,   64,   65,   91,   67,   88,   89,
 /*   580 */    90,   71,   72,   73,   74,   75,   76,   77,   78,    7,
 /*   590 */    88,   89,   90,   91,   91,   13,   14,   79,   91,   91,
 /*   600 */    82,   83,   84,   79,   91,   84,   88,   89,   90,   88,
 /*   610 */    89,   90,   88,   89,   90,   91,   34,   35,   36,   37,
 /*   620 */    38,   91,   91,   41,   42,   43,   44,   45,   46,   47,
 /*   630 */    48,   49,   50,   51,   52,   53,   54,    7,   91,    2,
 /*   640 */    88,   89,   90,   13,   84,   85,    9,   84,   88,   89,
 /*   650 */    90,   88,   89,   90,   84,   91,   91,   84,   88,   89,
 /*   660 */    90,   88,   89,   90,   34,   35,   36,   37,   38,   91,
 /*   670 */    91,   41,   42,   43,   44,   45,   46,   47,   48,   49,
 /*   680 */    50,   51,   52,   53,   54,    7,   88,   89,   90,   59,
 /*   690 */    84,   13,   55,   56,   88,   89,   90,   84,   88,   89,
 /*   700 */    90,   88,   89,   90,   66,   91,   66,   69,   70,   69,
 /*   710 */    70,   91,   34,   35,   36,   37,   38,   91,   91,   41,
 /*   720 */    42,   43,   44,   45,   46,   47,   48,   49,   50,   51,
 /*   730 */    52,   53,   54,    7,   88,   89,   90,   88,   89,   90,
 /*   740 */    88,   89,   90,   88,   89,   90,   88,   89,   90,   88,
 /*   750 */    89,   90,   88,   89,   90,   88,   89,   90,   91,   91,
 /*   760 */    34,   35,   36,   37,   38,   91,   91,   41,   42,   43,
 /*   770 */    44,   45,   46,   47,   48,   49,   50,   51,   52,   53,
 /*   780 */    54,    1,    2,   91,   91,    5,    6,   91,    8,   88,
 /*   790 */    89,   90,   88,   89,   90,   88,   89,   90,   91,   91,
 /*   800 */    20,   88,   89,   90,   88,   89,   90,   91,   88,   89,
 /*   810 */    90,   88,   89,   90,   91,   88,   89,   90,   91,   39,
 /*   820 */    40,    1,   88,   89,   90,    5,    6,   91,    8,   88,
 /*   830 */    89,   90,   88,   89,   90,   88,   89,   90,   91,   91,
 /*   840 */    20,   61,    1,   91,   64,   65,    5,    6,   91,    8,
 /*   850 */    91,   71,   72,   73,   74,   75,   76,   77,   78,   39,
 /*   860 */    40,   20,   91,    1,    2,   91,   91,    5,    6,   91,
 /*   870 */     8,   88,   89,   90,   88,   89,   90,   91,   91,   59,
 /*   880 */    39,   40,   20,   91,   64,   65,   88,   89,   90,   59,
 /*   890 */    91,   71,   72,   73,   74,   75,   76,   77,   78,   69,
 /*   900 */    70,   39,   40,   91,    1,   64,   65,   66,    5,    6,
 /*   910 */    91,    8,   71,   72,   73,   74,   75,   76,   77,   78,
 /*   920 */    88,   89,   90,   20,   91,   91,   64,   65,   91,   91,
 /*   930 */    88,   89,   90,   71,   72,   73,   74,   75,   76,   77,
 /*   940 */    78,   91,   39,   40,   91,   91,   91,   91,   91,   91,
 /*   950 */    91,   91,   91,   91,   91,   91,   91,   91,   91,   91,
 /*   960 */    91,   91,   91,   60,   91,   91,   91,   64,   65,   91,
 /*   970 */    91,    7,   91,   91,   71,   72,   73,   74,   75,   76,
 /*   980 */    77,   78,   91,   91,   91,   91,   91,   91,   91,   91,
 /*   990 */    91,   91,   91,   91,   91,   91,   91,   91,   91,   91,
 /*  1000 */    36,   37,   38,   91,   91,   41,   42,   43,   44,   45,
 /*  1010 */    46,   47,   48,   49,   50,   51,   52,   53,   54,    1,
 /*  1020 */    91,   91,   91,    5,    6,   91,    8,   91,   91,   91,
 /*  1030 */    91,   91,   91,   91,   91,   91,   91,   91,   20,   91,
 /*  1040 */    91,   91,   91,   91,   91,   91,   91,   91,   91,   91,
 /*  1050 */    91,   91,   91,   91,   91,   91,   91,   39,   40,   91,
 /*  1060 */    91,   91,   91,   91,   91,   91,   91,   91,   91,   91,
 /*  1070 */    91,   91,   91,   91,   91,   91,   91,   91,   91,   91,
 /*  1080 */    91,   91,   64,   65,   91,   91,   91,   91,   91,   71,
 /*  1090 */    72,   73,   74,   75,   76,   77,   78,
};
#define YY_SHIFT_USE_DFLT (-61)
#define YY_SHIFT_MAX 126
static const short yy_shift_ofst[] = {
 /*     0 */   510,   -1,   77,   77,  276,  117,  117,  354,  117,  157,
 /*    10 */   236,  432,  780,  903,  841,  862,  820, 1018, 1018, 1018,
 /*    20 */  1018, 1018, 1018, 1018, 1018, 1018, 1018, 1018, 1018, 1018,
 /*    30 */  1018, 1018, 1018, 1018, 1018, 1018, 1018, 1018, 1018, 1018,
 /*    40 */  1018, 1018, 1018, 1018, 1018, 1018, 1018, 1018, 1018, 1018,
 /*    50 */  1018, 1018, 1018, 1018, 1018, 1018, 1018,  101,  101,  637,
 /*    60 */   -60,  385,   29,   94,   94,  155,  582,  630,  678,  678,
 /*    70 */   678,  678,  726,  726,  726,  726,  726,  726,  726,  726,
 /*    80 */   726,  726,  516,  964,   61,  161,  216,  216,  216,  278,
 /*    90 */   337,  351,  415,  427,   -4,  359,  434,  494,    4,  220,
 /*   100 */   311,  830,   31,  640,  252,  638,  221,   59,   63,   78,
 /*   110 */     2,  132,  196,    1,  173,  175,  243,  212,  226,  286,
 /*   120 */   315,  253,  277,  280,  281,  295,  292,
};
#define YY_REDUCE_USE_DFLT (-74)
#define YY_REDUCE_MAX 65
static const short yy_reduce_ofst[] = {
 /*     0 */   452,  288,  360,  438,    5,  164,  518,   45,  204,   85,
 /*    10 */    85,   85,  560,  521,  613,  368,  563,  570,  606,  573,
 /*    20 */   524,  598,  552,  502,  490,  449,  401,  245,   97,  -58,
 /*    30 */   -73,  798,  783,  744,  734,  723,  716,  707,  701,  664,
 /*    40 */   658,  652,  646,  610,  842,  649,  655,  661,  667,  704,
 /*    50 */   713,  720,  727,  741,  747,  786,  832,  139,   84,  -26,
 /*    60 */   -10,    9,   66,   74,   96,  121,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   276,  276,  276,  276,  276,  276,  276,  276,  276,  276,
 /*    10 */   276,  276,  276,  276,  276,  276,  276,  276,  276,  276,
 /*    20 */   276,  276,  276,  276,  276,  276,  276,  276,  276,  276,
 /*    30 */   276,  276,  276,  276,  276,  276,  276,  276,  276,  276,
 /*    40 */   276,  276,  276,  276,  276,  276,  276,  276,  276,  276,
 /*    50 */   276,  276,  276,  276,  276,  276,  276,  272,  272,  260,
 /*    60 */   276,  276,  276,  272,  272,  276,  276,  276,  218,  217,
 /*    70 */   245,  216,  207,  209,  208,  206,  203,  201,  205,  259,
 /*    80 */   202,  204,  226,  236,  234,  225,  235,  219,  220,  228,
 /*    90 */   229,  227,  232,  233,  248,  230,  231,  247,  238,  237,
 /*   100 */   221,  276,  276,  276,  222,  276,  276,  276,  276,  276,
 /*   110 */   276,  276,  223,  248,  276,  276,  276,  276,  224,  197,
 /*   120 */   276,  276,  276,  276,  276,  276,  273,  184,  262,  244,
 /*   130 */   270,  271,  187,  188,  263,  264,  269,  241,  267,  214,
 /*   140 */   268,  215,  275,  240,  199,  239,  189,  198,  190,  196,
 /*   150 */   191,  186,  195,  192,  194,  193,  185,  210,  257,  182,
 /*   160 */   256,  212,  255,  242,  254,  213,  253,  183,  251,  252,
 /*   170 */   250,  211,  249,  274,  246,  243,  258,  266,  265,  261,
 /*   180 */   200,
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
 /*  14 */ "statement ::= expr_list error",
 /*  15 */ "statement ::= IF expr_list statement_block ELSE statement_block",
 /*  16 */ "statement ::= IF expr_list statement_block",
 /*  17 */ "statement ::= WHILE expr_list statement_block",
 /*  18 */ "statement ::= FUNCTION IDENTIFIER LEFTPAREN func_args RIGHTPAREN statement_block",
 /*  19 */ "statement ::= FOR ident_list IN expression statement_block",
 /*  20 */ "statement ::= lvalue PLUSEQUALS expression",
 /*  21 */ "statement ::= lvalue DASHEQUALS expression",
 /*  22 */ "statement ::= lvalue STAREQUALS expression",
 /*  23 */ "statement ::= lvalue SLASHEQUALS expression",
 /*  24 */ "statement ::= lvalue BITWISE_OREQUALS expression",
 /*  25 */ "statement ::= lvalue BITWISE_ANDEQUALS expression",
 /*  26 */ "statement ::= lvalue BITWISE_XOREQUALS expression",
 /*  27 */ "statement ::= lvalue SHIFTLEFTEQUALS expression",
 /*  28 */ "statement ::= lvalue SHIFTRIGHTEQUALS expression",
 /*  29 */ "statement ::= ENDSTATEMENT",
 /*  30 */ "statement ::= SCOPESTARTBLOCK ENDBLOCK",
 /*  31 */ "statement ::= SCOPESTARTBLOCK statement_list ENDBLOCK",
 /*  32 */ "statement ::= error ENDSTATEMENT",
 /*  33 */ "paren_expr_list ::= LEFTPAREN expr_list RIGHTPAREN",
 /*  34 */ "paren_expr_list ::= LEFTPAREN RIGHTPAREN",
 /*  35 */ "expr_list ::= expr_list COMMA expression",
 /*  36 */ "expr_list ::= expr_list FATCOMMA expression",
 /*  37 */ "expr_list ::= expression",
 /*  38 */ "expression ::= NOT expression",
 /*  39 */ "expression ::= BITWISE_NOT expression",
 /*  40 */ "expression ::= expression PLUS expression",
 /*  41 */ "expression ::= expression DASH expression",
 /*  42 */ "expression ::= expression STAR expression",
 /*  43 */ "expression ::= expression SLASH expression",
 /*  44 */ "expression ::= expression AND expression",
 /*  45 */ "expression ::= expression OR expression",
 /*  46 */ "expression ::= expression CMP expression",
 /*  47 */ "expression ::= expression EQUALS expression",
 /*  48 */ "expression ::= expression NOTEQUALS expression",
 /*  49 */ "expression ::= expression GREATERTHAN expression",
 /*  50 */ "expression ::= expression GREATERTHANOREQUAL expression",
 /*  51 */ "expression ::= expression LESSTHAN expression",
 /*  52 */ "expression ::= expression LESSTHANOREQUAL expression",
 /*  53 */ "expression ::= expression BITWISE_XOR expression",
 /*  54 */ "expression ::= expression BITWISE_AND expression",
 /*  55 */ "expression ::= expression BITWISE_OR expression",
 /*  56 */ "expression ::= expression SHIFTLEFT expression",
 /*  57 */ "expression ::= expression SHIFTRIGHT expression",
 /*  58 */ "expression ::= expression MOD paren_expr_list",
 /*  59 */ "expression ::= expression MOD expression",
 /*  60 */ "expression ::= ARRAYOPENBRACKET CLOSEBRACKET",
 /*  61 */ "expression ::= ARRAYOPENBRACKET expr_list CLOSEBRACKET",
 /*  62 */ "expression ::= MAPSTARTBLOCK ENDBLOCK",
 /*  63 */ "expression ::= MAPSTARTBLOCK expr_list ENDBLOCK",
 /*  64 */ "expression ::= lvalue ASSIGN expression",
 /*  65 */ "expression ::= lvalue ASSIGN error",
 /*  66 */ "expression ::= expression INHERITS expression",
 /*  67 */ "expression ::= lvalue",
 /*  68 */ "expression ::= INTEGER",
 /*  69 */ "expression ::= NEGATIVE INTEGER",
 /*  70 */ "expression ::= FLOATNUM",
 /*  71 */ "expression ::= NEGATIVE FLOATNUM",
 /*  72 */ "expression ::= LITERALSTRING",
 /*  73 */ "expression ::= REGEXSTRING",
 /*  74 */ "expression ::= TRUE",
 /*  75 */ "expression ::= FALSE",
 /*  76 */ "expression ::= NULL",
 /*  77 */ "expression ::= FUNCTION LEFTPAREN func_args RIGHTPAREN statement_block",
 /*  78 */ "expression ::= expression QUESTIONMARK expression COLON expression",
 /*  79 */ "lvalue ::= lvalue_indexable",
 /*  80 */ "lvalue ::= VAR IDENTIFIER",
 /*  81 */ "lvalue ::= VAR GROUPLEFTPAREN ident_list RIGHTPAREN",
 /*  82 */ "lvalue ::= GROUPLEFTPAREN expr_list RIGHTPAREN",
 /*  83 */ "lvalue_indexable ::= THIS",
 /*  84 */ "lvalue_indexable ::= lvalue_indexable paren_expr_list",
 /*  85 */ "lvalue_indexable ::= lvalue_indexable OPENBRACKET expression CLOSEBRACKET",
 /*  86 */ "lvalue_indexable ::= lvalue_indexable PERIOD IDENTIFIER",
 /*  87 */ "lvalue_indexable ::= lvalue_indexable COLONCOLON IDENTIFIER",
 /*  88 */ "lvalue_indexable ::= IDENTIFIER",
 /*  89 */ "ident_list ::= ident_list COMMA IDENTIFIER",
 /*  90 */ "ident_list ::= IDENTIFIER",
 /*  91 */ "ident_list ::=",
 /*  92 */ "func_args ::= ident_list",
 /*  93 */ "func_args ::= ident_list COMMA ELLIPSIS IDENTIFIER",
 /*  94 */ "func_args ::= ELLIPSIS IDENTIFIER",
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
#line 757 "/Users/jdrago/work/eureka/lib/ekParser.c"
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
  { 82, 2 },
  { 82, 5 },
  { 82, 3 },
  { 82, 3 },
  { 82, 6 },
  { 82, 5 },
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
#line 1137 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 1: /* statement_list ::= statement_list statement */
#line 135 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxListAppend(C->E, yymsp[-1].minor.yy94, yymsp[0].minor.yy94, 0); }
#line 1142 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 2: /* statement_list ::= statement */
      case 5: /* statement_block ::= statement */ yytestcase(yyruleno==5);
#line 138 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateList(C->E, EST_STATEMENTLIST, yymsp[0].minor.yy94); }
#line 1148 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 3: /* statement_block ::= STARTBLOCK ENDBLOCK */
      case 29: /* statement ::= ENDSTATEMENT */ yytestcase(yyruleno==29);
      case 30: /* statement ::= SCOPESTARTBLOCK ENDBLOCK */ yytestcase(yyruleno==30);
#line 150 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateList(C->E, EST_STATEMENTLIST, NULL); }
#line 1155 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 4: /* statement_block ::= STARTBLOCK statement_list ENDBLOCK */
      case 33: /* paren_expr_list ::= LEFTPAREN expr_list RIGHTPAREN */ yytestcase(yyruleno==33);
      case 82: /* lvalue ::= GROUPLEFTPAREN expr_list RIGHTPAREN */ yytestcase(yyruleno==82);
#line 153 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = yymsp[-1].minor.yy94; }
#line 1162 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 6: /* statement ::= BREAK ENDSTATEMENT */
#line 168 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBreak(C->E, yymsp[-1].minor.yy0.line); }
#line 1167 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 7: /* statement ::= BREAK error */
      case 10: /* statement ::= RETURN expr_list error */ yytestcase(yyruleno==10);
      case 12: /* statement ::= RETURN paren_expr_list error */ yytestcase(yyruleno==12);
      case 14: /* statement ::= expr_list error */ yytestcase(yyruleno==14);
#line 171 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ ekCompileExplainError(C, "expected ;"); }
#line 1175 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 8: /* statement ::= RETURN ENDSTATEMENT */
#line 174 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateReturn(C->E, yymsp[-1].minor.yy0.line, NULL); }
#line 1180 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 9: /* statement ::= RETURN expr_list ENDSTATEMENT */
      case 11: /* statement ::= RETURN paren_expr_list ENDSTATEMENT */ yytestcase(yyruleno==11);
#line 177 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateReturn(C->E, yymsp[-1].minor.yy94->line, yymsp[-1].minor.yy94); }
#line 1186 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 13: /* statement ::= expr_list ENDSTATEMENT */
#line 189 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateStatementExpr(C->E, yymsp[-1].minor.yy94); }
#line 1191 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 15: /* statement ::= IF expr_list statement_block ELSE statement_block */
#line 195 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateIfElse(C->E, yymsp[-3].minor.yy94, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1196 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 16: /* statement ::= IF expr_list statement_block */
#line 198 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateIfElse(C->E, yymsp[-1].minor.yy94, yymsp[0].minor.yy94, NULL, ekFalse); }
#line 1201 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 17: /* statement ::= WHILE expr_list statement_block */
#line 201 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateWhile(C->E, yymsp[-1].minor.yy94, yymsp[0].minor.yy94); }
#line 1206 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 18: /* statement ::= FUNCTION IDENTIFIER LEFTPAREN func_args RIGHTPAREN statement_block */
#line 204 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateFunctionDecl(C->E, &yymsp[-4].minor.yy0, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, yymsp[-5].minor.yy0.line); }
#line 1211 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 19: /* statement ::= FOR ident_list IN expression statement_block */
#line 207 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateFor(C->E, yymsp[-3].minor.yy94, yymsp[-1].minor.yy94, yymsp[0].minor.yy94); }
#line 1216 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 20: /* statement ::= lvalue PLUSEQUALS expression */
#line 210 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_ADD, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1221 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 21: /* statement ::= lvalue DASHEQUALS expression */
#line 213 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_SUB, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1226 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 22: /* statement ::= lvalue STAREQUALS expression */
#line 216 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_MUL, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1231 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 23: /* statement ::= lvalue SLASHEQUALS expression */
#line 219 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_DIV, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1236 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 24: /* statement ::= lvalue BITWISE_OREQUALS expression */
#line 222 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_BITWISE_OR, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1241 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 25: /* statement ::= lvalue BITWISE_ANDEQUALS expression */
#line 225 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_BITWISE_AND, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1246 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 26: /* statement ::= lvalue BITWISE_XOREQUALS expression */
#line 228 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_BITWISE_XOR, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1251 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 27: /* statement ::= lvalue SHIFTLEFTEQUALS expression */
#line 231 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_SHIFTLEFT, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1256 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 28: /* statement ::= lvalue SHIFTRIGHTEQUALS expression */
#line 234 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_SHIFTRIGHT, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1261 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 31: /* statement ::= SCOPESTARTBLOCK statement_list ENDBLOCK */
#line 243 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateScope(C->E, yymsp[-1].minor.yy94); }
#line 1266 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 34: /* paren_expr_list ::= LEFTPAREN RIGHTPAREN */
#line 260 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateList(C->E, EST_EXPRESSIONLIST, NULL); }
#line 1271 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 35: /* expr_list ::= expr_list COMMA expression */
#line 272 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxListAppend(C->E, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, 0); }
#line 1276 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 36: /* expr_list ::= expr_list FATCOMMA expression */
#line 275 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxListAppend(C->E, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ESLF_AUTOLITERAL); }
#line 1281 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 37: /* expr_list ::= expression */
#line 278 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateList(C->E, EST_EXPRESSIONLIST, yymsp[0].minor.yy94); }
#line 1286 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 38: /* expression ::= NOT expression */
#line 290 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateUnary(C->E, EST_NOT, yymsp[0].minor.yy94, yymsp[0].minor.yy94->line); }
#line 1291 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 39: /* expression ::= BITWISE_NOT expression */
#line 293 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateUnary(C->E, EST_BITWISE_NOT, yymsp[0].minor.yy94, yymsp[0].minor.yy94->line); }
#line 1296 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 40: /* expression ::= expression PLUS expression */
#line 296 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_ADD, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1301 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 41: /* expression ::= expression DASH expression */
#line 299 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_SUB, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1306 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 42: /* expression ::= expression STAR expression */
#line 302 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_MUL, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1311 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 43: /* expression ::= expression SLASH expression */
#line 305 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_DIV, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1316 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 44: /* expression ::= expression AND expression */
#line 308 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_AND, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1321 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 45: /* expression ::= expression OR expression */
#line 311 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_OR, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1326 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 46: /* expression ::= expression CMP expression */
#line 314 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_CMP, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1331 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 47: /* expression ::= expression EQUALS expression */
#line 317 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_EQUALS, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1336 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 48: /* expression ::= expression NOTEQUALS expression */
#line 320 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_NOTEQUALS, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1341 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 49: /* expression ::= expression GREATERTHAN expression */
#line 323 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_GREATERTHAN, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1346 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 50: /* expression ::= expression GREATERTHANOREQUAL expression */
#line 326 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_GREATERTHANOREQUAL, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1351 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 51: /* expression ::= expression LESSTHAN expression */
#line 329 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_LESSTHAN, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1356 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 52: /* expression ::= expression LESSTHANOREQUAL expression */
#line 332 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_LESSTHANOREQUAL, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1361 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 53: /* expression ::= expression BITWISE_XOR expression */
#line 335 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_BITWISE_XOR, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1366 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 54: /* expression ::= expression BITWISE_AND expression */
#line 338 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_BITWISE_AND, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1371 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 55: /* expression ::= expression BITWISE_OR expression */
#line 341 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_BITWISE_OR, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1376 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 56: /* expression ::= expression SHIFTLEFT expression */
#line 344 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_SHIFTLEFT, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1381 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 57: /* expression ::= expression SHIFTRIGHT expression */
#line 347 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_SHIFTRIGHT, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1386 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 58: /* expression ::= expression MOD paren_expr_list */
      case 59: /* expression ::= expression MOD expression */ yytestcase(yyruleno==59);
#line 350 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateStringFormat(C->E, yymsp[-2].minor.yy94, yymsp[0].minor.yy94); }
#line 1392 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 60: /* expression ::= ARRAYOPENBRACKET CLOSEBRACKET */
#line 356 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateUnary(C->E, EST_ARRAY, NULL, yymsp[-1].minor.yy0.line); }
#line 1397 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 61: /* expression ::= ARRAYOPENBRACKET expr_list CLOSEBRACKET */
#line 359 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateUnary(C->E, EST_ARRAY, yymsp[-1].minor.yy94, yymsp[-1].minor.yy94->line); }
#line 1402 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 62: /* expression ::= MAPSTARTBLOCK ENDBLOCK */
#line 362 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateUnary(C->E, EST_MAP, NULL, yymsp[-1].minor.yy0.line); }
#line 1407 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 63: /* expression ::= MAPSTARTBLOCK expr_list ENDBLOCK */
#line 365 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateUnary(C->E, EST_MAP, yymsp[-1].minor.yy94, yymsp[-1].minor.yy94->line); }
#line 1412 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 64: /* expression ::= lvalue ASSIGN expression */
#line 368 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateAssignment(C->E, yymsp[-2].minor.yy94, yymsp[0].minor.yy94); }
#line 1417 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 65: /* expression ::= lvalue ASSIGN error */
#line 371 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ ekCompileExplainError(C, "assignment expected expression"); }
#line 1422 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 66: /* expression ::= expression INHERITS expression */
#line 374 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateInherits(C->E, yymsp[-2].minor.yy94, yymsp[0].minor.yy94); }
#line 1427 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 67: /* expression ::= lvalue */
      case 79: /* lvalue ::= lvalue_indexable */ yytestcase(yyruleno==79);
#line 377 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = yymsp[0].minor.yy94; }
#line 1433 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 68: /* expression ::= INTEGER */
#line 380 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateKInt(C->E, &yymsp[0].minor.yy0, 0); }
#line 1438 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 69: /* expression ::= NEGATIVE INTEGER */
#line 383 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateKInt(C->E, &yymsp[0].minor.yy0, CKO_NEGATIVE); }
#line 1443 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 70: /* expression ::= FLOATNUM */
#line 386 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateKFloat(C->E, &yymsp[0].minor.yy0, 0); }
#line 1448 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 71: /* expression ::= NEGATIVE FLOATNUM */
#line 389 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateKFloat(C->E, &yymsp[0].minor.yy0, CKO_NEGATIVE); }
#line 1453 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 72: /* expression ::= LITERALSTRING */
#line 392 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateKString(C->E, &yymsp[0].minor.yy0, 0); }
#line 1458 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 73: /* expression ::= REGEXSTRING */
#line 395 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateKString(C->E, &yymsp[0].minor.yy0, 1); }
#line 1463 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 74: /* expression ::= TRUE */
#line 398 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBool(C->E, &yymsp[0].minor.yy0, ekTrue); }
#line 1468 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 75: /* expression ::= FALSE */
#line 401 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateBool(C->E, &yymsp[0].minor.yy0, ekFalse); }
#line 1473 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 76: /* expression ::= NULL */
#line 404 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateNull(C->E, yymsp[0].minor.yy0.line); }
#line 1478 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 77: /* expression ::= FUNCTION LEFTPAREN func_args RIGHTPAREN statement_block */
#line 407 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateFunctionDecl(C->E, NULL, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, yymsp[-4].minor.yy0.line); }
#line 1483 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 78: /* expression ::= expression QUESTIONMARK expression COLON expression */
#line 410 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateIfElse(C->E, yymsp[-4].minor.yy94, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1488 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 80: /* lvalue ::= VAR IDENTIFIER */
#line 426 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxMarkVar(C->E, ekSyntaxCreateList(C->E, EST_IDENTIFIERLIST, ekSyntaxCreateIdentifier(C->E, &yymsp[0].minor.yy0))); }
#line 1493 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 81: /* lvalue ::= VAR GROUPLEFTPAREN ident_list RIGHTPAREN */
#line 429 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxMarkVar(C->E, yymsp[-1].minor.yy94); }
#line 1498 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 83: /* lvalue_indexable ::= THIS */
#line 444 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateThis(C->E, yymsp[0].minor.yy0.line); }
#line 1503 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 84: /* lvalue_indexable ::= lvalue_indexable paren_expr_list */
#line 447 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateCall(C->E, yymsp[-1].minor.yy94, yymsp[0].minor.yy94); }
#line 1508 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 85: /* lvalue_indexable ::= lvalue_indexable OPENBRACKET expression CLOSEBRACKET */
#line 450 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateIndex(C->E, yymsp[-3].minor.yy94, yymsp[-1].minor.yy94, ekFalse); }
#line 1513 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 86: /* lvalue_indexable ::= lvalue_indexable PERIOD IDENTIFIER */
#line 453 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateIndex(C->E, yymsp[-2].minor.yy94, ekSyntaxCreateKString(C->E, &yymsp[0].minor.yy0, 0), ekFalse); }
#line 1518 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 87: /* lvalue_indexable ::= lvalue_indexable COLONCOLON IDENTIFIER */
#line 456 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateIndex(C->E, yymsp[-2].minor.yy94, ekSyntaxCreateKString(C->E, &yymsp[0].minor.yy0, 0), ekTrue); }
#line 1523 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 88: /* lvalue_indexable ::= IDENTIFIER */
#line 459 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateIdentifier(C->E, &yymsp[0].minor.yy0); }
#line 1528 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 89: /* ident_list ::= ident_list COMMA IDENTIFIER */
#line 471 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxListAppend(C->E, yymsp[-2].minor.yy94, ekSyntaxCreateIdentifier(C->E, &yymsp[0].minor.yy0), 0); }
#line 1533 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 90: /* ident_list ::= IDENTIFIER */
#line 474 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateList(C->E, EST_IDENTIFIERLIST, ekSyntaxCreateIdentifier(C->E, &yymsp[0].minor.yy0)); }
#line 1538 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 91: /* ident_list ::= */
#line 477 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateList(C->E, EST_IDENTIFIERLIST, NULL); }
#line 1543 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 92: /* func_args ::= ident_list */
#line 486 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateFunctionArgs(C->E, yymsp[0].minor.yy94, NULL); }
#line 1548 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 93: /* func_args ::= ident_list COMMA ELLIPSIS IDENTIFIER */
#line 489 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateFunctionArgs(C->E, yymsp[-3].minor.yy94, &yymsp[0].minor.yy0); }
#line 1553 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      case 94: /* func_args ::= ELLIPSIS IDENTIFIER */
#line 492 "/Users/jdrago/work/eureka/lib/ekParser.y"
{ yygotominor.yy94 = ekSyntaxCreateFunctionArgs(C->E, ekSyntaxCreateList(C->E, EST_IDENTIFIERLIST, NULL), &yymsp[0].minor.yy0); }
#line 1558 "/Users/jdrago/work/eureka/lib/ekParser.c"
        break;
      default:
      /* (32) statement ::= error ENDSTATEMENT */ yytestcase(yyruleno==32);
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
#line 1623 "/Users/jdrago/work/eureka/lib/ekParser.c"
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
