// Coverage test. This script should use every grammar feature and op that Yap can parse/perform.

var a = 1 + -2;
var b = 1.2 - -1.3;

if(a < b) {}


if(a <= b) {}
if(a > b) {}
if(a >= b) {}
if(a == b) {}
if(a != b) {}

print("Yep!\n");


var d = a * a;
d = a / a;

var s = "woo";
var t = "noo";
if(s cmp t) {}
if(a and not b) {}
if(a and ~b) {}
if(a && ~b) {}
if(a or b) {}
if(a || b) {}
if(a <=> b) {}
if(a ^ !b) {}

var otherliteralstring = 'wat';

var v = a;
v ^= b;
v &= b;
v |= b;

var w = a << 1;
w = a >> 1;

a <<= 1;
a >>= 1;

a += 1;
a -= 1;
a *= 1;
a /= 1;

var arr = array(1, 2, 3);
for(q in arr)
{
    print("Number: %d\n" % q);
}

var loop = 10;
while(loop > 0)
{
    loop -= 1;
    break;
}

if(a > 0)
{
}
else
{
}


if(a | b) {}
if(a & b) {}

var d1 = dict(a => 1);

var formatted = "%s %d" % ("yep", 5);

function foo(d, e, f)
{
    d += 1;
    return d;
}

function varargs(...args) {}
function varargs(a , b, ...args) {}

var anonfunc = function(g, h)
{
};

anonfunc();

var obj = dict();
obj.foo = 1;
obj.bar = 1;
obj["baz"] = 1;
obj.init = function()
{
    this.wat = 1;
    obj::otherfunc();
};


var anotherobj = dict();
anotherobj inherits obj;

var shouldbenull = anotherobj["naw"];

var ar = array(1, 2, 3);
push(ar, 4);

"""
heredoc! weeeeee
more weeeeee
"""
# this type of comment

