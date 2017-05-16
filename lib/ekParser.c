/* Driver template for the LEMON parser generator.
** The author disclaims copyright to this source code.
*/
/* First off, code is included that follows the "include" declaration
** in the input grammar file. */
#include "ekTypes.h"
#include <stdio.h>
#line 22 "/mnt/f/work/eureka/lib/ekParser.y"

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
#line 26 "/mnt/f/work/eureka/lib/ekParser.c"
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
    ekSyntax * yy94;
    ekToken yy170;
    int yy183;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define ekParseARG_SDECL  ekCompiler * C;
#define ekParseARG_PDECL , ekCompiler * C
#define ekParseARG_FETCH  ekCompiler * C  = yypParser->C
#define ekParseARG_STORE yypParser->C  = C
#define YYNSTATE 182
#define YYNRULE 96
#define YYERRORSYMBOL 79
#define YYERRSYMDT yy183
#define YYFALLBACK 1
#define YY_NO_ACTION      (YYNSTATE + YYNRULE + 2)
#define YY_ACCEPT_ACTION  (YYNSTATE + YYNRULE + 1)
#define YY_ERROR_ACTION   (YYNSTATE + YYNRULE)

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
    /*     0 */ 20, 131, 8, 5, 16, 108, 24, 15, 17, 21,
    /*    10 */ 42, 24, 44, 156, 18, 69, 114, 59, 63, 163,
    /*    20 */ 126, 52, 54, 53, 56, 55, 47, 51, 49, 50,
    /*    30 */ 101, 114, 59, 29, 39, 37, 28, 38, 25, 23,
    /*    40 */ 31, 32, 30, 35, 36, 33, 34, 43, 41, 40,
    /*    50 */ 22, 26, 27, 13, 40, 22, 26, 27, 13, 132,
    /*    60 */ 167, 12, 19, 109, 164, 150, 64, 161, 24, 172,
    /*    70 */ 139, 142, 143, 144, 145, 146, 147, 110, 20, 65,
    /*    80 */ 8, 5, 16, 108, 123, 15, 128, 151, 155, 61,
    /*    90 */ 134, 129, 18, 71, 95, 59, 63, 163, 28, 38,
    /*   100 */ 21, 42, 31, 32, 30, 35, 36, 33, 34, 43,
    /*   110 */ 41, 40, 22, 26, 27, 13, 25, 23, 20, 161,
    /*   120 */ 8, 5, 16, 108, 123, 15, 62, 151, 117, 61,
    /*   130 */ 116, 124, 18, 71, 95, 59, 63, 163, 167, 12,
    /*   140 */ 19, 109, 164, 158, 64, 111, 21, 42, 139, 142,
    /*   150 */ 143, 144, 145, 146, 147, 110, 25, 23, 20, 180,
    /*   160 */ 8, 136, 16, 108, 123, 15, 10, 170, 24, 61,
    /*   170 */ 21, 42, 18, 71, 95, 59, 63, 163, 167, 12,
    /*   180 */ 19, 109, 164, 181, 64, 87, 114, 59, 139, 142,
    /*   190 */ 143, 144, 145, 146, 147, 110, 25, 23, 13, 38,
    /*   200 */ 119, 124, 31, 32, 30, 35, 36, 33, 34, 43,
    /*   210 */ 41, 40, 22, 26, 27, 13, 138, 175, 167, 12,
    /*   220 */ 19, 109, 164, 24, 64, 72, 114, 59, 139, 142,
    /*   230 */ 143, 144, 145, 146, 147, 110, 20, 17, 8, 4,
    /*   240 */ 16, 108, 123, 15, 24, 151, 153, 61, 46, 120,
    /*   250 */ 18, 71, 95, 59, 63, 163, 57, 31, 32, 30,
    /*   260 */ 35, 36, 33, 34, 43, 41, 40, 22, 26, 27,
    /*   270 */ 13, 106, 114, 59, 25, 23, 88, 114, 59, 32,
    /*   280 */ 30, 35, 36, 33, 34, 43, 41, 40, 22, 26,
    /*   290 */ 27, 13, 107, 114, 59, 171, 167, 12, 19, 109,
    /*   300 */ 164, 6, 64, 24, 27, 13, 139, 142, 143, 144,
    /*   310 */ 145, 146, 147, 110, 182, 20, 7, 8, 24, 16,
    /*   320 */ 108, 123, 15, 58, 151, 176, 61, 140, 141, 18,
    /*   330 */ 71, 95, 59, 63, 163, 113, 114, 59, 160, 30,
    /*   340 */ 35, 36, 33, 34, 43, 41, 40, 22, 26, 27,
    /*   350 */ 13, 165, 166, 25, 23, 20, 169, 8, 24, 16,
    /*   360 */ 108, 112, 15, 26, 27, 13, 24, 179, 173, 18,
    /*   370 */ 178, 114, 59, 63, 163, 167, 12, 19, 109, 164,
    /*   380 */ 174, 64, 86, 114, 59, 139, 142, 143, 144, 145,
    /*   390 */ 146, 147, 110, 25, 23, 35, 36, 33, 34, 43,
    /*   400 */ 41, 40, 22, 26, 27, 13, 34, 43, 41, 40,
    /*   410 */ 22, 26, 27, 13, 148, 167, 12, 19, 109, 164,
    /*   420 */ 280, 64, 24, 125, 280, 139, 142, 143, 144, 145,
    /*   430 */ 146, 147, 110, 20, 24, 8, 280, 16, 108, 123,
    /*   440 */ 15, 24, 151, 152, 61, 280, 280, 18, 71, 95,
    /*   450 */ 59, 63, 163, 83, 114, 59, 92, 114, 59, 280,
    /*   460 */ 36, 33, 34, 43, 41, 40, 22, 26, 27, 13,
    /*   470 */ 160, 25, 23, 33, 34, 43, 41, 40, 22, 26,
    /*   480 */ 27, 13, 43, 41, 40, 22, 26, 27, 13, 90,
    /*   490 */ 114, 59, 168, 167, 12, 19, 109, 164, 280, 64,
    /*   500 */ 280, 24, 280, 139, 142, 143, 144, 145, 146, 147,
    /*   510 */ 110, 20, 280, 8, 280, 16, 108, 123, 15, 280,
    /*   520 */ 151, 154, 61, 24, 280, 18, 71, 95, 59, 63,
    /*   530 */ 163, 123, 279, 9, 170, 57, 61, 91, 114, 59,
    /*   540 */ 71, 95, 59, 41, 40, 22, 26, 27, 13, 25,
    /*   550 */ 23, 39, 37, 28, 38, 280, 280, 31, 32, 30,
    /*   560 */ 35, 36, 33, 34, 43, 41, 40, 22, 26, 27,
    /*   570 */ 13, 167, 12, 19, 109, 164, 280, 64, 96, 114,
    /*   580 */ 59, 139, 142, 143, 144, 145, 146, 147, 110, 24,
    /*   590 */ 97, 114, 59, 280, 280, 44, 45, 123, 118, 11,
    /*   600 */ 170, 280, 61, 280, 149, 102, 71, 95, 59, 71,
    /*   610 */ 114, 59, 21, 42, 24, 280, 29, 39, 37, 28,
    /*   620 */ 38, 280, 280, 31, 32, 30, 35, 36, 33, 34,
    /*   630 */ 43, 41, 40, 22, 26, 27, 13, 24, 159, 123,
    /*   640 */ 280, 120, 127, 44, 61, 26, 27, 13, 71, 95,
    /*   650 */ 59, 60, 66, 280, 280, 71, 114, 59, 22, 26,
    /*   660 */ 27, 13, 280, 280, 29, 39, 37, 28, 38, 280,
    /*   670 */ 280, 31, 32, 30, 35, 36, 33, 34, 43, 41,
    /*   680 */ 40, 22, 26, 27, 13, 24, 93, 114, 59, 157,
    /*   690 */ 103, 44, 280, 104, 71, 114, 59, 71, 114, 59,
    /*   700 */ 280, 2, 280, 280, 3, 71, 114, 59, 71, 114,
    /*   710 */ 59, 280, 29, 39, 37, 28, 38, 280, 280, 31,
    /*   720 */ 32, 30, 35, 36, 33, 34, 43, 41, 40, 22,
    /*   730 */ 26, 27, 13, 24, 105, 280, 14, 280, 71, 114,
    /*   740 */ 59, 280, 280, 48, 94, 114, 59, 85, 114, 59,
    /*   750 */ 89, 114, 59, 84, 114, 59, 100, 114, 59, 280,
    /*   760 */ 29, 39, 37, 28, 38, 280, 280, 31, 32, 30,
    /*   770 */ 35, 36, 33, 34, 43, 41, 40, 22, 26, 27,
    /*   780 */ 13, 20, 14, 280, 280, 16, 108, 280, 15, 121,
    /*   790 */ 122, 280, 280, 99, 114, 59, 70, 114, 59, 280,
    /*   800 */ 163, 98, 114, 59, 67, 114, 59, 280, 73, 114,
    /*   810 */ 59, 1, 114, 59, 280, 74, 114, 59, 280, 25,
    /*   820 */ 23, 20, 14, 280, 280, 16, 108, 280, 15, 68,
    /*   830 */ 114, 59, 75, 114, 59, 76, 114, 59, 280, 280,
    /*   840 */ 163, 130, 20, 280, 115, 164, 16, 108, 280, 15,
    /*   850 */ 280, 139, 142, 143, 144, 145, 146, 147, 110, 25,
    /*   860 */ 23, 163, 280, 20, 77, 114, 59, 16, 108, 280,
    /*   870 */ 15, 78, 114, 59, 79, 114, 59, 80, 114, 59,
    /*   880 */ 25, 23, 163, 280, 115, 164, 81, 114, 59, 280,
    /*   890 */ 280, 139, 142, 143, 144, 145, 146, 147, 110, 280,
    /*   900 */ 280, 25, 23, 280, 20, 115, 164, 177, 16, 108,
    /*   910 */ 280, 15, 139, 142, 143, 144, 145, 146, 147, 110,
    /*   920 */ 280, 135, 280, 163, 280, 133, 115, 164, 21, 42,
    /*   930 */ 82, 114, 59, 139, 142, 143, 144, 145, 146, 147,
    /*   940 */ 110, 162, 25, 23, 21, 42, 280, 280, 280, 280,
    /*   950 */ 280, 280, 280, 280, 280, 280, 280, 280, 280, 280,
    /*   960 */ 280, 280, 280, 137, 280, 280, 280, 115, 164, 280,
    /*   970 */ 280, 24, 280, 280, 139, 142, 143, 144, 145, 146,
    /*   980 */ 147, 110, 280, 280, 280, 280, 280, 280, 280, 280,
    /*   990 */ 280, 280, 280, 280, 280, 280, 280, 280, 280, 280,
    /*  1000 */ 37, 28, 38, 280, 280, 31, 32, 30, 35, 36,
    /*  1010 */ 33, 34, 43, 41, 40, 22, 26, 27, 13, 20,
    /*  1020 */ 280, 280, 280, 16, 108, 280, 15, 280, 280, 280,
    /*  1030 */ 280, 280, 280, 280, 280, 280, 280, 280, 163, 280,
    /*  1040 */ 280, 280, 280, 280, 280, 280, 280, 280, 280, 280,
    /*  1050 */ 280, 280, 280, 280, 280, 280, 280, 25, 23, 280,
    /*  1060 */ 280, 280, 280, 280, 280, 280, 280, 280, 280, 280,
    /*  1070 */ 280, 280, 280, 280, 280, 280, 280, 280, 280, 280,
    /*  1080 */ 280, 280, 115, 164, 280, 280, 280, 280, 280, 139,
    /*  1090 */ 142, 143, 144, 145, 146, 147, 110,
};
static const YYCODETYPE yy_lookahead[] = {
    /*     0 */ 1, 61, 3, 4, 5, 6, 7, 8, 12, 69,
    /*    10 */ 70, 7, 13, 85, 15, 88, 89, 90, 19, 20,
    /*    20 */ 18, 25, 26, 27, 28, 29, 30, 31, 32, 33,
    /*    30 */ 88, 89, 90, 34, 35, 36, 37, 38, 39, 40,
    /*    40 */ 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
    /*    50 */ 51, 52, 53, 54, 50, 51, 52, 53, 54, 79,
    /*    60 */ 61, 62, 63, 64, 65, 79, 67, 65, 7, 79,
    /*    70 */ 71, 72, 73, 74, 75, 76, 77, 78, 1, 1,
    /*    80 */ 3, 4, 5, 6, 79, 8, 61, 82, 83, 84,
    /*    90 */ 59, 79, 15, 88, 89, 90, 19, 20, 37, 38,
    /*   100 */ 69, 70, 41, 42, 43, 44, 45, 46, 47, 48,
    /*   110 */ 49, 50, 51, 52, 53, 54, 39, 40, 1, 65,
    /*   120 */ 3, 4, 5, 6, 79, 8, 87, 82, 83, 84,
    /*   130 */ 86, 87, 15, 88, 89, 90, 19, 20, 61, 62,
    /*   140 */ 63, 64, 65, 65, 67, 87, 69, 70, 71, 72,
    /*   150 */ 73, 74, 75, 76, 77, 78, 39, 40, 1, 61,
    /*   160 */ 3, 60, 5, 6, 79, 8, 81, 82, 7, 84,
    /*   170 */ 69, 70, 15, 88, 89, 90, 19, 20, 61, 62,
    /*   180 */ 63, 64, 65, 79, 67, 88, 89, 90, 71, 72,
    /*   190 */ 73, 74, 75, 76, 77, 78, 39, 40, 54, 38,
    /*   200 */ 86, 87, 41, 42, 43, 44, 45, 46, 47, 48,
    /*   210 */ 49, 50, 51, 52, 53, 54, 79, 60, 61, 62,
    /*   220 */ 63, 64, 65, 7, 67, 88, 89, 90, 71, 72,
    /*   230 */ 73, 74, 75, 76, 77, 78, 1, 12, 3, 66,
    /*   240 */ 5, 6, 79, 8, 7, 82, 83, 84, 68, 69,
    /*   250 */ 15, 88, 89, 90, 19, 20, 2, 41, 42, 43,
    /*   260 */ 44, 45, 46, 47, 48, 49, 50, 51, 52, 53,
    /*   270 */ 54, 88, 89, 90, 39, 40, 88, 89, 90, 42,
    /*   280 */ 43, 44, 45, 46, 47, 48, 49, 50, 51, 52,
    /*   290 */ 53, 54, 88, 89, 90, 60, 61, 62, 63, 64,
    /*   300 */ 65, 16, 67, 7, 53, 54, 71, 72, 73, 74,
    /*   310 */ 75, 76, 77, 78, 0, 1, 66, 3, 7, 5,
    /*   320 */ 6, 79, 8, 2, 82, 83, 84, 71, 72, 15,
    /*   330 */ 88, 89, 90, 19, 20, 88, 89, 90, 65, 43,
    /*   340 */ 44, 45, 46, 47, 48, 49, 50, 51, 52, 53,
    /*   350 */ 54, 65, 65, 39, 40, 1, 61, 3, 7, 5,
    /*   360 */ 6, 69, 8, 52, 53, 54, 7, 85, 65, 15,
    /*   370 */ 88, 89, 90, 19, 20, 61, 62, 63, 64, 65,
    /*   380 */ 65, 67, 88, 89, 90, 71, 72, 73, 74, 75,
    /*   390 */ 76, 77, 78, 39, 40, 44, 45, 46, 47, 48,
    /*   400 */ 49, 50, 51, 52, 53, 54, 47, 48, 49, 50,
    /*   410 */ 51, 52, 53, 54, 60, 61, 62, 63, 64, 65,
    /*   420 */ 91, 67, 7, 18, 91, 71, 72, 73, 74, 75,
    /*   430 */ 76, 77, 78, 1, 7, 3, 91, 5, 6, 79,
    /*   440 */ 8, 7, 82, 83, 84, 91, 91, 15, 88, 89,
    /*   450 */ 90, 19, 20, 88, 89, 90, 88, 89, 90, 91,
    /*   460 */ 45, 46, 47, 48, 49, 50, 51, 52, 53, 54,
    /*   470 */ 65, 39, 40, 46, 47, 48, 49, 50, 51, 52,
    /*   480 */ 53, 54, 48, 49, 50, 51, 52, 53, 54, 88,
    /*   490 */ 89, 90, 60, 61, 62, 63, 64, 65, 91, 67,
    /*   500 */ 91, 7, 91, 71, 72, 73, 74, 75, 76, 77,
    /*   510 */ 78, 1, 91, 3, 91, 5, 6, 79, 8, 91,
    /*   520 */ 82, 83, 84, 7, 91, 15, 88, 89, 90, 19,
    /*   530 */ 20, 79, 80, 81, 82, 2, 84, 88, 89, 90,
    /*   540 */ 88, 89, 90, 49, 50, 51, 52, 53, 54, 39,
    /*   550 */ 40, 35, 36, 37, 38, 91, 91, 41, 42, 43,
    /*   560 */ 44, 45, 46, 47, 48, 49, 50, 51, 52, 53,
    /*   570 */ 54, 61, 62, 63, 64, 65, 91, 67, 88, 89,
    /*   580 */ 90, 71, 72, 73, 74, 75, 76, 77, 78, 7,
    /*   590 */ 88, 89, 90, 91, 91, 13, 14, 79, 65, 81,
    /*   600 */ 82, 91, 84, 91, 61, 84, 88, 89, 90, 88,
    /*   610 */ 89, 90, 69, 70, 7, 91, 34, 35, 36, 37,
    /*   620 */ 38, 91, 91, 41, 42, 43, 44, 45, 46, 47,
    /*   630 */ 48, 49, 50, 51, 52, 53, 54, 7, 66, 79,
    /*   640 */ 91, 69, 82, 13, 84, 52, 53, 54, 88, 89,
    /*   650 */ 90, 84, 85, 91, 91, 88, 89, 90, 51, 52,
    /*   660 */ 53, 54, 91, 91, 34, 35, 36, 37, 38, 91,
    /*   670 */ 91, 41, 42, 43, 44, 45, 46, 47, 48, 49,
    /*   680 */ 50, 51, 52, 53, 54, 7, 88, 89, 90, 59,
    /*   690 */ 84, 13, 91, 84, 88, 89, 90, 88, 89, 90,
    /*   700 */ 91, 84, 91, 91, 84, 88, 89, 90, 88, 89,
    /*   710 */ 90, 91, 34, 35, 36, 37, 38, 91, 91, 41,
    /*   720 */ 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,
    /*   730 */ 52, 53, 54, 7, 84, 91, 2, 91, 88, 89,
    /*   740 */ 90, 91, 91, 9, 88, 89, 90, 88, 89, 90,
    /*   750 */ 88, 89, 90, 88, 89, 90, 88, 89, 90, 91,
    /*   760 */ 34, 35, 36, 37, 38, 91, 91, 41, 42, 43,
    /*   770 */ 44, 45, 46, 47, 48, 49, 50, 51, 52, 53,
    /*   780 */ 54, 1, 2, 91, 91, 5, 6, 91, 8, 55,
    /*   790 */ 56, 91, 91, 88, 89, 90, 88, 89, 90, 91,
    /*   800 */ 20, 88, 89, 90, 88, 89, 90, 91, 88, 89,
    /*   810 */ 90, 88, 89, 90, 91, 88, 89, 90, 91, 39,
    /*   820 */ 40, 1, 2, 91, 91, 5, 6, 91, 8, 88,
    /*   830 */ 89, 90, 88, 89, 90, 88, 89, 90, 91, 91,
    /*   840 */ 20, 61, 1, 91, 64, 65, 5, 6, 91, 8,
    /*   850 */ 91, 71, 72, 73, 74, 75, 76, 77, 78, 39,
    /*   860 */ 40, 20, 91, 1, 88, 89, 90, 5, 6, 91,
    /*   870 */ 8, 88, 89, 90, 88, 89, 90, 88, 89, 90,
    /*   880 */ 39, 40, 20, 91, 64, 65, 88, 89, 90, 91,
    /*   890 */ 91, 71, 72, 73, 74, 75, 76, 77, 78, 91,
    /*   900 */ 91, 39, 40, 91, 1, 64, 65, 66, 5, 6,
    /*   910 */ 91, 8, 71, 72, 73, 74, 75, 76, 77, 78,
    /*   920 */ 91, 59, 91, 20, 91, 66, 64, 65, 69, 70,
    /*   930 */ 88, 89, 90, 71, 72, 73, 74, 75, 76, 77,
    /*   940 */ 78, 66, 39, 40, 69, 70, 91, 91, 91, 91,
    /*   950 */ 91, 91, 91, 91, 91, 91, 91, 91, 91, 91,
    /*   960 */ 91, 91, 91, 60, 91, 91, 91, 64, 65, 91,
    /*   970 */ 91, 7, 91, 91, 71, 72, 73, 74, 75, 76,
    /*   980 */ 77, 78, 91, 91, 91, 91, 91, 91, 91, 91,
    /*   990 */ 91, 91, 91, 91, 91, 91, 91, 91, 91, 91,
    /*  1000 */ 36, 37, 38, 91, 91, 41, 42, 43, 44, 45,
    /*  1010 */ 46, 47, 48, 49, 50, 51, 52, 53, 54, 1,
    /*  1020 */ 91, 91, 91, 5, 6, 91, 8, 91, 91, 91,
    /*  1030 */ 91, 91, 91, 91, 91, 91, 91, 91, 20, 91,
    /*  1040 */ 91, 91, 91, 91, 91, 91, 91, 91, 91, 91,
    /*  1050 */ 91, 91, 91, 91, 91, 91, 91, 39, 40, 91,
    /*  1060 */ 91, 91, 91, 91, 91, 91, 91, 91, 91, 91,
    /*  1070 */ 91, 91, 91, 91, 91, 91, 91, 91, 91, 91,
    /*  1080 */ 91, 91, 64, 65, 91, 91, 91, 91, 91, 71,
    /*  1090 */ 72, 73, 74, 75, 76, 77, 78,
};
#define YY_SHIFT_USE_DFLT (-61)
#define YY_SHIFT_MAX 126
static const short yy_shift_ofst[] = {
    /*     0 */ 510, -1, 77, 77, 117, 157, 117, 117, 235, 314,
    /*    10 */ 354, 432, 780, 820, 841, 862, 903, 1018, 1018, 1018,
    /*    20 */ 1018, 1018, 1018, 1018, 1018, 1018, 1018, 1018, 1018, 1018,
    /*    30 */ 1018, 1018, 1018, 1018, 1018, 1018, 1018, 1018, 1018, 1018,
    /*    40 */ 1018, 1018, 1018, 1018, 1018, 1018, 1018, 1018, 1018, 1018,
    /*    50 */ 1018, 1018, 1018, 1018, 1018, 1018, 1018, 2, 2, 734,
    /*    60 */ -60, 543, 180, 25, 54, 54, 98, 582, 630, 678,
    /*    70 */ 678, 678, 678, 726, 726, 726, 726, 726, 726, 726,
    /*    80 */ 726, 726, 726, 516, 964, 61, 161, 216, 216, 216,
    /*    90 */ 237, 296, 351, 415, 427, -4, 359, 434, 494, 4,
    /*   100 */ 607, 311, 859, 31, 101, 875, 593, 251, 256, 533,
    /*   110 */ 78, 572, 405, 144, 225, 254, 173, 285, 321, 250,
    /*   120 */ 273, 286, 287, 295, 292, 303, 315,
};
#define YY_REDUCE_USE_DFLT (-74)
#define YY_REDUCE_MAX 66
static const short yy_reduce_ofst[] = {
    /*     0 */ 452, 5, 45, 163, 242, 85, 360, 438, 518, 560,
    /*    10 */ 560, 560, 567, 282, 521, 606, 609, 137, 617, 620,
    /*    20 */ 650, -73, -58, 97, 183, 188, 204, 247, 294, 365,
    /*    30 */ 368, 401, 449, 490, 502, 598, 656, 659, 662, 665,
    /*    40 */ 668, 705, 708, 713, 716, 720, 723, 727, 741, 744,
    /*    50 */ 747, 776, 783, 786, 789, 798, 842, 44, 114, -72,
    /*    60 */ -20, -14, -10, 12, 39, 58, 104,
};
static const YYACTIONTYPE yy_default[] = {
    /*     0 */ 278, 278, 278, 278, 278, 278, 278, 278, 278, 278,
    /*    10 */ 278, 278, 278, 278, 278, 278, 278, 278, 278, 278,
    /*    20 */ 278, 278, 278, 278, 278, 278, 278, 278, 278, 278,
    /*    30 */ 278, 278, 278, 278, 278, 278, 278, 278, 278, 278,
    /*    40 */ 278, 278, 278, 278, 278, 278, 278, 278, 278, 278,
    /*    50 */ 278, 278, 278, 278, 278, 278, 278, 274, 274, 262,
    /*    60 */ 278, 278, 278, 278, 274, 274, 278, 278, 278, 218,
    /*    70 */ 219, 220, 247, 261, 203, 204, 205, 206, 207, 208,
    /*    80 */ 209, 210, 211, 228, 238, 236, 227, 221, 222, 237,
    /*    90 */ 230, 231, 229, 234, 235, 250, 232, 233, 249, 240,
    /*   100 */ 239, 223, 278, 278, 278, 278, 224, 225, 278, 278,
    /*   110 */ 278, 278, 278, 226, 250, 278, 278, 198, 278, 278,
    /*   120 */ 278, 278, 278, 278, 275, 278, 278, 183, 188, 189,
    /*   130 */ 190, 191, 192, 216, 244, 243, 246, 245, 248, 251,
    /*   140 */ 252, 254, 253, 255, 256, 257, 258, 259, 186, 195,
    /*   150 */ 196, 187, 197, 199, 200, 201, 267, 268, 263, 264,
    /*   160 */ 272, 273, 265, 266, 271, 269, 270, 212, 214, 215,
    /*   170 */ 184, 213, 202, 276, 277, 185, 260, 217, 242, 241,
    /*   180 */ 193, 194,
};
#define YY_SZ_ACTTAB (int)(sizeof(yy_action) / sizeof(yy_action[0]))

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
    0, /*          $ => nothing */
    0, /* GROUPLEFTPAREN => nothing */
    1, /*  LEFTPAREN => GROUPLEFTPAREN */
    5, /* SCOPESTARTBLOCK => MAPSTARTBLOCK */
    3, /* STARTBLOCK => SCOPESTARTBLOCK */
    0, /* MAPSTARTBLOCK => nothing */
    0, /*   NEGATIVE => nothing */
    6, /*       DASH => NEGATIVE */
    0, /* ARRAYOPENBRACKET => nothing */
    8, /* OPENBRACKET => ARRAYOPENBRACKET */
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
    YYACTIONTYPE stateno; /* The state-number */
    YYCODETYPE major;     /* The major token value.  This is the code
                          ** number for the token at this stack level */
    YYMINORTYPE minor;    /* The user-supplied minor token value.  This
                          ** is the value of the token  */
};
typedef struct yyStackEntry yyStackEntry;

