#!/usr/bin/perl

use strict;

my $header;
open($header, '>', 'ekLexerNames.h') or die "cant open ekLexerNames.h for write";

print $header <<END;
const char * ekTokenIDToString(int id)
{
    switch(id)
    {
END

my $fh;
open($fh, '<', 'ekParser.h') or die "cant open ekParser.h for read";
while(<$fh>)
{
    if(/^#define\s+(\S+)\s+(\d+)/)
    {
        my($name, $id) = ($1, $2);
        print $header "        case $id: return \"$name\";\n";
    }
}
close($fh);

print $header <<END;
    };
    return "--unknown--";
}
END

close($header);
print "Generated ekLexerNames.h\n";
