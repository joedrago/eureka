var Vehicle = object("name" => "Vehicle");
Vehicle.drive = function()
{
    print("driving generically: %s\n" % (this.name));
};

var Car = Vehicle("name" => "Car");
Car.drive = function()
{
    print("driving as a Car: %s\n" % (this.name));
    Vehicle::drive();
};

var c = Car();
c.drive();

