package cmd

import (
	"flag"
	"fmt"
	"os"

	"github.com/ananthvk/imgur-cli/internal"
	"github.com/ananthvk/imgur-cli/internal/color"
)

var writer = flag.CommandLine.Output()
var noColor = false

func ColorError() {
	if !noColor {
		fmt.Fprint(writer, color.BrightRedBold)
	}
}

func ColorHighlight() {
	if !noColor {
		fmt.Fprint(writer, color.WhiteBold)
	}

}

func ColorSuccess() {
	if !noColor {
		fmt.Fprint(writer, color.GreenBold)
	}

}

func ColorInfo() {
	if !noColor {
		fmt.Fprint(writer, color.BrightCyan)
	}
}

func ColorReset() {
	if !noColor {
		fmt.Fprint(writer, color.Reset)
	}
}

// Custom usage/help function
func Usage() {
	ColorSuccess()
	fmt.Fprintf(writer, "imgur-cli: A CLI to upload images to imgur anonymously\n")
	ColorReset()
	ColorInfo()
	fmt.Fprintf(writer, "Get the latest version at https://github.com/ananthvk/imgur-cli\n\n")
	ColorReset()
	ColorHighlight()
	fmt.Fprintf(writer, "Usage:\n\n")
	ColorReset()
	flag.PrintDefaults()
	/*
		flag.VisitAll(func(f *flag.Flag) {
			fmt.Fprintf(writer, " %v\n", f.Usage)
		})
	*/
	ColorHighlight()
	fmt.Fprint(writer, "\nThe available commands are:\n\n")
	ColorReset()
	fmt.Fprintf(writer, "%s\t%s\n", "upload", "Uploads the given file(s) to imgur")
	fmt.Fprintf(writer, "%s\t%s\n", "delete", "Delete the files specified by the delete hash(es)")
	ColorHighlight()
	fmt.Fprintf(writer, "\nExamples:\n\n")
	ColorReset()
	fmt.Fprintf(writer, "%s upload cat.png\n", os.Args[0])
	fmt.Fprintf(writer, "%s delete <Delete hash>\n", os.Args[0])
}

// Tells the user to run the command with the help flag and exit
func exitRunHelp() {
	ColorInfo()
	fmt.Fprintf(writer, "Run \"%s --help\" for help\n", os.Args[0])
	ColorReset()
	os.Exit(2)
}

func Run() {
	flag.Usage = Usage
	flag.BoolVar(&noColor, "no-color", false, "do not color the output")
	_ = noColor
	flag.Parse()
	if len(flag.Args()) < 1 {
		ColorError()
		fmt.Fprint(writer, "No arguments provided\n")
		ColorReset()
		exitRunHelp()
	}
	args := flag.Args()
	// Handle the two sub commands - upload and delete
	// Also perform basic error checking
	switch args[0] {
	case "upload":
		if len(args) == 1 {
			ColorError()
			fmt.Fprintf(writer, "No files provided to upload to imgur\n")
			ColorReset()
			exitRunHelp()
		}
		internal.Upload(args[1:], noColor)
	case "delete":
		if len(args) == 1 {
			ColorError()
			fmt.Fprintf(writer, "No delete hashes provided to delete from imgur\n")
			ColorReset()
			exitRunHelp()
		}
		internal.Delete(args[1:])
	default:
		ColorError()
		fmt.Fprintf(writer, "%s : unknown command\n", args[0])
		ColorReset()
		exitRunHelp()
	}
}
