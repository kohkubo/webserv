package tests

import (
	"integrationtest/httpresp"
	"integrationtest/httptest"
	"integrationtest/webserv"
	"net/http"
	"os"
)

var content_dir1 = "in dir1"

func mkdirExitIfError(name string) {
	if err := os.MkdirAll(name, 0750); err != nil {
		webserv.ExitWithKill(err)
	}
}

func writeFileExitIfError(name string, data string) {
	if err := os.WriteFile(name, []byte(data), 0750); err != nil {
		webserv.ExitWithKill(err)
	}
}

func makeTestPath() func() {
	testRoot := "tmp/"
	testDir := testRoot + "autoindex/"
	dir1 := testDir + "dir1/"
	dir2 := testDir + "dir2/"
	dir3 := testDir + "dir3/"
	mkdirExitIfError(dir1)
	mkdirExitIfError(dir2)
	mkdirExitIfError(dir3)
	writeFileExitIfError(dir1+"index.html", content_dir1)

	// dummy for checking if dir listing is sorted
	mkdirExitIfError(testDir + "a_dummy_dir/")
	writeFileExitIfError(testDir+"a_dummy_file", "")
	writeFileExitIfError(testDir+"dummy_file", "")

	return func() { os.RemoveAll(testRoot) }
}

var testAutoindex = testCatergory{
	categoryName: "autoindex",
	config:       "integrationtest/conf/autoindex.conf",
	testCases: []testCase{
		{
			caseName: "simple",
			test: func() bool {
				clear := makeTestPath()
				defer clear()
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
						"        <li><a href=\"a_dummy_dir/\">a_dummy_dir/ </a></li>\n" +
						"        <li><a href=\"dir1/\">dir1/ </a></li>\n" +
						"        <li><a href=\"dir2/\">dir2/ </a></li>\n" +
						"        <li><a href=\"dir3/\">dir3/ </a></li>\n" +
						"        <li><a href=\"a_dummy_file\">a_dummy_file </a></li>\n" +
						"        <li><a href=\"dummy_file\">dummy_file </a></li>\n" +
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
			caseName: "no dir/file",
			test: func() bool {
				clear := makeTestPath()
				defer clear()
				port := "50001"
				expectStatusCode := 200
				expectBody := []byte(
					"<!DOCTYPE html>\n" +
						"<html>\n" +
						"   <head>\n" +
						"      <title>Index of /autoindex/dir3/</title>\n" +
						"   </head>\n" +
						"   <body>\n" +
						"      <h1>Index of /autoindex/dir3/</h1>\n" +
						"      <ul style=\"list-style:none\">\n" +
						"        <li><a href=\"../\">../ </a></li>\n" +
						"    </ul>\n" +
						"   </body>\n" +
						"</html>")
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET /autoindex/dir3/ HTTP/1.1\r\n" +
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
				clear := makeTestPath()
				defer clear()
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
				clear := makeTestPath()
				defer clear()
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
