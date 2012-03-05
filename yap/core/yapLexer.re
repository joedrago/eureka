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
    "#"             { goto comment; }
    "..."           { return YTT_ELLIPSIS; }
    "&&"            { return YTT_AND; }
    "&="            { return YTT_BITWISE_ANDEQUALS; }
    "&"             { return YTT_BITWISE_AND; }
    "||"            { return YTT_OR; }
    "|="            { return YTT_BITWISE_OREQUALS; }
    "|"             { return YTT_BITWISE_OR; }
    "<=>"           { return YTT_CMP; }
    "cmp"           { return YTT_CMP; }
    "=="            { return YTT_EQUALS; }
    "="             { return YTT_ASSIGN; }
    "!="            { return YTT_NOTEQUALS; }
    "!"             { return YTT_NOT; }
    "^="            { return YTT_BITWISE_XOREQUALS; }
    "^"             { return YTT_BITWISE_XOR; }
    "~"             { return YTT_BITWISE_NOT; }
    ">="            { return YTT_GREATERTHANOREQUAL; }
    ">"             { return YTT_GREATERTHAN; }
    "<<="           { return YTT_SHIFTLEFTEQUALS; }
    ">>="           { return YTT_SHIFTRIGHTEQUALS; }
    "<<"            { return YTT_SHIFTLEFT; }
    ">>"            { return YTT_SHIFTRIGHT; }
    "<="            { return YTT_LESSTHANOREQUAL; }
    "<"             { return YTT_LESSTHAN; }
    "=>"            { return YTT_FATCOMMA; } /* The fat comma! (see Perl) */
    ","             { return YTT_COMMA; }
    ";"             { return YTT_SEMI; }
    "+="            { return YTT_PLUSEQUALS; }
    "+"             { return YTT_PLUS; }
    "-="            { return YTT_DASHEQUALS; }
    "-"             { return YTT_DASH; }
    "*="            { return YTT_STAREQUALS; }
    "*"             { return YTT_STAR; }
    "/="            { return YTT_SLASHEQUALS; }
    "/"             { return YTT_SLASH; }
    "%"             { return YTT_MOD; }
    "{"             { return YTT_OPENBRACE; }
    "}"             { return YTT_CLOSEBRACE; }
    "["             { return YTT_OPENBRACKET; }
    "]"             { return YTT_CLOSEBRACKET; }
    "."             { return YTT_PERIOD; }
    "::"            { return YTT_COLONCOLON; }

    "break"         { return YTT_BREAK; }
    "this"          { return YTT_THIS; }
    "while"         { return YTT_WHILE; }
    "var"           { return YTT_VAR; }
    "null"          { return YTT_NULL; }
    "function"      { return YTT_FUNCTION; }
    "return"        { return YTT_RETURN; }
    "if"            { return YTT_IF; }
    "else"          { return YTT_ELSE; }
    "and"           { return YTT_AND; }
    "or"            { return YTT_OR; }
    "not"           { return YTT_NOT; }
    "for"           { return YTT_FOR; }
    "in"            { return YTT_IN; }
    "inherits"      { return YTT_INHERITS; }

    "("             { return YTT_LEFTPAREN; }
    ")"             { return YTT_RIGHTPAREN; }

    ([a-zA-Z_$] | UniversalChar) ([a-zA-Z_0-9$] | UniversalChar)*
        { return YTT_IDENTIFIER; }

    (['] (EscapeSequence|any\[\n\r\\']|UniversalChar)+ ['])
        { return YTT_LITERALSTRING; }

    ((FractionalConstant ExponentPart?) | (Digit+ ExponentPart)) FloatingSuffix?
        { return YTT_FLOATNUM; }

    Integer
        { return YTT_INTEGER; }

    (["] (EscapeSequence|any\[\n\r\\"]|UniversalChar)* ["])
        { return YTT_LITERALSTRING; }

    [ \t\v\f]+
        { return YTT_SPACE; }

    Newline
    {
        l->line++;
        return YTT_NEWLINE;
    }

    "\000"
    {
        return YTT_EOF;
    }

    any        { return YTT_UNKNOWN; }
*/

comment:
/*!re2c
    Newline
    {
        l->line++;
        return YTT_COMMENT;
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
        return YTT_HEREDOC;
    }

    any            { goto heredoc; }
*/
