package main

import (
	"encoding/json"
	"fmt"
	"time"
)

type WeThingInfo struct {
	Id          int64     `db:"id"`
	UserId      int64     `db:"user_id"`
	Title       string    `db:"title"`
	CategoryId  int64     `db:"category_id"`
	PubLocation string    `db:"pub_location"`
	CreateTime  time.Time `db:"create_time"`
	Weather     string    `db:"weather"`
}

type WeThingDetail struct {
	WeThingInfo
	MsgContent string `db:"content"`
	Category
}

type WeThingRecord struct {
	WeThingInfo
	Category
}

type UserInfo struct {
	UserId      int64
	UserName    string
	NikeName    string
	Sex         string
	Age         int
	icon        string
	MobilePhone string
}

type Category struct {
	CategoryId   int64  `db:"id"`
	CategoryName string `db:"category_name"`
	CategoryNo   int    `db:"category_no"`
}

func main() {

	msginfo := &WeThingDetail{}
	msginfo.MsgContent = "go 同一个package下不同文件中函数调用报未定义问题,"

	msginfo.UserId = 20181007
	msginfo.WeThingInfo.CategoryId = 3
	msginfo.Title = "测试标题"
	msginfo.PubLocation = "深圳宝安"
	msginfo.Weather = "多云转晴"

	data, err := json.Marshal(msginfo)
	if err != nil {
		fmt.Println("json marshal failed")
		return
	}

	fmt.Printf("json:%s\n", string(data))

	//json反序列化
	fmt.Println("unmarshal result is \n\n")
	msginfo1 := &WeThingDetail{}
	err = json.Unmarshal([]byte(data), msginfo1)
	if err != nil {
		fmt.Println("unmarhsal failed")
		return
	}
	fmt.Printf("c1:%#v\n", msginfo1)

}
