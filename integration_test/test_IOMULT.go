package main

import (
	"bufio"
	"fmt"
	"io"
	"log"
	"net"
	"net/http"
)

func connect(port string) net.Conn {
	conn, err := net.Dial("tcp", "localhost:"+port)
	if err != nil {
		log.Fatal("dial err: ", err)
	}
	return conn
}

func parse_request(conn net.Conn) {
	r := bufio.NewReader(conn)
	resp, err := http.ReadResponse(r, nil) // optionのreqは必要性がわからん, typeの違い以外で使われている形跡なし, DELETEちゃんと送れてた
	if err != nil {
		log.Fatal("readresponse error: ", err)
	}
	defer resp.Body.Close()
	body, err := io.ReadAll(resp.Body)
	fmt.Printf("%s\n", body)
}

func testIOMULT() {
	conn5001 := connect("5001")
	conn5500 := connect("5500")
	fmt.Fprintf(conn5500, "GET /")
	fmt.Fprintf(conn5001, "DELETE /nosuch HTTP/1.1\r\nHost: localhost:5500\r\nUser-Agent: Go-http-client/1.1\r\nAccept-Encoding: gzip\r\n\r\n")
	parse_request(conn5001)
	fmt.Fprintf(conn5500, " HTTP/1.1\r\nHost: localhost:5500\r\nUser-Agent: Go-http-client/1.1\r\nAccept-Encoding: gzip\r\n\r\n")
	parse_request(conn5500)
}
