package httptest

import (
	"fmt"
	"integration_test/webserv"
	"math/rand"
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
	Conn           net.Conn
	Close          bool
	GotResponse    *http.Response
	ReponseChecker ReponseChecker
	sendCnt        int
	sendLog        []string
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

// Connectionを引き継ぐコンストラクタ
func NewClientWithConn(prev_conn net.Conn, info TestSource) *Client {
	c := &Client{}
	c.Port = info.Port
	c.Request = info.Request
	c.Conn = prev_conn
	c.Close = true
	c.ReponseChecker = NewResponseChecker(info.ExpectStatusCode, info.ExpectHeader, info.ExpectBody)
	return c
}

// リクエストの送信, 受信, 結果の確認まで行う
// 成功->true, 失敗->false
func (c *Client) DoAndCheck() bool {
	if err := c.SendRequestAll(); err != nil {
		webserv.ExitWithKill(err)
	}
	if err := c.RecvResponse(); err != nil {
		webserv.ExitWithKill(err)
	}
	result, err := c.IsExpectedResponse()
	if err != nil {
		webserv.ExitWithKill(err)
	}
	return result
}

// リクエスト送信
func (c *Client) SendRequestAll() error {
	for c.sendCnt < len(c.Request) {
		if err := c.SendRequestRandom(); err != nil {
			return err
		}
	}
	return nil
}

// 決めた文字数のリクエストを送信
// IOMULTIで使おうかと
func (c *Client) SendRequestRandom() error {
	rand.Seed(time.Now().UnixNano())
	randN := rand.Intn(len(c.Request))
	if err := c.SendRequestN(randN); err != nil {
		return err
	}
	return nil
}

// 指定文字数のみリクエスト送信
func (c *Client) SendRequestN(n int) (err error) {
	limit := c.sendCnt + n
	if len(c.Request) < limit {
		limit = len(c.Request)
	}
	sendContent := c.Request[c.sendCnt:limit]
	sendN, err := fmt.Fprintf(c.Conn, sendContent)
	c.sendLog = append(c.sendLog, sendContent)
	// 連続で使用された場合にリクエストが分かれるようにsleep
	if err != nil {
		return fmt.Errorf("sendPartialRequest: %v", err)
	}
	time.Sleep(1 * time.Millisecond)
	c.sendCnt += sendN
	return err
}

// レスポンスを受ける
func (c *Client) RecvResponse() error {
	if c.sendCnt != len(c.Request) {
		return fmt.Errorf("recvResponse: all request is not send!")
	}
	resp, err := readParseResponse(c.Conn, resolveMethod(c.Request))
	if err != nil {
		return fmt.Errorf("recvResponse: %v", err)
	}
	c.GotResponse = resp
	if c.Close {
		c.Conn.Close()
	}
	return err
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
func (c *Client) IsExpectedResponse() (bool, error) {
	if c.GotResponse == nil {
		return false, fmt.Errorf("isExpectedResult: GotResponse is nil")
	}
	result, err := c.ReponseChecker.Check(c.GotResponse)
	if err != nil {
		return false, fmt.Errorf("isExpectedResult: %v", err)
	}
	c.GotResponse.Body.Close()
	testOK := result == 0
	if !testOK {
		fmt.Println("===Request log===")
		for _, r := range c.sendLog {
			fmt.Printf("[%s]%d\n", r, len(r))
		}
		fmt.Println("=================")
	}
	return testOK, err
}
