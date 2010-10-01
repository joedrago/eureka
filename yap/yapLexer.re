
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
    "#"            { goto comment; }

    "while"         { return YTT_COMMENT; }

    "("             { return YTT_LEFTPAREN; }
    ")"             { return YTT_RIGHTPAREN; }

    ([a-zA-Z_$] | UniversalChar) ([a-zA-Z_0-9$] | UniversalChar)*
        { return YTT_IDENTIFIER; }

    (['] (EscapeSequence|any\[\n\r\\']|UniversalChar)+ ['])
        { return YTT_LITERALSTRING; }

    (["] (EscapeSequence|any\[\n\r\\"]|UniversalChar)* ["])
        { return YTT_LITERALSTRING; }

    [ \t\v\f]+
        { return YTT_SPACE; }

    Newline
    {
        l->line++;
        l->col = 1;
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
        l->col = 1;
        return YTT_COMMENT;
    }

    any            { goto comment; }
*/
