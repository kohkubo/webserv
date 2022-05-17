package main

import (
	"fmt"
	"log"
	"net"
	"net/http"
	"strings"

	"time"
)

// NOTE: 呼び出し元でのエラー処理が面倒なので,
//       予期しないエラーに関してはlog.Fatal()してる

type Client struct {
	Port         string
	ReqPayload   []string
	ExpectHeader http.Header
	ExpectBody   []byte
	conn         net.Conn
	method       string
	resp         *http.Response
}

// constructor
func NewClient(c *Client) *Client {
	conn, err := connect(c.Port)
	if err != nil {
		log.Fatalf("NewClient: fail to connect: %v", err)
	}
	c.conn = conn
	c.method = resolveMethod(c.ReqPayload)
	return c
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
func (c *Client) sendRequest() {
	for _, r := range c.ReqPayload {
		_, err := fmt.Fprintf(c.conn, r)
		if err != nil {
			log.Fatalf("sendRequest: %v", err)
		}
	}
	c.ReqPayload = nil
}

// 先頭のReqPayloadのみ送信
func (c *Client) sendPartialRequest() {
	if len(c.ReqPayload) != 0 {
		r := c.ReqPayload[0]
		c.ReqPayload = c.ReqPayload[1:] // 最初の要素を削除したものに更新
		_, err := fmt.Fprintf(c.conn, r)
		time.Sleep(1 * time.Millisecond) // 連続で使用された場合にリクエストが分かれるように
		if err != nil {
			log.Fatalf("sendPartialRequest: %v", err)
		}
	}
}

// レスポンスを受ける
func (c *Client) recvResponse() {
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
func (c *Client) isExpectedResponse() bool {
	result, err := compareResponse(c.resp, c.ExpectHeader, c.ExpectBody)
	if err != nil {
		log.Fatalf("isExpectedResult: %v", err)
	}
	c.resp.Body.Close()
	return result == 0
}

// リクエストの送信, 受信, 結果の確認まで行う
func (c *Client) isTestOK() bool {
	c.sendRequest()
	c.recvResponse()
	if !c.isExpectedResponse() {
		return false
	}
	return true
}
