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

func testHandler(name string, test func() (bool, error)) {
	fmt.Print("[ " + name + " ] ")
	ok, err := test()
	if err != nil {
		log.Fatalf("erro occured!: %v", err)
	}
	if ok {
		fmt.Println(GREEN, "ok", RESET)
	} else {
		fmt.Println(RED, "error", RESET)
		os.Exit(1) // TODO: テスト全体でエラーがあれば最後にexit(1)する
	}
}

const (
	RED   = "\033[31m"
	GREEN = "\033[32m"
	RESET = "\033[0m"
)

// コネクションを確立, connを通して送受信できる
func connect(port string) net.Conn {
	conn, err := net.Dial("tcp", "localhost:"+port)
	if err != nil {
		log.Fatalf("failt to connect: %v", err)
	}
	return conn
}

// connを通してリクエスト文字列reqを送る
func send_request(conn net.Conn, req string) {
	_, err := fmt.Fprintf(conn, req)
	time.Sleep(1 * time.Millisecond)
	if err != nil {
		log.Fatalf("failt to send request: %v", err)
	}
}

// connを通してリクエストを受け取る, パースする
func recv_response(conn net.Conn, method string) *http.Response {
	r := bufio.NewReader(conn)
	req := &http.Request{
		Method: method,
	}
	resp, err := http.ReadResponse(r, req)
	if err != nil {
		log.Fatalf("failt to read response: %v", err)
	}
	return resp
}

// レスポンスが期待するヘッダーとボディを持っているか確認
func compare_response(resp *http.Response, expectHeader http.Header, expectBody []byte) int {
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
			log.Fatalf("failt to read response: %v", err)
		}
		if !reflect.DeepEqual(body, expectBody) {
			fmt.Printf("body diff: actual=%v, expect=%v\n", body, expectBody)
			diff_flag++
		}
	}
	return diff_flag
}

// レスポンスの確認
func check_response(conn net.Conn, method string, expectHeader http.Header, expectBody []byte) int {
	defer conn.Close()
	resp := recv_response(conn, method)
	defer resp.Body.Close()
	return compare_response(resp, expectHeader, expectBody)
}

// タイムアウト
// 複数クライアント(A, B, C)にコネクションと3分割したメッセージを用意して, ランダムに送信する
func testIOMULT() {
	fmt.Println("IOMULT test")
	testHandler("3client", func() (bool, error) {
		type Client struct {
			Port         string
			ReqPayload   []string
			Conn         net.Conn
			Resp         *http.Response
			ExpectHeader http.Header
			ExpectBody   []byte
		}

		connA := connect("5500")
		connB := connect("5001")
		connC := connect("5001")

		msgA_1 := "GET /"
		msgA_2 := " HTTP/1.1\r\nHost: localhost:5500\r\nUse"
		msgA_3 := "r-Agent: Go-http-client/1.1\r\nAccept-Encoding: gzip\r\n\r\n"

		msgB_1 := "GET /nosuch HT"
		msgB_2 := "TP/1.1\r\nHost: localhost:55"
		msgB_3 := "00\r\nUser-Agent: Go-http-client/1.1\r\nAccept-Encoding: gzip\r\n\r\n"

		msgC_1 := "DELETE /nosuch HTTP/1.1\r"
		msgC_2 := "\nHost: localhost:5500\r\nUser-Agent: Go-http-c"
		msgC_3 := "lient/1.1\r\nAccept-Encoding: gzip\r\n\r\n"

		send_request(connA, msgA_1)
		send_request(connB, msgB_1)
		send_request(connC, msgC_1)
		send_request(connB, msgB_2)
		send_request(connA, msgA_2)
		send_request(connC, msgC_2)
		send_request(connB, msgB_3)
		diff_flag := check_response(connB, "GET", nil, FileToBytes(NOT_FOUND_PAGE))
		send_request(connC, msgC_3)
		diff_flag += check_response(connC, "DELETE", nil, FileToBytes(NOT_FOUND_PAGE))
		send_request(connA, msgA_3)
		diff_flag += check_response(connA, "GET", nil, FileToBytes(HELLO_WORLD_PAGE))

		if diff_flag == 0 {
			return true, nil
		} else {
			return false, nil
		}
	})
}
