
#var range = array()
#push(range, 5)

var range = object()
range.init = func(start, end)
    this.offset_ = start
    this.count_  = end - start + 1

range.count = func()
    return this.count_

range.get = func(i)
    return this.offset_ + i

var z = array()
push(z, 5)
push(z, 7)
push(z, 8)

var walk = object()
walk.init = func(a)
    this.a = a

walk.count = func()
    return length(this.a)

walk.get = func(i)
    return this.a[i]

for x,y in walk(z)
    print("hello %s %s\n" % (x,y))

#func someClassInit(a, b, c)
#    var someLocal = "this still happens"
#    if this
#        this.a = a
#        this.b = b
#        this.c = c
#        this.d = someLocal
#
#var SomeClass = object()
#SomeClass.init = someClassInit
#
#var b = SomeClass(3, 4, 5)
#print("a: %s / b: %s\n\n" % (b.a, b.b))
#someClassInit(3, 4, 5)
#
