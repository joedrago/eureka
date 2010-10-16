var i_am_true = 1

if(i_am_true)
    print("true!\n")
else
    print("false!\n")

var foo

foo = (2+3)*4
print("With parens   : ", foo, "\n")

foo = 2+3*4
print("Without parens: ", foo, "\n")

func annoying(text)
    var lots = 5
    while lots
        print(text)
        print("\n")
        lots = lots - 1

annoying("SPAM")
