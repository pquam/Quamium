package main

import "strings"

func parseHTML(html string) []string {
	//parse html
	body := strings.Split(html, "<")

	return body
}
