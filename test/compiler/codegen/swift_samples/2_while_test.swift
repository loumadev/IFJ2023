var counter : Int = 3

write("while(counter > 0) { ... }", "\n")
while (counter > 0) {
    write("counter: ", counter, "\n")
    counter = counter - 1
}

counter = 3

write("\n")

write("while(counter != (0-1)) { ... }", "\n")
while (counter == (0-1)) {
    write("counter: ", counter, "\n")
    counter = counter - 1
}

write("\n")
counter = 3

write("while(counter >= 0) { ... }\n")
while (counter >= 0) {
    write("counter: ", counter, "\n")
    counter = counter - 1
}

write("\n")
counter = 0
write("while(counter <= 3) { ... }\n")
while (counter <= 3) {
    write("counter: ", counter, "\n")
    counter = counter + 1
}


write("\n")
var foo : Int = 4

write("while(counter < foo) { ... }\n")
while (counter < foo) {
    write("counter: ", counter, "\n")
    counter = counter + 1
}

write("\n")

write("Let's print a 3x3 matrix\n")
var x = 3
var y = 3

while (x > 0) {
    while (y > 0) {
        write("[", x, ", ", y, "]", "\n")
        y = y - 1
    }
    x = x - 1
    y = 3
}
