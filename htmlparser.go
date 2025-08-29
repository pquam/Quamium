package main

import "strings"

func parseHTML(html string) string {

	inTag := false
	var body strings.Builder

	for _, c := range html {
		if c == '<' {
			inTag = true
		} else if c == '>' {
			inTag = false
		} else if !inTag {
			body.WriteRune(c)
		}
	}

	result := body.String()

	return result
}
