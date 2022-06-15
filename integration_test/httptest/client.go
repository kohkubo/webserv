package httptest

import (
	"fmt"
	"integration_test/webserv"
	"net"
	"net/http"
	"strings"
	"time"
)

type TestInfo struct {
	Port             string
	Request          string
	ExpectStatusCode int
	ExpectHeader     http.Header
	ExpectBody       []byte
}

type Client struct {
	port           string
	request        string
	sendCnt        int
	conn           net.Conn
	gotResp        *http.Response
	reponseChecker ReponseChecker
}

type ReponseChecker interface {
	Check(*http.Response) (int, error)
}

// constructor
func NewClient(info TestInfo) *Client {
	c := &Client{}
	c.port = info.Port
	c.request = info.Request
	c.reponseChecker = NewResponseChecker(info.ExpectStatusCode, info.ExpectHeader, info.ExpectBody)
	conn, err := connect(c.port)
	if err != nil {
		webserv.ExitWithKill(fmt.Errorf("NewClient: fail to connect: %v", err))
	}
	c.conn = conn
	return c
}

// リクエスト送信
func (c *Client) SendAllRequest() {
	c.SendPartialRequest(len(c.request))
}

// 先頭のReqPayloadのみ送信
func (c *Client) SendPartialRequest(n int) {
	len := len(c.request)
	if c.sendCnt < len {
		limit := c.sendCnt + n
		if len < limit {
			limit = len
		}
		sendN, err := fmt.Fprintf(c.conn, c.request[c.sendCnt:limit])
		// 連続で使用された場合にリクエストが分かれるようにsleep
		time.Sleep(1 * time.Millisecond)
		if err != nil {
			webserv.ExitWithKill(fmt.Errorf("sendPartialRequest: %v", err))
		}
		c.sendCnt += sendN
	}
}

// レスポンスを受ける
func (c *Client) RecvResponse() {
	if c.sendCnt != len(c.request) {
		webserv.ExitWithKill(fmt.Errorf("recvResponse: all request is not send!"))
	}
	resp, err := readParseResponse(c.conn, resolveMethod(c.request))
	if err != nil {
		webserv.ExitWithKill(fmt.Errorf("recvResponse: %v", err))
	}
	c.gotResp = resp
	c.conn.Close()
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
	result, err := c.reponseChecker.Check(c.gotResp)
	if err != nil {
		webserv.ExitWithKill(fmt.Errorf("isExpectedResult: %v", err))
	}
	c.gotResp.Body.Close()
	return result == 0
}

// リクエストの送信, 受信, 結果の確認まで行う
// 成功->true, 失敗->false
func (c *Client) DoAndCheck() bool {
	c.SendAllRequest()
	c.RecvResponse()
	return c.IsExpectedResponse()
}
