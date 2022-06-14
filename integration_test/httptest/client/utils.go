package client

import (
	"bufio"
	"fmt"
	"io"
	"net"
	"net/http"
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
func readParseResponse(src io.Reader, reqMethod string) (*http.Response, error) {
	ch_resp := make(chan *http.Response)
	ch_error := make(chan error)
	// resposenを受け取る
	go func() {
		r := bufio.NewReader(src)
		req := &http.Request{
			Method: reqMethod,
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
