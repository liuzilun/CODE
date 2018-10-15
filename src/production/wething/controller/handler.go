package controller

import (
	"fmt"
	"strconv"
	"myblog/logic"
	"net/http"

	"github.com/gin-gonic/gin"
)

var (
	uploadConfig map[string]interface{}
)

func IndexHandle(c *gin.Context) {
	articleRecordList, err := logic.GetArticleRecordList(0, 15)

	if err != nil {
		fmt.Printf("get article failed,err:%v \n", err)
		c.HTML(http.StatusInternalServerError, "views/500.html", nil)
		return
	}

	allCategoryList, err := logic.GetAllCategoryList()
	if err != nil {
		fmt.Printf("get category list failed, err:%v\n", err)
	}

	for _, v := range allCategoryList {

		fmt.Printf(" handle get GetAllCategoryList success, %#v\n", v)
	}

	var data map[string]interface{} = make(map[string]interface{}, 20)

	data["article_list"] = articleRecordList
	data["category_list"] = allCategoryList

	c.HTML(http.StatusOK, "views/index.html", data)
}

func NewArticle(c *gin.Context) {

	allCategoryList, err := logic.GetAllCategoryList()
	if err != nil {
		fmt.Printf("get category list failed, err:%v\n", err)
		c.HTML(http.StatusInternalServerError, "views/500.html", nil)
	}

	c.HTML(http.StatusOK, "views/post_article.html", allCategoryList)
}


func ArticleSubmit(c *gin.Context){
	content:=c.PostForm("content")
	author:=c.PostForm("author")
	categoryIdstr:= c.PostForm("category_id")
	title:= c.PostForm("title")

	categoryId,err := strconv.ParseInt(categoryIdstr,10,64)
	if err != nil {
		fmt.Printf("get category list failed, err:%v\n", err)
		c.HTML(http.StatusInternalServerError, "views/500.html", nil)
	}
	err = logic.InsertArticle(content,author,title,categoryId)

	if err != nil {
		fmt.Printf("get category list failed, err:%v\n", err)
		c.HTML(http.StatusInternalServerError, "views/500.html", nil)
	}

	c.Redirect(http.StatusMovedPermanently,"/")





}

func ArticleDetail(c *gin.Context){

	articleIdStr := c.Query("article_id")
	articleId, err := strconv.ParseInt(articleIdStr, 10, 64)
	if err != nil {
		c.HTML(http.StatusInternalServerError, "views/500.html", nil)
		return
	}

	articleDetail, err := logic.GetArticleDetail(articleId)
	if err != nil {
		fmt.Printf("get article detail failed,article_id:%d err:%v\n", articleId, err)
		c.HTML(http.StatusInternalServerError, "views/500.html", nil)
		return
	}

	fmt.Printf("article detail:%#v\n", articleDetail)

	relativeArticle, err := logic.GetRelativeAricleList(articleId)
	if err != nil {
		fmt.Printf("get relative article failed, err:%v\n", err)
	}

	prevArticle, nextArticle, err := logic.GetPrevAndNextArticleInfo(articleId)
	if err != nil {
		fmt.Printf("get prev or next article failed, err:%v\n", err)
	}

	allCategoryList, err := logic.GetAllCategoryList()
	if err != nil {
		fmt.Printf("get all category failed, err:%v\n", err)
	}

	commentList, err := logic.GetCommentList(articleId)
	if err != nil {
		fmt.Printf("get comment list failed, err:%v\n", err)
	}

	fmt.Printf("relative article size:%d article_id:%d\n", len(relativeArticle), articleId)
	var m map[string]interface{} = make(map[string]interface{}, 10)
	m["detail"] = articleDetail
	m["relative_article"] = relativeArticle
	m["prev"] = prevArticle
	m["next"] = nextArticle
	m["category"] = allCategoryList
	m["article_id"] = articleId
	m["comment_list"] = commentList

	c.HTML(http.StatusOK, "views/detail.html", m)
}