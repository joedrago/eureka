// ----------------------------------------------------------
// Creating the range() iterator

var range = object();
range.init = function(self, start, end)
{
    self.start = start;
    self.end = end;
    return self;
};

range.count = function(self)
{
    return 1 + (self.end - self.start);
};

range.get = function(self, i)
{
    return self.start + i;
};

// ----------------------------------------------------------
// Iterator usage

print("Range:");
for(v in range(1, 10))
{
    print(" %d" % (v));
}
print("\n");

// ----------------------------------------------------------
// Array iteration

var a = array();
push(a, 5);
push(a, 6);
push(a, 7);
push(a, 8);
print("Array:");
for(w in a)
{
    print(" %d" % (w));
}
print("\n");

// ----------------------------------------------------------
// Member Function Execution

var obj = range(1, 100);
print("Object count: %d\n" % ( obj:count() ));

