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
#define YYNOCODE 90
#define YYACTIONTYPE unsigned short int
#define ekParseTOKENTYPE  ekToken
typedef union
{
    int yyinit;
    ekParseTOKENTYPE yy0;
    ekToken yy50;
    ekSyntax *yy168;
    int yy179;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define ekParseARG_SDECL  ekCompiler *C ;
#define ekParseARG_PDECL , ekCompiler *C
#define ekParseARG_FETCH  ekCompiler *C  = yypParser->C
#define ekParseARG_STORE yypParser->C  = C
#define YYNSTATE 175
#define YYNRULE 87
#define YYERRORSYMBOL 77
#define YYERRSYMDT yy179
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
static const YYACTIONTYPE yy_action[] =
{
    /*     0 */    19,  127,    8,    4,   16,  106,  124,   15,   45,  146,
    /*    10 */   117,  102,  116,  125,   17,   67,   91,   59,  112,  158,
    /*    20 */    57,   52,   54,   53,   56,   55,   47,   51,   49,   50,
    /*    30 */    98,  128,   26,   13,   67,  114,   59,  151,   24,   22,
    /*    40 */    19,   61,    8,    4,   16,  106,  124,   15,  156,  146,
    /*    50 */   148,  102,  119,  125,   17,   67,   91,   59,  112,  158,
    /*    60 */   162,   12,   18,  107,  159,  156,  120,  135,   20,   41,
    /*    70 */   137,  140,  141,  142,  143,  109,   20,   41,   24,   22,
    /*    80 */    19,  130,    8,  118,   16,  106,  124,   15,   10,  165,
    /*    90 */    23,  102,   46,  121,   17,   67,   91,   59,  112,  158,
    /*   100 */   162,   12,   18,  107,  159,  153,  120,  108,  131,   13,
    /*   110 */   137,  140,  141,  142,  143,  109,   20,   41,   24,   22,
    /*   120 */    27,   37,  138,  139,   30,   31,   29,   34,   35,   32,
    /*   130 */    33,   42,   40,   39,   21,   25,   26,   13,  110,  169,
    /*   140 */   162,   12,   18,  107,  159,   23,  120,   65,  114,   59,
    /*   150 */   137,  140,  141,  142,  143,  109,   19,   45,    8,    3,
    /*   160 */    16,  106,  124,   15,   23,  146,  170,  102,   57,    5,
    /*   170 */    17,   67,   91,   59,  112,  158,   37,  126,   58,   30,
    /*   180 */    31,   29,   34,   35,   32,   33,   42,   40,   39,   21,
    /*   190 */    25,   26,   13,  132,   24,   22,   20,   41,   30,   31,
    /*   200 */    29,   34,   35,   32,   33,   42,   40,   39,   21,   25,
    /*   210 */    26,   13,   97,  114,   59,  166,  162,   12,   18,  107,
    /*   220 */   159,   60,  120,   23,  155,  155,  137,  140,  141,  142,
    /*   230 */   143,  109,  175,   19,    6,    8,  160,   16,  106,  124,
    /*   240 */    15,  164,  146,  147,  102,  145,  133,   17,   67,   91,
    /*   250 */    59,  112,  158,   20,   41,   23,   20,   41,   31,   29,
    /*   260 */    34,   35,   32,   33,   42,   40,   39,   21,   25,   26,
    /*   270 */    13,   24,   22,   19,  161,    8,  111,   16,  106,  124,
    /*   280 */    15,   23,  146,  149,  102,  167,  168,   17,   67,   91,
    /*   290 */    59,  112,  158,  162,   12,   18,  107,  159,  174,  120,
    /*   300 */    25,   26,   13,  137,  140,  141,  142,  143,  109,  264,
    /*   310 */   173,   24,   22,  172,  114,   59,  264,   29,   34,   35,
    /*   320 */    32,   33,   42,   40,   39,   21,   25,   26,   13,  264,
    /*   330 */   264,   23,  144,  162,   12,   18,  107,  159,  264,  120,
    /*   340 */    25,   26,   13,  137,  140,  141,  142,  143,  109,   19,
    /*   350 */   264,    8,   23,   16,  106,  124,   15,   23,  146,  150,
    /*   360 */   102,  264,  264,   17,   67,   91,   59,  112,  158,   35,
    /*   370 */    32,   33,   42,   40,   39,   21,   25,   26,   13,   99,
    /*   380 */    83,  114,   59,   67,  114,   59,  264,   24,   22,   34,
    /*   390 */    35,   32,   33,   42,   40,   39,   21,   25,   26,   13,
    /*   400 */    39,   21,   25,   26,   13,  264,   23,  264,  163,  162,
    /*   410 */    12,   18,  107,  159,   23,  120,  264,  264,  264,  137,
    /*   420 */   140,  141,  142,  143,  109,   19,  264,    8,  264,   16,
    /*   430 */   106,  124,   15,   11,  165,  154,  102,   23,  121,   17,
    /*   440 */    67,   91,   59,  112,  158,   32,   33,   42,   40,   39,
    /*   450 */    21,   25,   26,   13,   33,   42,   40,   39,   21,   25,
    /*   460 */    26,   13,  264,   24,   22,   38,   36,   27,   37,  264,
    /*   470 */   264,   30,   31,   29,   34,   35,   32,   33,   42,   40,
    /*   480 */    39,   21,   25,   26,   13,  162,   12,   18,  107,  159,
    /*   490 */   264,  120,  264,  264,  264,  137,  140,  141,  142,  143,
    /*   500 */   109,  264,  124,   23,  264,  129,  264,  102,   23,   43,
    /*   510 */    44,   67,   91,   59,  264,  124,  263,    9,  165,  264,
    /*   520 */   102,  264,  264,   23,   67,   91,   59,  104,  114,   59,
    /*   530 */    28,   38,   36,   27,   37,  264,  264,   30,   31,   29,
    /*   540 */    34,   35,   32,   33,   42,   40,   39,   21,   25,   26,
    /*   550 */    13,   23,   21,   25,   26,   13,  100,   43,  264,  264,
    /*   560 */    67,  114,   59,  264,   42,   40,   39,   21,   25,   26,
    /*   570 */    13,  101,   23,  264,  264,   67,  114,   59,   28,   38,
    /*   580 */    36,   27,   37,  264,  264,   30,   31,   29,   34,   35,
    /*   590 */    32,   33,   42,   40,   39,   21,   25,   26,   13,   23,
    /*   600 */    84,  114,   59,    1,  264,   43,   14,   67,  114,   59,
    /*   610 */     7,  264,  264,   48,   40,   39,   21,   25,   26,   13,
    /*   620 */   105,  114,   59,  264,  264,  264,   28,   38,   36,   27,
    /*   630 */    37,  264,  264,   30,   31,   29,   34,   35,   32,   33,
    /*   640 */    42,   40,   39,   21,   25,   26,   13,   23,  264,  264,
    /*   650 */     2,  152,  264,   43,   67,  114,   59,  264,  103,  122,
    /*   660 */   123,  264,   67,  114,   59,  113,  114,   59,   82,  114,
    /*   670 */    59,  264,  264,  264,   28,   38,   36,   27,   37,  264,
    /*   680 */   264,   30,   31,   29,   34,   35,   32,   33,   42,   40,
    /*   690 */    39,   21,   25,   26,   13,   23,   79,  114,   59,  264,
    /*   700 */    88,  114,   59,   86,  114,   59,   87,  114,   59,   92,
    /*   710 */   114,   59,  264,   93,  114,   59,   89,  114,   59,  264,
    /*   720 */   264,  264,   28,   38,   36,   27,   37,  264,  264,   30,
    /*   730 */    31,   29,   34,   35,   32,   33,   42,   40,   39,   21,
    /*   740 */    25,   26,   13,   23,  264,   90,  114,   59,  264,   81,
    /*   750 */   114,   59,   85,  114,   59,   80,  114,   59,  264,  264,
    /*   760 */   264,   96,  114,   59,   95,  114,   59,  264,   66,  114,
    /*   770 */    59,  264,   36,   27,   37,  264,  264,   30,   31,   29,
    /*   780 */    34,   35,   32,   33,   42,   40,   39,   21,   25,   26,
    /*   790 */    13,   19,   14,  264,  264,   16,  106,  264,   15,  264,
    /*   800 */    94,  114,   59,   62,  114,   59,  264,  264,  264,  264,
    /*   810 */   158,  264,   19,   69,  114,   59,   16,  106,  264,   15,
    /*   820 */    68,  114,   59,   63,  114,   59,   70,  114,   59,   24,
    /*   830 */    22,  158,  264,   19,   64,  114,   59,   16,  106,  264,
    /*   840 */    15,   71,  114,   59,   72,  114,   59,  264,  264,  264,
    /*   850 */    24,   22,  158,  264,  115,  159,   73,  114,   59,  264,
    /*   860 */   264,  137,  140,  141,  142,  143,  109,   74,  114,   59,
    /*   870 */   264,   24,   22,  264,   19,  115,  159,  171,   16,  106,
    /*   880 */   264,   15,  137,  140,  141,  142,  143,  109,   75,  114,
    /*   890 */    59,  134,  264,  158,  264,  264,  115,  159,  264,   76,
    /*   900 */   114,   59,  264,  137,  140,  141,  142,  143,  109,   77,
    /*   910 */   114,   59,   24,   22,  264,   19,   78,  114,   59,   16,
    /*   920 */   106,  157,   15,  264,   20,   41,  264,  264,  264,  264,
    /*   930 */   264,  264,  264,  136,  158,  264,  264,  115,  159,  264,
    /*   940 */   264,  264,  264,  264,  137,  140,  141,  142,  143,  109,
    /*   950 */   264,  264,  264,   24,   22,  264,  264,  264,  264,  264,
    /*   960 */   264,  264,  264,  264,  264,  264,  264,  264,  264,  264,
    /*   970 */   264,  264,  264,  264,  264,  264,  264,  264,  115,  159,
    /*   980 */   264,  264,  264,  264,  264,  137,  140,  141,  142,  143,
    /*   990 */   109,
};
static const YYCODETYPE yy_lookahead[] =
{
    /*     0 */     1,   18,    3,    4,    5,    6,   77,    8,   12,   80,
    /*    10 */    81,   82,   84,   85,   15,   86,   87,   88,   19,   20,
    /*    20 */     2,   25,   26,   27,   28,   29,   30,   31,   32,   33,
    /*    30 */    82,   83,   53,   54,   86,   87,   88,   83,   39,   40,
    /*    40 */     1,    1,    3,    4,    5,    6,   77,    8,   65,   80,
    /*    50 */    81,   82,   84,   85,   15,   86,   87,   88,   19,   20,
    /*    60 */    61,   62,   63,   64,   65,   65,   67,   60,   69,   70,
    /*    70 */    71,   72,   73,   74,   75,   76,   69,   70,   39,   40,
    /*    80 */     1,   61,    3,   65,    5,    6,   77,    8,   79,   80,
    /*    90 */     7,   82,   68,   69,   15,   86,   87,   88,   19,   20,
    /*   100 */    61,   62,   63,   64,   65,   65,   67,   85,   61,   54,
    /*   110 */    71,   72,   73,   74,   75,   76,   69,   70,   39,   40,
    /*   120 */    37,   38,   71,   72,   41,   42,   43,   44,   45,   46,
    /*   130 */    47,   48,   49,   50,   51,   52,   53,   54,   85,   60,
    /*   140 */    61,   62,   63,   64,   65,    7,   67,   86,   87,   88,
    /*   150 */    71,   72,   73,   74,   75,   76,    1,   12,    3,   66,
    /*   160 */     5,    6,   77,    8,    7,   80,   81,   82,    2,   16,
    /*   170 */    15,   86,   87,   88,   19,   20,   38,   18,    2,   41,
    /*   180 */    42,   43,   44,   45,   46,   47,   48,   49,   50,   51,
    /*   190 */    52,   53,   54,   66,   39,   40,   69,   70,   41,   42,
    /*   200 */    43,   44,   45,   46,   47,   48,   49,   50,   51,   52,
    /*   210 */    53,   54,   86,   87,   88,   60,   61,   62,   63,   64,
    /*   220 */    65,    2,   67,    7,   65,   65,   71,   72,   73,   74,
    /*   230 */    75,   76,    0,    1,   66,    3,   65,    5,    6,   77,
    /*   240 */     8,   61,   80,   81,   82,   61,   59,   15,   86,   87,
    /*   250 */    88,   19,   20,   69,   70,    7,   69,   70,   42,   43,
    /*   260 */    44,   45,   46,   47,   48,   49,   50,   51,   52,   53,
    /*   270 */    54,   39,   40,    1,   65,    3,   69,    5,    6,   77,
    /*   280 */     8,    7,   80,   81,   82,   65,   65,   15,   86,   87,
    /*   290 */    88,   19,   20,   61,   62,   63,   64,   65,   61,   67,
    /*   300 */    52,   53,   54,   71,   72,   73,   74,   75,   76,   89,
    /*   310 */    83,   39,   40,   86,   87,   88,   89,   43,   44,   45,
    /*   320 */    46,   47,   48,   49,   50,   51,   52,   53,   54,   89,
    /*   330 */    89,    7,   60,   61,   62,   63,   64,   65,   89,   67,
    /*   340 */    52,   53,   54,   71,   72,   73,   74,   75,   76,    1,
    /*   350 */    89,    3,    7,    5,    6,   77,    8,    7,   80,   81,
    /*   360 */    82,   89,   89,   15,   86,   87,   88,   19,   20,   45,
    /*   370 */    46,   47,   48,   49,   50,   51,   52,   53,   54,   82,
    /*   380 */    86,   87,   88,   86,   87,   88,   89,   39,   40,   44,
    /*   390 */    45,   46,   47,   48,   49,   50,   51,   52,   53,   54,
    /*   400 */    50,   51,   52,   53,   54,   89,    7,   89,   60,   61,
    /*   410 */    62,   63,   64,   65,    7,   67,   89,   89,   89,   71,
    /*   420 */    72,   73,   74,   75,   76,    1,   89,    3,   89,    5,
    /*   430 */     6,   77,    8,   79,   80,   66,   82,    7,   69,   15,
    /*   440 */    86,   87,   88,   19,   20,   46,   47,   48,   49,   50,
    /*   450 */    51,   52,   53,   54,   47,   48,   49,   50,   51,   52,
    /*   460 */    53,   54,   89,   39,   40,   35,   36,   37,   38,   89,
    /*   470 */    89,   41,   42,   43,   44,   45,   46,   47,   48,   49,
    /*   480 */    50,   51,   52,   53,   54,   61,   62,   63,   64,   65,
    /*   490 */    89,   67,   89,   89,   89,   71,   72,   73,   74,   75,
    /*   500 */    76,   89,   77,    7,   89,   80,   89,   82,    7,   13,
    /*   510 */    14,   86,   87,   88,   89,   77,   78,   79,   80,   89,
    /*   520 */    82,   89,   89,    7,   86,   87,   88,   86,   87,   88,
    /*   530 */    34,   35,   36,   37,   38,   89,   89,   41,   42,   43,
    /*   540 */    44,   45,   46,   47,   48,   49,   50,   51,   52,   53,
    /*   550 */    54,    7,   51,   52,   53,   54,   82,   13,   89,   89,
    /*   560 */    86,   87,   88,   89,   48,   49,   50,   51,   52,   53,
    /*   570 */    54,   82,    7,   89,   89,   86,   87,   88,   34,   35,
    /*   580 */    36,   37,   38,   89,   89,   41,   42,   43,   44,   45,
    /*   590 */    46,   47,   48,   49,   50,   51,   52,   53,   54,    7,
    /*   600 */    86,   87,   88,   82,   89,   13,    2,   86,   87,   88,
    /*   610 */    66,   89,   89,    9,   49,   50,   51,   52,   53,   54,
    /*   620 */    86,   87,   88,   89,   89,   89,   34,   35,   36,   37,
    /*   630 */    38,   89,   89,   41,   42,   43,   44,   45,   46,   47,
    /*   640 */    48,   49,   50,   51,   52,   53,   54,    7,   89,   89,
    /*   650 */    82,   59,   89,   13,   86,   87,   88,   89,   82,   55,
    /*   660 */    56,   89,   86,   87,   88,   86,   87,   88,   86,   87,
    /*   670 */    88,   89,   89,   89,   34,   35,   36,   37,   38,   89,
    /*   680 */    89,   41,   42,   43,   44,   45,   46,   47,   48,   49,
    /*   690 */    50,   51,   52,   53,   54,    7,   86,   87,   88,   89,
    /*   700 */    86,   87,   88,   86,   87,   88,   86,   87,   88,   86,
    /*   710 */    87,   88,   89,   86,   87,   88,   86,   87,   88,   89,
    /*   720 */    89,   89,   34,   35,   36,   37,   38,   89,   89,   41,
    /*   730 */    42,   43,   44,   45,   46,   47,   48,   49,   50,   51,
    /*   740 */    52,   53,   54,    7,   89,   86,   87,   88,   89,   86,
    /*   750 */    87,   88,   86,   87,   88,   86,   87,   88,   89,   89,
    /*   760 */    89,   86,   87,   88,   86,   87,   88,   89,   86,   87,
    /*   770 */    88,   89,   36,   37,   38,   89,   89,   41,   42,   43,
    /*   780 */    44,   45,   46,   47,   48,   49,   50,   51,   52,   53,
    /*   790 */    54,    1,    2,   89,   89,    5,    6,   89,    8,   89,
    /*   800 */    86,   87,   88,   86,   87,   88,   89,   89,   89,   89,
    /*   810 */    20,   89,    1,   86,   87,   88,    5,    6,   89,    8,
    /*   820 */    86,   87,   88,   86,   87,   88,   86,   87,   88,   39,
    /*   830 */    40,   20,   89,    1,   86,   87,   88,    5,    6,   89,
    /*   840 */     8,   86,   87,   88,   86,   87,   88,   89,   89,   89,
    /*   850 */    39,   40,   20,   89,   64,   65,   86,   87,   88,   89,
    /*   860 */    89,   71,   72,   73,   74,   75,   76,   86,   87,   88,
    /*   870 */    89,   39,   40,   89,    1,   64,   65,   66,    5,    6,
    /*   880 */    89,    8,   71,   72,   73,   74,   75,   76,   86,   87,
    /*   890 */    88,   59,   89,   20,   89,   89,   64,   65,   89,   86,
    /*   900 */    87,   88,   89,   71,   72,   73,   74,   75,   76,   86,
    /*   910 */    87,   88,   39,   40,   89,    1,   86,   87,   88,    5,
    /*   920 */     6,   66,    8,   89,   69,   70,   89,   89,   89,   89,
    /*   930 */    89,   89,   89,   60,   20,   89,   89,   64,   65,   89,
    /*   940 */    89,   89,   89,   89,   71,   72,   73,   74,   75,   76,
    /*   950 */    89,   89,   89,   39,   40,   89,   89,   89,   89,   89,
    /*   960 */    89,   89,   89,   89,   89,   89,   89,   89,   89,   89,
    /*   970 */    89,   89,   89,   89,   89,   89,   89,   89,   64,   65,
    /*   980 */    89,   89,   89,   89,   89,   71,   72,   73,   74,   75,
    /*   990 */    76,
};
#define YY_SHIFT_USE_DFLT (-22)
#define YY_SHIFT_MAX 128
static const short yy_shift_ofst[] =
{
    /*     0 */   424,   -1,   -1,   39,   79,   39,   39,   39,  155,  232,
    /*    10 */   272,  348,  790,  790,  811,  832,  873,  914,  914,  914,
    /*    20 */   914,  914,  914,  914,  914,  914,  914,  914,  914,  914,
    /*    30 */   914,  914,  914,  914,  914,  914,  914,  914,  914,  914,
    /*    40 */   914,  914,  914,  914,  914,  914,  914,  914,  914,  914,
    /*    50 */   914,  914,  914,  914,  914,  914,  914,  -17,  -17,  604,
    /*    60 */     0,    0,  496,  544,  592,  640,  640,  640,  640,  688,
    /*    70 */   688,  688,  688,  688,  688,  688,  688,  688,  688,  430,
    /*    80 */   736,   83,  138,  157,  157,  157,  216,  274,  345,  324,
    /*    90 */   399,   -4,  407,  516,  565,  350,  501,  248,   47,  127,
    /*   100 */   187,    7,  184,  855,  288,  -21,   51,   18,   24,   40,
    /*   110 */   369,  159,   20,   55,  145,  166,   93,  153,  176,  168,
    /*   120 */   219,  160,  171,  209,  180,  207,  220,  221,  237,
};
#define YY_REDUCE_USE_DFLT (-73)
#define YY_REDUCE_MAX 61
static const short yy_reduce_ofst[] =
{
    /*     0 */   438,  -71,  -31,   85,    9,  162,  202,  278,  354,  425,
    /*    10 */   425,  425,  -52,  227,  297,  474,  489,  521,  568,  576,
    /*    20 */    61,  126,  294,  441,  514,  534,  579,  582,  610,  614,
    /*    30 */   617,  620,  623,  627,  630,  659,  663,  666,  669,  675,
    /*    40 */   678,  682,  714,  717,  727,  734,  737,  740,  748,  755,
    /*    50 */   758,  770,  781,  802,  813,  823,  830,  -72,  -32,  -46,
    /*    60 */    22,   53,
};
static const YYACTIONTYPE yy_default[] =
{
    /*     0 */   262,  262,  262,  262,  262,  262,  262,  262,  262,  262,
    /*    10 */   262,  262,  262,  262,  262,  262,  262,  262,  262,  262,
    /*    20 */   262,  262,  262,  262,  262,  262,  262,  262,  262,  262,
    /*    30 */   262,  262,  262,  262,  262,  262,  262,  262,  262,  262,
    /*    40 */   262,  262,  262,  262,  262,  262,  262,  262,  262,  262,
    /*    50 */   262,  262,  262,  262,  262,  262,  262,  258,  258,  246,
    /*    60 */   258,  258,  262,  262,  262,  205,  206,  207,  234,  245,
    /*    70 */   190,  191,  192,  193,  194,  195,  196,  197,  198,  215,
    /*    80 */   225,  223,  214,  208,  209,  224,  217,  218,  216,  221,
    /*    90 */   222,  236,  219,  220,  235,  227,  226,  210,  262,  262,
    /*   100 */   262,  262,  262,  262,  211,  212,  262,  262,  262,  262,
    /*   110 */   262,  262,  262,  213,  236,  262,  262,  186,  262,  262,
    /*   120 */   262,  262,  262,  262,  262,  259,  262,  262,  262,  176,
    /*   130 */   181,  182,  203,  231,  230,  233,  232,  237,  238,  240,
    /*   140 */   239,  241,  242,  243,  179,  184,  180,  185,  187,  188,
    /*   150 */   189,  251,  252,  247,  248,  256,  257,  249,  250,  255,
    /*   160 */   253,  254,  199,  201,  202,  177,  200,  260,  261,  178,
    /*   170 */   244,  204,  229,  228,  183,
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
static const YYCODETYPE yyFallback[] =
{
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
struct yyStackEntry
{
    YYACTIONTYPE stateno;  /* The state-number */
    YYCODETYPE major;      /* The major token value.  This is the code
                         ** number for the token at this stack level */
    YYMINORTYPE minor;     /* The user-supplied minor token value.  This
                         ** is the value of the token  */
};
typedef struct yyStackEntry yyStackEntry;

/* The state of the parser is completely contained in an instance of
** the following structure */
struct yyParser
{
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
void ekParseTrace(FILE *TraceFILE, char *zTracePrompt)
{
    yyTraceFILE = TraceFILE;
    yyTracePrompt = zTracePrompt;
    if(yyTraceFILE==0) { yyTracePrompt = 0; }
    else if(yyTracePrompt==0) { yyTraceFILE = 0; }
}
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing shifts, the names of all terminals and nonterminals
** are required.  The following table supplies these names */
static const char *const yyTokenName[] =
{
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
    "FLOATNUM",      "LITERALSTRING",  "REGEXSTRING",   "NULL",
    "VAR",           "error",         "chunk",         "statement_list",
    "statement",     "statement_block",  "expr_list",     "paren_expr_list",
    "func_args",     "ident_list",    "expression",    "lvalue",
    "lvalue_indexable",
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *const yyRuleName[] =
{
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
    /*  68 */ "expression ::= NULL",
    /*  69 */ "expression ::= FUNCTION LEFTPAREN func_args RIGHTPAREN statement_block",
    /*  70 */ "expression ::= expression QUESTIONMARK expression COLON expression",
    /*  71 */ "lvalue ::= lvalue_indexable",
    /*  72 */ "lvalue ::= VAR IDENTIFIER",
    /*  73 */ "lvalue ::= VAR GROUPLEFTPAREN ident_list RIGHTPAREN",
    /*  74 */ "lvalue ::= GROUPLEFTPAREN expr_list RIGHTPAREN",
    /*  75 */ "lvalue_indexable ::= THIS",
    /*  76 */ "lvalue_indexable ::= lvalue_indexable paren_expr_list",
    /*  77 */ "lvalue_indexable ::= lvalue_indexable OPENBRACKET expression CLOSEBRACKET",
    /*  78 */ "lvalue_indexable ::= lvalue_indexable PERIOD IDENTIFIER",
    /*  79 */ "lvalue_indexable ::= lvalue_indexable COLONCOLON IDENTIFIER",
    /*  80 */ "lvalue_indexable ::= IDENTIFIER",
    /*  81 */ "ident_list ::= ident_list COMMA IDENTIFIER",
    /*  82 */ "ident_list ::= IDENTIFIER",
    /*  83 */ "ident_list ::=",
    /*  84 */ "func_args ::= ident_list",
    /*  85 */ "func_args ::= ident_list COMMA ELLIPSIS IDENTIFIER",
    /*  86 */ "func_args ::= ELLIPSIS IDENTIFIER",
};
#endif /* NDEBUG */


#if YYSTACKDEPTH<=0
/*
** Try to increase the size of the parser stack.
*/
static void yyGrowStack(yyParser *p)
{
    int newSize;
    yyStackEntry *pNew;

    newSize = p->yystksz*2 + 100;
    pNew = ekRealloc(p->yystack, newSize*sizeof(pNew[0]));
    if(pNew)
    {
        p->yystack = pNew;
        p->yystksz = newSize;
#ifndef NDEBUG
        if(yyTraceFILE)
        {
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
void *ekParseAlloc(struct ekContext *E)
{
    yyParser *pParser;
    pParser = (yyParser *)ekAlloc(sizeof(yyParser));
    if(pParser)
    {
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
)
{
    ekParseARG_FETCH;
    switch(yymajor)
    {
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
        case 79: /* statement_list */
        case 80: /* statement */
        case 81: /* statement_block */
        case 82: /* expr_list */
        case 83: /* paren_expr_list */
        case 85: /* ident_list */
        case 86: /* expression */
        case 87: /* lvalue */
        case 88: /* lvalue_indexable */
        {
#line 132 "/home/joe/private/work/eureka/lib/ekParser.y"
            ekSyntaxDestroy(C->E, (yypminor->yy168));
#line 728 "/home/joe/private/work/eureka/lib/ekParser.c"
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
static int yy_pop_parser_stack(yyParser *pParser)
{
    YYCODETYPE yymajor;
    yyStackEntry *yytos = &pParser->yystack[pParser->yyidx];

    if(pParser->yyidx<0) { return 0; }
#ifndef NDEBUG
    if(yyTraceFILE && pParser->yyidx>=0)
    {
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
                )
{
    yyParser *pParser = (yyParser *)p;
    if(pParser==0) { return; }
    while(pParser->yyidx>=0) { yy_pop_parser_stack(pParser); }
#if YYSTACKDEPTH<=0
    ekFree(pParser->yystack);
#endif
    ekFree((void *)pParser);
}

/*
** Return the peak depth of the stack for a parser.
*/
#ifdef YYTRACKMAXSTACKDEPTH
int ekParseStackPeak(void *p)
{
    yyParser *pParser = (yyParser *)p;
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
)
{
    int i;
    int stateno = pParser->yystack[pParser->yyidx].stateno;

    if(stateno>YY_SHIFT_MAX || (i = yy_shift_ofst[stateno])==YY_SHIFT_USE_DFLT)
    {
        return yy_default[stateno];
    }
    assert(iLookAhead!=YYNOCODE);
    i += iLookAhead;
    if(i<0 || i>=YY_SZ_ACTTAB || yy_lookahead[i]!=iLookAhead)
    {
        if(iLookAhead>0)
        {
#ifdef YYFALLBACK
            YYCODETYPE iFallback;            /* Fallback token */
            if(iLookAhead<sizeof(yyFallback)/sizeof(yyFallback[0])
               && (iFallback = yyFallback[iLookAhead])!=0)
            {
#ifndef NDEBUG
                if(yyTraceFILE)
                {
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
                if(j>=0 && j<YY_SZ_ACTTAB && yy_lookahead[j]==YYWILDCARD)
                {
#ifndef NDEBUG
                    if(yyTraceFILE)
                    {
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
    }
    else
    {
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
)
{
    int i;
#ifdef YYERRORSYMBOL
    if(stateno>YY_REDUCE_MAX)
    {
        return yy_default[stateno];
    }
#else
    assert(stateno<=YY_REDUCE_MAX);
#endif
    i = yy_reduce_ofst[stateno];
    assert(i!=YY_REDUCE_USE_DFLT);
    assert(iLookAhead!=YYNOCODE);
    i += iLookAhead;
#ifdef YYERRORSYMBOL
    if(i<0 || i>=YY_SZ_ACTTAB || yy_lookahead[i]!=iLookAhead)
    {
        return yy_default[stateno];
    }
#else
    assert(i>=0 && i<YY_SZ_ACTTAB);
    assert(yy_lookahead[i]==iLookAhead);
#endif
    return yy_action[i];
}

/*
** The following routine is called if the stack overflows.
*/
static void yyStackOverflow(yyParser *yypParser, YYMINORTYPE *yypMinor)
{
    ekParseARG_FETCH;
    yypParser->yyidx--;
#ifndef NDEBUG
    if(yyTraceFILE)
    {
        fprintf(yyTraceFILE,"%sStack Overflow!\n",yyTracePrompt);
    }
#endif
    while(yypParser->yyidx>=0) { yy_pop_parser_stack(yypParser); }
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
)
{
    yyStackEntry *yytos;
    yypParser->yyidx++;
#ifdef YYTRACKMAXSTACKDEPTH
    if(yypParser->yyidx>yypParser->yyidxMax)
    {
        yypParser->yyidxMax = yypParser->yyidx;
    }
#endif
#if YYSTACKDEPTH>0
    if(yypParser->yyidx>=YYSTACKDEPTH)
    {
        yyStackOverflow(yypParser, yypMinor);
        return;
    }
#else
    if(yypParser->yyidx>=yypParser->yystksz)
    {
        yyGrowStack(yypParser);
        if(yypParser->yyidx>=yypParser->yystksz)
        {
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
    if(yyTraceFILE && yypParser->yyidx>0)
    {
        int i;
        fprintf(yyTraceFILE,"%sShift %d\n",yyTracePrompt,yyNewState);
        fprintf(yyTraceFILE,"%sStack:",yyTracePrompt);
        for(i=1; i<=yypParser->yyidx; i++)
        {
            fprintf(yyTraceFILE," %s",yyTokenName[yypParser->yystack[i].major]);
        }
        fprintf(yyTraceFILE,"\n");
    }
#endif
}

/* The following table contains information about every rule that
** is used during the reduce.
*/
static const struct
{
    YYCODETYPE lhs;         /* Symbol on the left-hand side of the rule */
    unsigned char nrhs;     /* Number of right-hand side symbols in the rule */
} yyRuleInfo[] =
{
    { 78, 1 },
    { 79, 2 },
    { 79, 1 },
    { 81, 2 },
    { 81, 3 },
    { 81, 1 },
    { 80, 2 },
    { 80, 3 },
    { 80, 3 },
    { 80, 2 },
    { 80, 5 },
    { 80, 3 },
    { 80, 3 },
    { 80, 6 },
    { 80, 7 },
    { 80, 3 },
    { 80, 3 },
    { 80, 3 },
    { 80, 3 },
    { 80, 3 },
    { 80, 3 },
    { 80, 3 },
    { 80, 3 },
    { 80, 3 },
    { 80, 1 },
    { 80, 2 },
    { 80, 3 },
    { 80, 2 },
    { 83, 3 },
    { 83, 2 },
    { 82, 3 },
    { 82, 3 },
    { 82, 1 },
    { 86, 2 },
    { 86, 2 },
    { 86, 3 },
    { 86, 3 },
    { 86, 3 },
    { 86, 3 },
    { 86, 3 },
    { 86, 3 },
    { 86, 3 },
    { 86, 3 },
    { 86, 3 },
    { 86, 3 },
    { 86, 3 },
    { 86, 3 },
    { 86, 3 },
    { 86, 3 },
    { 86, 3 },
    { 86, 3 },
    { 86, 3 },
    { 86, 3 },
    { 86, 3 },
    { 86, 3 },
    { 86, 2 },
    { 86, 3 },
    { 86, 2 },
    { 86, 3 },
    { 86, 3 },
    { 86, 3 },
    { 86, 1 },
    { 86, 1 },
    { 86, 2 },
    { 86, 1 },
    { 86, 2 },
    { 86, 1 },
    { 86, 1 },
    { 86, 1 },
    { 86, 5 },
    { 86, 5 },
    { 87, 1 },
    { 87, 2 },
    { 87, 4 },
    { 87, 3 },
    { 88, 1 },
    { 88, 2 },
    { 88, 4 },
    { 88, 3 },
    { 88, 3 },
    { 88, 1 },
    { 85, 3 },
    { 85, 1 },
    { 85, 0 },
    { 84, 1 },
    { 84, 4 },
    { 84, 2 },
};

static void yy_accept(yyParser *); /* Forward Declaration */

/*
** Perform a reduce action and the shift that must immediately
** follow the reduce.
*/
static void yy_reduce(
    yyParser *yypParser,         /* The parser */
    int yyruleno                 /* Number of the rule by which to reduce */
)
{
    int yygoto;                     /* The next state */
    int yyact;                      /* The next action */
    YYMINORTYPE yygotominor;        /* The LHS of the rule reduced */
    yyStackEntry *yymsp;            /* The top of the parser's stack */
    int yysize;                     /* Amount to pop the stack */
    ekParseARG_FETCH;
    yymsp = &yypParser->yystack[yypParser->yyidx];
#ifndef NDEBUG
    if(yyTraceFILE && yyruleno>=0
       && yyruleno<(int)(sizeof(yyRuleName)/sizeof(yyRuleName[0])))
    {
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


    switch(yyruleno)
    {
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
            { C->root = yymsp[0].minor.yy168; }
#line 1100 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 1: /* statement_list ::= statement_list statement */
#line 135 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxListAppend(C->E, yymsp[-1].minor.yy168, yymsp[0].minor.yy168, 0); }
#line 1105 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 2: /* statement_list ::= statement */
        case 5: /* statement_block ::= statement */ yytestcase(yyruleno==5);
#line 138 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateList(C->E, EST_STATEMENTLIST, yymsp[0].minor.yy168); }
#line 1111 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 3: /* statement_block ::= STARTBLOCK ENDBLOCK */
        case 24: /* statement ::= ENDSTATEMENT */ yytestcase(yyruleno==24);
        case 25: /* statement ::= SCOPESTARTBLOCK ENDBLOCK */ yytestcase(yyruleno==25);
#line 150 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateList(C->E, EST_STATEMENTLIST, NULL); }
#line 1118 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 4: /* statement_block ::= STARTBLOCK statement_list ENDBLOCK */
        case 28: /* paren_expr_list ::= LEFTPAREN expr_list RIGHTPAREN */ yytestcase(yyruleno==28);
        case 74: /* lvalue ::= GROUPLEFTPAREN expr_list RIGHTPAREN */ yytestcase(yyruleno==74);
#line 153 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = yymsp[-1].minor.yy168; }
#line 1125 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 6: /* statement ::= BREAK ENDSTATEMENT */
#line 168 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateBreak(C->E, yymsp[-1].minor.yy0.line); }
#line 1130 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 7: /* statement ::= RETURN expr_list ENDSTATEMENT */
        case 8: /* statement ::= RETURN paren_expr_list ENDSTATEMENT */ yytestcase(yyruleno==8);
#line 171 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateReturn(C->E, yymsp[-1].minor.yy168); }
#line 1136 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 9: /* statement ::= expr_list ENDSTATEMENT */
#line 177 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateStatementExpr(C->E, yymsp[-1].minor.yy168); }
#line 1141 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 10: /* statement ::= IF expr_list statement_block ELSE statement_block */
#line 180 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateIfElse(C->E, yymsp[-3].minor.yy168, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, ekFalse); }
#line 1146 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 11: /* statement ::= IF expr_list statement_block */
#line 183 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateIfElse(C->E, yymsp[-1].minor.yy168, yymsp[0].minor.yy168, NULL, ekFalse); }
#line 1151 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 12: /* statement ::= WHILE expr_list statement_block */
#line 186 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateWhile(C->E, yymsp[-1].minor.yy168, yymsp[0].minor.yy168); }
#line 1156 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 13: /* statement ::= FUNCTION IDENTIFIER LEFTPAREN func_args RIGHTPAREN statement_block */
#line 189 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateFunctionDecl(C->E, &yymsp[-4].minor.yy0, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, yymsp[-5].minor.yy0.line); }
#line 1161 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 14: /* statement ::= FOR LEFTPAREN ident_list IN expression RIGHTPAREN statement_block */
#line 192 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateFor(C->E, yymsp[-4].minor.yy168, yymsp[-2].minor.yy168, yymsp[0].minor.yy168); }
#line 1166 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 15: /* statement ::= lvalue PLUSEQUALS expression */
#line 195 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateBinary(C->E, EST_ADD, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, ekTrue); }
#line 1171 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 16: /* statement ::= lvalue DASHEQUALS expression */
#line 198 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateBinary(C->E, EST_SUB, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, ekTrue); }
#line 1176 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 17: /* statement ::= lvalue STAREQUALS expression */
#line 201 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateBinary(C->E, EST_MUL, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, ekTrue); }
#line 1181 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 18: /* statement ::= lvalue SLASHEQUALS expression */
#line 204 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateBinary(C->E, EST_DIV, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, ekTrue); }
#line 1186 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 19: /* statement ::= lvalue BITWISE_OREQUALS expression */
#line 207 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateBinary(C->E, EST_BITWISE_OR, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, ekTrue); }
#line 1191 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 20: /* statement ::= lvalue BITWISE_ANDEQUALS expression */
#line 210 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateBinary(C->E, EST_BITWISE_AND, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, ekTrue); }
#line 1196 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 21: /* statement ::= lvalue BITWISE_XOREQUALS expression */
#line 213 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateBinary(C->E, EST_BITWISE_XOR, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, ekTrue); }
#line 1201 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 22: /* statement ::= lvalue SHIFTLEFTEQUALS expression */
#line 216 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateBinary(C->E, EST_SHIFTLEFT, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, ekTrue); }
#line 1206 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 23: /* statement ::= lvalue SHIFTRIGHTEQUALS expression */
#line 219 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateBinary(C->E, EST_SHIFTRIGHT, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, ekTrue); }
#line 1211 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 26: /* statement ::= SCOPESTARTBLOCK statement_list ENDBLOCK */
#line 228 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateScope(C->E, yymsp[-1].minor.yy168); }
#line 1216 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 29: /* paren_expr_list ::= LEFTPAREN RIGHTPAREN */
#line 245 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateList(C->E, EST_EXPRESSIONLIST, NULL); }
#line 1221 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 30: /* expr_list ::= expr_list COMMA expression */
#line 257 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxListAppend(C->E, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, 0); }
#line 1226 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 31: /* expr_list ::= expr_list FATCOMMA expression */
#line 260 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxListAppend(C->E, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, ESLF_AUTOLITERAL); }
#line 1231 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 32: /* expr_list ::= expression */
#line 263 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateList(C->E, EST_EXPRESSIONLIST, yymsp[0].minor.yy168); }
#line 1236 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 33: /* expression ::= NOT expression */
#line 275 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateUnary(C->E, EST_NOT, yymsp[0].minor.yy168, yymsp[0].minor.yy168->line); }
#line 1241 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 34: /* expression ::= BITWISE_NOT expression */
#line 278 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateUnary(C->E, EST_BITWISE_NOT, yymsp[0].minor.yy168, yymsp[0].minor.yy168->line); }
#line 1246 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 35: /* expression ::= expression PLUS expression */
#line 281 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateBinary(C->E, EST_ADD, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, ekFalse); }
#line 1251 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 36: /* expression ::= expression DASH expression */
#line 284 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateBinary(C->E, EST_SUB, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, ekFalse); }
#line 1256 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 37: /* expression ::= expression STAR expression */
#line 287 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateBinary(C->E, EST_MUL, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, ekFalse); }
#line 1261 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 38: /* expression ::= expression SLASH expression */
#line 290 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateBinary(C->E, EST_DIV, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, ekFalse); }
#line 1266 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 39: /* expression ::= expression AND expression */
#line 293 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateBinary(C->E, EST_AND, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, ekFalse); }
#line 1271 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 40: /* expression ::= expression OR expression */
#line 296 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateBinary(C->E, EST_OR, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, ekFalse); }
#line 1276 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 41: /* expression ::= expression CMP expression */
#line 299 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateBinary(C->E, EST_CMP, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, ekFalse); }
#line 1281 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 42: /* expression ::= expression EQUALS expression */
#line 302 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateBinary(C->E, EST_EQUALS, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, ekFalse); }
#line 1286 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 43: /* expression ::= expression NOTEQUALS expression */
#line 305 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateBinary(C->E, EST_NOTEQUALS, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, ekFalse); }
#line 1291 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 44: /* expression ::= expression GREATERTHAN expression */
#line 308 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateBinary(C->E, EST_GREATERTHAN, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, ekFalse); }
#line 1296 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 45: /* expression ::= expression GREATERTHANOREQUAL expression */
#line 311 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateBinary(C->E, EST_GREATERTHANOREQUAL, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, ekFalse); }
#line 1301 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 46: /* expression ::= expression LESSTHAN expression */
#line 314 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateBinary(C->E, EST_LESSTHAN, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, ekFalse); }
#line 1306 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 47: /* expression ::= expression LESSTHANOREQUAL expression */
#line 317 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateBinary(C->E, EST_LESSTHANOREQUAL, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, ekFalse); }
#line 1311 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 48: /* expression ::= expression BITWISE_XOR expression */
#line 320 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateBinary(C->E, EST_BITWISE_XOR, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, ekFalse); }
#line 1316 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 49: /* expression ::= expression BITWISE_AND expression */
#line 323 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateBinary(C->E, EST_BITWISE_AND, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, ekFalse); }
#line 1321 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 50: /* expression ::= expression BITWISE_OR expression */
#line 326 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateBinary(C->E, EST_BITWISE_OR, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, ekFalse); }
#line 1326 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 51: /* expression ::= expression SHIFTLEFT expression */
#line 329 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateBinary(C->E, EST_SHIFTLEFT, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, ekFalse); }
#line 1331 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 52: /* expression ::= expression SHIFTRIGHT expression */
#line 332 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateBinary(C->E, EST_SHIFTRIGHT, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, ekFalse); }
#line 1336 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 53: /* expression ::= expression MOD paren_expr_list */
        case 54: /* expression ::= expression MOD expression */ yytestcase(yyruleno==54);
#line 335 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateStringFormat(C->E, yymsp[-2].minor.yy168, yymsp[0].minor.yy168); }
#line 1342 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 55: /* expression ::= ARRAYOPENBRACKET CLOSEBRACKET */
#line 341 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateUnary(C->E, EST_ARRAY, NULL, yymsp[-1].minor.yy0.line); }
#line 1347 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 56: /* expression ::= ARRAYOPENBRACKET expr_list CLOSEBRACKET */
#line 344 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateUnary(C->E, EST_ARRAY, yymsp[-1].minor.yy168, yymsp[-1].minor.yy168->line); }
#line 1352 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 57: /* expression ::= MAPSTARTBLOCK ENDBLOCK */
#line 347 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateUnary(C->E, EST_MAP, NULL, yymsp[-1].minor.yy0.line); }
#line 1357 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 58: /* expression ::= MAPSTARTBLOCK expr_list ENDBLOCK */
#line 350 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateUnary(C->E, EST_MAP, yymsp[-1].minor.yy168, yymsp[-1].minor.yy168->line); }
#line 1362 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 59: /* expression ::= lvalue ASSIGN expression */
#line 353 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateAssignment(C->E, yymsp[-2].minor.yy168, yymsp[0].minor.yy168); }
#line 1367 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 60: /* expression ::= expression INHERITS expression */
#line 356 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateInherits(C->E, yymsp[-2].minor.yy168, yymsp[0].minor.yy168); }
#line 1372 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 61: /* expression ::= lvalue */
        case 71: /* lvalue ::= lvalue_indexable */ yytestcase(yyruleno==71);
#line 359 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = yymsp[0].minor.yy168; }
#line 1378 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 62: /* expression ::= INTEGER */
#line 362 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateKInt(C->E, &yymsp[0].minor.yy0, 0); }
#line 1383 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 63: /* expression ::= NEGATIVE INTEGER */
#line 365 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateKInt(C->E, &yymsp[0].minor.yy0, CKO_NEGATIVE); }
#line 1388 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 64: /* expression ::= FLOATNUM */
#line 368 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateKFloat(C->E, &yymsp[0].minor.yy0, 0); }
#line 1393 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 65: /* expression ::= NEGATIVE FLOATNUM */
#line 371 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateKFloat(C->E, &yymsp[0].minor.yy0, CKO_NEGATIVE); }
#line 1398 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 66: /* expression ::= LITERALSTRING */
#line 374 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateKString(C->E, &yymsp[0].minor.yy0, 0); }
#line 1403 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 67: /* expression ::= REGEXSTRING */
#line 377 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateKString(C->E, &yymsp[0].minor.yy0, 1); }
#line 1408 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 68: /* expression ::= NULL */
#line 380 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateNull(C->E, yymsp[0].minor.yy0.line); }
#line 1413 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 69: /* expression ::= FUNCTION LEFTPAREN func_args RIGHTPAREN statement_block */
#line 383 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateFunctionDecl(C->E, NULL, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, yymsp[-4].minor.yy0.line); }
#line 1418 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 70: /* expression ::= expression QUESTIONMARK expression COLON expression */
#line 386 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateIfElse(C->E, yymsp[-4].minor.yy168, yymsp[-2].minor.yy168, yymsp[0].minor.yy168, ekTrue); }
#line 1423 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 72: /* lvalue ::= VAR IDENTIFIER */
#line 402 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxMarkVar(C->E, ekSyntaxCreateList(C->E, EST_IDENTIFIERLIST, ekSyntaxCreateIdentifier(C->E, &yymsp[0].minor.yy0))); }
#line 1428 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 73: /* lvalue ::= VAR GROUPLEFTPAREN ident_list RIGHTPAREN */
#line 405 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxMarkVar(C->E, yymsp[-1].minor.yy168); }
#line 1433 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 75: /* lvalue_indexable ::= THIS */
#line 420 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateThis(C->E, yymsp[0].minor.yy0.line); }
#line 1438 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 76: /* lvalue_indexable ::= lvalue_indexable paren_expr_list */
#line 423 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateCall(C->E, yymsp[-1].minor.yy168, yymsp[0].minor.yy168); }
#line 1443 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 77: /* lvalue_indexable ::= lvalue_indexable OPENBRACKET expression CLOSEBRACKET */
#line 426 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateIndex(C->E, yymsp[-3].minor.yy168, yymsp[-1].minor.yy168, ekFalse); }
#line 1448 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 78: /* lvalue_indexable ::= lvalue_indexable PERIOD IDENTIFIER */
#line 429 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateIndex(C->E, yymsp[-2].minor.yy168, ekSyntaxCreateKString(C->E, &yymsp[0].minor.yy0, 0), ekFalse); }
#line 1453 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 79: /* lvalue_indexable ::= lvalue_indexable COLONCOLON IDENTIFIER */
#line 432 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateIndex(C->E, yymsp[-2].minor.yy168, ekSyntaxCreateKString(C->E, &yymsp[0].minor.yy0, 0), ekTrue); }
#line 1458 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 80: /* lvalue_indexable ::= IDENTIFIER */
#line 435 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateIdentifier(C->E, &yymsp[0].minor.yy0); }
#line 1463 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 81: /* ident_list ::= ident_list COMMA IDENTIFIER */
#line 447 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxListAppend(C->E, yymsp[-2].minor.yy168, ekSyntaxCreateIdentifier(C->E, &yymsp[0].minor.yy0), 0); }
#line 1468 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 82: /* ident_list ::= IDENTIFIER */
#line 450 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateList(C->E, EST_IDENTIFIERLIST, ekSyntaxCreateIdentifier(C->E, &yymsp[0].minor.yy0)); }
#line 1473 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 83: /* ident_list ::= */
#line 453 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateList(C->E, EST_IDENTIFIERLIST, NULL); }
#line 1478 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 84: /* func_args ::= ident_list */
#line 462 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateFunctionArgs(C->E, yymsp[0].minor.yy168, NULL); }
#line 1483 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 85: /* func_args ::= ident_list COMMA ELLIPSIS IDENTIFIER */
#line 465 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateFunctionArgs(C->E, yymsp[-3].minor.yy168, &yymsp[0].minor.yy0); }
#line 1488 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        case 86: /* func_args ::= ELLIPSIS IDENTIFIER */
#line 468 "/home/joe/private/work/eureka/lib/ekParser.y"
            { yygotominor.yy168 = ekSyntaxCreateFunctionArgs(C->E, ekSyntaxCreateList(C->E, EST_IDENTIFIERLIST, NULL), &yymsp[0].minor.yy0); }
#line 1493 "/home/joe/private/work/eureka/lib/ekParser.c"
            break;
        default:
            /* (27) statement ::= error ENDSTATEMENT */ yytestcase(yyruleno==27);
            break;
    };
    yygoto = yyRuleInfo[yyruleno].lhs;
    yysize = yyRuleInfo[yyruleno].nrhs;
    yypParser->yyidx -= yysize;
    yyact = yy_find_reduce_action(yymsp[-yysize].stateno,(YYCODETYPE)yygoto);
    if(yyact < YYNSTATE)
    {
#ifdef NDEBUG
        /* If we are not debugging and the reduce action popped at least
        ** one element off the stack, then we can push the new element back
        ** onto the stack here, and skip the stack overflow test in yy_shift().
        ** That gives a significant speed improvement. */
        if(yysize)
        {
            yypParser->yyidx++;
            yymsp -= yysize-1;
            yymsp->stateno = (YYACTIONTYPE)yyact;
            yymsp->major = (YYCODETYPE)yygoto;
            yymsp->minor = yygotominor;
        }
        else
#endif
        {
            yy_shift(yypParser,yyact,yygoto,&yygotominor);
        }
    }
    else
    {
        assert(yyact == YYNSTATE + YYNRULE + 1);
        yy_accept(yypParser);
    }
}

/*
** The following code executes when the parse fails
*/
#ifndef YYNOERRORRECOVERY
static void yy_parse_failed(
    yyParser *yypParser           /* The parser */
)
{
    ekParseARG_FETCH;
#ifndef NDEBUG
    if(yyTraceFILE)
    {
        fprintf(yyTraceFILE,"%sFail!\n",yyTracePrompt);
    }
#endif
    while(yypParser->yyidx>=0) { yy_pop_parser_stack(yypParser); }
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
)
{
    ekParseARG_FETCH;
#define TOKEN (yyminor.yy0)
#line 117 "/home/joe/private/work/eureka/lib/ekParser.y"
    ekCompileSyntaxError(C, &TOKEN);
#line 1558 "/home/joe/private/work/eureka/lib/ekParser.c"
    ekParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following is executed when the parser accepts
*/
static void yy_accept(
    yyParser *yypParser           /* The parser */
)
{
    ekParseARG_FETCH;
#ifndef NDEBUG
    if(yyTraceFILE)
    {
        fprintf(yyTraceFILE,"%sAccept!\n",yyTracePrompt);
    }
#endif
    while(yypParser->yyidx>=0) { yy_pop_parser_stack(yypParser); }
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
)
{
    YYMINORTYPE yyminorunion;
    int yyact;            /* The parser action. */
    int yyendofinput;     /* True if we are at the end of input */
#ifdef YYERRORSYMBOL
    int yyerrorhit = 0;   /* True if yymajor has invoked an error */
#endif
    yyParser *yypParser;  /* The parser */

    /* (re)initialize the parser, if necessary */
    yypParser = (yyParser *)yyp;
    if(yypParser->yyidx<0)
    {
#if YYSTACKDEPTH<=0
        if(yypParser->yystksz <=0)
        {
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
    if(yyTraceFILE)
    {
        fprintf(yyTraceFILE,"%sInput %s\n",yyTracePrompt,yyTokenName[yymajor]);
    }
#endif

    do
    {
        yyact = yy_find_shift_action(yypParser,(YYCODETYPE)yymajor);
        if(yyact<YYNSTATE)
        {
            assert(!yyendofinput);    /* Impossible to shift the $ token */
            yy_shift(yypParser,yyact,yymajor,&yyminorunion);
            yypParser->yyerrcnt--;
            yymajor = YYNOCODE;
        }
        else if(yyact < YYNSTATE + YYNRULE)
        {
            yy_reduce(yypParser,yyact-YYNSTATE);
        }
        else
        {
#ifdef YYERRORSYMBOL
            int yymx;
#endif
            assert(yyact == YY_ERROR_ACTION);
#ifndef NDEBUG
            if(yyTraceFILE)
            {
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
            if(yypParser->yyerrcnt<0)
            {
                yy_syntax_error(yypParser,yymajor,yyminorunion);
            }
            yymx = yypParser->yystack[yypParser->yyidx].major;
            if(yymx==YYERRORSYMBOL || yyerrorhit)
            {
#ifndef NDEBUG
                if(yyTraceFILE)
                {
                    fprintf(yyTraceFILE,"%sDiscard input token %s\n",
                            yyTracePrompt,yyTokenName[yymajor]);
                }
#endif
                yy_destructor(yypParser, (YYCODETYPE)yymajor,&yyminorunion);
                yymajor = YYNOCODE;
            }
            else
            {
                while(
                    yypParser->yyidx >= 0 &&
                    yymx != YYERRORSYMBOL &&
                    (yyact = yy_find_reduce_action(
                                 yypParser->yystack[yypParser->yyidx].stateno,
                                 YYERRORSYMBOL)) >= YYNSTATE
                )
                {
                    yy_pop_parser_stack(yypParser);
                }
                if(yypParser->yyidx < 0 || yymajor==0)
                {
                    yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
                    yy_parse_failed(yypParser);
                    yymajor = YYNOCODE;
                }
                else if(yymx!=YYERRORSYMBOL)
                {
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
            if(yypParser->yyerrcnt<=0)
            {
                yy_syntax_error(yypParser,yymajor,yyminorunion);
            }
            yypParser->yyerrcnt = 3;
            yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
            if(yyendofinput)
            {
                yy_parse_failed(yypParser);
            }
            yymajor = YYNOCODE;
#endif
        }
    }
    while(yymajor!=YYNOCODE && yypParser->yyidx>=0);
    return;
}
