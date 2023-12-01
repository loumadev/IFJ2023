// func foo() -> Void  {
//   write("Hello, World!")
// }
//
// foo()

func bar(_ n : Int) -> Int {
    if (n == 0) {
        return 0
    } else {
        return n + bar(n - 1)
    }
}

let a = bar(3)
write("Result of bar: ", a, "\n")

// func sum(_ a: Int, _ b: Int) -> Int {
//   return a + b
// }
//
// let resultOfSum = sum(1, 2)
// write("Result of sum: ", resultOfSum, "\n")