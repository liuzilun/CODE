package main

import (
	"myblog/controller"
	"myblog/dal/db"

	"github.com/gin-gonic/gin"
)

func main() {

	router := gin.Default()

	dns := "root:123@tcp(192.168.1.109:3306)/myblog?parseTime=true"

	err := db.Init(dns)
	if err != nil {
		panic(err)
	}

	router.Static("/static/", "./static")
	router.LoadHTMLGlob("views/*")

	router.GET("/", controller.IndexHandle)

	//发布文章页面
	router.GET("/article/new/", controller.NewArticle)
	//文章提交接口
	router.POST("/article/submit/", controller.ArticleSubmit)

	//文章
	router.GET("/article/detail/", controller.ArticleDetail)

	//router.POST("/upload/file/",controller.UploadFile)

	router.Run("192.168.1.109:8080")
}
