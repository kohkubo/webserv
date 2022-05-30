package tester

import (
	"fmt"
	"log"
	"net"
	"net/http"
	"strings"
	"time"
)

type Client struct {
	Port             string
	ReqPayload       []string
	ExpectStatusCode int
	ExpectHeader     http.Header
	ExpectBody       []byte
	conn             net.Conn
	method           string
	resp             *http.Response
}

// constructor
func NewClient(c *Client) (*Client, error) {
	conn, err := connect(c.Port)
	if err != nil {
		return nil, fmt.Errorf("NewClient: fail to connect: %v", err)
	}
	c.conn = conn
	c.method = resolveMethod(c.ReqPayload)
	return c, nil
}

// リクエスト文字列を元にmethod(recvResponseで必要になる)を解決する
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

// リクエスト送信
func (c *Client) SendRequest() error {
	for _, r := range c.ReqPayload {
		_, err := fmt.Fprintf(c.conn, r)
		if err != nil {
			return fmt.Errorf("sendRequest: %v", err)
		}
	}
	c.ReqPayload = nil
	return nil
}

// 先頭のReqPayloadのみ送信
func (c *Client) SendPartialRequest() error {
	if len(c.ReqPayload) != 0 {
		r := c.ReqPayload[0]
		c.ReqPayload = c.ReqPayload[1:] // 最初の要素を削除したものに更新
		_, err := fmt.Fprintf(c.conn, r)
		time.Sleep(1 * time.Millisecond) // 連続で使用された場合にリクエストが分かれるように
		if err != nil {
			return fmt.Errorf("sendPartialRequest: %v", err)
		}
	}
	return nil
}

// レスポンスを受ける
func (c *Client) RecvResponse() {
	if len(c.ReqPayload) != 0 {
		log.Fatalf("recvResponse: ReqPayload is not empty!")
	}
	resp, err := readParseResponse(c.conn, c.method)
	if err != nil {
		log.Fatalf("recvResponse: %v", err)
	}
	c.resp = resp
	c.conn.Close()
}

// レスポンスが期待するものか確認する
func (c *Client) IsExpectedResponse() bool {
	result, err := compareResponse(c.resp, c.ExpectStatusCode, c.ExpectHeader, c.ExpectBody)
	if err != nil {
		log.Fatalf("isExpectedResult: %v", err)
	}
	c.resp.Body.Close()
	return result == 0
}

// リクエストの送信, 受信, 結果の確認まで行う
// 成功->true, 失敗->false
func (c *Client) Test() bool {
	c.SendRequest()
	c.RecvResponse()
	if !c.IsExpectedResponse() {
		return false
	}
	return true
}
