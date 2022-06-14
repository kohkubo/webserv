package client

import (
	"bufio"
	"fmt"
	"io"
	"net"
	"net/http"
	"os"
	"time"

	"github.com/google/go-cmp/cmp"
)

// コネクションを確立, connを通して送受信できる
func connect(port string) (net.Conn, error) {
	conn, err := net.Dial("tcp", "localhost:"+port)
	if err != nil {
		return nil, fmt.Errorf("connect: %w", err)
	}
	return conn, nil
}

// src(conn)からリクエストを受け取りパースする
func readParseResponse(src io.Reader, method string) (*http.Response, error) {
	ch_resp := make(chan *http.Response)
	ch_error := make(chan error)
	// resposenを受け取る
	go func() {
		r := bufio.NewReader(src)
		req := &http.Request{
			Method: method,
		}
		resp, err := http.ReadResponse(r, req)
		if err != nil {
			ch_error <- err
		}
		ch_resp <- resp
	}()
	select {
	case <-time.After(10 * time.Second):
		return nil, fmt.Errorf("timeout to read response")
	case resp := <-ch_resp:
		return resp, nil
	case err := <-ch_error:
		return nil, err
	}
}

// レスポンスが期待するヘッダーとボディを持っているか確認
func compareResponse(resp *http.Response, expectStatusCode int, expectHeader http.Header, expectBody []byte) (int, error) {
	var diff_flag int
	diff_checker := func(title string, x interface{}, y interface{}) {
		if diff := cmp.Diff(x, y); diff != "" {
			fmt.Fprintf(os.Stderr, "%s mismatch (-want +got):\n%s", title, diff)
			diff_flag++
		}
	}
	diff_checker("status", expectStatusCode, resp.StatusCode)
	diff_checker("close", true, resp.Close)
	diff_checker("header", expectHeader, resp.Header)
	body, err := io.ReadAll(resp.Body)
	if err != nil {
		return 0, fmt.Errorf("failt to read response: %v", err)
	}
	diff_checker("body", expectBody, body)
	return diff_flag, nil
}
