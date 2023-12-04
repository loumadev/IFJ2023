// func __stringify__(_ n: Double?) -> String {
// 	if(n == nil) { return "nil" }
// 	if(n == 0) { return "0" }

// 	var num = n!

// 	var isNegative = false
// 	if(num < 0) {
// 		isNegative = true
// 		num = 0 - num
// 	}

// 	var integerPart = Int2Double(Double2Int(num))
// 	var fractionalPart = num - integerPart
// 	var hasFractionalPart = fractionalPart > 0

// 	var integerResult = ""
// 	var divisor = 1.0

// 	// Find the appropriate divisor to get the most significant digit of the integer part
// 	while(integerPart / divisor >= 10) {
// 		divisor = divisor * 10
// 	}

// 	// Extract digits for the integer part
// 	while(divisor >= 1) {
// 		let digit = Double2Int(integerPart / divisor)
// 		integerResult = integerResult + chr(digit + 48) // Convert digit to character code
// 		integerPart = Int2Double(__modulo__(integerPart, divisor))
// 		divisor = divisor / 10
// 	}

// 	let precision = 15
// 	var position = 0
// 	var fractionalResult = ""

// 	var floatOffset = length(integerResult) + 1 // +1 for the decimal point
// 	var zeroIndex = 0

// 	// Extract digits for the fractional part
// 	while(precision > position && fractionalPart > 0) {
// 		fractionalPart = fractionalPart * 10
// 		let digit = Double2Int(fractionalPart)
// 		fractionalResult = fractionalResult + chr(digit + 48) // Convert digit to character code
// 		fractionalPart = fractionalPart - Int2Double(digit)
// 		position = position + 1

// 		if(digit == 0) {
// 			if(zeroIndex == 0) {
// 				zeroIndex = position
// 			}
// 		} else {
// 			zeroIndex = 0
// 		}
// 	}

// 	// Add fractional part if it exists
// 	if(hasFractionalPart) {
// 		integerResult = integerResult + "." + fractionalResult
// 	}

// 	// Remove trailing zeros
// 	if(zeroIndex > 0) {
// 		integerResult = substring(of: integerResult, startingAt: 0, endingBefore: floatOffset + zeroIndex - 1)!
// 	}

// 	// Add negative sign if needed
// 	if(isNegative) {
// 		integerResult = "-" + integerResult
// 	}

// 	return integerResult
// }

// func __stringify__(_ n: Int?) -> String {
// 	if(n == nil) { return "nil" }

// 	return __stringify__(Int2Double(n!))
// }

// func __stringify__(_ b: Bool?) -> String {
// 	if(b == nil) { return "nil" }

// 	if(b!) {
// 		return "true"
// 	} else {
// 		return "false"
// 	}
// }

// func __stringify__(_ s: String?) -> String {
// 	if(s == nil) { return "nil" }

// 	return s!
// }

// func __modulo__(_ a: Double, _ b: Double) -> Int {
// 	return Double2Int(a - Int2Double(Double2Int(a / b)) * b)
// }


// var int = 123
// var float = 456.789
// var bool = true
// var str = "Hello World!"
// var nilVal: Int? = nil
// var result = "My int is " + __stringify__(int)
// 			+ ", float is " + __stringify__(float)
// 			+ ", boolean is " + __stringify__(bool)
// 			+ ", string is " + __stringify__(str)
// 			+ ", and nil is " + __stringify__(nilVal)
// write(result, "\n")


func f() -> Int {return 1}
func f() -> Double {return 2.0}
func f() -> String {return "3"}
func f() -> Bool {return true}

var int = 123
var float = 456.789
var bool = true
var str = "Hello World!"
var nilVal: Int? = nil

var result = "My int is \(int), float is \(float), boolean is \(bool), string is \(str), and nil is \(nilVal)"
var expr = "Expression = \((2 + f()) * (f() - 3.5) * 5)"
var nested = "Nested = \("Nested = \("Nested = \(1 + 1)")")"

write("\n", result, "\n", expr, "\n", nested, "\n")

