package tests

import (
	"integration_test/tester"
	"net/http"
)

const autoindex_html = `<!DOCTYPE html>
<html>
   <head>
      <title>Index of /autoindex/</title>
   </head>
   <body>
      <h1>Index of /autoindex/</h1>
      <ul style="list-style:none">
        <li><a href="../">../ </a></li>
        <li><a href="dir2/">dir2/ </a></li>
        <li><a href="test.html">test.html </a></li>
        <li><a href="dir1/">dir1/ </a></li>
    </ul>
   </body>
</html>`

func TestAUTOINDEX() {
	testHandler("simple", func() (bool, error) {
		clientA, err := tester.NewClient(&tester.Client{
			Port: "5001",
			ReqPayload: []string{
				"GET /autoindex/ HTTP/1.1\r\n",
				"Host: localhost:5001\r\n",
				"User-Agent: curl/7.79.1\r\n",
				`Accept: */*` + "\r\n",
				"\r\n",
			},
			ExpectStatusCode: http.StatusOK,
			ExpectHeader:     nil,
			ExpectBody:       []byte(autoindex_html),
		})
		if err != nil {
			return false, err
		}
		return clientA.Test()
	})
}
