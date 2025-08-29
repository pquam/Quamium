package main

import (
	"bufio"
	"crypto/tls"
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

type getRequest struct {
	get        string
	host       string
	userAgent  string
	connection string
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
		path = strings.TrimRight(addr, "/")
	}

	//todo Check if host/path is cached before connecting to server

	port := "80"
	if strings.Contains(host, ":") {
		port = strings.Split(host, ":")[1]
	}
	if scheme == "https" {
		port = "443"
	}

	fmt.Println("Connecting to " + path + " via " + port)

	//connect to socket via http
	var conn net.Conn
	var err error

	if scheme == "https" {
		conn, err = tls.Dial("tcp", net.JoinHostPort(host, port), &tls.Config{
			ServerName: host, // SNI + cert verification
		})
	} else {
		conn, err = net.Dial("tcp", net.JoinHostPort(host, port))
	}
	if err != nil {
		log.Fatal(err)
	}

	//request web page data via http
	//request := "GET " + scheme + "://" + path + " HTTP/1.0\r\nHost: " + host + "\r\nUser-Agent: PatrickQuam (X11; Ubuntu; Linux x86_64; rv:142.0) Gecko/20100101 Quamium" + "\r\n\r\n"

	fmt.Println("Request: \n" + get(scheme, host, path))
	conn.Write([]byte(get(scheme, host, path)))

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

func get(scheme string, host string, path string) string {

	var myRequest getRequest = getRequest{
		"GET " + scheme + "://" + path + " HTTP/1.1\r\n",
		"Host: " + host + "\r\n",
		"User-Agent: PatrickQuam (X11; Ubuntu; Linux x86_64; rv:142.0) Gecko/20100101 Quamium\r\n",
		"connection: close\r\n\r\n",
	}

	return myRequest.get + myRequest.host + myRequest.userAgent + myRequest.connection
}
