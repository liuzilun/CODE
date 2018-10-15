package main

import "fmt"
import "strings"
import "time"

func Adder() func(int) int {
	var x int
	return func(d int) int {
		x += d
		return x
	}
}

func testClosure() {
	f := Adder()
	ret := f(1)
	fmt.Printf("ret = %d\n ", ret)
	ret = f(20)
	fmt.Printf("ret = %d\n ", ret)
	ret = f(100)
	fmt.Printf("ret = %d\n ", ret)

	f1 := Adder()
	ret = f1(1)
	fmt.Printf("ret = %d\n ", ret)
}

func makeSuffixFunc(suffix string) func(string) string {
	return func(name string) string {
		if !strings.HasSuffix(name, suffix) {
			return name + suffix
		}
		return name
	}
}

func testClouse3() {
	func1 := makeSuffixFunc(".bmp")
	func2 := makeSuffixFunc(".jpg")

	fmt.Println(func1("test1"))
	fmt.Println(func2("test2"))

}

func calc(base int) (func(int) int, func(int) int) {
	add := func(i int) int {
		base += i
		return base
	}
	sub := func(i int) int {
		base -= i
		return base
	}
	return add, sub
}

func gorutingTest() {
	for i := 0; i < 5; i++ {
		go func(index int) {
			fmt.Println(index)
		}(i)

	}
	time.Sleep(time.Second)

}
func testcalc() {
	func1, func2 := calc(10)

	fmt.Println(func1(1), func2(2))
	fmt.Println(func1(3), func2(4))
	fmt.Println(func1(5), func2(6))
	fmt.Println(func1(7), func2(8))
	fmt.Println(func1(9), func2(10))
}
func main() {
	//testClosure()
	//testClouse3()
	//testcalc()
	gorutingTest()
}
