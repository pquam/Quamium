package main

import (
	"bufio"
	"fmt"
	"io"
	"log"
	"net"
	"strings"
)

type response struct {
	url     string
	status  string
	headers string
	body    string
}

func startServer(addr string) response {

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
		path += strings.TrimRight(addr, "/")
	}

	//todo Check if host/path is cached before connecting to server

	port := "80"
	if strings.Contains(host, ":") {
		port = strings.Split(host, ":")[1]
	}
	if scheme == "https" {
		port = "443"
	}

	fmt.Println("Connecting to " + host + path + " via " + port)

	//connect to socket via http
	conn, err := net.Dial("tcp", host+":"+port)
	if err != nil {
		log.Fatal(err)
	}

	//request web page data via http
	request := "GET " + path + " HTTP/1.0\r\nHost: " + host + "\r\n\r\n"
	fmt.Println("Request: \n" + request)
	conn.Write([]byte(request))

	//read response
	resp := bufio.NewReader(conn)

	status, err := resp.ReadString('\n')
	if err != nil {
		log.Fatal(err)
	}
	fmt.Println("Status:\n" + status + "\nHeaders: \n")

	headers := ""
	for {
		line, _ := resp.ReadString('\n')
		if line == "\r\n" {
			break
		}
		headers += line + "\n"
		fmt.Print(line)
	}

	body, err := io.ReadAll(resp)
	if err != nil {
		log.Fatal(err)
	}

	fmt.Println("\nBody:\n")

	conn.Close()

	return response{
		host + "/" + path,
		status,
		headers,
		string(body),
	}
}
