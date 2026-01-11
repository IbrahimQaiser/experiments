//go:build windows
// +build windows

package main

import (
	"os"

	"github.com/ananthvk/imgur-cli/cmd"
	"golang.org/x/sys/windows"
)

func main() {

	stdout := windows.Handle(os.Stdout.Fd())
	var originalMode uint32

	windows.GetConsoleMode(stdout, &originalMode)
	windows.SetConsoleMode(stdout, originalMode|windows.ENABLE_VIRTUAL_TERMINAL_PROCESSING)
	cmd.Run()
}
