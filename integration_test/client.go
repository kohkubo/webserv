package main

import (
	"bufio"
	"fmt"
	"io"
	"log"
	"net"
	"net/http"
	"os"
	"reflect"
	"time"
)

type Client struct {
	Port         string
	ReqPayload   []string
	Method       string
	ExpectHeader http.Header
	ExpectBody   []byte
	conn         net.Conn
	resp         *http.Response
}

// コネクションを確立, connを通して送受信できる
func (c *Client) connect() {
	conn, err := net.Dial("tcp", "localhost:"+c.Port)
	if err != nil {
		log.Fatalf("connect: %v", err)
	}
	c.conn = conn
}

func (c *Client) sendRequest() {
	for _, r := range c.ReqPayload {
		_, err := fmt.Fprintf(c.conn, r)
		if err != nil {
			log.Fatalf("sendRequest: %v", err)
		}
	}
}

// 先頭のReqPayloadのみ送信
func (c *Client) sendPartialRequest() {
	if len(c.ReqPayload) != 0 {
		r := c.ReqPayload[0]
		c.ReqPayload = c.ReqPayload[1:] // 最初の要素を削除
		_, err := fmt.Fprintf(c.conn, r)
		time.Sleep(1 * time.Millisecond) // 連続で使用された場合にリクエストが分かれるように
		if err != nil {
			log.Fatalf("sendPartialRequest: %v", err)
		}
	}
}

// レスポンスを受ける
func (c *Client) recvResponse() error {
	if len(c.ReqPayload) != 0 {
		return fmt.Errorf("ReqPayload is not empty!")
	}
	resp, err := getResponse(c.conn, c.Method)
	if err != nil {
		return fmt.Errorf("recvResponse: %w", err)
	}
	c.resp = resp
	return nil
}

func (c *Client) isExpectedResult() bool {
	defer c.conn.Close()
	err := c.recvResponse()
	if err != nil {
		log.Fatalf("isExpectedResult: %v", err)
	}
	defer c.resp.Body.Close()
	result, err := compareResponse(c.resp, c.ExpectHeader, c.ExpectBody)
	if err != nil {
		log.Fatalf("isExpectedResult: %v", err)
	}
	return result == 0
}

func connectAll(a ...*Client) {
	for _, c := range a {
		c.connect()
	}
}

// src(conn)からリクエストを受け取りパースする
// TODO: タイムアウト実装
func getResponse(src io.Reader, method string) (*http.Response, error) {
	r := bufio.NewReader(src)
	req := &http.Request{
		Method: method,
	}
	resp, err := http.ReadResponse(r, req)
	if err != nil {
		return nil, fmt.Errorf("failt to read response: %w", err)
	}
	return resp, nil
}

// レスポンスが期待するヘッダーとボディを持っているか確認
func compareResponse(resp *http.Response, expectHeader http.Header, expectBody []byte) (int, error) {
	var diff_flag int
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
			fmt.Printf("body diff: actual=%v, expect=%v\n", body, expectBody)
			diff_flag++
		}
	}
	return diff_flag, nil
}
