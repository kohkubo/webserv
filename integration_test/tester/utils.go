package tester

import (
	"bufio"
	"fmt"
	"io"
	"net"
	"net/http"
	"os"
	"reflect"
	"time"
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
	case <-time.After(10 * time.Second): // 他case文が10秒以上止まると当case文が動く
		return nil, fmt.Errorf("read response: time out!")
	case resp := <-ch_resp:
		return resp, nil
	case err := <-ch_error:
		return nil, err
	}
}

// レスポンスが期待するヘッダーとボディを持っているか確認
func compareResponse(resp *http.Response, expectStatusCode int, expectHeader http.Header, expectBody []byte) (int, error) {
	var diff_flag int
	if resp.StatusCode != expectStatusCode {
		fmt.Fprintf(os.Stderr, "status code diff: actual=%v expect=%v\n", resp.StatusCode, expectStatusCode)
		diff_flag++
	}
	for expect_k, expect_v := range expectHeader {
		if actual_v, exist := resp.Header[expect_k]; !exist {
			fmt.Fprintf(os.Stderr, "header diff: no such header %v\n", expect_k)
			diff_flag++
		} else if !reflect.DeepEqual(actual_v, expect_v) {
			fmt.Fprintf(os.Stderr, "header diff: key=%v:  actual=%v expect=%v\n", expect_k, actual_v, expect_v)
			diff_flag++
		}
	}
	if expectBody != nil {
		body, err := io.ReadAll(resp.Body)
		if err != nil {
			return 0, fmt.Errorf("failt to read response: %v", err)
		}
		if !reflect.DeepEqual(body, expectBody) {
			fmt.Printf("body diff: actual=%s, expect=%s\n", body, expectBody)
			diff_flag++
		}
	}
	return diff_flag, nil
}
