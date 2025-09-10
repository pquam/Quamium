package main

import (
	"strings"
)

type tag struct {
	tag string
}

type text struct {
	text string
}

func lex(html string) []interface{} {

	out := []interface{}{}

	inTag := false

	var tag tag
	var text text
	var buffer strings.Builder

	for _, c := range html {
		if c == '<' {
			inTag = true
			tag.tag = buffer.String()
			out = append(out, tag)
			buffer.Reset()
		} else if c == '>' {
			inTag = false
			text.text = buffer.String()
			out = append(out, text)
		} else if !inTag {
			buffer.WriteRune(c)
		}
	}

	if !inTag && (buffer.Len() > 0) {
		text.text = buffer.String()
		out = append(out, text)
	}

	return out
}
