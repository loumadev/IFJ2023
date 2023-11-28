// late initialization
let foo1: Int
foo1 = 1

write("foo1: Int => ", foo1 ,"\n")

// nil by default
let foo2: String?
write("foo2: String? => ", foo2 ,"\n")

//var is mutable
var foo3: Int?
write("foo3: Int? => ", foo3 ,"\n")

foo3 = 3
write("foo3: Int? [assigned 3] => ", foo3 ,"\n")

// expressions
var foo4: Int? = 4 + 3
write("foo4: 4 + 3 => ", foo4 ,"\n")

// reading from variable
// todo: handle unwrap
// foo4 = foo4! + 1
// write("foo4, foo4 + 1: ", foo4 ,"\n")