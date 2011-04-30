#!/usr/bin/perl

use strict;

my $header;
open($header, '>', 'yapLexerNames.h') or die "cant open yapLexerNames.h for write";

print $header <<END;
const char * yapTokenIDToString(int id)
{
    switch(id)
    {
END

my $fh;
open($fh, '<', 'yapParser.h') or die "cant open yapParser.h for read";
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
print "Generated yapLexerNames.h\n";
