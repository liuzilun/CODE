package main

import (
	"fmt"

	"github.com/garyburd/redigo/redis"
)

func main() {

	c, err := redis.Dial("tcp", "192.168.1.106:9001")
	if err != nil {
		fmt.Printf("connect redis failed %v \n", err)
		return
	}

	defer c.Close()

	_, err = c.Do("HSet", "books", "abd", 100)
	if err != nil {
		fmt.Printf("Do hash failed err :%v\n", err)
		return
	}
	r, err1 := redis.Int(c.Do("HGet", "books", "abd"))
	if err1 != nil {
		fmt.Println("get abd failed,", err)
		return
	}
	fmt.Println(r)
	return
}
