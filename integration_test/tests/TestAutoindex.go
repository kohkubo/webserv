package tests

import (
	"integration_test/httpresp"
	"integration_test/httptest"
	"net/http"
)

// TODO: autoindex
var testAutoindex = testCatergory{
	categoryName: "autoindex",
	config:       "integration_test/conf/autoindex.conf",
	testCases: []testCase{
		//{
		//	// ソートして再挑戦
		//	caseName: "simple",
		//	test: func() bool {
		//		dirPath := "tmp/autoindex/"
		//		dirPath1 := dirPath + "dir1/"
		//		dirPath2 := dirPath + "dir2/"
		//		filePath := dirPath + "test.html"
		//		filePath1 := dirPath1 + "index.html"
		//		filePath2 := dirPath2 + "test.html"
		//		if err := os.MkdirAll(filepath.Dir(dirPath), 0750); err != nil {
		//			webserv.ExitWithKill(err)
		//		}
		//		if err := os.MkdirAll(filepath.Dir(dirPath1), 0750); err != nil {
		//			webserv.ExitWithKill(err)
		//		}
		//		if err := os.MkdirAll(filepath.Dir(dirPath2), 0750); err != nil {
		//			webserv.ExitWithKill(err)
		//		}
		//		if _, err := os.Create(filePath); err != nil {
		//			webserv.ExitWithKill(err)
		//		}
		//		if _, err := os.Create(filePath1); err != nil {
		//			webserv.ExitWithKill(err)
		//		}
		//		if _, err := os.Create(filePath2); err != nil {
		//			webserv.ExitWithKill(err)
		//		}
		//		defer os.RemoveAll("tmp/")
		//		port := "50001"
		//		expectStatusCode := 200
		//		expectBody := []byte(
		//			"<!DOCTYPE html>\n" +
		//				"<html>\n" +
		//				"   <head>\n" +
		//				"      <title>Index of /autoindex/</title>\n" +
		//				"   </head>\n" +
		//				"   <body>\n" +
		//				"      <h1>Index of /autoindex/</h1>\n" +
		//				"      <ul style=\"list-style:none\">\n" +
		//				"        <li><a href=\"../\">../ </a></li>\n" +
		//				"        <li><a href=\"dir2/\">dir2/ </a></li>\n" +
		//				"        <li><a href=\"test.html\">test.html </a></li>\n" +
		//				"        <li><a href=\"dir1/\">dir1/ </a></li>\n" +
		//				"    </ul>\n" +
		//				"   </body>\n" +
		//				"</html>\n")
		//		clientA := httptest.NewClient(httptest.TestSource{
		//			Port: port,
		//			Request: "GET /autoindex/ HTTP/1.1\r\n" +
		//				"Host: localhost:" + port + "\r\n" +
		//				"User-Agent: curl/7.79.1\r\n" +
		//				`Accept: */*` + "\r\n" +
		//				"\r\n",
		//			ExpectStatusCode: expectStatusCode,
		//			ExpectHeader: http.Header{
		//				"Content-Length": {lenStr(expectBody)},
		//				"Content-Type":   {"text/html"},
		//			},
		//			ExpectBody: expectBody,
		//		})
		//		return clientA.DoAndCheck()
		//	},
		//},
		{
			caseName: "forbidden",
			test: func() bool {
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
				port := "50001"
				expectStatusCode := 200
				expectBody := []byte("in test_autoindex/dir1")

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
