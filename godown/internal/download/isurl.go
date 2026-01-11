package download

import (
	"net/url"
)

// IsUrl checks if the given string represents a valid URL
// A valid URL is defined as having a scheme and a host
func IsUrl(str string) bool {
	u, err := url.Parse(str)
	return err == nil && u.Scheme != "" && u.Host != ""
}
