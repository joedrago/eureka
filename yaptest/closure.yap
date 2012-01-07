
function makeClosure()
{
    var a = 13;
    return function()
    {
        a += 1;
        return a;
    };
}

var f = makeClosure();
print("%d\n" % (f()));
print("%d\n" % (f()));
print("%d\n" % (f()));
print("%d\n" % (f()));

f = makeClosure();
print("%d\n" % (f()));
print("%d\n" % (f()));
print("%d\n" % (f()));
print("%d\n" % (f()));

