package main

import(
	"fmt"
	_"github.com/go-sql-driver/mysql"
	"github.com/jmoiron/sqlx"
)
type User struct {
	Id      int    `db:"Id"`
	Name    string `db:"Name"`
	Sex     string `db:"Sex"`
	Address string `db:"Address"`
	Phone   string `db:"Phone"`
	Age     int    `db:"Age"`
}

var DB * sqlx.DB


func initDb() error{
	var err error
	dsn := "root:123@tcp(192.168.2.155:3306)/golang_db"
	DB,err =sqlx.Open("mysql",dsn)

	if err!=nil{
		return err
	}

	DB.SetMaxOpenConns(100)
	DB.SetMaxIdleConns(16)

	return nil
}

func testUpdate(){
	sqlstr := "update user_info set name=? where id=?"

	result,err:=DB.Exec(sqlstr,"abc",4)
	if err!=nil{
		fmt.Printf("update failed,err:%v\n",err)
		return
	}
	count,err1:= result.RowsAffected()
	if err!=nil{
		fmt.Printf("result failed,err:%v\n",err1)
		return
    }
	
	fmt.Printf("effect nun:%d\n",count)
	return
}

func testQuery(){
	sqlstr:= "select Id,Name,Sex,Address,Phone,Age from user_info where Id=?"

	var user User

	err:=DB.Get(&user,sqlstr,2)

	if err!=nil{
		fmt.Printf("get failed,err:%v\n",err)
		return
	}

	fmt.Printf("user:%#v\n",user)
	
	return
}

func testQueryMulti(){
	sqlstr:= "select Id,Name,Sex,Address,Phone,Age from user_info  where Id>?"

	var user []User

	err:=DB.Select(&user,sqlstr,1)
	if err!=nil{
		fmt.Printf("get failed,err:%v\n",err)
		return
	}
	fmt.Printf("user:%#v\n",user)
}

func queryDB(name string){

	sqlstr:=fmt.Sprintf("select Id,Name,Age from user_info where Name='%s'",name)
	var user[]User
	err:=DB.Select(&user,sqlstr)
	if err!=nil{
		fmt.Printf("select failed,err:%v\n",err)
		return
	}

	for _,v:=range user{
		fmt.Printf("user:%v\n",v)
	}
}
func sqlInject(){
 queryDB("abc")

}
func main(){
	err:=initDb()
    if err!=nil{
		fmt.Printf("init db failed,err:%v\n",err)
		return
	}

	//testQuery()
	//testQueryMulti()

	//testUpdate()

	sqlInject()
	return
}