package main

import (
	"fmt"
	"os"
	"bufio"
	"io/ioutil"
)

/*
func ioutil() {

	content, err := ioutil.ReadFile("./io.go")
	if err != nil {
		fmt.Println("read file failed, err:", err)
		return
	}

	fmt.Println(string(content))
}*/
func main() {
	//var buf [16]byte

	//os.Stdin.Read(buf[:])

	//fmt.Println(string(buf[:]))
	//os.Stdout.WriteString(string(buf[:]))

	//sIoProc()
	/*
		content, err := ioutil.ReadFile("./io.go")
		if err != nil {
			fmt.Println("read file failed, err:", err)
			return
		}

		fmt.Println(string(content))
		return
	*/
	//fwrite()
	buffioWrite()

	writeioutil() 
}

//文件操作
/*func IoProc() {
	file, err := os.Open("./io.go")
	if err != nil {
		fmt.Printf("open file failed %v\n", err)
		return
	}
	defer file.Close()
	var content []byte
	var buf [128]byte

	for {

		n, err := file.Read(buf[:])
		if err == io.EOF {
			break

		}
		if err != nil {
			fmt.Println("read falied,", err)
			return
		}
		content = append(content, buf[:n]...)

	}

	fmt.Println("data:", string(content))
}
*/
//ioutil操作文件

func fwrite() {
	file, err := os.OpenFile("./test.dat", os.O_CREATE|os.O_WRONLY, 0666)
	if err != nil {
		fmt.Printf("open failed,%v", err)
		return
	}
	defer file.Close()
	str := "hello world"
	file.Write([]byte(str))
	file.WriteString(str)

}

func buffioWrite(){
	
	file, err := os.OpenFile("./test1.dat", os.O_CREATE|os.O_WRONLY, 0666)
	if err != nil {
		fmt.Printf("open failed,%v", err)
		return
	}
	writer:= bufio.NewWriter(file)
	
	for i :=0;i<10;i++{
		writer.WriteString("hello world")
	}
	
	writer.Flush()
	defer file.Close()
	str := "hello world"
	file.Write([]byte(str))
	file.WriteString(str)
}


func writeioutil() {

	str := "hello world\n"
	err := ioutil.WriteFile("./test1.dat",[]byte(str),0755)
	if err != nil {
		fmt.Println("write file failed, err:", err)
		return
	}
}