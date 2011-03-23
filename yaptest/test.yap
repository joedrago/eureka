func i(self)
    print("init!\n")
    this.whatever = 7

var o = object()
o.init = i

var b = o()

print("whatever is: %s\n\n" % (b.whatever))

