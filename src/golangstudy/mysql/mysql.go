package main

import (
	"database/sql"
	"fmt"
	_ "github.com/go-sql-driver/mysql"
)

var DB *sql.DB

func initDB() error {
	var err error

	dns := "root:123@tcp(192.168.2.155:3306)/golang_db"
	DB, err = sql.Open("mysql", dns)

	if err != nil {
		fmt.Printf("open database failed,%v", err)
		return err
	}
	DB.SetMaxIdleConns(100)
	DB.SetMaxOpenConns(16)
	return nil
}

type User struct {
	Id      int    `db:"Id"`
	Name    string `db:"Name"`
	Sex     string `db:"Sex"`
	Address string `db:"Address"`
	Age     int    `db:"Age"`
	Phone   string `db:Phone`
}

func testQueryData() {
	//for i := 0; i < 1000; i++ {
	sqlstr := "select Id,Name,Age,Sex,Address,Phone from user_info where Id=?"

	var user User
	row := DB.QueryRow(sqlstr, 1)

	// if nil != row {
	fmt.Printf("QueryRow database\n")
	// 	continue
	// }
	//查询出来一定要Scan使用并关闭
	err := row.Scan(&user.Id, &user.Name, &user.Age, &user.Sex, &user.Address, &user.Phone)
	if err != nil {
		fmt.Printf("Scan database failed,err:%v\n", err)
		return
	}
	fmt.Printf("id:%d  name:%s Age:%d  Sex:%s Address:%s Phone:%s\n", user.Id, user.Name, user.Age, user.Sex, user.Address, user.Phone)
	//}
}

func testQueryMultiRow() {
	sqlstr := "select Id,Name,Age,Sex,Address,Phone from user_info where Id>?"

	fmt.Printf("QueryMutiRows database \n")
	rows, err := DB.Query(sqlstr, 0)
	//rows 对象一定是要关闭
	defer func() {
		if rows != nil {
			rows.Close()
		}
	}()

	if err != nil {
		fmt.Printf("QueryRow database failed,row:%v\n", rows)
	}
	var user User
	for rows.Next() {

		err := rows.Scan(&user.Id, &user.Name, &user.Age, &user.Sex, &user.Address, &user.Phone)
		if err != nil {
			fmt.Printf("Scan database failed,err:%v\n", err)
			return
		}
		fmt.Printf("id:%d  name:%s Age:%d  Sex:%s Address:%s Phone:%s\n", user.Id, user.Name, user.Age, user.Sex, user.Address, user.Phone)
	}

}

func testInsertData() {
	sqlstr := "insert into user_info(Name,Age,Sex,Address,Phone) values(?,?,?,?,?)"

	result, err := DB.Exec(sqlstr, "tom", 18, "男", "shenzhchongqint", "1234565452")

	if err != nil {
		fmt.Printf("insert failed,err:%v\n", err)
		return
	}
	id, err := result.LastInsertId()
	if err != nil {
		fmt.Printf("get last insert id failed\n")
		return
	}
	fmt.Printf("insert success in is : %d\n", id)
}

func testUpdatetData() {
	sqlstr := "update user_info set Name=?,Age=?,Sex=?,Address=?,Phone=? where id >?"

	result, err := DB.Exec(sqlstr, "刘子伦", 25, "男", "重庆荣昌", "11111111111", 2)

	if err != nil {
		fmt.Printf("update failed,err:%v\n", err)
		return
	}
	affected, err := result.RowsAffected()
	if err != nil {
		fmt.Printf("get affect rows failed err:%v \n", err)
		return
	}
	fmt.Printf("update success affect rows: %d\n", affected)
}

func testsqlPrepare() {
	sqlstr := "select Id,Name,Age,Sex,Address,Phone from user_info where Id>?"
	stmt, err := DB.Prepare(sqlstr)
	if err != nil {
		fmt.Printf("prepare failed,err:%v\n", err)
		return
	}

	defer func() {
		if stmt != nil {
			stmt.Close()
		}

	}()
	rows, err1 := stmt.Query(0)

	//rows 对象一定是要关闭
	defer func() {
		if rows != nil {
			rows.Close()
		}
	}()

	if err1 != nil {
		fmt.Printf("stmtQueryRow database failed,row:%v\n", rows)
	}
	var user User
	for rows.Next() {

		err = rows.Scan(&user.Id, &user.Name, &user.Age, &user.Sex, &user.Address, &user.Phone)
		if err != nil {
			fmt.Printf("stmt Scan database failed,err:%v\n", err)
			return
		}
		fmt.Printf("id:%d  name:%s Age:%d  Sex:%s Address:%s Phone:%s\n", user.Id, user.Name, user.Age, user.Sex, user.Address, user.Phone)
	}
}

//事务测试
func testTrans() {
	conn, err := DB.Begin()
	if err != nil {
		if conn != nil {
			conn.Rollback()
		}
		fmt.Printf("begin failed,err:%v\n", err)
		return
	}

	sqlstr := "update user_info set age=100 where id = ?"
	_, err = conn.Exec(sqlstr, 5)
	if err != nil {
		conn.Rollback()
		fmt.Printf("exec sql:%s failed,err:%v\n", sqlstr, err)
		return
	}
	sqlstr = "update user_info dset age=102 where id = ?"
	_, err = conn.Exec(sqlstr, 4)
	if err != nil {
		conn.Rollback()
		fmt.Printf("exec sql:%s secone failed,err:%v\n", sqlstr, err)
		return
	}
	err = conn.Commit()
	if err != nil {
		fmt.Printf("commit failed ,err:%s,err:%v", sqlstr, err)
		conn.Rollback()
	}
	return
}
func main() {
	err := initDB()
	if err != nil {
		fmt.Printf("init db failed,err:%v\n", err)
		return
	}
	//testQueryData()

	//testQueryMultiRow()

	///testInsertData()

	//testUpdatetData()

	//testsqlPrepare()

	testTrans()
	return
}
