package main

import (
	"bufio"
	"fmt"
	"io"
	"log"
	"net"
	"net/http"
)

func testIOMULT() {
	conn, err := net.Dial("tcp", "localhost:5001")
	if err != nil {
		log.Fatal("dial err: ", err)
	}
	req := "DELETE /nosuch HTTP/1.1\r\nHost: localhost:5500\r\nUser-Agent: Go-http-client/1.1\r\nAccept-Encoding: gzip\r\n\r\n"
	fmt.Fprintf(conn, req)
	r := bufio.NewReader(conn)
	resp, err := http.ReadResponse(r, nil) // optionのreqは必要性がわからん, typeの違い以外で使われている形跡なし, DELETEちゃんと送れてた
	if err != nil {
		log.Fatal("readresponse error: ", err)
	}
	defer resp.Body.Close()
	body, err := io.ReadAll(resp.Body)
	fmt.Printf("%s\n", body)
}
