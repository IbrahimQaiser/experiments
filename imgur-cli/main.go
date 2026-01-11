//go:build !windows
// +build !windows

package main

import "github.com/ananthvk/imgur-cli/cmd"

func main() {
	cmd.Run()
}
