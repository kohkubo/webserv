package main

import (
	"bufio"
	"bytes"
	"fmt"
	"io"
	"log"
	"net"
	"net/http"
	"os"
	"time"
)

// リクエストとレスポンスの表示
//func main() {
//    req, err := http.NewRequest(http.MethodGet, serverAddr, nil)
//    if err != nil {
//        log.Fatal(err)
//    }
//    req.Header.Add("test-header", "test-header-value")

//    reqDump, err := httputil.DumpRequestOut(req, true)
//    if err != nil {
//        log.Fatal(err)
//    }

//    fmt.Printf("REQUEST:\n%s", string(reqDump))

//    resp, err := http.DefaultClient.Do(req)
//    if err != nil {
//        log.Fatal(err)
//    }

//    respDump, err := httputil.DumpResponse(resp, true)
//    if err != nil {
//        log.Fatal(err)
//    }

//    fmt.Printf("RESPONSE:\n%s", string(respDump))
//}

// コネクションを確立, connを通して送受信できる
func connect(port string) net.Conn {
	conn, err := net.Dial("tcp", "localhost:"+port)
	if err != nil {
		log.Fatal("dial err: ", err)
	}
	return conn
}

// connを通してリクエスト文字列を送る
// TMP: sleepは小分けにしたメッセージが同時に送ることにならないように
func send_request(conn net.Conn, req string) {
	fmt.Fprintf(conn, req)
	time.Sleep(1 * time.Millisecond)
}

// リクエストのパース, レスポンスボディの確認
// TODO: 関数を分ける
// TODO: レスポンスヘッダーの確認を入れる
func parse_response(conn net.Conn, method string, expectBody []byte) {
	defer conn.Close()
	r := bufio.NewReader(conn)
	req := &http.Request{
		Method: method,
	}
	resp, err := http.ReadResponse(r, req)
	if err != nil {
		log.Fatal("ReadResponse error: ", err)
	}
	defer resp.Body.Close()
	body, err := io.ReadAll(resp.Body)
	if err != nil {
		log.Fatal("ReadAll error: ", err)
	}
	if bytes.Compare(body, expectBody) != 0 {
		fmt.Printf("error! body=%v, expect=%v\n", body, expectBody)
		os.Exit(1)
	}
	fmt.Println("ok!")
}

// 複数クライアント(A, B, C)にコネクションと3分割したメッセージを用意して, ランダムに送信する
// TODO: 各処理を go routineで走らせる
// TODO: 同じconnに送る
func testIOMULT() {
	fmt.Println("IOMULT test")
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
	parse_response(connB, "GET", FileToBytes(NOT_FOUND_PAGE))
	send_request(connC, msgC_3)
	parse_response(connC, "DELETE", FileToBytes(NOT_FOUND_PAGE))
	send_request(connA, msgA_3)
	parse_response(connA, "GET", FileToBytes(HELLO_WORLD_PAGE))
}
