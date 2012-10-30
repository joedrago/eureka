function pair(a, b)
{
    var o = object();
    o.a = a;
    o.b = b;
    return o;
}

var list = array();
push(list, pair(20, 20));
push(list, pair("QQ", "QQ"));

push(list, pair(int("80"), int("90")));

push(list, pair(15, 20));
push(list, pair(99, 20));

push(list, pair("AA", "ZZ"));
push(list, pair("AA", "AZ"));

push(list, pair(0, !0));
push(list, pair(1, !1));

push(list, pair(0, 0));
push(list, pair(0, 1));
push(list, pair(1, 0));
push(list, pair(1, 1));

for(e in list)
{
    var a = e.a;
    var b = e.b;

    print("a: %s, b: %s, ==: %d, !=: %d, <:%d, <=:%d, >:%d, >=:%d, &&: %d, ||: %d, <=>: %d, cmp: %d\n" % (
        a,
        b,
        a == b,
        a != b,
        a < b,
        a <= b,
        a > b,
        a >= b,
        a && b,
        a || b,
        a <=> b,
        a cmp b
    ));
}
