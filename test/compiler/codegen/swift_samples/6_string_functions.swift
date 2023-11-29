write("[TEST] String functions", "\n")

write("length function", "\n")
let a = "abc"
let a_length = length(a)
write("Length of \"abc\": ", a_length, "\n")

let b = "a\nb"
let b_length = length(b)
write("Length of \"a\\nb\": ", b_length, "\n")

write("\n")

write("chr function", "\n")
let c = chr(65)
write("65 in ASCII table is A", "\n")
write("65 was translated as: ", c, "\n")

write("\n")

write("ord function", "\n")
let d = ord("A")
write("A in ASCII table is 65", "\n")
write("A was translated as: ", d, "\n")

write("\n")

let e = ord("")
write("Empty string should be translated as 0", "\n")
write("Empty string was translated as: ", e, "\n")

write("\n")

write("substr function", "\n")
let f = substring(of: "abc", startingAt: 1,  endingBefore: 4)
write("Substring of \"abc\" from 1 to 2: ", f, "\n")

write("[TEST] Tests done.", "\n")