package main

import (
	"fmt"
	"html/template"
	"net/http"
)

func login(w http.ResponseWriter, r *http.Request) {
	method := r.Method
	fmt.Printf("request method is %v \n", method)

	if method == "GET" {

		fmt.Printf("request method before ParseFiles  %v\n ", method)
		t, err := template.ParseFiles("./login.html")
		if err != nil {
			fmt.Fprint(w, "load login.html failed\n")
			return
		}

		fmt.Printf("request method after ParseFiles %v \n", err)
		err1 := t.Execute(w, nil)

		fmt.Printf("Execute method  %v \n", err1)

	} else if method == "POST" {

		fmt.Printf("request method  is not get\n", method)
		r.ParseForm()
		fmt.Printf("username:%s\n", r.FormValue("username"))

		fmt.Printf("passwd:%s\n", r.FormValue("password"))

		fmt.Fprint(w, "username:$s login sucess\n", r.FormValue("username"))

	}

}
func main() {
	http.HandleFunc("/login", login)
	err := http.ListenAndServe("127.0.0.1:9090", nil)

	fmt.Printf("http.HandleFunc %v ", err)
	if err != nil {
		fmt.Printf("linten server failed,err:%v\n", err)
		return
	}
}
