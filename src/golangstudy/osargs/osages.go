package main

import (
	"flag"
	"fmt"
	"os"

	"github.com/urfave/cli"
)

var recusive bool
var test string
var level int

func init() {
	flag.BoolVar(&recusive, "r", false, "recusive xxx")
	flag.StringVar(&test, "t", "default string", "string option")
	flag.IntVar(&level, "1", 1, "level of xxxx")

	flag.Parse()
}

func main() {
	/*	fmt.Println("args[0]=", os.Args[0])

		if len(os.Args) > 1 {
			for index, v := range os.Args {
				if index == 0 {
					continue
				}
				fmt.Printf("args[%d]=%v \n", index, v)
			}
		}
	*/
	/*
		fmt.Printf("recusive:%v\n", recusive)
		fmt.Printf("test:%v\n", test)
		fmt.Printf("level:%v\n", level)
	*/

	cli_example()
	return
}

func flag_argus() {
	fmt.Printf("recusive:%v\n", recusive)
	fmt.Printf("")
}

func cli_example() {

	app := cli.NewApp()
	app.Name = "greet"
	app.Usage = "fight the lone liness!"
	app.Action = func(c *cli.Context) error {
		fmt.Println("Hello friend!")
		return nil
	}

	app.Run(os.Args)

}
