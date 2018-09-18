package main

import (
	"fmt"
	"html/template"
	"net/http"
	"os"
)

type Address struct {
	City     string
	Province string
}

type UserInfo struct {
	Name string
	Sex  string
	Age  int
	Address
}

func index(w http.ResponseWriter, r *http.Request) {
	//method := r.Method

	// if method == "GET" {

	// 	t, err := template.ParseFiles("./login.html")
	// 	if err != nil {
	// 		fmt.Fprint(w, "load login.html failed\n")
	// 		return
	// 	}
	// 	err1 := t.Execute(w, nil)

	// } else if method == "POST" {

	// 	r.ParseForm()

	// }
	t, err := template.ParseFiles("./index.html")

	if err != nil {
		fmt.Printf("load index.html failed")
		return
	}

	user := UserInfo{
		Name: "Mary",
		Sex:  "男",
		Age:  18,
		Address: Address{
			City:     "北京",
			Province: "北京市",
		},
	}

	//m := make(map[string]interface{})

	//m["Name"] = "Mary"
	//m["Sex"] = "男"
	//m["Age"] = 19

	t.Execute(w, user)

	t.Execute(os.Stdout, user)

}
func main() {
	http.HandleFunc("/index", index)
	err := http.ListenAndServe("127.0.0.1:9090", nil)

	fmt.Printf("http.HandleFunc %v ", err)
	if err != nil {
		fmt.Printf("linten server failed,err:%v\n", err)
		return
	}
}
