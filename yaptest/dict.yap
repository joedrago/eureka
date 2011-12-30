var a = dict("name"   => "bob",
             "colors" => dict("bob"  => "blue",
                              "fred" => "red")
             );

var b = keys(a);
print("%d keys.\n" % (length(b)));
for(i in b)
{
    print("%s = %s\n" % (i, a[i]));
}

