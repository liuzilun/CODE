package db

import (
	"fmt"
	"wething/model"
)

func InsertMsg(msginfo *model.WeThingDetail) (weid int64, err error) {

	sqlstr := "insert into wethinginfo(user_id,category_id,title,msg_content,pub_location,weather) values(?,?,?,?,?,?)"
	result, err := DB.Exec(sqlstr, msginfo.UserId, msginfo.WeThingInfo.CategoryId, msginfo.Title, msginfo.MsgContent, msginfo.PubLocation, msginfo.Weather)
	if err != nil {
		fmt.Printf("insert WeThingInfo failed err:%v", err)
		return -1, err
	}
	weid, err1 := result.LastInsertId()
	if err1 != nil {
		fmt.Printf("insert WeThingInfo get LastInsertId failed err:%v", err)
		return -1, err1
	}
	return weid, nil
}
