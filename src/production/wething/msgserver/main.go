package main

import (
	"fmt"
	"time"
	"wething/msgserver/msgserver"
)

func main() {

	lifetime := time.Second * 0
	var pMsg *msgserver.MqMsqHand

	pMsg = &msgserver.MqMsqHand{}
	pMsg.Init()
	pMsg.MsgConsume()

	if lifetime > 0 {
		fmt.Printf("running for %s", lifetime)
		time.Sleep(lifetime)
	} else {
		fmt.Printf("running forever")
		select {}
	}

}
