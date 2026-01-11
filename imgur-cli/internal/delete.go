package internal

import (
	"encoding/json"
	"fmt"
	"log"
	"net/http"
	"net/url"
	"os"
	"strings"
	"sync"
)

var wgd sync.WaitGroup

func deleteFile(deleteHash string, client *http.Client) {
	defer wgd.Done()
	u, err := url.Parse(API_URL)
	if err != nil {
		log.Fatal("internal error:", "Invalid API url\n")
	}
	u.Path += "/" + deleteHash
	deleteUrl := u.String()
	req, err := http.NewRequest(http.MethodDelete, deleteUrl, nil)
	if err != nil {
		log.Println("internal error: while creating http request")
		log.Fatal(err)
	}
	req.Header.Set("Authorization", "Client-Id "+os.Getenv(CLIENT_ID_ENV_NAME))
	response, err := client.Do(req)
	if err != nil {
		log.Println("error: could not upload file to imgur")
		log.Fatal(err)
	}
	defer response.Body.Close()
	// Read the response and decode the json into the struct

	type DeleteResponse struct {
		Success bool `json:"success"` // Whether the request failed or not
		Status  int  `json:"status"`  // HTTP status
	}
	var resp DeleteResponse
	err = json.NewDecoder(response.Body).Decode(&resp)
	if err != nil {
		log.Println("error: could not read response")
		log.Fatal(err)
	}
	// Handle HTTP errors
	switch resp.Status {
	case http.StatusOK, http.StatusAccepted, http.StatusCreated, http.StatusNonAuthoritativeInfo:
		logger.Println("Success: Deleted image from imgur")
	case http.StatusForbidden, http.StatusUnauthorized:
		log.Print("You are not allowed to delete this image!")
		log.Print("Check if you have entered the delete hash correctly")
		log.Fatal("Also check if you have set a valid ", CLIENT_ID_ENV_NAME)
	default:
		log.Println("Error:", resp.Status)
	}
}

func Delete(deleteHashes []string) {
	if len(strings.TrimSpace(os.Getenv(CLIENT_ID_ENV_NAME))) == 0 {
		fmt.Fprintf(os.Stderr, HELP_ENV_MESSAGE)
		os.Exit(2)
	}
	client := &http.Client{Timeout: TIMEOUT}
	for _, deleteHash := range deleteHashes {
		wgd.Add(1)
		deleteFile(deleteHash, client)
	}
	wgd.Wait()
}
