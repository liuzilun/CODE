package main

import (
	"fmt"
)

type Animal interface {
	Talk()
	Eat()
	Name() string
}

type Dog struct {
}

func (d Dog) Talk() {
	fmt.Println("汪汪汪")
}
func (d Dog) Eat() {
	fmt.Println("我在吃骨头")
}

func (d Dog) Name() string {
	fmt.Println("我是汪财")

	return " 汪财"
}

func main() {
	var d Dog
	var a Animal
	a = d

	a.Eat()
	a.Name()
	a.Talk()
}
