package httptest

import (
	"fmt"
	"integration_test/httpresp"
	"io"
	"net/http"
	"os"

	"github.com/google/go-cmp/cmp"
)

// "Connection"フィールドに関しては,
// http.ResponseではCloseというメンバーに設定されていたので
// TestInfoのHeader mapからConnectionだけ判定して, mapから削除
func NewResponseChecker(statusCode int, header http.Header, body []byte) ReponseChecker {
	c := &ResponseChecker{}
	c.Status = httpresp.Status(statusCode)
	c.StatusCode = statusCode
	c.Proto = "HTTP/1.1"
	c.Header = header
	c.Body = body
	c.Close = resolveClose(header)
	return c
}

// if expect conneciton_close: close=true, rm close field
// else: nothing to do
func resolveClose(header http.Header) bool {
	close := false
	if v, exist := header["Connection"]; exist && v[0] == "close" {
		close = true
		delete(header, "Connection")
	}
	return close
}

// 必要に応じてチェック項目(メンバー変数)を追加する
// http.Response参照
type ResponseChecker struct {
	Status     string
	StatusCode int
	Proto      string
	Header     http.Header
	Close      bool
	Body       []byte
}

// レスポンスが期待するヘッダーとボディを持っているか確認
func (c ResponseChecker) Check(got *http.Response) (int, error) {
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
	diff_checker("body", string(c.Body), string(body))
	return diff_flag, nil
}
