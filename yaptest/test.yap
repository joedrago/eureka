func someClassInit(a, b, c)
    var someLocal = "this still happens"
    if this
        this.a = a
        this.b = b
        this.c = c
        this.d = someLocal

var SomeClass = object()
SomeClass.init = someClassInit

var b = SomeClass(3, 4, 5)
print("a: %s / b: %s\n\n" % (b.a, b.b))
someClassInit(3, 4, 5)

