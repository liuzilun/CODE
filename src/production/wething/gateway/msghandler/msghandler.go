package msghandler

import (
	"wething/gateway/logic"

	"github.com/gin-gonic/gin"
)

////测试数据传送
func GetTestApiHtml(c *gin.Context) {

	username := c.Query("username")
	address := c.Query("address")

	c.JSON(200, gin.H{
		"message":  "pong",
		"username": username,
		"address":  address,
	})
}

func GetMsg(c *gin.Context) {
	//调用Logic模块获取数据

}

func PostMsg(c *gin.Context) {
	//调用Logic模块保存数据
	//获取json字符串，直接传
	// 后续注入MQ的发消息处理实列进来
	logic.PutMsgFromClient(msg)
}
