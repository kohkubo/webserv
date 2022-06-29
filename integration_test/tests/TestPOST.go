package tests

import (
	"fmt"
	"integration_test/httpresp"
	"integration_test/httptest"
	"integration_test/webserv"
	"net/http"
	"os"
	"path/filepath"

	"github.com/google/go-cmp/cmp"
)

var testPOST = testCatergory{
	categoryName: "POST",
	config:       "integration_test/conf/post.conf",
	testCases: []testCase{
		{
			caseName: "simple",
			test: func() bool {
				// setup file to delete
				postFilePath := "/tmp/post.txt"                           // httpリクエストで指定するターゲットURI
				rootRelativePath := "html"                                // configで指定されているrootへの(integration_testからの)相対パス
				deleteFileRelativePath := rootRelativePath + postFilePath // ターゲットURIへの相対パス
				os.RemoveAll(filepath.Dir(deleteFileRelativePath))
				if err := os.MkdirAll(filepath.Dir(deleteFileRelativePath), 0750); err != nil {
					webserv.ExitWithKill(err)
				}
				defer os.RemoveAll(filepath.Dir(deleteFileRelativePath))

				expectStatusCode := 201
				expectBody := httpresp.ErrorBody(expectStatusCode)
				port := "50001"
				postContent := "posted content by integration test"

				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "POST " + postFilePath + " HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
						"Connection: close\r\n" +
						"Content-Length: " + lenStr([]byte(postContent)) + "\r\n" +
						"User-Agent: curl/7.79.1\r\n" +
						`Accept: */*` + "\r\n" +
						"\r\n" +
						postContent,
					ExpectStatusCode: expectStatusCode,
					ExpectHeader: http.Header{
						"Connection":     {"close"},
						"Content-Length": {lenStr(expectBody)},
						"Content-Type":   {"text/html"},
						"Location":       {postFilePath},
					},
					ExpectBody: expectBody,
				})
				if ok := clientA.DoAndCheck(); !ok {
					return false
				}

				createdContent, err := os.ReadFile(deleteFileRelativePath)
				if err != nil {
					fmt.Fprintf(os.Stderr, "fail to read file")
					return false
				}
				if diff := cmp.Diff(postContent, string(createdContent)); diff != "" {
					fmt.Fprintf(os.Stderr, "created content mismatch (-want +got):\n%s", diff)
					return false
				}

				return true
			},
		},
		//{
		//	caseName: "no_such_file",
		//	test: func() bool {
		//		expectStatusCode := 404
		//		expectBody := httpresp.ErrorBody(expectStatusCode)
		//		port := "55000"
		//		clientA := httptest.NewClient(httptest.TestSource{
		//			Port: port,
		//			Request: "POST /no_such_file HTTP/1.1\r\n" +
		//				"Host: localhost:" + port + "\r\n" +
		//				"Connection: close\r\n" +
		//				"User-Agent: curl/7.79.1\r\n" +
		//				`Accept: */*` + "\r\n" +
		//				"\r\n",
		//			ExpectStatusCode: expectStatusCode,
		//			ExpectHeader: http.Header{
		//				"Connection":     {"close"},
		//				"Content-Length": {lenStr(expectBody)},
		//				"Content-Type":   {"text/html"},
		//			},
		//			ExpectBody: expectBody,
		//		})
		//		return clientA.DoAndCheck()
		//	},
		//},
		//{
		//	caseName: "403",
		//	test: func() bool {
		//		expectStatusCode := 403
		//		expectBody := httpresp.ErrorBody(expectStatusCode)
		//		port := "55000"
		//		clientA := httptest.NewClient(httptest.TestSource{
		//			Port: port,
		//			Request: "POST /000.html HTTP/1.1\r\n" +
		//				"Host: localhost:" + port + "\r\n" +
		//				"Connection: close\r\n" +
		//				"User-Agent: curl/7.79.1\r\n" +
		//				`Accept: */*` + "\r\n" +
		//				"\r\n",
		//			ExpectStatusCode: expectStatusCode,
		//			ExpectHeader: http.Header{
		//				"Connection":     {"close"},
		//				"Content-Length": {lenStr(expectBody)},
		//				"Content-Type":   {"text/html"},
		//			},
		//			ExpectBody: expectBody,
		//		})
		//		os.Chmod("html/000.html", 000)
		//		result := clientA.DoAndCheck()
		//		os.Chmod("html/000.html", 0755)
		//		return result
		//	},
		//},
	},
}
