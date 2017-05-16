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
    /*     0 */ 18, 154, 7, 4, 13, 107, 25, 16, 20, 22,
    /*    10 */ 42, 25, 44, 140, 17, 27, 28, 15, 65, 135,
    /*    20 */ 114, 52, 54, 53, 56, 55, 47, 51, 49, 50,
    /*    30 */ 88, 113, 59, 30, 21, 38, 29, 39, 26, 24,
    /*    40 */ 32, 33, 31, 36, 37, 34, 35, 43, 41, 40,
    /*    50 */ 23, 27, 28, 15, 40, 23, 27, 28, 15, 57,
    /*    60 */ 157, 12, 19, 110, 136, 15, 66, 131, 25, 112,
    /*    70 */ 173, 169, 167, 165, 163, 161, 158, 111, 18, 5,
    /*    80 */ 7, 4, 13, 107, 125, 16, 9, 166, 57, 60,
    /*    90 */ 25, 129, 17, 69, 95, 59, 65, 135, 29, 39,
    /*   100 */ 22, 42, 32, 33, 31, 36, 37, 34, 35, 43,
    /*   110 */ 41, 40, 23, 27, 28, 15, 26, 24, 21, 38,
    /*   120 */ 29, 39, 120, 25, 32, 33, 31, 36, 37, 34,
    /*   130 */ 35, 43, 41, 40, 23, 27, 28, 15, 157, 12,
    /*   140 */ 19, 110, 136, 25, 66, 164, 22, 42, 173, 169,
    /*   150 */ 167, 165, 163, 161, 158, 111, 182, 18, 64, 7,
    /*   160 */ 128, 13, 107, 122, 16, 41, 40, 23, 27, 28,
    /*   170 */ 15, 17, 38, 29, 39, 65, 135, 32, 33, 31,
    /*   180 */ 36, 37, 34, 35, 43, 41, 40, 23, 27, 28,
    /*   190 */ 15, 100, 113, 59, 62, 26, 24, 18, 174, 7,
    /*   200 */ 142, 13, 107, 125, 16, 11, 166, 25, 60, 46,
    /*   210 */ 122, 17, 69, 95, 59, 65, 135, 157, 12, 19,
    /*   220 */ 110, 136, 180, 66, 28, 15, 152, 173, 169, 167,
    /*   230 */ 165, 163, 161, 158, 111, 26, 24, 131, 39, 171,
    /*   240 */ 170, 32, 33, 31, 36, 37, 34, 35, 43, 41,
    /*   250 */ 40, 23, 27, 28, 15, 175, 156, 157, 12, 19,
    /*   260 */ 110, 136, 25, 66, 70, 113, 59, 173, 169, 167,
    /*   270 */ 165, 163, 161, 158, 111, 18, 20, 7, 4, 13,
    /*   280 */ 107, 125, 16, 109, 151, 144, 60, 133, 132, 17,
    /*   290 */ 69, 95, 59, 65, 135, 6, 32, 33, 31, 36,
    /*   300 */ 37, 34, 35, 43, 41, 40, 23, 27, 28, 15,
    /*   310 */ 25, 117, 126, 26, 24, 18, 179, 7, 155, 13,
    /*   320 */ 107, 125, 16, 25, 151, 119, 60, 121, 126, 17,
    /*   330 */ 69, 95, 59, 65, 135, 157, 12, 19, 110, 136,
    /*   340 */ 58, 66, 99, 113, 59, 173, 169, 167, 165, 163,
    /*   350 */ 161, 158, 111, 26, 24, 27, 28, 15, 33, 31,
    /*   360 */ 36, 37, 34, 35, 43, 41, 40, 23, 27, 28,
    /*   370 */ 15, 71, 113, 59, 168, 157, 12, 19, 110, 136,
    /*   380 */ 153, 66, 25, 115, 8, 173, 169, 167, 165, 163,
    /*   390 */ 161, 158, 111, 18, 148, 7, 25, 13, 107, 150,
    /*   400 */ 16, 130, 22, 42, 25, 125, 172, 17, 151, 177,
    /*   410 */ 60, 65, 135, 138, 69, 95, 59, 280, 31, 36,
    /*   420 */ 37, 34, 35, 43, 41, 40, 23, 27, 28, 15,
    /*   430 */ 130, 26, 24, 36, 37, 34, 35, 43, 41, 40,
    /*   440 */ 23, 27, 28, 15, 35, 43, 41, 40, 23, 27,
    /*   450 */ 28, 15, 160, 157, 12, 19, 110, 136, 280, 66,
    /*   460 */ 25, 280, 280, 173, 169, 167, 165, 163, 161, 158,
    /*   470 */ 111, 18, 25, 7, 280, 13, 107, 125, 16, 25,
    /*   480 */ 151, 181, 60, 280, 280, 17, 69, 95, 59, 65,
    /*   490 */ 135, 145, 280, 280, 143, 113, 59, 280, 37, 34,
    /*   500 */ 35, 43, 41, 40, 23, 27, 28, 15, 280, 26,
    /*   510 */ 24, 34, 35, 43, 41, 40, 23, 27, 28, 15,
    /*   520 */ 43, 41, 40, 23, 27, 28, 15, 98, 113, 59,
    /*   530 */ 127, 157, 12, 19, 110, 136, 280, 66, 68, 113,
    /*   540 */ 59, 173, 169, 167, 165, 163, 161, 158, 111, 18,
    /*   550 */ 280, 7, 280, 13, 107, 2, 16, 280, 280, 69,
    /*   560 */ 113, 59, 280, 17, 14, 18, 280, 65, 135, 13,
    /*   570 */ 107, 48, 16, 61, 63, 280, 280, 69, 113, 59,
    /*   580 */ 125, 279, 10, 166, 135, 60, 280, 26, 24, 69,
    /*   590 */ 95, 59, 75, 113, 59, 280, 125, 280, 280, 151,
    /*   600 */ 147, 60, 280, 26, 24, 69, 95, 59, 280, 157,
    /*   610 */ 12, 19, 110, 136, 280, 66, 280, 123, 124, 173,
    /*   620 */ 169, 167, 165, 163, 161, 158, 111, 25, 116, 136,
    /*   630 */ 141, 280, 280, 44, 45, 173, 169, 167, 165, 163,
    /*   640 */ 161, 158, 111, 280, 106, 280, 280, 25, 69, 113,
    /*   650 */ 59, 280, 280, 280, 30, 21, 38, 29, 39, 280,
    /*   660 */ 280, 32, 33, 31, 36, 37, 34, 35, 43, 41,
    /*   670 */ 40, 23, 27, 28, 15, 25, 1, 113, 59, 125,
    /*   680 */ 280, 44, 151, 149, 60, 74, 113, 59, 69, 95,
    /*   690 */ 59, 23, 27, 28, 15, 103, 67, 113, 59, 69,
    /*   700 */ 113, 59, 30, 21, 38, 29, 39, 280, 280, 32,
    /*   710 */ 33, 31, 36, 37, 34, 35, 43, 41, 40, 23,
    /*   720 */ 27, 28, 15, 25, 280, 162, 105, 178, 280, 44,
    /*   730 */ 69, 113, 59, 280, 125, 22, 42, 159, 280, 60,
    /*   740 */ 280, 280, 3, 69, 95, 59, 69, 113, 59, 280,
    /*   750 */ 30, 21, 38, 29, 39, 280, 280, 32, 33, 31,
    /*   760 */ 36, 37, 34, 35, 43, 41, 40, 23, 27, 28,
    /*   770 */ 15, 25, 280, 104, 76, 113, 59, 69, 113, 59,
    /*   780 */ 82, 113, 59, 81, 113, 59, 80, 113, 59, 79,
    /*   790 */ 113, 59, 78, 113, 59, 77, 113, 59, 30, 21,
    /*   800 */ 38, 29, 39, 280, 280, 32, 33, 31, 36, 37,
    /*   810 */ 34, 35, 43, 41, 40, 23, 27, 28, 15, 18,
    /*   820 */ 14, 280, 280, 13, 107, 280, 16, 280, 73, 113,
    /*   830 */ 59, 89, 113, 59, 102, 113, 59, 280, 135, 87,
    /*   840 */ 113, 59, 101, 113, 59, 280, 72, 113, 59, 84,
    /*   850 */ 113, 59, 134, 280, 280, 22, 42, 26, 24, 18,
    /*   860 */ 14, 280, 280, 13, 107, 139, 16, 280, 22, 42,
    /*   870 */ 108, 113, 59, 118, 113, 59, 280, 280, 135, 146,
    /*   880 */ 18, 280, 116, 136, 13, 107, 280, 16, 280, 173,
    /*   890 */ 169, 167, 165, 163, 161, 158, 111, 26, 24, 135,
    /*   900 */ 86, 113, 59, 83, 113, 59, 280, 85, 113, 59,
    /*   910 */ 280, 280, 92, 113, 59, 90, 113, 59, 26, 24,
    /*   920 */ 280, 18, 116, 136, 280, 13, 107, 280, 16, 173,
    /*   930 */ 169, 167, 165, 163, 161, 158, 111, 280, 280, 176,
    /*   940 */ 135, 280, 18, 116, 136, 280, 13, 107, 280, 16,
    /*   950 */ 173, 169, 167, 165, 163, 161, 158, 111, 280, 26,
    /*   960 */ 24, 135, 91, 113, 59, 280, 280, 280, 96, 113,
    /*   970 */ 59, 97, 113, 59, 93, 113, 59, 280, 280, 137,
    /*   980 */ 26, 24, 280, 280, 116, 136, 94, 113, 59, 280,
    /*   990 */ 280, 173, 169, 167, 165, 163, 161, 158, 111, 280,
    /*  1000 */ 280, 280, 280, 280, 280, 116, 136, 280, 280, 280,
    /*  1010 */ 280, 280, 173, 169, 167, 165, 163, 161, 158, 111,
};
static const YYCODETYPE yy_lookahead[] = {
    /*     0 */ 1, 61, 3, 4, 5, 6, 7, 8, 12, 69,
    /*    10 */ 70, 7, 13, 65, 15, 52, 53, 54, 19, 20,
    /*    20 */ 18, 25, 26, 27, 28, 29, 30, 31, 32, 33,
    /*    30 */ 88, 89, 90, 34, 35, 36, 37, 38, 39, 40,
    /*    40 */ 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
    /*    50 */ 51, 52, 53, 54, 50, 51, 52, 53, 54, 2,
    /*    60 */ 61, 62, 63, 64, 65, 54, 67, 65, 7, 87,
    /*    70 */ 71, 72, 73, 74, 75, 76, 77, 78, 1, 66,
    /*    80 */ 3, 4, 5, 6, 79, 8, 81, 82, 2, 84,
    /*    90 */ 7, 60, 15, 88, 89, 90, 19, 20, 37, 38,
    /*   100 */ 69, 70, 41, 42, 43, 44, 45, 46, 47, 48,
    /*   110 */ 49, 50, 51, 52, 53, 54, 39, 40, 35, 36,
    /*   120 */ 37, 38, 65, 7, 41, 42, 43, 44, 45, 46,
    /*   130 */ 47, 48, 49, 50, 51, 52, 53, 54, 61, 62,
    /*   140 */ 63, 64, 65, 7, 67, 61, 69, 70, 71, 72,
    /*   150 */ 73, 74, 75, 76, 77, 78, 0, 1, 1, 3,
    /*   160 */ 66, 5, 6, 69, 8, 49, 50, 51, 52, 53,
    /*   170 */ 54, 15, 36, 37, 38, 19, 20, 41, 42, 43,
    /*   180 */ 44, 45, 46, 47, 48, 49, 50, 51, 52, 53,
    /*   190 */ 54, 88, 89, 90, 87, 39, 40, 1, 65, 3,
    /*   200 */ 65, 5, 6, 79, 8, 81, 82, 7, 84, 68,
    /*   210 */ 69, 15, 88, 89, 90, 19, 20, 61, 62, 63,
    /*   220 */ 64, 65, 65, 67, 53, 54, 79, 71, 72, 73,
    /*   230 */ 74, 75, 76, 77, 78, 39, 40, 65, 38, 71,
    /*   240 */ 72, 41, 42, 43, 44, 45, 46, 47, 48, 49,
    /*   250 */ 50, 51, 52, 53, 54, 79, 60, 61, 62, 63,
    /*   260 */ 64, 65, 7, 67, 88, 89, 90, 71, 72, 73,
    /*   270 */ 74, 75, 76, 77, 78, 1, 12, 3, 4, 5,
    /*   280 */ 6, 79, 8, 69, 82, 83, 84, 79, 61, 15,
    /*   290 */ 88, 89, 90, 19, 20, 16, 41, 42, 43, 44,
    /*   300 */ 45, 46, 47, 48, 49, 50, 51, 52, 53, 54,
    /*   310 */ 7, 86, 87, 39, 40, 1, 85, 3, 79, 5,
    /*   320 */ 6, 79, 8, 7, 82, 83, 84, 86, 87, 15,
    /*   330 */ 88, 89, 90, 19, 20, 61, 62, 63, 64, 65,
    /*   340 */ 2, 67, 88, 89, 90, 71, 72, 73, 74, 75,
    /*   350 */ 76, 77, 78, 39, 40, 52, 53, 54, 42, 43,
    /*   360 */ 44, 45, 46, 47, 48, 49, 50, 51, 52, 53,
    /*   370 */ 54, 88, 89, 90, 60, 61, 62, 63, 64, 65,
    /*   380 */ 61, 67, 7, 18, 66, 71, 72, 73, 74, 75,
    /*   390 */ 76, 77, 78, 1, 61, 3, 7, 5, 6, 79,
    /*   400 */ 8, 65, 69, 70, 7, 79, 79, 15, 82, 83,
    /*   410 */ 84, 19, 20, 65, 88, 89, 90, 91, 43, 44,
    /*   420 */ 45, 46, 47, 48, 49, 50, 51, 52, 53, 54,
    /*   430 */ 65, 39, 40, 44, 45, 46, 47, 48, 49, 50,
    /*   440 */ 51, 52, 53, 54, 47, 48, 49, 50, 51, 52,
    /*   450 */ 53, 54, 60, 61, 62, 63, 64, 65, 91, 67,
    /*   460 */ 7, 91, 91, 71, 72, 73, 74, 75, 76, 77,
    /*   470 */ 78, 1, 7, 3, 91, 5, 6, 79, 8, 7,
    /*   480 */ 82, 83, 84, 91, 91, 15, 88, 89, 90, 19,
    /*   490 */ 20, 85, 91, 91, 88, 89, 90, 91, 45, 46,
    /*   500 */ 47, 48, 49, 50, 51, 52, 53, 54, 91, 39,
    /*   510 */ 40, 46, 47, 48, 49, 50, 51, 52, 53, 54,
    /*   520 */ 48, 49, 50, 51, 52, 53, 54, 88, 89, 90,
    /*   530 */ 60, 61, 62, 63, 64, 65, 91, 67, 88, 89,
    /*   540 */ 90, 71, 72, 73, 74, 75, 76, 77, 78, 1,
    /*   550 */ 91, 3, 91, 5, 6, 84, 8, 91, 91, 88,
    /*   560 */ 89, 90, 91, 15, 2, 1, 91, 19, 20, 5,
    /*   570 */ 6, 9, 8, 84, 85, 91, 91, 88, 89, 90,
    /*   580 */ 79, 80, 81, 82, 20, 84, 91, 39, 40, 88,
    /*   590 */ 89, 90, 88, 89, 90, 91, 79, 91, 91, 82,
    /*   600 */ 83, 84, 91, 39, 40, 88, 89, 90, 91, 61,
    /*   610 */ 62, 63, 64, 65, 91, 67, 91, 55, 56, 71,
    /*   620 */ 72, 73, 74, 75, 76, 77, 78, 7, 64, 65,
    /*   630 */ 66, 91, 91, 13, 14, 71, 72, 73, 74, 75,
    /*   640 */ 76, 77, 78, 91, 84, 91, 91, 7, 88, 89,
    /*   650 */ 90, 91, 91, 91, 34, 35, 36, 37, 38, 91,
    /*   660 */ 91, 41, 42, 43, 44, 45, 46, 47, 48, 49,
    /*   670 */ 50, 51, 52, 53, 54, 7, 88, 89, 90, 79,
    /*   680 */ 91, 13, 82, 83, 84, 88, 89, 90, 88, 89,
    /*   690 */ 90, 51, 52, 53, 54, 84, 88, 89, 90, 88,
    /*   700 */ 89, 90, 34, 35, 36, 37, 38, 91, 91, 41,
    /*   710 */ 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,
    /*   720 */ 52, 53, 54, 7, 91, 59, 84, 59, 91, 13,
    /*   730 */ 88, 89, 90, 91, 79, 69, 70, 82, 91, 84,
    /*   740 */ 91, 91, 84, 88, 89, 90, 88, 89, 90, 91,
    /*   750 */ 34, 35, 36, 37, 38, 91, 91, 41, 42, 43,
    /*   760 */ 44, 45, 46, 47, 48, 49, 50, 51, 52, 53,
    /*   770 */ 54, 7, 91, 84, 88, 89, 90, 88, 89, 90,
    /*   780 */ 88, 89, 90, 88, 89, 90, 88, 89, 90, 88,
    /*   790 */ 89, 90, 88, 89, 90, 88, 89, 90, 34, 35,
    /*   800 */ 36, 37, 38, 91, 91, 41, 42, 43, 44, 45,
    /*   810 */ 46, 47, 48, 49, 50, 51, 52, 53, 54, 1,
    /*   820 */ 2, 91, 91, 5, 6, 91, 8, 91, 88, 89,
    /*   830 */ 90, 88, 89, 90, 88, 89, 90, 91, 20, 88,
    /*   840 */ 89, 90, 88, 89, 90, 91, 88, 89, 90, 88,
    /*   850 */ 89, 90, 66, 91, 91, 69, 70, 39, 40, 1,
    /*   860 */ 2, 91, 91, 5, 6, 66, 8, 91, 69, 70,
    /*   870 */ 88, 89, 90, 88, 89, 90, 91, 91, 20, 61,
    /*   880 */ 1, 91, 64, 65, 5, 6, 91, 8, 91, 71,
    /*   890 */ 72, 73, 74, 75, 76, 77, 78, 39, 40, 20,
    /*   900 */ 88, 89, 90, 88, 89, 90, 91, 88, 89, 90,
    /*   910 */ 91, 91, 88, 89, 90, 88, 89, 90, 39, 40,
    /*   920 */ 91, 1, 64, 65, 91, 5, 6, 91, 8, 71,
    /*   930 */ 72, 73, 74, 75, 76, 77, 78, 91, 91, 60,
    /*   940 */ 20, 91, 1, 64, 65, 91, 5, 6, 91, 8,
    /*   950 */ 71, 72, 73, 74, 75, 76, 77, 78, 91, 39,
    /*   960 */ 40, 20, 88, 89, 90, 91, 91, 91, 88, 89,
    /*   970 */ 90, 88, 89, 90, 88, 89, 90, 91, 91, 59,
    /*   980 */ 39, 40, 91, 91, 64, 65, 88, 89, 90, 91,
    /*   990 */ 91, 71, 72, 73, 74, 75, 76, 77, 78, 91,
    /*  1000 */ 91, 91, 91, 91, 91, 64, 65, 91, 91, 91,
    /*  1010 */ 91, 91, 71, 72, 73, 74, 75, 76, 77, 78,
};
#define YY_SHIFT_USE_DFLT (-61)
#define YY_SHIFT_MAX 126
static const short yy_shift_ofst[] = {
    /*     0 */ 548, -1, 77, 77, 470, 274, 274, 314, 274, 196,
    /*    10 */ 156, 392, 818, 879, 564, 858, 920, 941, 941, 941,
    /*    20 */ 941, 941, 941, 941, 941, 941, 941, 941, 941, 941,
    /*    30 */ 941, 941, 941, 941, 941, 941, 941, 941, 941, 941,
    /*    40 */ 941, 941, 941, 941, 941, 941, 941, 941, 941, 941,
    /*    50 */ 941, 941, 941, 941, 941, 941, 941, 2, 2, 562,
    /*    60 */ -60, 333, 141, 319, 172, 227, 172, 668, 620, 716,
    /*    70 */ 716, 716, 716, 764, 764, 764, 764, 764, 764, 764,
    /*    80 */ 764, 764, 764, 83, 136, 61, 200, 255, 255, 255,
    /*    90 */ 316, 375, 389, 453, 465, -4, 397, 472, 116, 4,
    /*   100 */ 640, 303, -37, 31, 799, 666, 786, 168, 171, 365,
    /*   110 */ 57, 157, 94, 264, 135, 133, 86, 13, 11, 279,
    /*   120 */ 338, 318, 336, 348, -52, 84, 214,
};
#define YY_REDUCE_USE_DFLT (-59)
#define YY_REDUCE_MAX 66
static const short yy_reduce_ofst[] = {
    /*     0 */ 501, 398, 517, 242, 5, 326, 600, 124, 202, 655,
    /*    10 */ 655, 655, 489, 611, 689, 406, 642, 658, 560, 471,
    /*    20 */ 176, 761, 758, 754, 751, 746, 743, 782, 785, 812,
    /*    30 */ 815, 824, 827, 874, 880, 883, 886, 898, 819, -58,
    /*    40 */ 103, 254, 283, 439, 450, 504, 588, 597, 608, 686,
    /*    50 */ 692, 695, 698, 701, 704, 707, 740, 225, 241, 231,
    /*    60 */ 147, 320, 327, 239, -18, 208, 107,
};
static const YYACTIONTYPE yy_default[] = {
    /*     0 */ 278, 278, 278, 278, 278, 278, 278, 278, 278, 278,
    /*    10 */ 278, 278, 278, 278, 278, 278, 278, 278, 278, 278,
    /*    20 */ 278, 278, 278, 278, 278, 278, 278, 278, 278, 278,
    /*    30 */ 278, 278, 278, 278, 278, 278, 278, 278, 278, 278,
    /*    40 */ 278, 278, 278, 278, 278, 278, 278, 278, 278, 278,
    /*    50 */ 278, 278, 278, 278, 278, 278, 278, 274, 274, 262,
    /*    60 */ 278, 278, 278, 278, 274, 278, 274, 278, 278, 220,
    /*    70 */ 247, 219, 218, 211, 203, 261, 204, 210, 209, 208,
    /*    80 */ 207, 206, 205, 228, 238, 236, 227, 221, 237, 222,
    /*    90 */ 230, 231, 229, 234, 235, 250, 232, 233, 249, 240,
    /*   100 */ 239, 223, 224, 278, 278, 278, 278, 278, 225, 278,
    /*   110 */ 278, 278, 278, 250, 278, 278, 278, 278, 226, 198,
    /*   120 */ 278, 278, 278, 278, 278, 278, 275, 185, 264, 246,
    /*   130 */ 272, 273, 188, 189, 265, 266, 271, 243, 269, 216,
    /*   140 */ 270, 217, 277, 242, 200, 241, 190, 199, 191, 197,
    /*   150 */ 192, 187, 196, 193, 195, 194, 186, 212, 259, 183,
    /*   160 */ 214, 258, 244, 257, 215, 256, 184, 255, 213, 253,
    /*   170 */ 254, 252, 202, 251, 276, 248, 245, 260, 268, 267,
    /*   180 */ 263, 201,
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
    int yyidx;                  /* Index of top element in stack */
#ifdef YYTRACKMAXSTACKDEPTH
    int yyidxMax;               /* Maximum value of yyidx */
#endif
    int yyerrcnt;               /* Shifts left before out of the error */
    ekParseARG_SDECL            /* A place to hold %extra_argument */
#if YYSTACKDEPTH <= 0
    int yystksz;                /* Current side of the stack */
    yyStackEntry * yystack;     /* The parser's stack */
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
#line 132 "/Users/jdrago/work/eureka/lib/ekParser.y"
            ekSyntaxDestroy(C->E, (yypminor->yy94));
#line 742 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        default:  break;/* If no destructor action specified: do nothing */
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
                 void * p    /* The parser to be deleted */
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
    yyParser * pParser,     /* The parser */
    YYCODETYPE iLookAhead   /* The look-ahead token */
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
            YYCODETYPE iFallback;      /* Fallback token */
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
    int stateno,            /* Current state number */
    YYCODETYPE iLookAhead   /* The look-ahead token */
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
    yyParser * yypParser,       /* The parser to be shifted */
    int yyNewState,             /* The new state to shift in */
    int yyMajor,                /* The major token to shift in */
    YYMINORTYPE * yypMinor      /* Pointer to the minor token to shift in */
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
    YYCODETYPE lhs;       /* Symbol on the left-hand side of the rule */
    unsigned char nrhs;   /* Number of right-hand side symbols in the rule */
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
    yyParser * yypParser,      /* The parser */
    int yyruleno               /* Number of the rule by which to reduce */
    )
{
    int yygoto;                   /* The next state */
    int yyact;                    /* The next action */
    YYMINORTYPE yygotominor;      /* The LHS of the rule reduced */
    yyStackEntry * yymsp;         /* The top of the parser's stack */
    int yysize;                   /* Amount to pop the stack */
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
#line 123 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { C->root = yymsp[0].minor.yy94; }
#line 1123 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 1: /* statement_list ::= statement_list statement */
#line 135 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxListAppend(C->E, yymsp[-1].minor.yy94, yymsp[0].minor.yy94, 0); }
#line 1128 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 2: /* statement_list ::= statement */
        case 5: /* statement_block ::= statement */ yytestcase(yyruleno == 5);
#line 138 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateList(C->E, EST_STATEMENTLIST, yymsp[0].minor.yy94); }
#line 1134 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 3: /* statement_block ::= STARTBLOCK ENDBLOCK */
        case 30: /* statement ::= ENDSTATEMENT */ yytestcase(yyruleno == 30);
        case 31: /* statement ::= SCOPESTARTBLOCK ENDBLOCK */ yytestcase(yyruleno == 31);
#line 150 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateList(C->E, EST_STATEMENTLIST, NULL); }
#line 1141 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 4: /* statement_block ::= STARTBLOCK statement_list ENDBLOCK */
        case 34: /* paren_expr_list ::= LEFTPAREN expr_list RIGHTPAREN */ yytestcase(yyruleno == 34);
        case 83: /* lvalue ::= GROUPLEFTPAREN expr_list RIGHTPAREN */ yytestcase(yyruleno == 83);
#line 153 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = yymsp[-1].minor.yy94; }
#line 1148 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 6: /* statement ::= BREAK ENDSTATEMENT */
#line 168 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBreak(C->E, yymsp[-1].minor.yy0.line); }
#line 1153 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 7: /* statement ::= BREAK error */
        case 10: /* statement ::= RETURN expr_list error */ yytestcase(yyruleno == 10);
        case 12: /* statement ::= RETURN paren_expr_list error */ yytestcase(yyruleno == 12);
        case 14: /* statement ::= expr_list error */ yytestcase(yyruleno == 14);
#line 171 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { ekCompileExplainError(C, "expected ;"); }
#line 1161 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 8: /* statement ::= RETURN ENDSTATEMENT */
#line 174 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateReturn(C->E, yymsp[-1].minor.yy0.line, NULL); }
#line 1166 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 9: /* statement ::= RETURN expr_list ENDSTATEMENT */
        case 11: /* statement ::= RETURN paren_expr_list ENDSTATEMENT */ yytestcase(yyruleno == 11);
#line 177 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateReturn(C->E, yymsp[-1].minor.yy94->line, yymsp[-1].minor.yy94); }
#line 1172 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 13: /* statement ::= expr_list ENDSTATEMENT */
#line 189 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateStatementExpr(C->E, yymsp[-1].minor.yy94); }
#line 1177 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 15: /* statement ::= IF expr_list statement_block ELSE statement_block */
#line 195 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateIfElse(C->E, yymsp[-3].minor.yy94, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1182 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 16: /* statement ::= IF expr_list statement_block */
#line 198 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateIfElse(C->E, yymsp[-1].minor.yy94, yymsp[0].minor.yy94, NULL, ekFalse); }
#line 1187 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 17: /* statement ::= WHILE expr_list statement_block */
#line 201 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateWhile(C->E, yymsp[-1].minor.yy94, yymsp[0].minor.yy94); }
#line 1192 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 18: /* statement ::= FUNCTION IDENTIFIER LEFTPAREN func_args RIGHTPAREN statement_block */
#line 204 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateFunctionDecl(C->E, &yymsp[-4].minor.yy0, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, yymsp[-5].minor.yy0.line); }
#line 1197 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 19: /* statement ::= FOR ident_list IN expression statement_block */
#line 207 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateFor(C->E, yymsp[-3].minor.yy94, yymsp[-1].minor.yy94, yymsp[0].minor.yy94); }
#line 1202 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 20: /* statement ::= FOR ident_list error */
#line 210 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { ekCompileExplainError(C, "expected 'in'"); }
#line 1207 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 21: /* statement ::= lvalue PLUSEQUALS expression */
#line 213 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_ADD, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1212 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 22: /* statement ::= lvalue DASHEQUALS expression */
#line 216 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_SUB, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1217 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 23: /* statement ::= lvalue STAREQUALS expression */
#line 219 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_MUL, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1222 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 24: /* statement ::= lvalue SLASHEQUALS expression */
#line 222 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_DIV, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1227 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 25: /* statement ::= lvalue BITWISE_OREQUALS expression */
#line 225 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_BITWISE_OR, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1232 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 26: /* statement ::= lvalue BITWISE_ANDEQUALS expression */
#line 228 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_BITWISE_AND, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1237 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 27: /* statement ::= lvalue BITWISE_XOREQUALS expression */
#line 231 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_BITWISE_XOR, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1242 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 28: /* statement ::= lvalue SHIFTLEFTEQUALS expression */
#line 234 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_SHIFTLEFT, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1247 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 29: /* statement ::= lvalue SHIFTRIGHTEQUALS expression */
#line 237 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_SHIFTRIGHT, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1252 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 32: /* statement ::= SCOPESTARTBLOCK statement_list ENDBLOCK */
#line 246 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateScope(C->E, yymsp[-1].minor.yy94); }
#line 1257 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 35: /* paren_expr_list ::= LEFTPAREN RIGHTPAREN */
#line 263 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateList(C->E, EST_EXPRESSIONLIST, NULL); }
#line 1262 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 36: /* expr_list ::= expr_list COMMA expression */
#line 275 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxListAppend(C->E, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, 0); }
#line 1267 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 37: /* expr_list ::= expr_list FATCOMMA expression */
#line 278 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxListAppend(C->E, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ESLF_AUTOLITERAL); }
#line 1272 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 38: /* expr_list ::= expression */
#line 281 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateList(C->E, EST_EXPRESSIONLIST, yymsp[0].minor.yy94); }
#line 1277 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 39: /* expression ::= NOT expression */
#line 293 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateUnary(C->E, EST_NOT, yymsp[0].minor.yy94, yymsp[0].minor.yy94->line); }
#line 1282 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 40: /* expression ::= BITWISE_NOT expression */
#line 296 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateUnary(C->E, EST_BITWISE_NOT, yymsp[0].minor.yy94, yymsp[0].minor.yy94->line); }
#line 1287 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 41: /* expression ::= expression PLUS expression */
#line 299 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_ADD, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1292 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 42: /* expression ::= expression DASH expression */
#line 302 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_SUB, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1297 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 43: /* expression ::= expression STAR expression */
#line 305 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_MUL, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1302 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 44: /* expression ::= expression SLASH expression */
#line 308 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_DIV, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1307 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 45: /* expression ::= expression AND expression */
#line 311 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_AND, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1312 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 46: /* expression ::= expression OR expression */
#line 314 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_OR, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1317 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 47: /* expression ::= expression CMP expression */
#line 317 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_CMP, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1322 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 48: /* expression ::= expression EQUALS expression */
#line 320 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_EQUALS, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1327 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 49: /* expression ::= expression NOTEQUALS expression */
#line 323 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_NOTEQUALS, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1332 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 50: /* expression ::= expression GREATERTHAN expression */
#line 326 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_GREATERTHAN, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1337 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 51: /* expression ::= expression GREATERTHANOREQUAL expression */
#line 329 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_GREATERTHANOREQUAL, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1342 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 52: /* expression ::= expression LESSTHAN expression */
#line 332 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_LESSTHAN, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1347 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 53: /* expression ::= expression LESSTHANOREQUAL expression */
#line 335 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_LESSTHANOREQUAL, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1352 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 54: /* expression ::= expression BITWISE_XOR expression */
#line 338 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_BITWISE_XOR, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1357 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 55: /* expression ::= expression BITWISE_AND expression */
#line 341 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_BITWISE_AND, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1362 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 56: /* expression ::= expression BITWISE_OR expression */
#line 344 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_BITWISE_OR, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1367 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 57: /* expression ::= expression SHIFTLEFT expression */
#line 347 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_SHIFTLEFT, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1372 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 58: /* expression ::= expression SHIFTRIGHT expression */
#line 350 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBinary(C->E, EST_SHIFTRIGHT, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekFalse); }
#line 1377 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 59: /* expression ::= expression MOD paren_expr_list */
        case 60: /* expression ::= expression MOD expression */ yytestcase(yyruleno == 60);
#line 353 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateStringFormat(C->E, yymsp[-2].minor.yy94, yymsp[0].minor.yy94); }
#line 1383 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 61: /* expression ::= ARRAYOPENBRACKET CLOSEBRACKET */
#line 359 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateUnary(C->E, EST_ARRAY, NULL, yymsp[-1].minor.yy0.line); }
#line 1388 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 62: /* expression ::= ARRAYOPENBRACKET expr_list CLOSEBRACKET */
#line 362 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateUnary(C->E, EST_ARRAY, yymsp[-1].minor.yy94, yymsp[-1].minor.yy94->line); }
#line 1393 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 63: /* expression ::= MAPSTARTBLOCK ENDBLOCK */
#line 365 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateUnary(C->E, EST_MAP, NULL, yymsp[-1].minor.yy0.line); }
#line 1398 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 64: /* expression ::= MAPSTARTBLOCK expr_list ENDBLOCK */
#line 368 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateUnary(C->E, EST_MAP, yymsp[-1].minor.yy94, yymsp[-1].minor.yy94->line); }
#line 1403 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 65: /* expression ::= lvalue ASSIGN expression */
#line 371 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateAssignment(C->E, yymsp[-2].minor.yy94, yymsp[0].minor.yy94); }
#line 1408 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 66: /* expression ::= lvalue ASSIGN error */
#line 374 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { ekCompileExplainError(C, "assignment expected expression"); }
#line 1413 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 67: /* expression ::= expression INHERITS expression */
#line 377 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateInherits(C->E, yymsp[-2].minor.yy94, yymsp[0].minor.yy94); }
#line 1418 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 68: /* expression ::= lvalue */
        case 80: /* lvalue ::= lvalue_indexable */ yytestcase(yyruleno == 80);
#line 380 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = yymsp[0].minor.yy94; }
#line 1424 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 69: /* expression ::= INTEGER */
#line 383 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateKInt(C->E, &yymsp[0].minor.yy0, 0); }
#line 1429 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 70: /* expression ::= NEGATIVE INTEGER */
#line 386 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateKInt(C->E, &yymsp[0].minor.yy0, CKO_NEGATIVE); }
#line 1434 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 71: /* expression ::= FLOATNUM */
#line 389 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateKFloat(C->E, &yymsp[0].minor.yy0, 0); }
#line 1439 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 72: /* expression ::= NEGATIVE FLOATNUM */
#line 392 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateKFloat(C->E, &yymsp[0].minor.yy0, CKO_NEGATIVE); }
#line 1444 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 73: /* expression ::= LITERALSTRING */
#line 395 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateKString(C->E, &yymsp[0].minor.yy0, 0); }
#line 1449 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 74: /* expression ::= REGEXSTRING */
#line 398 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateKString(C->E, &yymsp[0].minor.yy0, 1); }
#line 1454 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 75: /* expression ::= TRUE */
#line 401 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBool(C->E, &yymsp[0].minor.yy0, ekTrue); }
#line 1459 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 76: /* expression ::= FALSE */
#line 404 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateBool(C->E, &yymsp[0].minor.yy0, ekFalse); }
#line 1464 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 77: /* expression ::= NULL */
#line 407 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateNull(C->E, yymsp[0].minor.yy0.line); }
#line 1469 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 78: /* expression ::= FUNCTION LEFTPAREN func_args RIGHTPAREN statement_block */
#line 410 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateFunctionDecl(C->E, NULL, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, yymsp[-4].minor.yy0.line); }
#line 1474 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 79: /* expression ::= expression QUESTIONMARK expression COLON expression */
#line 413 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateIfElse(C->E, yymsp[-4].minor.yy94, yymsp[-2].minor.yy94, yymsp[0].minor.yy94, ekTrue); }
#line 1479 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 81: /* lvalue ::= VAR IDENTIFIER */
#line 429 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxMarkVar(C->E, ekSyntaxCreateList(C->E, EST_IDENTIFIERLIST, ekSyntaxCreateIdentifier(C->E, &yymsp[0].minor.yy0))); }
#line 1484 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 82: /* lvalue ::= VAR GROUPLEFTPAREN ident_list RIGHTPAREN */
#line 432 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxMarkVar(C->E, yymsp[-1].minor.yy94); }
#line 1489 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 84: /* lvalue_indexable ::= THIS */
#line 447 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateThis(C->E, yymsp[0].minor.yy0.line); }
#line 1494 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 85: /* lvalue_indexable ::= lvalue_indexable paren_expr_list */
#line 450 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateCall(C->E, yymsp[-1].minor.yy94, yymsp[0].minor.yy94); }
#line 1499 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 86: /* lvalue_indexable ::= lvalue_indexable OPENBRACKET expression CLOSEBRACKET */
#line 453 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateIndex(C->E, yymsp[-3].minor.yy94, yymsp[-1].minor.yy94, ekFalse); }
#line 1504 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 87: /* lvalue_indexable ::= lvalue_indexable PERIOD IDENTIFIER */
#line 456 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateIndex(C->E, yymsp[-2].minor.yy94, ekSyntaxCreateKString(C->E, &yymsp[0].minor.yy0, 0), ekFalse); }
#line 1509 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 88: /* lvalue_indexable ::= lvalue_indexable COLONCOLON IDENTIFIER */
#line 459 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateIndex(C->E, yymsp[-2].minor.yy94, ekSyntaxCreateKString(C->E, &yymsp[0].minor.yy0, 0), ekTrue); }
#line 1514 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 89: /* lvalue_indexable ::= IDENTIFIER */
#line 462 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateIdentifier(C->E, &yymsp[0].minor.yy0); }
#line 1519 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 90: /* ident_list ::= ident_list COMMA IDENTIFIER */
#line 474 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxListAppend(C->E, yymsp[-2].minor.yy94, ekSyntaxCreateIdentifier(C->E, &yymsp[0].minor.yy0), 0); }
#line 1524 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 91: /* ident_list ::= IDENTIFIER */
#line 477 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateList(C->E, EST_IDENTIFIERLIST, ekSyntaxCreateIdentifier(C->E, &yymsp[0].minor.yy0)); }
#line 1529 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 92: /* ident_list ::= */
#line 480 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateList(C->E, EST_IDENTIFIERLIST, NULL); }
#line 1534 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 93: /* func_args ::= ident_list */
#line 489 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateFunctionArgs(C->E, yymsp[0].minor.yy94, NULL); }
#line 1539 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 94: /* func_args ::= ident_list COMMA ELLIPSIS IDENTIFIER */
#line 492 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateFunctionArgs(C->E, yymsp[-3].minor.yy94, &yymsp[0].minor.yy0); }
#line 1544 "/Users/jdrago/work/eureka/lib/ekParser.c"
            break;
        case 95: /* func_args ::= ELLIPSIS IDENTIFIER */
#line 495 "/Users/jdrago/work/eureka/lib/ekParser.y"
            { yygotominor.yy94 = ekSyntaxCreateFunctionArgs(C->E, ekSyntaxCreateList(C->E, EST_IDENTIFIERLIST, NULL), &yymsp[0].minor.yy0); }
#line 1549 "/Users/jdrago/work/eureka/lib/ekParser.c"
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
    yyParser * yypParser        /* The parser */
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
    yyParser * yypParser,        /* The parser */
    int yymajor,                 /* The major type of the error token */
    YYMINORTYPE yyminor          /* The minor type of the error token */
    )
{
    ekParseARG_FETCH;
#define TOKEN (yyminor.yy0)
#line 117 "/Users/jdrago/work/eureka/lib/ekParser.y"
    ekCompileSyntaxError(C, &TOKEN, "Syntax Error");
#line 1614 "/Users/jdrago/work/eureka/lib/ekParser.c"
    ekParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following is executed when the parser accepts
*/
static void yy_accept(
    yyParser * yypParser        /* The parser */
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
    void * yyp,                /* The parser */
    int yymajor,               /* The major token code number */
    ekParseTOKENTYPE yyminor   /* The value for the token */
    ekParseARG_PDECL           /* Optional %extra_argument parameter */
    )
{
    YYMINORTYPE yyminorunion;
    int yyact;          /* The parser action. */
    int yyendofinput;   /* True if we are at the end of input */
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
        } else if (yyact < YYNSTATE + YYNRULE)  {
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
                } else if (yymx != YYERRORSYMBOL)   {
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
#endif /* ifdef YYERRORSYMBOL */
        }
    } while (yymajor != YYNOCODE && yypParser->yyidx >= 0);
    return;
}
