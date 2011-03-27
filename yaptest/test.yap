class foo
    func y()
        print("1\n")

class woo : foo()
    func z()
        print("2\n")

var o = woo()
o.y()
o.z()

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
#walk.count = func()
#    return length(this.a)
#walk.get = func(i)
#    return this.a[i]
#
#var reverse = object()
#reverse.init = func(iter)
#    this.iter = iter
#    this.count_ = iter.count()
#reverse.count = func()
#    return this.count_
#reverse.get = func(i)
#    return this.iter.get(this.count_ - 1 - i)
#
#var z = array()
#push(z, 5)
#push(z, 7)
#push(z, 8)
#
#for x in reverse(walk(z))
#    print("sweet: %s\n" % (x))
#
