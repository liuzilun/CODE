package main

import (
	"fmt"

	"github.com/garyburd/redigo/redis"
)

func redisSet() {
	c, err := redis.Dial("tcp", "192.168.1.106:9001")

	if err != nil {
		fmt.Printf("con redis failed ,err:%v \n", err)
		return
	}

	defer c.Close()
	_, err = c.Do("Set", "abc", 100)

	if err != nil {
		fmt.Printf("set failed err:%v \n", err)
		return
	}
	r, err1 := redis.Int(c.Do("Get", "abc"))
	if err != nil {
		fmt.Printf("get abd failed,err: %v\n", err1)
		return
	}
	fmt.Printf("get data from redis: %v\n", r)
	return
}

func MSet() {
	c, err := redis.Dial("tcp", "192.168.1.106:9001")

	if err != nil {
		fmt.Printf("con redis failed ,err:%v \n", err)
		return
	}
	defer c.Close()
	_, err = c.Do("MSet", "abc", 100, "efg", 200)

	if err != nil {
		fmt.Printf("set failed err:%v \n", err)
		return
	}
	r, err1 := redis.Ints(c.Do("MGet", "abc", "efg"))
	if err != nil {
		fmt.Printf("get abd failed,err: %v\n", err1)
		return
	}
	for _, v := range r {
		fmt.Printf("get data from redis: %v\n", v)
	}
	return
}

//缓存超时时间
func expiretime() {
	c, err := redis.Dial("tcp", "192.168.1.106:9001")

	if err != nil {
		fmt.Printf("con redis failed ,err:%v \n", err)
		return
	}
	defer c.Close()
	_, err = c.Do("expire", "abc", 10)

	if err != nil {
		fmt.Printf("set failed err:%v \n", err)
		return
	}
}

//队列
func testQueue() {
	c, err := redis.Dial("tcp", "192.168.1.106:9001")
	if err != nil {
		fmt.Println(err)
		return
	}
	defer c.Close()
	_, err = c.Do("lpush", "book_list", "abd", "ceg", 300)
	if err != nil {
		fmt.Printf("lpush failed %v\n", err)
		return
	}

	r, err1 := redis.String(c.Do("lpop", "book_list"))
	if err1 != nil {
		fmt.Println(err1)
		return
	}
	fmt.Println(r)
}
func main() {
	//redisSet()
	//MSet()
	//expiretime()
	testQueue()
	return
}
