package testcase

import (
	"fmt"
	"io"
	"net/http"
)

const (
	PreURI = "http://localhost:"
)

// addFieldsについて
//   "Content-Length"など(おそらく)内部でbodyの長さによって書き変わっていて, 反映されないものもある
//   Request structのContent-Length変数が設定されていることは確認した
type TestCase struct {
	Name           string
	Method         string
	Port           string
	Url            string
	Body           io.Reader
	AddQuery       map[string]string // 追加パラメータ:e.g. ["foo"]="bar" ('URL?foo=bar')
	AddFields      map[string]string // 追加フィールド:e.g. ["Content-Type"]="txt/html"
	WantStatusCode int
	WantBody       []byte
}

func (t *TestCase) NewRequest() (*http.Request, error) {
	req, err := http.NewRequest(t.Method, PreURI+t.Port+t.Url, t.Body)
	if err != nil {
		return nil, fmt.Errorf("fail to send request: %v", err)
	}

	// URLパラメータ追加
	q := req.URL.Query()
	for key, value := range t.AddQuery {
		q.Add(key, value)
	}
	req.URL.RawQuery = q.Encode()

	// ヘッダーフィールド追加
	for key, value := range t.AddFields {
		req.Header.Add(key, value)
	}
	return req, nil
}
