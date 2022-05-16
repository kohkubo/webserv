package main

import (
	"fmt"
	"log"
	"net"
	"net/http"
	"strings"

	"time"
)

type Client struct {
	Port         string
	ReqPayload   []string
	ExpectHeader http.Header
	ExpectBody   []byte
	conn         net.Conn
	method       string
	resp         *http.Response
}

// constructor的な存在
func NewClient(c *Client) *Client {
	c.connect()
	c.method = resolveMethod(c.ReqPayload)
	return c
}

func resolveMethod(reqPayload []string) string {
	var buff string
	for _, v := range reqPayload {
		buff += v
		if len("DELETE") < len(buff) {
			break
		}
	}
	switch {
	case strings.HasPrefix(buff, "POST"):
		return "POST"
	case strings.HasPrefix(buff, "DELETE"):
		return "DELETE"
	default:
		return "GET"
	}
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
		return fmt.Errorf("recvResponse: ReqPayload is not empty!")
	}
	resp, err := readResponse(c.conn, c.method)
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
