let a = "abcdefghijk"
let minus1 = 0 - 1

let b1 = substring(of : a, startingAt: 0, endingBefore: 11)
let b2 = substring(of : a, startingAt: 2, endingBefore: 5)
let b3 = substring(of : a, startingAt: minus1, endingBefore: 5)
let b4 = substring(of : a, startingAt: 0, endingBefore: minus1)
let b5 = substring(of : a, startingAt: 5, endingBefore: 5)
let b6 = substring(of : a, startingAt: 5, endingBefore: 4)
let b7 = substring(of : a, startingAt: 11, endingBefore: 12)

if let b3
{
    write("WRONG3!\n")
}
else { }

if let b4
{
    write("WRONG4!\n")
}
else { }

if let b5
{
    write("WRONG5!\n")
}
else { }

if let b6
{
    write("WRONG6!\n")
}
else { }

if let b7
{
    write("WRONG6!\n")
}
else { }

write(b1, "\n")
write(b2, "\n")
write(b3, "\n")
write(b4, "\n")
write(b5, "\n")
write(b6, "\n")
write(b7, "\n")
write("done")
