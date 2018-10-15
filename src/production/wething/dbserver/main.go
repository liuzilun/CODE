package main

import (
	"fmt"
	"wething/dbserver/db"
	"wething/dbserver/dbgrpc"
)

func main() {

	//初始化db
	dns := "root:123@tcp(192.168.1.109:3306)/wething?parseTime=true"
	err := db.Init(dns)
	if err != nil {
		panic(err)
	}
	fmt.Printf("DB: init sucess dns:%v \n", dns)

	//初始化grpc
	err = protodbserver.Init()
	if err != nil {
		fmt.Printf("init grpc server failed err:%v \n!", err)
		return
	}
	fmt.Printf("init grpc server success \n!")

}
