package main
import (
	"fmt"
)


func cacl(a,b int)(sum int){
	return a+b
}


func main(){
	sum:=cacl(2,3)

	fmt.Printf("sum:%d\n",sum)
	return
	
}