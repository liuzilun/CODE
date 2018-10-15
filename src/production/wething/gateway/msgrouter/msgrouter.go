package msgrouter

import (
	"wething/gateway/msghandler"

	"github.com/gin-gonic/gin"
)

func Gatewayinit() {

	router := gin.Default()

	router.Static("/static/", "./static")
	router.LoadHTMLGlob("views/*")

	router.GET("/", msghandler.GetTestApiHtml)

	router.GET("/msg", msghandler.GetMsg)
	router.POST("/msg", msghandler.PostMsg)

	//router.GET("/api/xmldata", GetXmlData)
	//router.GET("/api/yamldata", GetYamlData)
	//router.GET("/api/paramsdata", GetParamsJsonData)

	//router.GET("/", controller.IndexHandle)

	router.Run("127.0.0.1:8080")
}
