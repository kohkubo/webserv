package tester

import (
	"fmt"
	"integration_test/webserv"
	"net"
	"net/http"
	"strings"
	"time"
)

type Client struct {
	Port         string
	Request      []string
	statusCode   int
	ExpectHeader http.Header
	ExpectBody   []byte
	conn         net.Conn
	method       string
	resp         *http.Response
}

// constructor
func NewClient(baseC Client) *Client {
	newC := baseC
	conn, err := connect(newC.Port)
	if err != nil {
		webserv.ExitWithKill(fmt.Errorf("NewClient: fail to connect: %v", err))
	}
	newC.conn = conn
	newC.method = resolveMethod(newC.Request)
	return &newC
}

// リクエスト文字列を元にmethod(recvResponseで必要になる)を解決する
func resolveMethod(Request []string) string {
	var buff string
	for _, v := range Request {
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
func (c *Client) SendRequest() {
	for _, r := range c.Request {
		_, err := fmt.Fprintf(c.conn, r)
		if err != nil {
			webserv.ExitWithKill(fmt.Errorf("sendRequest: %v", err))
		}
	}
	c.Request = nil
}

// 先頭のRequestのみ送信
func (c *Client) SendPartialRequest() {
	if len(c.Request) != 0 {
		r := c.Request[0]
		c.Request = c.Request[1:] // 最初の要素を削除したものに更新
		_, err := fmt.Fprintf(c.conn, r)
		time.Sleep(1 * time.Millisecond) // 連続で使用された場合にリクエストが分かれるように
		if err != nil {
			webserv.ExitWithKill(fmt.Errorf("sendPartialRequest: %v", err))
		}
	}
}

// レスポンスを受ける
func (c *Client) RecvResponse() {
	if len(c.Request) != 0 {
		webserv.ExitWithKill(fmt.Errorf("recvResponse: Request is not empty!"))
	}
	resp, err := readParseResponse(c.conn, c.method)
	if err != nil {
		webserv.ExitWithKill(fmt.Errorf("recvResponse: %v", err))
	}
	c.resp = resp
	c.conn.Close()
}

// レスポンスが期待するものか確認する
func (c *Client) IsExpectedResponse() bool {
	result, err := compareResponse(c.resp, c.statusCode, c.ExpectHeader, c.ExpectBody)
	if err != nil {
		webserv.ExitWithKill(fmt.Errorf("isExpectedResult: %v", err))
	}
	c.resp.Body.Close()
	return result == 0
}

// リクエストの送信, 受信, 結果の確認まで行う
// 成功->true, 失敗->false
func (c *Client) DoAndCheck() bool {
	c.SendRequest()
	c.RecvResponse()
	return c.IsExpectedResponse()
}
