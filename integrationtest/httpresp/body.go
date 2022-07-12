package httpresp

import "strconv"

func ErrorBody(statusCode int) []byte {
	return []byte(
		"<!DOCTYPE html>\n" +
			"<html>\n" +
			"    <head>\n" +
			"        <title>" +
			strconv.Itoa(statusCode) +
			"</title>\n" +
			"    </head>\n" +
			"    <body>\n" +
			"<h2>" +
			Status(statusCode) +
			"</h2>\n" +
			"provided by webserv\n" +
			"    </body>\n" +
			"</html>")
}
