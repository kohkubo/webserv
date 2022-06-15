package httptest

import (
	"fmt"
	"integration_test/response"
	"integration_test/webserv"
	"io"
	"net/http"
	"os"

	"github.com/google/go-cmp/cmp"
)

// "Connection"フィールドに関しては,
// http.ResponseではCloseというメンバーに設定されていたので
// TestInfoのHeader mapからConnectionだけ判定して, mapから削除
func NewResponseChecker(statusCode int, header http.Header, body []byte) ReponseChecker {
	newResp := &Checker{}
	newResp.Status = response.Statuses[statusCode]
	newResp.StatusCode = statusCode
	newResp.Proto = "HTTP/1.1"
	newResp.Header = header
	newResp.Body = body
	newResp.Close = resolveClose(header)
	return newResp
}

func resolveClose(header http.Header) bool {
	close := false
	if v, exist := header["Connection"]; !exist {
		webserv.ExitWithKill(fmt.Errorf("Connection field not specified"))
	} else if v[0] == "close" {
		close = true
	} else if v[0] == "keep-alive" {
		close = false
	} else {
		webserv.ExitWithKill(fmt.Errorf("unknown Connection field: %v", v))
	}
	delete(header, "Connection")
	return close
}

// 必要に応じてチェック項目(メンバー変数)を追加する
// http.Response参照
type Checker struct {
	Status     string
	StatusCode int
	Proto      string
	Header     http.Header
	Close      bool
	Body       []byte
}

// レスポンスが期待するヘッダーとボディを持っているか確認
func (c Checker) Check(got *http.Response) (int, error) {
	var diff_flag int
	diff_checker := func(title string, x interface{}, y interface{}) {
		if diff := cmp.Diff(x, y); diff != "" {
			fmt.Fprintf(os.Stderr, "%s mismatch (-want +got):\n%s", title, diff)
			diff_flag++
		}
	}
	diff_checker("status", c.Status, got.Status)
	diff_checker("status code", c.StatusCode, got.StatusCode)
	diff_checker("proto", c.Proto, got.Proto)
	diff_checker("header", c.Header, got.Header)
	diff_checker("close", c.Close, got.Close)
	body, err := io.ReadAll(got.Body)
	if err != nil {
		return 0, fmt.Errorf("failt to read response: %v", err)
	}
	diff_checker("body", c.Body, body)
	return diff_flag, nil
}
