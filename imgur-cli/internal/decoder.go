package internal

import (
	"encoding/json"
	"fmt"
	"io"
)

type ImgData struct {
	Link        string           `json:"link"`
	DeleteHash  string           `json:"deletehash"`
	Id          string           `json:"id"`    // ID of the uploaded image
	Error       *json.RawMessage `json:"error"` // Only available when Success is false
	ErrorString string
}

type Response struct {
	Data    ImgData `json:"data"`    // Data, depends on Success
	Success bool    `json:"success"` // Whether the request failed or not
	Status  int     `json:"status"`  // HTTP status
}

// This function decodes the response from imgur and populates the Response struct
func DecodeResponse(r io.Reader) (response Response, err error) {
	err = json.NewDecoder(r).Decode(&response)
	if err != nil {
		return
	}
	// There are no errors
	if response.Data.Error == nil {
		response.Data.ErrorString = ""
		return
	}
	err = json.Unmarshal(*response.Data.Error, &response.Data.ErrorString)
	if err != nil {
		type ImgurException struct {
			Code    int
			Message string
		}
		var imex ImgurException
		err = json.Unmarshal(*response.Data.Error, &imex)
		if err != nil {
			return
		}
		response.Data.ErrorString = "Error: " + fmt.Sprint(imex.Code) + " - " + imex.Message
	}
	return
}
