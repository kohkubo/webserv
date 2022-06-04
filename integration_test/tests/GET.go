package tests

import (
	"fmt"
	"integration_test/exe"
	"integration_test/response"
	"integration_test/tester"
	"net/http"
	"os"
)

type Smallcategolly struct {
	Name string
	Test func() (bool, error)
}

// exeの中身を持ってきただけなので後にリファクタ
func (t *Smallcategolly) TestT() {
	if exe.IsFatal() {
		return
	}

	fmt.Print("[" + t.Name + "] ")
	ok, err := t.Test()
	switch {
	case err != nil:
		fmt.Fprintf(os.Stderr, "fatal error : %v", err)
		exe.CountTestFatal++
	case ok:
		fmt.Println("\033[32m", "ok", "\033[0m")
	default:
		fmt.Println("\033[31m", "error", "\033[0m")
		exe.CountTestFail++
	}
}

type Bigcategolly struct {
	Config string
	Tests  []Smallcategolly
}

var GET = Bigcategolly{
	Config: "integration_test/conf/test.conf", //configここで用意した方がわかりやすいかと
	Tests: []Smallcategolly{
		{
			Name: "GET / ",
			Test: func() (bool, error) {
				ExpectBody, err := exe.FileToBytes("../html/index.html")
				if err != nil {
					return false, fmt.Errorf("failt to get bytes from file")
				}
				clientA, err := tester.NewClient(&tester.Client{
					Port: "5000",
					ReqPayload: []string{
						"GET / HTTP/1.1\r\n",
						"Host: localhost:5000\r\n",
						"User-Agent: curl/7.79.1\r\n",
						`Accept: */*` + "\r\n",
						"\r\n",
					},
					ExpectStatusCode: http.StatusOK,
					ExpectHeader:     nil,
					ExpectBody:       ExpectBody,
				})
				if err != nil {
					return false, err
				}
				return clientA.Test()
			},
		},
		{
			Name: "GET /dir1/index2.html ",
			Test: func() (bool, error) {
				ExpectBody, err := exe.FileToBytes("../html/dir1/index2.html")
				if err != nil {
					return false, fmt.Errorf("failt to get bytes from file")
				}
				clientA, err := tester.NewClient(&tester.Client{
					Port: "5000",
					ReqPayload: []string{
						"GET /dir1/index2.html HTTP/1.1\r\n",
						"Host: localhost:5000\r\n",
						"User-Agent: curl/7.79.1\r\n",
						`Accept: */*` + "\r\n",
						"\r\n",
					},
					ExpectStatusCode: http.StatusOK,
					ExpectHeader:     nil,
					ExpectBody:       ExpectBody,
				})
				if err != nil {
					return false, err
				}
				return clientA.Test()
			},
		},
		{
			Name: "GET /no_such_file_404",
			Test: func() (bool, error) {
				clientA, err := tester.NewClient(&tester.Client{
					Port: "5000",
					ReqPayload: []string{
						"GET /no_such_file_404 HTTP/1.1\r\n",
						"Host: localhost:5000\r\n",
						"User-Agent: curl/7.79.1\r\n",
						`Accept: */*` + "\r\n",
						"\r\n",
					},
					ExpectStatusCode: http.StatusNotFound,
					ExpectHeader:     nil,
					ExpectBody:       response.Content_404,
				})
				if err != nil {
					return false, err
				}
				return clientA.Test()
			},
		},

		// TODO: ファイル直接指定の場合のアクセス権限test

		{
			Name: "index解決後のアクセス権限確認test",
			Test: func() (bool, error) {
				clientA, err := tester.NewClient(&tester.Client{
					Port: "5000",
					ReqPayload: []string{
						"GET / HTTP/1.1\r\n",
						"Host: localhost:5000\r\n",
						"User-Agent: curl/7.79.1\r\n",
						`Accept: */*` + "\r\n",
						"\r\n",
					},
					ExpectStatusCode: 403,
					ExpectHeader:     nil,
					ExpectBody:       response.Content_403,
				})
				if err != nil {
					return false, err
				}
				os.Chmod("../html/index.html", 000)
				ok, err := clientA.Test()
				os.Chmod("../html/index.html", 0755)
				return ok, err
			},
		},
	},
}
