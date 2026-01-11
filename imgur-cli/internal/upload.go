package internal

import (
	"bytes"
	"fmt"
	"io"
	"log"
	"mime/multipart"
	"net/http"
	"os"
	"path/filepath"
	"strings"
	"sync"

	"github.com/ananthvk/imgur-cli/internal/color"
)

var logger = log.New(os.Stdout, "", 0)
var errLogger = log.New(os.Stderr, "", 0)
var wg sync.WaitGroup
var noColor = false

// Upload - Uploads the list of files to imgur.
// files - Slice of strings, which represents paths of the files to upload

func uploadFile(file string, client *http.Client) {
	defer wg.Done()
	// Create a buffer for storing the multipart data
	buffer := new(bytes.Buffer)
	// Create a multipart writer
	writer := multipart.NewWriter(buffer)
	imgWriter, err := writer.CreateFormFile("image", filepath.Base(file))
	if err != nil {
		if !noColor {
			errLogger.Print(color.BrightRedBold)
		}
		errLogger.Println("internal error: creating writer", file)
		if !noColor {
			errLogger.Print(color.Reset)
		}
		errLogger.Fatal(err)
	}
	// Read the file from disk
	fil, err := os.Open(file)
	if err != nil {
		if !noColor {
			errLogger.Print(color.BrightRedBold)
		}
		errLogger.Println("error: could not open ", file)
		if !noColor {
			errLogger.Print(color.Reset)
		}
		errLogger.Fatal(err)
	}
	defer fil.Close()
	// Copy the file to the buffer through imgWriter
	_, err = io.Copy(imgWriter, fil)
	if err != nil {
		if !noColor {
			errLogger.Print(color.BrightRedBold)
		}
		errLogger.Println("internal error: while copying", file)
		if !noColor {
			errLogger.Print(color.Reset)
		}
		errLogger.Fatal(err)
	}
	writer.Close()
	// Create the headers, and fields
	req, err := http.NewRequest(http.MethodPost, API_URL, buffer)
	if err != nil {
		if !noColor {
			errLogger.Print(color.BrightRedBold)
		}
		errLogger.Println("internal error: while creating http request")
		if !noColor {
			errLogger.Print(color.Reset)
		}
		errLogger.Fatal(err)
	}
	req.Header.Set("Authorization", "Client-Id "+os.Getenv(CLIENT_ID_ENV_NAME))
	req.Header.Set("Content-Type", writer.FormDataContentType())
	response, err := client.Do(req)
	if err != nil {
		if !noColor {
			errLogger.Print(color.BrightRedBold)
		}
		errLogger.Println("error: could not upload file to imgur")
		if !noColor {
			errLogger.Print(color.Reset)
		}
		errLogger.Fatal(err)
	}
	defer response.Body.Close()
	// Read the response and decode the json into the struct

	resp, err := DecodeResponse(response.Body)
	if err != nil {
		if !noColor {
			errLogger.Print(color.BrightRedBold)
		}
		errLogger.Println("error: could not read response")
		if !noColor {
			errLogger.Print(color.Reset)
		}
		errLogger.Fatal(err)
	}
	// Handle HTTP errors
	switch resp.Status {
	case http.StatusOK, http.StatusAccepted, http.StatusCreated, http.StatusNonAuthoritativeInfo:
		if !noColor {
			logger.Print(color.GreenBold)
		}
		logger.Println("Success: uploaded image to imgur!")
		if !noColor {
			logger.Print(color.Reset)
		}
		logger.Println("Image URL:", resp.Data.Link)
		logger.Println("Delete hash:", resp.Data.DeleteHash)
		if !noColor {
			logger.Print(color.WhiteBold)
		}
		logger.Println("Please keep the above delete hash safe as it is required to remove the image from imgur.")
		if !noColor {
			logger.Print(color.Reset)
		}
	case http.StatusUnauthorized:
		if !noColor {
			errLogger.Print(color.BrightRedBold)
		}
		errLogger.Println("Unauthorized")
		if !noColor {
			errLogger.Print(color.Reset)
		}
		errLogger.Fatal("Please check if you have set a valid", CLIENT_ID_ENV_NAME)
	default:
		if !noColor {
			errLogger.Print(color.BrightRedBold)
		}
		errLogger.Println("Error:", resp.Status)
		if !noColor {
			errLogger.Print(color.Reset)
		}
		errLogger.Fatal(resp.Data.ErrorString)
	}
}
func Upload(files []string, NoColor bool) {
	noColor = NoColor
	if len(strings.TrimSpace(os.Getenv(CLIENT_ID_ENV_NAME))) == 0 {
		fmt.Fprintf(os.Stderr, HELP_ENV_MESSAGE)
		os.Exit(2)
	}
	client := &http.Client{Timeout: TIMEOUT}
	for _, file := range files {
		wg.Add(1)
		go uploadFile(file, client)
	}
	wg.Wait()
}
