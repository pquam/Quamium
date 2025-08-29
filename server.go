package main

import (
	"bufio"
	"fmt"
	"log"
	"net"
	"strings"
)

func startServer(addr string) string {

	//disect url
	scheme := "http"

	if strings.Contains(addr, "://") {
		scheme = strings.Split(addr, "://")[0]
		addr = strings.Split(addr, "://")[1]
	}

	host := ""
	if strings.Contains(addr, "/") {
		host = strings.Split(addr, "/")[0]
	} else {
		host = addr
	}

	path := "/"
	if strings.Contains(addr, "/") {
		path += strings.Split(addr, "/")[1]
	}

	port := "80"
	if strings.Contains(host, ":") {
		port = strings.Split(host, ":")[1]
	}
	if scheme == "https" {
		port = "443"
	}

	fmt.Println("Connecting to " + host + "/" + path + " via " + port)

	//connect to socket via http
	s, err := net.Dial("tcp", host+":"+port)
	if err != nil {
		log.Fatal(err)
	}

	//request web page data via http
	request := "GET " + path + " HTTP/1.0\r\nHost: " + addr + "\r\n\r\n"
	fmt.Println("Request: \n" + request)
	s.Write([]byte(request))

	//read response
	response, err := bufio.NewReader(s).ReadString('\n')
	if err != nil {
		log.Fatal(err)
	}

	s.Close()

	return response
}
