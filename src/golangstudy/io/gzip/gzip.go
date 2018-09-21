package main

import (
	"compress/gzip"
	"fmt"
	"io"
	"os"
)

func main() {

	file, err := os.Open("./ceshi.gzip")
	if err != nil {
		return

	}
	defer file.Close()

	reader, err := gzip.NewReader(file)

	if err != nil {
		fmt.Printf("读取失败%v", err)
		return
	}

	var content []byte
	var buf [128]byte

	for {
		n, err := reader.Read(buf[:])
		if err == io.EOF {
			break
		}
		if err != nil {
			fmt.Println("read file:", err)
			return
		}

		content = append(content, buf[:n]...)
	}

	//
	fmt.Println("read data,", content)
}
