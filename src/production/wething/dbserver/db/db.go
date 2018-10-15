package db

import (
	"fmt"

	_ "github.com/go-sql-driver/mysql"

	"github.com/jmoiron/sqlx"
)

var (
	DB *sqlx.DB
)

func Init(dns string) error {
	var err error
	DB, err = sqlx.Open("mysql", dns)
	if err != nil {
		fmt.Printf("Open database failed,err:%v", err)
		return err
	}

	err = DB.Ping()
	if err != nil {
		fmt.Printf("Open database failed,err:%v", err)
		return err
	}

	DB.SetMaxOpenConns(100)
	DB.SetMaxIdleConns(16)
	return nil
}
