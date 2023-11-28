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

