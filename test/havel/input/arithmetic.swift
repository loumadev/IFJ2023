let a = 1 + 2
let b = 4 * 3
let c = 20 - 1

write(a, "\n", b, "\n", c, "\n")

let af = 1.0 + 2.0
let bf = 4.0 * (3.0 * 1)
let cf = 20.1 - 1.1

write(af, "\n", bf, "\n", cf, "\n")

let da = 25.0 / 5.0
let db = 25 / 6

write(da, "\n", db, "\n")

// automatic conversion
let ac = 1 + 2.0
let bc = 4 * 3.0
let cc = 20 - 1.0
let acc = 1.0 + 2
let bcc = 4.0 * 3
let ccc = 20.0 - 1

write(ac, "\n", bc, "\n", cc, "\n")
write(acc, "\n", bcc, "\n", ccc, "\n")

// concat
let str = "Hello "
let str2 = str + "world\n"

write(str2)

// coalescence
let nil_val : Int? = nil
let non_nil : Int? = 3
let res1 = nil_val ?? 1
let res2 = non_nil ?? 1
write(res1, "\n", res2, "\n")

// force unwrap
let unwrapped = non_nil!
write(unwrapped, "\n")
