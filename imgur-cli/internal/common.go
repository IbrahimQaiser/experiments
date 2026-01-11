package internal

import "time"

const API_URL string = "https://api.imgur.com/3/image"
const CLIENT_ID_ENV_NAME = "IMGUR_CLIENT_ID"
const TIMEOUT = 15 * time.Second
const HELP_ENV_MESSAGE = "imgur-cli\nThis program requires a client id to upload images to imgur.\n\n" +
	CLIENT_ID_ENV_NAME + ` environment variable not set.
	Get a client id from https://api.imgur.com/oauth2/addclient

	If you are on windows,
	set ` + CLIENT_ID_ENV_NAME + "=[CLIENT_ID]" + `

	If you are on linux,
	export ` + CLIENT_ID_ENV_NAME + "=[CLIENT_ID]\n" + `
	`
