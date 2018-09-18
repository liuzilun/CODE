package main

import (
	"fmt"
	"time"

	"github.com/garyburd/redigo/redis"
)

var pool *redis.Pool

//初始化一个连接池
func newPool(server, password string) *redis.Pool {
	return &redis.Pool{
		MaxIdle:     64,
		MaxActive:   1000,
		IdleTimeout: 240 * time.Second,
		Dial: func() (redis.Conn, error) {
			c, err := redis.Dial("tcp", server)
			if err != nil {
				fmt.Printf("redis dial failed err:%v", err)
				return nil, err
			}
			/*_, err1 := c.Do("AUTH", password)
			if err1 != nil {
				c.Close()
				fmt.Printf("redis AUTH failed err:%v", err1)
				return nil, err1
			}*/
			return c, nil
		},
		TestOnBorrow: func(c redis.Conn, t time.Time) error {
			if time.Since(t) < time.Minute {
				fmt.Printf("time.Since(t) < time.Minute:%v", t)
				return nil
			}
			_, err := c.Do("PING")
			if err != nil {
				fmt.Printf("PING err:%v", err)
				return err
			}
			return err
		},
	}
}

func main() {

	pool = newPool("192.168.1.106:9001", "")

	for {
		time.Sleep(time.Second)
		conn := pool.Get()

		_, err1 := conn.Do("Set", "abc", 100)
		if err1 != nil {
			fmt.Printf("onn.Do Set failed err:%v", err1)
		}

		r, err := redis.Int(conn.Do("Get", "abd"))
		if err != nil {
			fmt.Println(err)
			continue
		}

		fmt.Printf("get from redis result:%v", r)
	}
}
