
var SomeClass = object()
SomeClass.init = func(a, b, c)
    this.a = a
    this.b = b
    this.c = c

var b = SomeClass(3, 4, 5)
print("a: %s / b: %s\n\n" % (b.a, b.b))

