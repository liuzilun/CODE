package logic

import (
	"encoding/json"
	"fmt"
	"testing"
	"wething/model"
)

func TestPutMsgFromClient(t *testing.T) {

	msginfo := &model.WeThingDetail{}
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
	PutMsgFromClient(string(data))
}
