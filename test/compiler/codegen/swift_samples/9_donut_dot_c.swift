// func substring(of s: String, startingAt i: Int, endingBefore j: Int) -> String? {
// 	let start = s.index(s.startIndex, offsetBy: i)
// 	let end = s.index(s.startIndex, offsetBy: j)
// 	return String(s[start..<end])
// }

// func ord(_ s: String) -> Int {
// 	return Int(s.unicodeScalars.first!.value)
// }

// func chr(_ i: Int) -> String {
// 	return String(UnicodeScalar(i)!)
// }

// func Int2Double(_ term: Int) -> Double {
// 	return Double(term)
// }

// func Double2Int(_ term: Double) -> Int {
// 	return Int(term)
// }

// func length(_ s: String) -> Int {
// 	return s.count
// }

// func write(_ s: String, _ t: String) {
// 	print(s + t, terminator: "")
// }


let PI = 3.141592
let HPI = PI * 0.5

func sin(_ x: Double) -> Double {
    var result = 0.0
    var sign = 1.0
    var power = 1.0
    while power <= 20.0 {
        result = result + (sign * (pow(x, power) / factorial(power)))
        sign = 0 - sign
        power = power + 2.0
    }
    return result
}

func cos(_ x: Double) -> Double {
    return sin(HPI - x)
}

func pow(_ x: Double, _ y: Double) -> Double {
    var result = 1.0
    var y = y
    while y > 0.0 {
        result = result * x
        y = y - 1.0
    }
    return result
}

func factorial(_ n: Double) -> Double {
    var result = 1.0
    var n = n
    while n > 1.0 {
        result = result * n
        n = n - 1.0
    }
    return result
}


var A = 1.0,
	B = 1.0
while(true) {
	var b = ""
	var z = ""
	A = A + 0.07
	B = B + 0.03
	var cA = cos(A),
		sA = sin(A),
		cB = cos(B),
		sB = sin(B)
	var m = 0
	var k = 0
	var l = 1760
	while(k < l) {
		if(m == 79) {
			b = b + "\n"
			m = 0
		}
		else {
			b = b + " "
			m = m + 1
		}
		z = z + " "

		k = k + 1
	}
	var j = 0.0
	while(j < 6.28) {
		// j <=> theta
		var ct = cos(j),
			st = sin(j)
		var i = 0.0
		while(i < 6.28) {
			// i <=> phi
			var sp = sin(i),
				cp = cos(i),
				h = ct + 2, // R1 + R2*cos(theta)
				D = 1 / (sp * h * sA + st * cA + 5), // this is 1/z
				t = sp * h * cA - st * sA // this is a clever factoring of some of the terms in x' and y'

			var x = Double2Int(40 + 30 * D * (cp * h * cB - t * sB)),
				y = Double2Int(12 + 15 * D * (cp * h * sB + t * cB)),
				o = x + 80 * y,
				N =
					Double2Int(8 *
						((st * sA - sp * ct * cA) * cB -
							sp * ct * sA -
							st * cA -
							cp * ct * sB))

			if(o >= 0 && o < l) {
				var z_o = Int2Double(ord(substring(of: z, startingAt: o, endingBefore: o + 1)!)) / 255.0
				if(y < 22 && y >= 0 && x >= 0 && x < 79 && D > z_o) {
					z = substring(of: z, startingAt: 0, endingBefore: o)! + chr(Double2Int(D * 255)) + substring(of: z, startingAt: o + 1, endingBefore: l)!
					var n = 0
					if(N > 0) {
						n = N
					}
					var c = substring(of: ".,-~:;=!*#$@", startingAt: n, endingBefore: n + 1)!
					b = substring(of: b, startingAt: 0, endingBefore: o)! + c + substring(of: b, startingAt: o + 1, endingBefore: l)!
				}
			}

			// i = i + 0.02
			// i = i + 0.1
			i = i + 0.4
		}

		// j = j + 0.02
		// j = j + 0.4
		j = j + 0.8
	}

	write(b)
}
