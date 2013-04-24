// ---------------------------------------------------------------------------
//                   Copyright Joe Drago 2010 - 2011.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

/*!re2c
re2c:indent:string = "    ";
re2c:yyfill:enable = 0;
any                = [\t\v\f\r\n\040-\377];
anyctrl            = [\001-\037];
OctalDigit         = [0-7];
Digit              = [0-9];
HexDigit           = [a-fA-F0-9];
Integer            = (("0" [xX] HexDigit+) | ("0" OctalDigit*) | ([1-9] Digit*));
ExponentStart      = [Ee] [+-];
ExponentPart       = [Ee] [+-]? Digit+;
FractionalConstant = (Digit* "." Digit+) | (Digit+ ".");
FloatingSuffix     = [fF] [lL]? | [lL] [fF]?;
IntegerSuffix      = [uU] [lL]? | [lL] [uU]?;
LongIntegerSuffix  = [uU] ([lL] [lL]) | ([lL] [lL]) [uU]?;
Backslash          = [\\] | "??/";
EscapeSequence     = Backslash ([abfnrtv?'"] | Backslash | "x" HexDigit+ | OctalDigit OctalDigit? OctalDigit?);
HexQuad            = HexDigit HexDigit HexDigit HexDigit;
UniversalChar      = Backslash ("u" HexQuad | "U" HexQuad HexQuad);
Newline            = "\r\n" | "\n" | "\r";
PPSpace            = ([ \t\f\v]|("/*"(any\[*]|Newline|("*"+(any\[*/]|Newline)))*"*"+"/"))*;
Pound              = "#" | "??=" | "%:";
NonDigit           = [a-zA-Z_$] | UniversalChar;
*/

/*!re2c
    "\"\"\""        { goto heredoc; }
    "//"            { goto comment; }
    "..."           { return ETT_ELLIPSIS; }
    "&&"            { return ETT_AND; }
    "&="            { return ETT_BITWISE_ANDEQUALS; }
    "&"             { return ETT_BITWISE_AND; }
    "||"            { return ETT_OR; }
    "|="            { return ETT_BITWISE_OREQUALS; }
    "|"             { return ETT_BITWISE_OR; }
    "<=>"           { return ETT_CMP; }
    "cmp"           { return ETT_CMP; }
    "=="            { return ETT_EQUALS; }
    "="             { return ETT_ASSIGN; }
    "!="            { return ETT_NOTEQUALS; }
    "!"             { return ETT_NOT; }
    "^="            { return ETT_BITWISE_XOREQUALS; }
    "^"             { return ETT_BITWISE_XOR; }
    "~"             { return ETT_BITWISE_NOT; }
    ">="            { return ETT_GREATERTHANOREQUAL; }
    ">"             { return ETT_GREATERTHAN; }
    "<<="           { return ETT_SHIFTLEFTEQUALS; }
    ">>="           { return ETT_SHIFTRIGHTEQUALS; }
    "<<"            { return ETT_SHIFTLEFT; }
    ">>"            { return ETT_SHIFTRIGHT; }
    "<="            { return ETT_LESSTHANOREQUAL; }
    "<"             { return ETT_LESSTHAN; }
    "=>"            { return ETT_FATCOMMA; } /* The fat comma! (see Perl) */
    ","             { return ETT_COMMA; }
    ";"             { return ETT_SEMI; }
    "+="            { return ETT_PLUSEQUALS; }
    "+"             { return ETT_PLUS; }
    "-="            { return ETT_DASHEQUALS; }
    "-"             { return ETT_DASH; }
    "*="            { return ETT_STAREQUALS; }
    "*"             { return ETT_STAR; }
    "/="            { return ETT_SLASHEQUALS; }
    "/"             { return ETT_SLASH; }
    "%"             { return ETT_MOD; }
    "{"             { return ETT_OPENBRACE; }
    "}"             { return ETT_CLOSEBRACE; }
    "["             { return ETT_OPENBRACKET; }
    "]"             { return ETT_CLOSEBRACKET; }
    "."             { return ETT_PERIOD; }
    "::"            { return ETT_COLONCOLON; }
    ":"             { return ETT_COLON; }
    "\?"            { return ETT_QUESTIONMARK; }

    "break"         { return ETT_BREAK; }
    "this"          { return ETT_THIS; }
    "while"         { return ETT_WHILE; }
    "var"           { return ETT_VAR; }
    "null"          { return ETT_NULL; }
    "function"      { return ETT_FUNCTION; }
    "return"        { return ETT_RETURN; }
    "if"            { return ETT_IF; }
    "else"          { return ETT_ELSE; }
    "and"           { return ETT_AND; }
    "or"            { return ETT_OR; }
    "not"           { return ETT_NOT; }
    "for"           { return ETT_FOR; }
    "in"            { return ETT_IN; }
    "inherits"      { return ETT_INHERITS; }

    "("             { return ETT_LEFTPAREN; }
    ")"             { return ETT_RIGHTPAREN; }

    ([a-zA-Z_$] | UniversalChar) ([a-zA-Z_0-9$] | UniversalChar)*
        { return ETT_IDENTIFIER; }

    ((FractionalConstant ExponentPart?) | (Digit+ ExponentPart)) FloatingSuffix?
        { return ETT_FLOATNUM; }

    Integer
        { return ETT_INTEGER; }

    (["] (EscapeSequence|any\[\n\r\\"]|UniversalChar)* ["])
        { return ETT_LITERALSTRING; }

    [ \t\v\f]+
        { return ETT_SPACE; }

    Newline
    {
        l->line++;
        return ETT_NEWLINE;
    }

    "\000"
    {
        return ETT_EOF;
    }

    any        { return ETT_UNKNOWN; }
*/

comment:
/*!re2c
    Newline
    {
        l->line++;
        return ETT_COMMENT;
    }

    any            { goto comment; }
*/

heredoc:
/*!re2c
    Newline
    {
        l->line++;
	goto heredoc;
    }

    "\"\"\""
    {
        return ETT_HEREDOC;
    }

    any            { goto heredoc; }
*/
