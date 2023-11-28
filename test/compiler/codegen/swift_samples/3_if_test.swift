var isTrue : Bool = true

write("if (isTrue) { ... }", "\n")
if (isTrue) {
    write("isTrue is true", "\n")
}

write("\n")
write("if (false) { ... } else { ... }", "\n")
if (false) {
    write("true is true", "\n")
} else {
    write("false is false", "\n")
}

write("\n")
write("if (true) { ... } else { ... }", "\n")
if (true) {
    write("true is true", "\n")
} else {
    write("false is false", "\n")
}

write("\n")
let alwaysNil : Int?
write("if let alwaysNil { ... }", "\n")
if let alwaysNil {
    write("alwaysNil is not nil", "\n")
} else {
    write("alwaysNil is nil", "\n")
}

write("\n")
let alwaysNotNil : Int? = 1
write("if let alwaysNotNil { ... }", "\n")
if let alwaysNotNil {
    write("alwaysNotNil is not nil", "\n")
} else {
    write("alwaysNotNil is nil", "\n")
}