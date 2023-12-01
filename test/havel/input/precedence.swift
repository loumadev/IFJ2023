let a = 3 + 4 - 5
write(a, "\n")
let b = 3 - 4 * 5
write(b, "\n")
let c = 20 - 5 - 4
write(c, "\n")
let d = 2 * (1 + 3)
write(d, "\n")

if (10 - 9 < 12)
{
    write("true\n")
}
else
{
    write("false\n")
}

if (2 == 1 + 1)
{
    write("true\n")
}
else
{
    write("false\n")
}

let opt : Int? = nil
let val = opt ?? 1 + 4
write(val, "\n")
let opt2 : Int? = 5
let val2 = (opt2 ?? 1) + 6
write(val2, "\n")