/* The state of the parser is completely contained in an instance of
** the following structure */
struct yyParser
{
    int yyidx; /* Index of top element in stack */
#ifdef YYTRACKMAXSTACKDEPTH
    int yyidxMax; /* Maximum value of yyidx */
#endif
    int yyerrcnt;    /* Shifts left before out of the error */
    ekParseARG_SDECL /* A place to hold %extra_argument */
#if YYSTACKDEPTH <= 0
    int yystksz;            /* Current side of the stack */
    yyStackEntry * yystack; /* The parser's stack */
#else
    yyStackEntry yystack[YYSTACKDEPTH]; /* The parser's stack */
#endif
};
typedef struct yyParser yyParser;

#ifndef NDEBUG
#include <stdio.h>
static FILE * yyTraceFILE = 0;
static char * yyTracePrompt = 0;
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
void ekParseTrace(FILE * TraceFILE, char * zTracePrompt)
{
    yyTraceFILE = TraceFILE;
    yyTracePrompt = zTracePrompt;
    if (yyTraceFILE == 0) yyTracePrompt = 0;
    else if (yyTracePrompt == 0) yyTraceFILE = 0;
}
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing shifts, the names of all terminals and nonterminals
** are required.  The following table supplies these names */
static const char * const yyTokenName[] = {
    "$", "GROUPLEFTPAREN", "LEFTPAREN", "SCOPESTARTBLOCK",
    "STARTBLOCK", "MAPSTARTBLOCK", "NEGATIVE", "DASH",
    "ARRAYOPENBRACKET", "OPENBRACKET", "NEWLINE", "SEMI",
    "ASSIGN", "QUESTIONMARK", "COLON", "IF",
    "ELSE", "HEREDOC", "ELLIPSIS", "BREAK",
    "THIS", "UNKNOWN", "COMMENT", "SPACE",
    "EOF", "BITWISE_OREQUALS", "BITWISE_XOREQUALS", "BITWISE_ANDEQUALS",
    "SHIFTRIGHTEQUALS", "SHIFTLEFTEQUALS", "PLUSEQUALS", "SLASHEQUALS",
    "DASHEQUALS", "STAREQUALS", "OR", "BITWISE_OR",
    "BITWISE_XOR", "AND", "BITWISE_AND", "BITWISE_NOT",
    "NOT", "EQUALS", "NOTEQUALS", "CMP",
    "LESSTHAN", "LESSTHANOREQUAL", "GREATERTHAN", "GREATERTHANOREQUAL",
    "INHERITS", "SHIFTRIGHT", "SHIFTLEFT", "PLUS",
    "STAR", "SLASH", "MOD", "PERIOD",
    "COLONCOLON", "OPENBRACE", "CLOSEBRACE", "CLOSEBRACKET",
    "ENDBLOCK", "ENDSTATEMENT", "RETURN", "WHILE",
    "FUNCTION", "IDENTIFIER", "RIGHTPAREN", "FOR",
    "IN", "COMMA", "FATCOMMA", "INTEGER",
    "FLOATNUM", "LITERALSTRING", "REGEXSTRING", "TRUE",
    "FALSE", "NULL", "VAR", "error",
    "chunk", "statement_list", "statement", "statement_block",
    "expr_list", "paren_expr_list", "func_args", "ident_list",
    "expression", "lvalue", "lvalue_indexable",
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
 */
static const char * const yyRuleName[] = {
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
    /*  20 */ "statement ::= FOR ident_list error",
    /*  21 */ "statement ::= lvalue PLUSEQUALS expression",
    /*  22 */ "statement ::= lvalue DASHEQUALS expression",
    /*  23 */ "statement ::= lvalue STAREQUALS expression",
    /*  24 */ "statement ::= lvalue SLASHEQUALS expression",
    /*  25 */ "statement ::= lvalue BITWISE_OREQUALS expression",
    /*  26 */ "statement ::= lvalue BITWISE_ANDEQUALS expression",
    /*  27 */ "statement ::= lvalue BITWISE_XOREQUALS expression",
    /*  28 */ "statement ::= lvalue SHIFTLEFTEQUALS expression",
    /*  29 */ "statement ::= lvalue SHIFTRIGHTEQUALS expression",
    /*  30 */ "statement ::= ENDSTATEMENT",
    /*  31 */ "statement ::= SCOPESTARTBLOCK ENDBLOCK",
    /*  32 */ "statement ::= SCOPESTARTBLOCK statement_list ENDBLOCK",
    /*  33 */ "statement ::= error ENDSTATEMENT",
    /*  34 */ "paren_expr_list ::= LEFTPAREN expr_list RIGHTPAREN",
    /*  35 */ "paren_expr_list ::= LEFTPAREN RIGHTPAREN",
    /*  36 */ "expr_list ::= expr_list COMMA expression",
    /*  37 */ "expr_list ::= expr_list FATCOMMA expression",
    /*  38 */ "expr_list ::= expression",
    /*  39 */ "expression ::= NOT expression",
    /*  40 */ "expression ::= BITWISE_NOT expression",
    /*  41 */ "expression ::= expression PLUS expression",
    /*  42 */ "expression ::= expression DASH expression",
    /*  43 */ "expression ::= expression STAR expression",
    /*  44 */ "expression ::= expression SLASH expression",
    /*  45 */ "expression ::= expression AND expression",
    /*  46 */ "expression ::= expression OR expression",
    /*  47 */ "expression ::= expression CMP expression",
    /*  48 */ "expression ::= expression EQUALS expression",
    /*  49 */ "expression ::= expression NOTEQUALS expression",
    /*  50 */ "expression ::= expression GREATERTHAN expression",
    /*  51 */ "expression ::= expression GREATERTHANOREQUAL expression",
    /*  52 */ "expression ::= expression LESSTHAN expression",
    /*  53 */ "expression ::= expression LESSTHANOREQUAL expression",
    /*  54 */ "expression ::= expression BITWISE_XOR expression",
    /*  55 */ "expression ::= expression BITWISE_AND expression",
    /*  56 */ "expression ::= expression BITWISE_OR expression",
    /*  57 */ "expression ::= expression SHIFTLEFT expression",
    /*  58 */ "expression ::= expression SHIFTRIGHT expression",
    /*  59 */ "expression ::= expression MOD paren_expr_list",
    /*  60 */ "expression ::= expression MOD expression",
    /*  61 */ "expression ::= ARRAYOPENBRACKET CLOSEBRACKET",
    /*  62 */ "expression ::= ARRAYOPENBRACKET expr_list CLOSEBRACKET",
    /*  63 */ "expression ::= MAPSTARTBLOCK ENDBLOCK",
    /*  64 */ "expression ::= MAPSTARTBLOCK expr_list ENDBLOCK",
    /*  65 */ "expression ::= lvalue ASSIGN expression",
    /*  66 */ "expression ::= lvalue ASSIGN error",
    /*  67 */ "expression ::= expression INHERITS expression",
    /*  68 */ "expression ::= lvalue",
    /*  69 */ "expression ::= INTEGER",
    /*  70 */ "expression ::= NEGATIVE INTEGER",
    /*  71 */ "expression ::= FLOATNUM",
    /*  72 */ "expression ::= NEGATIVE FLOATNUM",
    /*  73 */ "expression ::= LITERALSTRING",
    /*  74 */ "expression ::= REGEXSTRING",
    /*  75 */ "expression ::= TRUE",
    /*  76 */ "expression ::= FALSE",
    /*  77 */ "expression ::= NULL",
    /*  78 */ "expression ::= FUNCTION LEFTPAREN func_args RIGHTPAREN statement_block",
    /*  79 */ "expression ::= expression QUESTIONMARK expression COLON expression",
    /*  80 */ "lvalue ::= lvalue_indexable",
    /*  81 */ "lvalue ::= VAR IDENTIFIER",
    /*  82 */ "lvalue ::= VAR GROUPLEFTPAREN ident_list RIGHTPAREN",
    /*  83 */ "lvalue ::= GROUPLEFTPAREN expr_list RIGHTPAREN",
    /*  84 */ "lvalue_indexable ::= THIS",
    /*  85 */ "lvalue_indexable ::= lvalue_indexable paren_expr_list",
    /*  86 */ "lvalue_indexable ::= lvalue_indexable OPENBRACKET expression CLOSEBRACKET",
    /*  87 */ "lvalue_indexable ::= lvalue_indexable PERIOD IDENTIFIER",
    /*  88 */ "lvalue_indexable ::= lvalue_indexable COLONCOLON IDENTIFIER",
    /*  89 */ "lvalue_indexable ::= IDENTIFIER",
    /*  90 */ "ident_list ::= ident_list COMMA IDENTIFIER",
    /*  91 */ "ident_list ::= IDENTIFIER",
    /*  92 */ "ident_list ::=",
    /*  93 */ "func_args ::= ident_list",
    /*  94 */ "func_args ::= ident_list COMMA ELLIPSIS IDENTIFIER",
    /*  95 */ "func_args ::= ELLIPSIS IDENTIFIER",
};
#endif /* NDEBUG */

#if YYSTACKDEPTH <= 0
/*
** Try to increase the size of the parser stack.
*/
static void yyGrowStack(yyParser * p)
{
    int newSize;
    yyStackEntry * pNew;

    newSize = p->yystksz * 2 + 100;
    pNew = ekRealloc(p->yystack, newSize * sizeof(pNew[0]));
    if (pNew) {
        p->yystack = pNew;
        p->yystksz = newSize;
#ifndef NDEBUG
        if (yyTraceFILE) {
            fprintf(yyTraceFILE, "%sStack grows to %d entries!\n",
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
void * ekParseAlloc(struct ekContext * E)
{
    yyParser * pParser;
    pParser = (yyParser *)ekAlloc(sizeof(yyParser) );
    if (pParser) {
        pParser->yyidx = -1;
#ifdef YYTRACKMAXSTACKDEPTH
        pParser->yyidxMax = 0;
#endif
#if YYSTACKDEPTH <= 0
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
    yyParser * yypParser,  /* The parser */
    YYCODETYPE yymajor,    /* Type code for object to destroy */
    YYMINORTYPE * yypminor /* The object to be destroyed */
    )
{
    ekParseARG_FETCH;
    switch (yymajor) {
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
#line 132 "/mnt/f/work/eureka/lib/ekParser.y"
            ekSyntaxDestroy(C->E, (yypminor->yy94));
#line 758 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        default:  break; /* If no destructor action specified: do nothing */
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
static int yy_pop_parser_stack(yyParser * pParser)
{
    YYCODETYPE yymajor;
    yyStackEntry * yytos = &pParser->yystack[pParser->yyidx];

    if (pParser->yyidx < 0) return 0;
#ifndef NDEBUG
    if (yyTraceFILE && ( pParser->yyidx >= 0) ) {
        fprintf(yyTraceFILE, "%sPopping %s\n",
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
void ekParseFree(struct ekContext * E,
                 void * p /* The parser to be deleted */
                 )
{
    yyParser * pParser = (yyParser *)p;
    if (pParser == 0) return;
    while (pParser->yyidx >= 0) yy_pop_parser_stack(pParser);
#if YYSTACKDEPTH <= 0
    ekFree(pParser->yystack);
#endif
    ekFree((void *)pParser);
}

/*
** Return the peak depth of the stack for a parser.
*/
#ifdef YYTRACKMAXSTACKDEPTH
int ekParseStackPeak(void * p)
{
    yyParser * pParser = (yyParser *)p;
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
    yyParser * pParser,   /* The parser */
    YYCODETYPE iLookAhead /* The look-ahead token */
    )
{
    int i;
    int stateno = pParser->yystack[pParser->yyidx].stateno;

    if (( stateno > YY_SHIFT_MAX) || ( (i = yy_shift_ofst[stateno]) == YY_SHIFT_USE_DFLT) ) {
        return yy_default[stateno];
    }
    assert(iLookAhead != YYNOCODE);
    i += iLookAhead;
    if (( i < 0) || ( i >= YY_SZ_ACTTAB) || ( yy_lookahead[i] != iLookAhead) ) {
        if (iLookAhead > 0) {
#ifdef YYFALLBACK
            YYCODETYPE iFallback; /* Fallback token */
            if (( iLookAhead < sizeof(yyFallback) / sizeof(yyFallback[0]))
                && ( (iFallback = yyFallback[iLookAhead]) != 0) )
            {
#ifndef NDEBUG
                if (yyTraceFILE) {
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
                if (( j >= 0) && ( j < YY_SZ_ACTTAB) && ( yy_lookahead[j] == YYWILDCARD) ) {
#ifndef NDEBUG
                    if (yyTraceFILE) {
                        fprintf(yyTraceFILE, "%sWILDCARD %s => %s\n",
                            yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[YYWILDCARD]);
                    }
#endif              /* NDEBUG */
                    return yy_action[j];
                }
            }
#endif      /* YYWILDCARD */
        }
        return yy_default[stateno];
    } else {
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
    int stateno,          /* Current state number */
    YYCODETYPE iLookAhead /* The look-ahead token */
    )
{
    int i;
#ifdef YYERRORSYMBOL
    if (stateno > YY_REDUCE_MAX) {
        return yy_default[stateno];
    }
#else
    assert(stateno <= YY_REDUCE_MAX);
#endif
    i = yy_reduce_ofst[stateno];
    assert(i != YY_REDUCE_USE_DFLT);
    assert(iLookAhead != YYNOCODE);
    i += iLookAhead;
#ifdef YYERRORSYMBOL
    if (( i < 0) || ( i >= YY_SZ_ACTTAB) || ( yy_lookahead[i] != iLookAhead) ) {
        return yy_default[stateno];
    }
#else
    assert(i >= 0 && i < YY_SZ_ACTTAB);
    assert(yy_lookahead[i] == iLookAhead);
#endif
    return yy_action[i];
}

/*
** The following routine is called if the stack overflows.
*/
static void yyStackOverflow(yyParser * yypParser, YYMINORTYPE * yypMinor)
{
    ekParseARG_FETCH;
    yypParser->yyidx--;
#ifndef NDEBUG
    if (yyTraceFILE) {
        fprintf(yyTraceFILE, "%sStack Overflow!\n", yyTracePrompt);
    }
#endif
    while (yypParser->yyidx >= 0) yy_pop_parser_stack(yypParser);
    /* Here code is inserted which will execute if the parser
    ** stack every overflows */
    ekParseARG_STORE; /* Suppress warning about unused %extra_argument var */
}

/*
** Perform a shift action.
*/
static void yy_shift(
    yyParser * yypParser,  /* The parser to be shifted */
    int yyNewState,        /* The new state to shift in */
    int yyMajor,           /* The major token to shift in */
    YYMINORTYPE * yypMinor /* Pointer to the minor token to shift in */
    )
{
    yyStackEntry * yytos;
    yypParser->yyidx++;
#ifdef YYTRACKMAXSTACKDEPTH
    if (yypParser->yyidx > yypParser->yyidxMax) {
        yypParser->yyidxMax = yypParser->yyidx;
    }
#endif
#if YYSTACKDEPTH > 0
    if (yypParser->yyidx >= YYSTACKDEPTH) {
        yyStackOverflow(yypParser, yypMinor);
        return;
    }
#else
    if (yypParser->yyidx >= yypParser->yystksz) {
        yyGrowStack(yypParser);
        if (yypParser->yyidx >= yypParser->yystksz) {
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
    if (yyTraceFILE && ( yypParser->yyidx > 0) ) {
        int i;
        fprintf(yyTraceFILE, "%sShift %d\n", yyTracePrompt, yyNewState);
        fprintf(yyTraceFILE, "%sStack:", yyTracePrompt);
        for (i=1; i <= yypParser->yyidx; i++)
            fprintf(yyTraceFILE, " %s", yyTokenName[yypParser->yystack[i].major]);
        fprintf(yyTraceFILE, "\n");
    }
#endif
}

/* The following table contains information about every rule that
** is used during the reduce.
*/
static const struct
{
    YYCODETYPE lhs;     /* Symbol on the left-hand side of the rule */
    unsigned char nrhs; /* Number of right-hand side symbols in the rule */
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

static void yy_accept(yyParser *); /* Forward Declaration */

/*
** Perform a reduce action and the shift that must immediately
** follow the reduce.
*/
static void yy_reduce(
    yyParser * yypParser, /* The parser */
    int yyruleno          /* Number of the rule by which to reduce */
    )
{
    int yygoto;              /* The next state */
    int yyact;               /* The next action */
    YYMINORTYPE yygotominor; /* The LHS of the rule reduced */
    yyStackEntry * yymsp;    /* The top of the parser's stack */
    int yysize;              /* Amount to pop the stack */
    ekParseARG_FETCH;
    yymsp = &yypParser->yystack[yypParser->yyidx];
#ifndef NDEBUG
    if (yyTraceFILE && ( yyruleno >= 0)
        && (yyruleno < (int)(sizeof(yyRuleName) / sizeof(yyRuleName[0]))))
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

    switch (yyruleno) {
        /* Beginning here are the reduction cases.  A typical example
        ** follows:
        **   case 0:
        **  #line <lineno> <grammarfile>
        **     { ... }           // User supplied code
        **  #line <lineno> <thisfile>
        **     break;
        */
        case 0: /* chunk ::= statement_list */
#line 123 "/mnt/f/work/eureka/lib/ekParser.y"
            { C->root = yymsp[0].minor.yy94; }
#line 1139 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 1: /* statement_list ::= statement_list statement */
#line 135 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxListAppend(C->E, yymsp[-1].minor.yy94, yymsp[0].minor.yy94, 0); }
#line 1144 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 2: /* statement_list ::= statement */
        case 5: /* statement_block ::= statement */ yytestcase(yyruleno == 5);
#line 138 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateList(C->E, EST_STATEMENTLIST, yymsp[0].minor.yy94); }
#line 1150 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 3: /* statement_block ::= STARTBLOCK ENDBLOCK */
        case 30: /* statement ::= ENDSTATEMENT */ yytestcase(yyruleno == 30);
        case 31: /* statement ::= SCOPESTARTBLOCK ENDBLOCK */ yytestcase(yyruleno == 31);
#line 150 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateList(C->E, EST_STATEMENTLIST, NULL); }
#line 1157 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 4: /* statement_block ::= STARTBLOCK statement_list ENDBLOCK */
        case 34: /* paren_expr_list ::= LEFTPAREN expr_list RIGHTPAREN */ yytestcase(yyruleno == 34);
        case 83: /* lvalue ::= GROUPLEFTPAREN expr_list RIGHTPAREN */ yytestcase(yyruleno == 83);
#line 153 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = yymsp[-1].minor.yy94; }
#line 1164 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 6: /* statement ::= BREAK ENDSTATEMENT */
#line 168 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBreak(C->E, yymsp[-1].minor.yy0.line); }
#line 1169 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 7: /* statement ::= BREAK error */
#line 171 "/mnt/f/work/eureka/lib/ekParser.y"
            { ekCompileExplainError(C, "expected ;"); }
#line 1174 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 8: /* statement ::= RETURN ENDSTATEMENT */
#line 174 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateReturn(C->E, yymsp[-1].minor.yy0.line, NULL); }
#line 1179 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 9: /* statement ::= RETURN expr_list ENDSTATEMENT */
        case 11: /* statement ::= RETURN paren_expr_list ENDSTATEMENT */ yytestcase(yyruleno == 11);
#line 177 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateReturn(C->E, yymsp[-1].minor.yy94->line, yymsp[-1].minor.yy94); }
#line 1185 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 10: /* statement ::= RETURN expr_list error */
        case 12: /* statement ::= RETURN paren_expr_list error */ yytestcase(yyruleno == 12);
        case 14: /* statement ::= expr_list error */ yytestcase(yyruleno == 14);
#line 180 "/mnt/f/work/eureka/lib/ekParser.y"
            { ekSyntaxDestroy(C->E, yymsp[-1].minor.yy94); ekCompileExplainError(C, "expected ;"); }
#line 1192 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 13: /* statement ::= expr_list ENDSTATEMENT */
#line 189 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateStatementExpr(C->E, yymsp[-1].minor.yy94); }
#line 1197 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 15: /* statement ::= IF expr_list statement_block ELSE statement_block */
#line 195 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateIfElse(C->E, yymsp[-3].minor.yy94, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1202 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 16: /* statement ::= IF expr_list statement_block */
#line 198 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateIfElse(C->E, yymsp[-1].minor.yy94, yymsp[0].minor.yy94, NULL, ekFalse); }
#line 1207 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 17: /* statement ::= WHILE expr_list statement_block */
#line 201 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateWhile(C->E, yymsp[-1].minor.yy94, yymsp[0].minor.yy94); }
#line 1212 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 18: /* statement ::= FUNCTION IDENTIFIER LEFTPAREN func_args RIGHTPAREN statement_block */
#line 204 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateFunctionDecl(C->E, &yymsp[-4].minor.yy0, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, yymsp[-5].minor.yy0.line); }
#line 1217 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 19: /* statement ::= FOR ident_list IN expression statement_block */
#line 207 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateFor(C->E, yymsp[-3].minor.yy94, yymsp[-1].minor.yy94, yymsp[0].minor.yy94); }
#line 1222 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 20: /* statement ::= FOR ident_list error */
#line 210 "/mnt/f/work/eureka/lib/ekParser.y"
            { ekSyntaxDestroy(C->E, yymsp[-1].minor.yy94); ekCompileExplainError(C, "expected 'in'"); }
#line 1227 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 21: /* statement ::= lvalue PLUSEQUALS expression */
#line 213 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_ADD, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1232 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 22: /* statement ::= lvalue DASHEQUALS expression */
#line 216 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_SUB, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1237 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 23: /* statement ::= lvalue STAREQUALS expression */
#line 219 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_MUL, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1242 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 24: /* statement ::= lvalue SLASHEQUALS expression */
#line 222 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_DIV, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1247 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 25: /* statement ::= lvalue BITWISE_OREQUALS expression */
#line 225 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_BITWISE_OR, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1252 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 26: /* statement ::= lvalue BITWISE_ANDEQUALS expression */
#line 228 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_BITWISE_AND, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1257 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 27: /* statement ::= lvalue BITWISE_XOREQUALS expression */
#line 231 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_BITWISE_XOR, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1262 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 28: /* statement ::= lvalue SHIFTLEFTEQUALS expression */
#line 234 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_SHIFTLEFT, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1267 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 29: /* statement ::= lvalue SHIFTRIGHTEQUALS expression */
#line 237 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_SHIFTRIGHT, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1272 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 32: /* statement ::= SCOPESTARTBLOCK statement_list ENDBLOCK */
#line 246 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateScope(C->E, yymsp[-1].minor.yy94); }
#line 1277 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 35: /* paren_expr_list ::= LEFTPAREN RIGHTPAREN */
#line 263 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateList(C->E, EST_EXPRESSIONLIST, NULL); }
#line 1282 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 36: /* expr_list ::= expr_list COMMA expression */
#line 275 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxListAppend(C->E, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, 0); }
#line 1287 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 37: /* expr_list ::= expr_list FATCOMMA expression */
#line 278 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxListAppend(C->E, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ESLF_AUTOLITERAL); }
#line 1292 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 38: /* expr_list ::= expression */
#line 281 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateList(C->E, EST_EXPRESSIONLIST, yymsp[0].minor.yy94); }
#line 1297 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 39: /* expression ::= NOT expression */
#line 293 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateUnary(C->E, EST_NOT, yymsp[0].minor.yy94, yymsp[0].minor.yy94->line); }
#line 1302 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 40: /* expression ::= BITWISE_NOT expression */
#line 296 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateUnary(C->E, EST_BITWISE_NOT, yymsp[0].minor.yy94, yymsp[0].minor.yy94->line); }
#line 1307 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 41: /* expression ::= expression PLUS expression */
#line 299 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_ADD, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1312 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 42: /* expression ::= expression DASH expression */
#line 302 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_SUB, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1317 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 43: /* expression ::= expression STAR expression */
#line 305 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_MUL, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1322 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 44: /* expression ::= expression SLASH expression */
#line 308 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_DIV, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1327 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 45: /* expression ::= expression AND expression */
#line 311 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_AND, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1332 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 46: /* expression ::= expression OR expression */
#line 314 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_OR, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1337 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 47: /* expression ::= expression CMP expression */
#line 317 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_CMP, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1342 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 48: /* expression ::= expression EQUALS expression */
#line 320 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_EQUALS, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1347 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 49: /* expression ::= expression NOTEQUALS expression */
#line 323 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_NOTEQUALS, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1352 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 50: /* expression ::= expression GREATERTHAN expression */
#line 326 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_GREATERTHAN, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1357 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 51: /* expression ::= expression GREATERTHANOREQUAL expression */
#line 329 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_GREATERTHANOREQUAL, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1362 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 52: /* expression ::= expression LESSTHAN expression */
#line 332 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_LESSTHAN, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1367 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 53: /* expression ::= expression LESSTHANOREQUAL expression */
#line 335 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_LESSTHANOREQUAL, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1372 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 54: /* expression ::= expression BITWISE_XOR expression */
#line 338 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_BITWISE_XOR, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1377 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 55: /* expression ::= expression BITWISE_AND expression */
#line 341 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_BITWISE_AND, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1382 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 56: /* expression ::= expression BITWISE_OR expression */
#line 344 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_BITWISE_OR, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1387 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 57: /* expression ::= expression SHIFTLEFT expression */
#line 347 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_SHIFTLEFT, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1392 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 58: /* expression ::= expression SHIFTRIGHT expression */
#line 350 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_SHIFTRIGHT, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1397 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 59: /* expression ::= expression MOD paren_expr_list */
        case 60: /* expression ::= expression MOD expression */ yytestcase(yyruleno == 60);
#line 353 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateStringFormat(C->E, yymsp[-2].minor.yy94, yymsp[0].minor.yy94); }
#line 1403 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 61: /* expression ::= ARRAYOPENBRACKET CLOSEBRACKET */
#line 359 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateUnary(C->E, EST_ARRAY, NULL, yymsp[-1].minor.yy0.line); }
#line 1408 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 62: /* expression ::= ARRAYOPENBRACKET expr_list CLOSEBRACKET */
#line 362 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateUnary(C->E, EST_ARRAY, yymsp[-1].minor.yy94, yymsp[-1].minor.yy94->line); }
#line 1413 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 63: /* expression ::= MAPSTARTBLOCK ENDBLOCK */
#line 365 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateUnary(C->E, EST_MAP, NULL, yymsp[-1].minor.yy0.line); }
#line 1418 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 64: /* expression ::= MAPSTARTBLOCK expr_list ENDBLOCK */
#line 368 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateUnary(C->E, EST_MAP, yymsp[-1].minor.yy94, yymsp[-1].minor.yy94->line); }
#line 1423 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 65: /* expression ::= lvalue ASSIGN expression */
#line 371 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateAssignment(C->E, yymsp[-2].minor.yy94, yymsp[0].minor.yy94); }
#line 1428 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 66: /* expression ::= lvalue ASSIGN error */
#line 374 "/mnt/f/work/eureka/lib/ekParser.y"
            { ekSyntaxDestroy(C->E, yymsp[-2].minor.yy94); ekCompileExplainError(C, "assignment expected expression"); }
#line 1433 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 67: /* expression ::= expression INHERITS expression */
#line 377 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateInherits(C->E, yymsp[-2].minor.yy94, yymsp[0].minor.yy94); }
#line 1438 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 68: /* expression ::= lvalue */
        case 80: /* lvalue ::= lvalue_indexable */ yytestcase(yyruleno == 80);
#line 380 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = yymsp[0].minor.yy94; }
#line 1444 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 69: /* expression ::= INTEGER */
#line 383 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateKInt(C->E, &yymsp[0].minor.yy0, 0); }
#line 1449 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 70: /* expression ::= NEGATIVE INTEGER */
#line 386 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateKInt(C->E, &yymsp[0].minor.yy0, CKO_NEGATIVE); }
#line 1454 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 71: /* expression ::= FLOATNUM */
#line 389 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateKFloat(C->E, &yymsp[0].minor.yy0, 0); }
#line 1459 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 72: /* expression ::= NEGATIVE FLOATNUM */
#line 392 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateKFloat(C->E, &yymsp[0].minor.yy0, CKO_NEGATIVE); }
#line 1464 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 73: /* expression ::= LITERALSTRING */
#line 395 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateKString(C->E, &yymsp[0].minor.yy0, 0); }
#line 1469 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 74: /* expression ::= REGEXSTRING */
#line 398 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateKString(C->E, &yymsp[0].minor.yy0, 1); }
#line 1474 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 75: /* expression ::= TRUE */
#line 401 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBool(C->E, &yymsp[0].minor.yy0, ekTrue); }
#line 1479 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 76: /* expression ::= FALSE */
#line 404 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBool(C->E, &yymsp[0].minor.yy0, ekFalse); }
#line 1484 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 77: /* expression ::= NULL */
#line 407 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateNull(C->E, yymsp[0].minor.yy0.line); }
#line 1489 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 78: /* expression ::= FUNCTION LEFTPAREN func_args RIGHTPAREN statement_block */
#line 410 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateFunctionDecl(C->E, NULL, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, yymsp[-4].minor.yy0.line); }
#line 1494 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 79: /* expression ::= expression QUESTIONMARK expression COLON expression */
#line 413 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateIfElse(C->E, yymsp[-4].minor.yy94, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1499 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 81: /* lvalue ::= VAR IDENTIFIER */
#line 429 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxMarkVar(C->E, ekSyntaxCreateList(C->E, EST_IDENTIFIERLIST, ekSyntaxCreateIdentifier(C->E, &yymsp[0].minor.yy0))); }
#line 1504 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 82: /* lvalue ::= VAR GROUPLEFTPAREN ident_list RIGHTPAREN */
#line 432 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxMarkVar(C->E, yymsp[-1].minor.yy94); }
#line 1509 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 84: /* lvalue_indexable ::= THIS */
#line 447 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateThis(C->E, yymsp[0].minor.yy0.line); }
#line 1514 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 85: /* lvalue_indexable ::= lvalue_indexable paren_expr_list */
#line 450 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateCall(C->E, yymsp[-1].minor.yy94, yymsp[0].minor.yy94); }
#line 1519 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 86: /* lvalue_indexable ::= lvalue_indexable OPENBRACKET expression CLOSEBRACKET */
#line 453 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateIndex(C->E, yymsp[-3].minor.yy94, yymsp[-1].minor.yy94, ekFalse); }
#line 1524 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 87: /* lvalue_indexable ::= lvalue_indexable PERIOD IDENTIFIER */
#line 456 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateIndex(C->E, yymsp[-2].minor.yy94, ekSyntaxCreateKString(C->E, &yymsp[0].minor.yy0, 0), ekFalse); }
#line 1529 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 88: /* lvalue_indexable ::= lvalue_indexable COLONCOLON IDENTIFIER */
#line 459 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateIndex(C->E, yymsp[-2].minor.yy94, ekSyntaxCreateKString(C->E, &yymsp[0].minor.yy0, 0), ekTrue); }
#line 1534 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 89: /* lvalue_indexable ::= IDENTIFIER */
#line 462 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateIdentifier(C->E, &yymsp[0].minor.yy0); }
#line 1539 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 90: /* ident_list ::= ident_list COMMA IDENTIFIER */
#line 474 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxListAppend(C->E, yymsp[-2].minor.yy94, ekSyntaxCreateIdentifier(C->E, &yymsp[0].minor.yy0), 0); }
#line 1544 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 91: /* ident_list ::= IDENTIFIER */
#line 477 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateList(C->E, EST_IDENTIFIERLIST, ekSyntaxCreateIdentifier(C->E, &yymsp[0].minor.yy0)); }
#line 1549 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 92: /* ident_list ::= */
#line 480 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateList(C->E, EST_IDENTIFIERLIST, NULL); }
#line 1554 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 93: /* func_args ::= ident_list */
#line 489 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateFunctionArgs(C->E, yymsp[0].minor.yy94, NULL); }
#line 1559 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 94: /* func_args ::= ident_list COMMA ELLIPSIS IDENTIFIER */
#line 492 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateFunctionArgs(C->E, yymsp[-3].minor.yy94, &yymsp[0].minor.yy0); }
#line 1564 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        case 95: /* func_args ::= ELLIPSIS IDENTIFIER */
#line 495 "/mnt/f/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateFunctionArgs(C->E, ekSyntaxCreateList(C->E, EST_IDENTIFIERLIST, NULL), &yymsp[0].minor.yy0); }
#line 1569 "/mnt/f/work/eureka/lib/ekParser.c"
            break;
        default:
            /* (33) statement ::= error ENDSTATEMENT */ yytestcase(yyruleno == 33);
            break;
    }
    yygoto = yyRuleInfo[yyruleno].lhs;
    yysize = yyRuleInfo[yyruleno].nrhs;
    yypParser->yyidx -= yysize;
    yyact = yy_find_reduce_action(yymsp[-yysize].stateno, (YYCODETYPE)yygoto);
    if (yyact < YYNSTATE) {
#ifdef NDEBUG
        /* If we are not debugging and the reduce action popped at least
        ** one element off the stack, then we can push the new element back
        ** onto the stack here, and skip the stack overflow test in yy_shift().
        ** That gives a significant speed improvement. */
        if (yysize) {
            yypParser->yyidx++;
            yymsp -= yysize - 1;
            yymsp->stateno = (YYACTIONTYPE)yyact;
            yymsp->major = (YYCODETYPE)yygoto;
            yymsp->minor = yygotominor;
        } else
#endif
        {
            yy_shift(yypParser, yyact, yygoto, &yygotominor);
        }
    } else {
        assert(yyact == YYNSTATE + YYNRULE + 1);
        yy_accept(yypParser);
    }
}

/*
** The following code executes when the parse fails
*/
#ifndef YYNOERRORRECOVERY
static void yy_parse_failed(
    yyParser * yypParser /* The parser */
    )
{
    ekParseARG_FETCH;
#ifndef NDEBUG
    if (yyTraceFILE) {
        fprintf(yyTraceFILE, "%sFail!\n", yyTracePrompt);
    }
#endif
    while (yypParser->yyidx >= 0) yy_pop_parser_stack(yypParser);
    /* Here code is inserted which will be executed whenever the
    ** parser fails */
    ekParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}
#endif /* YYNOERRORRECOVERY */

/*
** The following code executes when a syntax error first occurs.
*/
static void yy_syntax_error(
    yyParser * yypParser, /* The parser */
    int yymajor,          /* The major type of the error token */
    YYMINORTYPE yyminor   /* The minor type of the error token */
    )
{
    ekParseARG_FETCH;
#define TOKEN (yyminor.yy0)
#line 117 "/mnt/f/work/eureka/lib/ekParser.y"
    ekCompileSyntaxError(C, &TOKEN, "Syntax Error");
#line 1634 "/mnt/f/work/eureka/lib/ekParser.c"
    ekParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following is executed when the parser accepts
*/
static void yy_accept(
    yyParser * yypParser /* The parser */
    )
{
    ekParseARG_FETCH;
#ifndef NDEBUG
    if (yyTraceFILE) {
        fprintf(yyTraceFILE, "%sAccept!\n", yyTracePrompt);
    }
#endif
    while (yypParser->yyidx >= 0) yy_pop_parser_stack(yypParser);
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
    void * yyp,              /* The parser */
    int yymajor,             /* The major token code number */
    ekParseTOKENTYPE yyminor /* The value for the token */
    ekParseARG_PDECL         /* Optional %extra_argument parameter */
    )
{
    YYMINORTYPE yyminorunion;
    int yyact;        /* The parser action. */
    int yyendofinput; /* True if we are at the end of input */
#ifdef YYERRORSYMBOL
    int yyerrorhit = 0; /* True if yymajor has invoked an error */
#endif
    yyParser * yypParser; /* The parser */

    /* (re)initialize the parser, if necessary */
    yypParser = (yyParser *)yyp;
    if (yypParser->yyidx < 0) {
#if YYSTACKDEPTH <= 0
        if (yypParser->yystksz <= 0) {
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
    yyendofinput = (yymajor == 0);
    ekParseARG_STORE;

#ifndef NDEBUG
    if (yyTraceFILE) {
        fprintf(yyTraceFILE, "%sInput %s\n", yyTracePrompt, yyTokenName[yymajor]);
    }
#endif

    do {
        yyact = yy_find_shift_action(yypParser, (YYCODETYPE)yymajor);
        if (yyact < YYNSTATE) {
            assert(!yyendofinput); /* Impossible to shift the $ token */
            yy_shift(yypParser, yyact, yymajor, &yyminorunion);
            yypParser->yyerrcnt--;
            yymajor = YYNOCODE;
        } else if (yyact < YYNSTATE + YYNRULE) {
            yy_reduce(yypParser, yyact - YYNSTATE);
        } else {
#ifdef YYERRORSYMBOL
            int yymx;
#endif
            assert(yyact == YY_ERROR_ACTION);
#ifndef NDEBUG
            if (yyTraceFILE) {
                fprintf(yyTraceFILE, "%sSyntax Error!\n", yyTracePrompt);
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
            if (yypParser->yyerrcnt < 0) {
                yy_syntax_error(yypParser, yymajor, yyminorunion);
            }
            yymx = yypParser->yystack[yypParser->yyidx].major;
            if (( yymx == YYERRORSYMBOL) || yyerrorhit) {
#ifndef NDEBUG
                if (yyTraceFILE) {
                    fprintf(yyTraceFILE, "%sDiscard input token %s\n",
                        yyTracePrompt, yyTokenName[yymajor]);
                }
#endif
                yy_destructor(yypParser, (YYCODETYPE)yymajor, &yyminorunion);
                yymajor = YYNOCODE;
            } else {
                while (
                    yypParser->yyidx >= 0 &&
                    yymx != YYERRORSYMBOL &&
                    (yyact = yy_find_reduce_action(
                         yypParser->yystack[yypParser->yyidx].stateno,
                         YYERRORSYMBOL)) >= YYNSTATE
                    )
                {
                    yy_pop_parser_stack(yypParser);
                }
                if (( yypParser->yyidx < 0) || ( yymajor == 0) ) {
                    yy_destructor(yypParser, (YYCODETYPE)yymajor, &yyminorunion);
                    yy_parse_failed(yypParser);
                    yymajor = YYNOCODE;
                } else if (yymx != YYERRORSYMBOL) {
                    YYMINORTYPE u2;
                    u2.YYERRSYMDT = 0;
                    yy_shift(yypParser, yyact, YYERRORSYMBOL, &u2);
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
            yy_syntax_error(yypParser, yymajor, yyminorunion);
            yy_destructor(yypParser, (YYCODETYPE)yymajor, &yyminorunion);
            yymajor = YYNOCODE;

#else       /* YYERRORSYMBOL is not defined */
      /* This is what we do if the grammar does not define ERROR:
      **
      **  * Report an error message, and throw away the input token.
      **
      **  * If the input token is $, then fail the parse.
      **
      ** As before, subsequent error messages are suppressed until
      ** three input tokens have been successfully shifted.
      */
            if (yypParser->yyerrcnt <= 0) {
                yy_syntax_error(yypParser, yymajor, yyminorunion);
            }
            yypParser->yyerrcnt = 3;
            yy_destructor(yypParser, (YYCODETYPE)yymajor, &yyminorunion);
            if (yyendofinput) {
                yy_parse_failed(yypParser);
            }
            yymajor = YYNOCODE;
#endif      /* ifdef YYERRORSYMBOL */
        }
    } while (yymajor != YYNOCODE && yypParser->yyidx >= 0);
    return;
}
