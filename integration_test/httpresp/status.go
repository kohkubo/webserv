package httpresp

import (
	"fmt"
	"net/http"
	"strconv"
)

func Status(statusCode int) string {
	return fmt.Sprintf("%s %s", strconv.Itoa(statusCode), http.StatusText(statusCode))
}
