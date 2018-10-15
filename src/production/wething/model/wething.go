package model

import "time"

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
