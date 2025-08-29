package main

import "fmt"

func main() {

	url := ""
	for {
		//get url from user
		fmt.Println("Input URL or QUIT:")

		fmt.Scanln(&url)

		if url == "QUIT" {
			break
		}

		fmt.Println("Data from URL: ", url)

		//start server
		response := startServer(url)

		//parse html
		//body := parseHTML(responce)

		//print body
		fmt.Println(response.body)
	}

}
