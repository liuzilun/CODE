package main

import (
	"bufio"
	"fmt"
	"os"
)

func main() {
	//var a int
	//var b string
	//var c float32

	//fmt.Fscanf(os.Stdin, "%d%s%f", a, b, c)

	//fmt.Println(a, b, c)

	//fmt.Fprintf(os.Stdout, "stdout %d %s %f\n", a, b, c)

	bufioRead()
}

//bufio 带缓冲的读取
func bufioRead() {
	//var inputReader *bufio.Reader
	//var input string
	var err error

	reader := bufio.NewReader(os.Stdin)

	input, err := reader.ReadString('\n')
	fmt.Println("read from connsole", input, err)
}
