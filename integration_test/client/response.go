package client

import (
	"fmt"
	"io"
	"net/http"
	"os"

	"github.com/google/go-cmp/cmp"
)

type reponseChecker interface {
	compare(*http.Response) (int, error)
}

func NewResponseChecker(info TestInfo) reponseChecker {
	newResp := &Response{}
	newResp.Status = "200 OK"
	newResp.StatusCode = info.ExpectStatusCode
	newResp.Proto = "HTTP/1.1"
	newResp.Header = info.ExpectHeader
	newResp.Body = info.ExpectBody
	newResp.Close = true
	return newResp
}

// 必要に応じてチェック項目(メンバー変数)を追加する
// http.Response参照
type Response struct {
	Status     string
	StatusCode int
	Proto      string
	Header     http.Header
	Close      bool
	Body       []byte
}

// レスポンスが期待するヘッダーとボディを持っているか確認
func (r Response) compare(got *http.Response) (int, error) {
	var diff_flag int
	diff_checker := func(title string, x interface{}, y interface{}) {
		if diff := cmp.Diff(x, y); diff != "" {
			fmt.Fprintf(os.Stderr, "%s mismatch (-want +got):\n%s", title, diff)
			diff_flag++
		}
	}
	diff_checker("status", r.Status, got.Status)
	diff_checker("status code", r.StatusCode, got.StatusCode)
	diff_checker("proto", r.Proto, got.Proto)
	diff_checker("header", r.Header, got.Header)
	diff_checker("close", r.Close, got.Close)
	body, err := io.ReadAll(got.Body)
	if err != nil {
		return 0, fmt.Errorf("failt to read response: %v", err)
	}
	diff_checker("body", r.Body, body)
	return diff_flag, nil
}
