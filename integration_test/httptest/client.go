package httptest

import (
	"fmt"
	"integration_test/webserv"
	"net"
	"net/http"
	"strings"
	"time"
)

type TestSource struct {
	Port             string
	Request          string
	ExpectStatusCode int
	ExpectHeader     http.Header
	ExpectBody       []byte
}

type Client struct {
	Port           string
	Request        string
	SendCnt        int
	Conn           net.Conn
	Close          bool
	GotResponse    *http.Response
	ReponseChecker ReponseChecker
}

type ReponseChecker interface {
	Check(*http.Response) (int, error)
}

// constructor
func NewClient(info TestSource) *Client {
	c := &Client{}
	c.Port = info.Port
	c.Request = info.Request
	conn, err := connect(c.Port)
	if err != nil {
		webserv.ExitWithKill(fmt.Errorf("NewClient: fail to connect: %v", err))
	}
	c.Conn = conn
	c.Close = true
	c.ReponseChecker = NewResponseChecker(info.ExpectStatusCode, info.ExpectHeader, info.ExpectBody)
	return c
}

// リクエスト送信
func (c *Client) SendAllRequest() {
	c.SendPartialRequest(len(c.Request))
}

// 先頭のReqPayloadのみ送信
func (c *Client) SendPartialRequest(n int) {
	len := len(c.Request)
	if c.SendCnt < len {
		limit := c.SendCnt + n
		if len < limit {
			limit = len
		}
		sendN, err := fmt.Fprintf(c.Conn, c.Request[c.SendCnt:limit])
		// 連続で使用された場合にリクエストが分かれるようにsleep
		time.Sleep(1 * time.Millisecond)
		if err != nil {
			webserv.ExitWithKill(fmt.Errorf("sendPartialRequest: %v", err))
		}
		c.SendCnt += sendN
	}
}

// レスポンスを受ける
func (c *Client) RecvResponse() {
	if c.SendCnt != len(c.Request) {
		webserv.ExitWithKill(fmt.Errorf("recvResponse: all request is not send!"))
	}
	resp, err := readParseResponse(c.Conn, resolveMethod(c.Request))
	if err != nil {
		webserv.ExitWithKill(fmt.Errorf("recvResponse: %v", err))
	}
	c.GotResponse = resp
	if c.Close {
		c.Conn.Close()
	}
}

// リクエスト文字列を元にmethod(recvResponseで必要になる)を解決する
func resolveMethod(req string) string {
	switch {
	case strings.HasPrefix(req, "POST"):
		return "POST"
	case strings.HasPrefix(req, "DELETE"):
		return "DELETE"
	default:
		return "GET"
	}
}

// レスポンスが期待するものか確認する
func (c *Client) IsExpectedResponse() bool {
	result, err := c.ReponseChecker.Check(c.GotResponse)
	if err != nil {
		webserv.ExitWithKill(fmt.Errorf("isExpectedResult: %v", err))
	}
	c.GotResponse.Body.Close()
	return result == 0
}

// リクエストの送信, 受信, 結果の確認まで行う
// 成功->true, 失敗->false
func (c *Client) DoAndCheck() bool {
	c.SendAllRequest()
	c.RecvResponse()
	return c.IsExpectedResponse()
}
