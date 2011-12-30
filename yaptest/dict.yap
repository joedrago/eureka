var a = dict("name"   => "bob",
             "colors" => dict("bob"  => "blue",
                              "fred" => "red")
             );
#a.name = "fred";
print("Name: %s [%s]\n" % (a.name, a.colors[a.name]));

