function printDepth(depth)
{
    while(depth > 0)
    {
        print(" ");
        depth -= 1;
    }
}

function print_r(depth, ... values)
{
    var suffix = 0;

    for(value in values)
    {
        if(depth)
            printDepth(depth);
        else
            print("V", suffix, ": ");

        var t = type(value);
        if(t == "int")
        {
            print("%d" % (value));
        }
        else if(t == "float")
        {
            print("%f" % (value));
        }
        else if(t == "string")
        {
            print("%s" % (value));
        }
        else if(t == "null")
        {
            print("(null)");
        }
        else if(t == "array")
        {
            print("[\n");
            for(entry in value)
            {
                print_r(depth+2, entry);
            }
            printDepth(depth);
            print("]");
        }
        else if(t == "object")
        {
            print("{\n");
            for(key in keys(value))
            {
                printDepth(depth+1);
                print("\"%s\":\n" % (key));
                print_r(depth+4, value[key]);
            }
            printDepth(depth);
            print("}");
        }

        print("\n");
        if(!depth)
            print("\n");
        suffix += 1;
    }
}

if(var a = regex_match("^", "dat derp"))
{
    print_r(0, a);
}
else
{
    print("didnt match (1)\n");
}


if(var b = regex_match("dat (.+)", "i love DAT thing", "i"))
{
    print(dump(b), "\n");
}
else
{
    print("didnt match (2)\n");
}

print("done\n");

print(dump(dict( name => "fred", value => "woo", list => array(1,2,3), func => print)));

