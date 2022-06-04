package tests

import (
	"fmt"
	"integration_test/exe"
	"integration_test/response"
	"integration_test/tester"
	"net/http"
	"os"
)

// TODO: 構造体はパッケージを分ける

// 一つ一つのテストに関する構造体
type TestCase struct {
	Name    string
	Content func() (bool, error)
}

// メソッド, テスト結果の合否を出力
func (t *TestCase) Test() {
	if exe.IsFatal() {
		return
	}

	fmt.Print("[" + t.Name + "] ")
	ok, err := t.Content()
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

// 大分類のテストケース
type Catergory struct {
	Name      string
	Config    string
	TestCases []TestCase
}

// メソッド, webservの起動~テスト実行まで行う
func (c Catergory) ExecuteTest() {
	if c.Config == "" {
		fmt.Fprintln(os.Stderr, "emtpy config") // とりあえず
		return
	}
	exe.RestartWebserv(c.Config)
	fmt.Println()
	fmt.Println(c.Name)
	fmt.Println("config:", c.Config)
	for _, t := range c.TestCases {
		t.Test()
	}
}

// テストの用意
var GET = Catergory{
	Name:   "GET",
	Config: "integration_test/conf/test.conf", //configをここで用意した方がわかりやすいかと
	TestCases: []TestCase{
		{
			Name: "GET / ",
			Content: func() (bool, error) {
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
			Content: func() (bool, error) {
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
			Content: func() (bool, error) {
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
			Content: func() (bool, error) {
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
