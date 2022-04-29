package main

import (
	"fmt"
	"io"
	"net/http"
)

// Resouse pass
const (
	HELLO_WORLD_PAGE           = "../html/index.html"
	FORBIDDEN_PAGE             = "../html/forbidden.html"
	NOT_FOUND_PAGE             = "../html/not_found.html"
	BAD_REQUEST_PAGE           = "../html/bad_request.html"
	NOT_IMPLEMENTED_PAGE       = "../html/not_implemented.html"
	INTERNAL_SERVER_ERROR_PAGE = "../html/internal_server_error.html"
	UNKNOWN_ERROR_PAGE         = "../html/unknown_error.html"
)

// Field name
const (
	HOST          = "Host"
	USERAGENT     = "User-Agent"
	ACCEPT        = "Accept"
	CONTENT_LEN   = "Content-Length"
	CONTENT_TYPE  = "Content-Type"
	CONNECTION    = "Connection"
	AUTHORIZATION = "Authorization"
)

const (
	PreURI = "http://localhost:"
)

type testCase struct {
	name           string
	method         string
	port           string
	url            string
	body           io.Reader
	addQuery       map[string]string // 追加パラメータ:e.g. ["foo"]="bar" ('URL?foo=bar')
	addFields      map[string]string // 追加フィールド:e.g. ["Content-Type"]="txt/html"
	wantStatusCode int
	wantBody       []byte
}

func (t testCase) NewRequest() (*http.Request, error) {
	req, err := http.NewRequest(t.method, PreURI+t.port+t.url, t.body)
	if err != nil {
		return nil, fmt.Errorf("fail to send request: %v", err)
	}

	// URLパラメータ追加
	q := req.URL.Query()
	for key, value := range t.addQuery {
		q.Add(key, value)
	}
	req.URL.RawQuery = q.Encode()

	// ヘッダーフィールド追加
	for key, value := range t.addFields {
		req.Header.Add(key, value)
	}
	return req, nil
}
