package main

import (
	"log"
	"os"

	"gioui.org/app"
)

func main() {

	/*
		url := ""
		//get url from user
		fmt.Println("Input URL or QUIT:")

		fmt.Scanln(&url)

		fmt.Println("Data from URL: ", url)

		//start server
		response := startServer(url)

		//parse html
		body := parseHTML(response.body)

	*/

	go func() {
		window := new(app.Window)
		window.Option(app.Title("Quamium"))
		err := run(window)
		if err != nil {
			log.Fatal(err)
		}
		os.Exit(0)
	}()
	app.Main()

}
