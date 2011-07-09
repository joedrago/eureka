// ----------------------------------------------------------------------------
// Keyword: inherits / usage of "static" vs "overridden" variables

var fruit = object();
fruit.type = "fruit"; // override me!
fruit.count = 0;      // static count of eaten fruit

fruit.eat = function(self)
{
    fruit.count = fruit.count + 1; // implement increment, kthx
    print("Eating %s (count: %d)\n" % (self.type, fruit.count));
};

var apple = object();
apple inherits fruit;
apple.type = "apple";
apple:eat();

var pear inherits fruit;
pear.type = "pear";
pear:eat();

// ----------------------------------------------------------------------------
// Keyword: with / multiple withs for a single var, and 'with' nesting

with var very = object()
{
    function awesome()
    {
        print("very awesome\n");
    }
}

with very
{
    with var very = object()
    {
        function awesome()
        {
            print("very VERY awesome\n");
        }
    }
}

very.awesome();
very.very.awesome();

// ----------------------------------------------------------------------------
// Variable creation in a 'with' statement, and mixing with/inherits

with var vehicle = object()
{
    var type = "generic";

    function printType(self)
    {
        print("vehicle type: %s\n" % (self.type));
    }
}

var car inherits vehicle;
car.type = "car";

with var boat inherits vehicle
{
    var type = "boat";
}

vehicle:printType();
car:printType();
boat:printType();

