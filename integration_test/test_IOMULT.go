package main

import (
	"bufio"
	"fmt"
	"io"
	"log"
	"net"
	"net/http"
	"time"
)

func connect(port string) net.Conn {
	conn, err := net.Dial("tcp", "localhost:"+port)
	if err != nil {
		log.Fatal("dial err: ", err)
	}
	return conn
}

func send_request(conn net.Conn, req string) {
	fmt.Fprintf(conn, req)
	time.Sleep(1 * time.Second)
}

func parse_response(conn net.Conn, method string) {
	defer conn.Close()
	r := bufio.NewReader(conn)
	req := &http.Request{
		Method: method,
	}
	resp, err := http.ReadResponse(r, req)
	if err != nil {
		log.Fatal("readresponse error: ", err)
	}
	defer resp.Body.Close()
	body, err := io.ReadAll(resp.Body)
	fmt.Printf("%s\n", body)
}

// TODO: 各処理を go routineで走らせる
// TODO: connを閉じる
// TODO: 同じconnに送る
func testIOMULT() {
	conn5001B := connect("5001")
	conn5001C := connect("5001")
	conn5500A := connect("5500")
	/* A */ send_request(conn5500A, "GET /")
	/* B */ send_request(conn5001B, "GET /nosuch HT")
	/* C */ send_request(conn5001C, "DELETE /nosuch HTTP/1.1\r")
	/* A */ send_request(conn5500A, " HTTP/1.1\r\nHost: localhost:5500\r\nUse")
	/* C */ send_request(conn5001C, "\nHost: localhost:5500\r\nUser-Agent: Go-http-c")
	/* B */ send_request(conn5001B, "TP/1.1\r\nHost: localhost:5500\r\nUser-Agent: Go-http-client/1.1\r\nAccept-Encoding: gzip\r\n\r\n")
	/* B */ parse_response(conn5001B, "GET")
	/* A */ send_request(conn5500A, "r-Agent: Go-http-client/1.1\r\nAccept-Encoding: gzip\r\n\r\n")
	/* C */ send_request(conn5001C, "lient/1.1\r\nAccept-Encoding: gzip\r\n\r\n")
	/* A */ parse_response(conn5500A, "GET")
	/* C */ parse_response(conn5001C, "DELETE")
}
