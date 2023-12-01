func concat(_ x : String, with y : String) -> String {
    let x = x + y
    return x + " " + y
}

let a = "ahoj "
var ct : String
ct = concat(a, with: "svete")
write(ct, a)
