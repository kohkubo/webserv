package tests

import (
	"integration_test/httpresp"
	"integration_test/httptest"
	"integration_test/webserv"
	"net/http"
	"os"
)

var content_dir1 = "in dir1"

func makeTestPath() func() {
	testRoot := "tmp/"
	testDir := testRoot + "autoindex/"
	dir1 := testDir + "dir1/"
	dir2 := testDir + "dir2/"
	file1 := dir1 + "index.html"
	if err := os.MkdirAll(dir1, 0750); err != nil {
		webserv.ExitWithKill(err)
	}
	if err := os.MkdirAll(dir2, 0750); err != nil {
		webserv.ExitWithKill(err)
	}
	if f, err := os.Create(file1); err != nil {
		webserv.ExitWithKill(err)
	} else {
		defer f.Close()
		f.WriteString(content_dir1)
	}
	return func() { os.RemoveAll(testRoot) }
}

// TODO: autoindex
var testAutoindex = testCatergory{
	categoryName: "autoindex",
	config:       "integration_test/conf/autoindex.conf",
	testCases: []testCase{
		{
			caseName: "simple",
			test: func() bool {
				defer makeTestPath()()
				port := "50001"
				expectStatusCode := 200
				expectBody := []byte(
					"<!DOCTYPE html>\n" +
						"<html>\n" +
						"   <head>\n" +
						"      <title>Index of /autoindex/</title>\n" +
						"   </head>\n" +
						"   <body>\n" +
						"      <h1>Index of /autoindex/</h1>\n" +
						"      <ul style=\"list-style:none\">\n" +
						"        <li><a href=\"../\">../ </a></li>\n" +
						"        <li><a href=\"dir1/\">dir1/ </a></li>\n" +
						"        <li><a href=\"dir2/\">dir2/ </a></li>\n" +
						"    </ul>\n" +
						"   </body>\n" +
						"</html>")
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET /autoindex/ HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
						"User-Agent: curl/7.79.1\r\n" +
						`Accept: */*` + "\r\n" +
						"\r\n",
					ExpectStatusCode: expectStatusCode,
					ExpectHeader: http.Header{
						"Content-Length": {lenStr(expectBody)},
						"Content-Type":   {"text/html"},
					},
					ExpectBody: expectBody,
				})
				return clientA.DoAndCheck()
			},
		},
		{
			caseName: "forbidden",
			test: func() bool {
				defer makeTestPath()()
				port := "50001"
				expectStatusCode := 403
				expectBody := httpresp.ErrorBody(403)
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET /autoindex/dir2/ HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
						"Connection: close\r\n" +
						"User-Agent: curl/7.79.1\r\n" +
						`Accept: */*` + "\r\n" +
						"\r\n",
					ExpectStatusCode: expectStatusCode,
					ExpectHeader: http.Header{
						"Connection":     {"close"},
						"Content-Length": {lenStr(expectBody)},
						"Content-Type":   {"text/html"},
					},
					ExpectBody: expectBody,
				})
				return clientA.DoAndCheck()
			},
		},
		{
			caseName: "index_priority",
			test: func() bool {
				defer makeTestPath()()
				port := "50001"
				expectStatusCode := 200
				expectBody := []byte(content_dir1)

				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET /autoindex/dir1/ HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
						"Connection: close\r\n" +
						"User-Agent: curl/7.79.1\r\n" +
						`Accept: */*` + "\r\n" +
						"\r\n",
					ExpectStatusCode: expectStatusCode,
					ExpectHeader: http.Header{
						"Connection":     {"close"},
						"Content-Length": {lenStr(expectBody)},
						"Content-Type":   {"text/html"},
					},
					ExpectBody: expectBody,
				})
				return clientA.DoAndCheck()
			},
		},
	},
}
