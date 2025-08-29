package main

import "fmt"

func main() {
	//get url from user
	fmt.Println("Input URL:")
	var url string
	fmt.Scanln(&url)

	fmt.Println("Data from URL: ", url)

	//start server
	responce := startServer(url)

	//parse html
	//body := parseHTML(responce)

	//print body
	fmt.Println(responce.body)
}
