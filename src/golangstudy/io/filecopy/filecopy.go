package main

import (
	"bufio"
	"fmt"
	"io"
	"os"
)

func main() {

	//CopyFile("t.txt","test1.dat")
}

func CopyFile(dstName, srcName string) (written int64, err error) {
	src, err := os.Open(srcName)
	if err != nil {
		return
	}
	defer src.Close()
	dst, err1 := os.OpenFile(dstName, os.O_WRONLY|os.O_CREATE, 0664)

	if err1 != nil {
		return
		fmt.Println("open failed err:\n", err1)
	}
	defer dst.Close()
	return io.Copy(dst, src)
}

func cat(r *bufio.Reader) {
	for {
		buf, err := r.ReadBytes('\n')
		if err == io.EOF {
			break
		}
		fmt.Fprintf(os.Stdout, "%s", buf)
	}
}
