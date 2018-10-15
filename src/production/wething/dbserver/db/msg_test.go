package db

import (
	"fmt"
	"testing"
	"wething/model"
)

func init() {
	dns := "root:123@tcp(192.168.1.109:3306)/wething?parseTime=true"

	err := Init(dns)
	if err != nil {
		panic(err)
	}

	fmt.Printf("TestInsertMsg: init sucess dns:%v", dns)
}

/*
type WeThingInfo struct {
	Id          int64     `db:"id"`
	UserId      int64     `db:"user_id"`
	Title       string	  `db:"title"`
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
*/

func TestInsertMsg(t *testing.T) {

	msg := model.WeThingDetail{}

	msg.UserId = 20181007
	msg.WeThingInfo.CategoryId = 3
	msg.WeThingInfo.Title = "测试标题"
	msg.MsgContent = "go 同一个package下不同文件中函数调用报未定义问题,"
	msg.PubLocation = "深圳宝安"
	msg.Weather = "多云转晴"
	weid, err := InsertMsg(msg)
	if err != nil {
		fmt.Printf("TestInsertMsg: insertMsg failed err:%v", err)
		return
	}
	fmt.Printf("TestInsertMsg: insertMsg sucess weid:%v", weid)
}
