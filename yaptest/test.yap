// ---------------------------------------------------------------------------
// Implementation of range

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

// ---------------------------------------------------------------------------
// Usage of range()

for(v in range(1, 10))
{
    print("v: %d\n" % (v));
}

// ---------------------------------------------------------------------------
// Array iteration

var a = array();
push(a, 5);
push(a, 6);
push(a, 7);
push(a, 8);
for(w in a)
{
    print("w: %d\n" % (w));
}

