package main

import (
	"strings"

	"gioui.org/app"
	"gioui.org/layout"
	"gioui.org/op"
	"gioui.org/widget"
	"gioui.org/widget/material"
)

func load(w *app.Window) error {

	theme := material.NewTheme()

	var ops op.Ops

	var searchButton widget.Clickable

	var searchBar widget.Editor

	for {

		switch e := w.Event().(type) {

		case app.FrameEvent:
			gtx := app.NewContext(&ops, e)

			if searchButton.Clicked(gtx) {

				// Read from the input
				inputString := searchBar.Text()
				inputString = strings.TrimSpace(inputString)

				// body = url.request()
				body := startServer(inputString).body

				// tokens = lex(body)
				tokens := lex(body)

				// self.display_list = Layout(tokens).display_list
				display_list := token_layout(tokens)

				content := material.H1(theme, "Word")

				for _, Word := range *display_list {
					content = material.H1(theme, Word.(text).text)
				}

				// self.draw()
				content.Layout(gtx)

			}

			layout.Flex{
				// Vertical alignment, from top to bottom
				Axis: layout.Vertical,
				// Empty space is left at the start, i.e. at the top
				Spacing: layout.SpaceStart,
			}.Layout(gtx)

			e.Frame(gtx.Ops)

		// this is sent when the application is closed.
		case app.DestroyEvent:
			return e.Err
		}

	}
}
