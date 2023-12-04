#define LF "\n"
"func readString() -> String? {return nil}" LF
"func readInt() -> Int? {return nil}" LF
"func readDouble() -> Double? {return nil}" LF
"func write() {}" LF                 // Parameters handled internally
"func Int2Double(_ term: Int) -> Double {return 0.0}" LF
"func Double2Int(_ term: Double) -> Int {return 0}" LF
"func length(_ s: String) -> Int {return 0}" LF
"func substring(of s: String, startingAt i: Int, endingBefore j: Int) -> String? {return nil}" LF
"func ord(_ c: String) -> Int {return 0}" LF
"func chr(_ i: Int) -> String {return \"\"}" LF
"" LF
"func __stringify__(_ n: Double?) -> String {" LF
"	if(n == nil) { return \"nil\" }" LF
"	if(n == 0) { return \"0\" }" LF
"" LF
"	var num = n!" LF
"" LF
"	var isNegative = false" LF
"	if(num < 0) {" LF
"		isNegative = true" LF
"		num = 0 - num" LF
"	}" LF
"" LF
"	var integerPart = Int2Double(Double2Int(num))" LF
"	var fractionalPart = num - integerPart" LF
"	var hasFractionalPart = fractionalPart > 0" LF
"" LF
"	var integerResult = \"\"" LF
"	var divisor = 1.0" LF
"" LF
"	// Find the appropriate divisor to get the most significant digit of the integer part" LF
"	while(integerPart / divisor >= 10) {" LF
"		divisor = divisor * 10" LF
"	}" LF
"" LF
"	// Extract digits for the integer part" LF
"	while(divisor >= 1) {" LF
"		let digit = Double2Int(integerPart / divisor)" LF
"		integerResult = integerResult + chr(digit + 48) // Convert digit to character code" LF
"		integerPart = Int2Double(__modulo__(integerPart, divisor))" LF
"		divisor = divisor / 10" LF
"	}" LF
"" LF
"	let precision = 15" LF
"	var position = 0" LF
"	var fractionalResult = \"\"" LF
"" LF
"	var floatOffset = length(integerResult) + 1 // +1 for the decimal point" LF
"	var zeroIndex = 0" LF
"" LF
"	// Extract digits for the fractional part" LF
"	while(precision > position && fractionalPart > 0) {" LF
"		fractionalPart = fractionalPart * 10" LF
"		let digit = Double2Int(fractionalPart)" LF
"		fractionalResult = fractionalResult + chr(digit + 48) // Convert digit to character code" LF
"		fractionalPart = fractionalPart - Int2Double(digit)" LF
"		position = position + 1" LF
"" LF
"		if(digit == 0) {" LF
"			if(zeroIndex == 0) {" LF
"				zeroIndex = position" LF
"			}" LF
"		} else {" LF
"			zeroIndex = 0" LF
"		}" LF
"	}" LF
"" LF
"	// Add fractional part if it exists" LF
"	if(hasFractionalPart) {" LF
"		integerResult = integerResult + \".\" + fractionalResult" LF
"	}" LF
"" LF
"	// Remove trailing zeros" LF
"	if(zeroIndex > 0) {" LF
"		integerResult = substring(of: integerResult, startingAt: 0, endingBefore: floatOffset + zeroIndex - 1)!" LF
"	}" LF
"" LF
"	// Add negative sign if needed" LF
"	if(isNegative) {" LF
"		integerResult = \"-\" + integerResult" LF
"	}" LF
"" LF
"	return integerResult" LF
"}" LF
"" LF
"func __stringify__(_ n: Int?) -> String {" LF
"	if(n == nil) { return \"nil\" }" LF
"" LF
"	return __stringify__(Int2Double(n!))" LF
"}" LF
"" LF
"func __stringify__(_ b: Bool?) -> String {" LF
"	if(b == nil) { return \"nil\" }" LF
"" LF
"	if(b!) {" LF
"		return \"true\"" LF
"	} else {" LF
"		return \"false\"" LF
"	}" LF
"}" LF
"" LF
"func __stringify__(_ s: String?) -> String {" LF
"	if(s == nil) { return \"nil\" }" LF
"" LF
"	return s!" LF
"}" LF
"" LF
"func __modulo__(_ a: Double, _ b: Double) -> Int {" LF
"	return Double2Int(a - Int2Double(Double2Int(a / b)) * b)" LF
"}" LF
"" LF
#undef LF
