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
			caseName: "create file and overwrite",
			test: func() bool {
				postFilePath := "/tmp/post.txt"                           // httpリクエストで指定するターゲットURI
				rootRelativePath := "html"                                // configで指定されているrootへの(integration_testからの)相対パス
				deleteFileRelativePath := rootRelativePath + postFilePath // ターゲットURIへの相対パス
				os.RemoveAll(filepath.Dir(deleteFileRelativePath))        // 既に存在していた時を考慮して削除
				if err := os.MkdirAll(filepath.Dir(deleteFileRelativePath), 0750); err != nil {
					webserv.ExitWithKill(err)
				}
				defer os.RemoveAll(filepath.Dir(deleteFileRelativePath))

				/* ファイルの作成 */
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

				/* ファイルの上書き */
				overWriteContent := "overwritten!!!!!! content by integration test"
				clientA = httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "POST " + postFilePath + " HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
						"Connection: close\r\n" +
						"Content-Length: " + lenStr([]byte(overWriteContent)) + "\r\n" +
						"User-Agent: curl/7.79.1\r\n" +
						`Accept: */*` + "\r\n" +
						"\r\n" +
						overWriteContent,
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
				createdContent, err = os.ReadFile(deleteFileRelativePath)
				if err != nil {
					fmt.Fprintf(os.Stderr, "fail to read file")
					return false
				}
				if diff := cmp.Diff(overWriteContent, string(createdContent)); diff != "" {
					fmt.Fprintf(os.Stderr, "created content mismatch (-want +got):\n%s", diff)
					return false
				}

				return true
			},
		},
		{
			caseName: " keepalive, 1:file作成, 2:上書き",
			test: func() bool {
				postFilePath := "/tmp/post.txt"                           // httpリクエストで指定するターゲットURI
				rootRelativePath := "html"                                // configで指定されているrootへの(integration_testからの)相対パス
				deleteFileRelativePath := rootRelativePath + postFilePath // ターゲットURIへの相対パス
				os.RemoveAll(filepath.Dir(deleteFileRelativePath))        // 既に存在していた時を考慮して削除
				if err := os.MkdirAll(filepath.Dir(deleteFileRelativePath), 0750); err != nil {
					webserv.ExitWithKill(err)
				}
				defer os.RemoveAll(filepath.Dir(deleteFileRelativePath))

				/* ファイルの作成+keepalive */
				expectStatusCode := 201
				expectBody := httpresp.ErrorBody(expectStatusCode)
				port := "50001"
				postContent := "posted content by integration test"
				client := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "POST " + postFilePath + " HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
						"Content-Length: " + lenStr([]byte(postContent)) + "\r\n" +
						"User-Agent: curl/7.79.1\r\n" +
						`Accept: */*` + "\r\n" +
						"\r\n" +
						postContent,
					ExpectStatusCode: expectStatusCode,
					ExpectHeader: http.Header{
						"Content-Length": {lenStr(expectBody)},
						"Content-Type":   {"text/html"},
						"Location":       {postFilePath},
					},
					ExpectBody: expectBody,
				})
				client.Close = false // connection closeしないようにする
				if ok := client.DoAndCheck(); !ok {
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

				/* 同じコネクションでファイルの上書き */
				overWriteContent := "overwritten!!!!!! content by integration test"
				client = httptest.NewClientWithConn(client.Conn, httptest.TestSource{
					Port: port,
					Request: "POST " + postFilePath + " HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
						"Connection: close\r\n" +
						"Content-Length: " + lenStr([]byte(overWriteContent)) + "\r\n" +
						"User-Agent: curl/7.79.1\r\n" +
						`Accept: */*` + "\r\n" +
						"\r\n" +
						overWriteContent,
					ExpectStatusCode: expectStatusCode,
					ExpectHeader: http.Header{
						"Connection":     {"close"},
						"Content-Length": {lenStr(expectBody)},
						"Content-Type":   {"text/html"},
						"Location":       {postFilePath},
					},
					ExpectBody: expectBody,
				})
				if ok := client.DoAndCheck(); !ok {
					return false
				}
				createdContent, err = os.ReadFile(deleteFileRelativePath)
				if err != nil {
					fmt.Fprintf(os.Stderr, "fail to read file")
					return false
				}
				if diff := cmp.Diff(overWriteContent, string(createdContent)); diff != "" {
					fmt.Fprintf(os.Stderr, "created content mismatch (-want +got):\n%s", diff)
					return false
				}

				return true
			},
		},
		{
			caseName: "dir specified",
			test: func() bool {
				expectStatusCode := 403
				expectBody := httpresp.ErrorBody(expectStatusCode)
				port := "50001"
				postContent := "posted content by integration test"

				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "POST /dir/ HTTP/1.1\r\n" +
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
					},
					ExpectBody: expectBody,
				})
				return clientA.DoAndCheck()
			},
		},
		{
			caseName: "not allow upload_file",
			test: func() bool {
				expectStatusCode := 403
				expectBody := httpresp.ErrorBody(expectStatusCode)
				port := "50001"
				postContent := "posted content by integration test"

				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "POST /notallow/tmp HTTP/1.1\r\n" +
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
					},
					ExpectBody: expectBody,
				})
				return clientA.DoAndCheck()
			},
		},
		{
			caseName: "dir not exists",
			test: func() bool {
				postFilePath := "/tmp/post.txt"                           // httpリクエストで指定するターゲットURI
				rootRelativePath := "html"                                // configで指定されているrootへの(integration_testからの)相対パス
				deleteFileRelativePath := rootRelativePath + postFilePath // ターゲットURIへの相対パス
				os.RemoveAll(filepath.Dir(deleteFileRelativePath))        // ディレクトリが存在しない時のテストなので削除

				expectStatusCode := 500
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
					},
					ExpectBody: expectBody,
				})
				return clientA.DoAndCheck()
			},
		},
	},
}
