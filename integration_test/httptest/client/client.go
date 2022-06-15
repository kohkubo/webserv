package client

import (
	"fmt"
	"integration_test/webserv"
	"net"
	"net/http"
	"strings"
	"time"
)

type ReponseChecker interface {
	Do(*http.Response) (int, error)
}

type Info struct {
	Port            string
	RequestPayload  []string
	ResponseChecker ReponseChecker
}

type Client struct {
	port           string
	reqPayload     []string
	conn           net.Conn
	gotResp        *http.Response
	reponseChecker ReponseChecker
}

// constructor
func NewClient(info Info) *Client {
	newC := &Client{}
	newC.port = info.Port
	newC.reqPayload = info.RequestPayload
	newC.reponseChecker = info.ResponseChecker
	conn, err := connect(newC.port)
	if err != nil {
		webserv.ExitWithKill(fmt.Errorf("NewClient: fail to connect: %v", err))
	}
	newC.conn = conn
	return newC
}

// リクエスト送信
func (c *Client) SendRequest() {
	for _, r := range c.reqPayload {
		_, err := fmt.Fprintf(c.conn, r)
		if err != nil {
			webserv.ExitWithKill(fmt.Errorf("sendRequest: %v", err))
		}
	}
	c.reqPayload = nil
}

// 先頭のReqPayloadのみ送信
func (c *Client) SendPartialRequest() {
	if len(c.reqPayload) != 0 {
		r := c.reqPayload[0]
		c.reqPayload = c.reqPayload[1:] // 最初の要素を削除したものに更新
		_, err := fmt.Fprintf(c.conn, r)
		time.Sleep(1 * time.Millisecond) // 連続で使用された場合にリクエストが分かれるように
		if err != nil {
			webserv.ExitWithKill(fmt.Errorf("sendPartialRequest: %v", err))
		}
	}
}

// レスポンスを受ける
func (c *Client) RecvResponse() {
	if len(c.reqPayload) != 0 {
		webserv.ExitWithKill(fmt.Errorf("recvResponse: ReqPayload is not empty!"))
	}
	resp, err := readParseResponse(c.conn, resolveMethod(c.reqPayload))
	if err != nil {
		webserv.ExitWithKill(fmt.Errorf("recvResponse: %v", err))
	}
	c.gotResp = resp
	c.conn.Close()
}

// リクエスト文字列を元にmethod(recvResponseで必要になる)を解決する
func resolveMethod(req []string) string {
	var buff string
	for _, v := range req {
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

// レスポンスが期待するものか確認する
func (c *Client) IsExpectedResponse() bool {
	result, err := c.reponseChecker.Do(c.gotResp)
	if err != nil {
		webserv.ExitWithKill(fmt.Errorf("isExpectedResult: %v", err))
	}
	c.gotResp.Body.Close()
	return result == 0
}

// リクエストの送信, 受信, 結果の確認まで行う
// 成功->true, 失敗->false
func (c *Client) DoAndCheck() bool {
	c.SendRequest()
	c.RecvResponse()
	return c.IsExpectedResponse()
}
