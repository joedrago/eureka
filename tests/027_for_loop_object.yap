var a = object(
    notIterated => 0,
    x => 1,
    y => 1,
    z => 1
);

var xyzIter = object(
    init => function(a)
    {
        this.obj = a;
        this.keys = array('x', 'y', 'z');
        return this;
    },
    count => function()
    {
        return 3;
    },
    get => function(i)
    {
        return this.obj[ this.keys[i]  ];
    }
);

for(b in xyzIter(a))
{
}
