
#var range = array()
#push(range, 5)

#var range = object()
#range.init = func(start, end)
#    this.offset_ = start
#    this.count_  = end - start + 1
#
#range.count = func()
#    return this.count_
#
#range.get = func(i)
#    return this.offset_ + i

#var walk = object()
#walk.init = func(a)
#    this.a = a
#
#walk.count = func()
#    return length(this.a)
#
#walk.get = func(i)
#    return this.a[i]

var z = array()
push(z, 5)
push(z, 7)
push(z, 8)

for x in z
    print("sweet: %s\n" % (x))

