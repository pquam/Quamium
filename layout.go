package main

import (
	"strings"

	"gioui.org/font"
)

type displayList []interface{}

type Word struct {
	text     string
	style    string
	fontface font.Font
	height   int
	width    int
	header   int
	weight   string
	slant    int
	xpos     int
	ypos     int
}

const (
	HSTEP  = 13
	VSTEP  = 18
	WIDTH  = 1024
	HEIGHT = 768
)

var cursor_x int = HSTEP
var cursor_y int = VSTEP
var weight string = "normal"
var style string = "normal"
var header int = 0
var fontface font.Typeface = font.Typeface("roman")

func token_layout(tokens []interface{}) *displayList {

	display_list := displayList{}

	for _, tok := range tokens {

		if isinstance(tok) {

			text := strings.Fields(tok.(text).text)

			for _, word := range text {

				if cursor_x >= WIDTH-HSTEP {
					cursor_x = HSTEP
					cursor_y += VSTEP
				}

				display_list = append(display_list, Word{
					text:     word,
					style:    style,
					fontface: font.Font{Typeface: fontface},
					header:   header,
					height:   16,
					width:    16,
					weight:   weight,
					slant:    0,
					xpos:     cursor_x,
					ypos:     cursor_y,
				})

				cursor_x += 100
			}

		} else {

		}

	}

	return &display_list

}

// toktype parameter is for extensibility but is not currently required
func isinstance(tok interface{}) bool {

	switch tok := tok.(type) {
	case tag:
		if tok.tag == "i" {
			style = "italic"
		}
		if tok.tag == "/i" {
			style = "normal"
		}
		if tok.tag == "b" {
			weight = "bold"
		}
		if tok.tag == "/b" {
			weight = "normal"
		}
		if tok.tag == "h1" {
			header = 1
		}
		if tok.tag == "/h1" {
			header = 0
		}
		return false

	case text:
		return true

	default:
		return true
	}
}
