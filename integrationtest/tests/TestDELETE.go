package tests

import (
	"errors"
	"fmt"
	"integrationtest/httpresp"
	"integrationtest/httptest"
	"integrationtest/webserv"
	"net/http"
	"os"
	"path/filepath"
)

var testDELETE = testCatergory{
	categoryName: "DELETE",
	config:       "integrationtest/conf/webserv.conf",
	testCases: []testCase{
		{
			caseName: "simple",
			test: func() bool {
				// setup file to delete
				deleteFilePath := "/tmp/delete.txt"                         // httpリクエストで指定するターゲットURI
				rootRelativePath := "html"                                  // configで指定されているrootへの(integrationtestからの)相対パス
				deleteFileRelativePath := rootRelativePath + deleteFilePath // ターゲットURIへの相対パス
				if err := os.MkdirAll(filepath.Dir(deleteFileRelativePath), 0750); err != nil {
					webserv.ExitWithKill(err)
				}
				if _, err := os.Create(deleteFileRelativePath); err != nil {
					webserv.ExitWithKill(err)
				}
				defer os.RemoveAll(filepath.Dir(deleteFileRelativePath))

				expectStatusCode := 204
				port := "55000"

				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "DELETE " + deleteFilePath + " HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
						"Connection: close\r\n" +
						"User-Agent: curl/7.79.1\r\n" +
						`Accept: */*` + "\r\n" +
						"\r\n",
					ExpectStatusCode: expectStatusCode,
					ExpectHeader: http.Header{
						"Connection": {"close"},
					},
					ExpectBody: []byte{},
				})
				if ok := clientA.DoAndCheck(); !ok {
					return false
				}

				// check file exists or deleted
				_, err := os.Stat(deleteFileRelativePath)
				switch {
				case errors.Is(err, os.ErrNotExist):
				case err != nil:
					webserv.ExitWithKill(err)
				default:
					fmt.Fprintf(os.Stderr, "file wasn't deleted")
					return false
				}
				return true
			},
		},
		{
			caseName: "no_such_file",
			test: func() bool {
				expectStatusCode := 404
				expectBody := httpresp.ErrorBody(expectStatusCode)
				port := "55000"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "DELETE /no_such_file HTTP/1.1\r\n" +
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
			caseName: "403",
			test: func() bool {
				expectStatusCode := 403
				expectBody := httpresp.ErrorBody(expectStatusCode)
				port := "55000"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "DELETE /000.html HTTP/1.1\r\n" +
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
				os.Chmod("html/000.html", 000)
				result := clientA.DoAndCheck()
				os.Chmod("html/000.html", 0755)
				return result
			},
		},
	},
}
