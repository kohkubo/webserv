//package main

//import (
//	"fmt"
//	"net/http"
//	"webtest/testcase"
//)

//func testDELETE() {	tests := []testcase.TestCase{
//	{ // sample test
//		Name:           "sample",
//		Port:           PORT_5500,
//		Url:            ROOT,
//		WantStatusCode: http.StatusOK,
//		WantBody:       FileToBytes(HELLO_WORLD_PAGE),

//		// リクエスト内容:
//		//   GET / HTTP/1.1
//		//   Host: localhost:5500
//		//   User-Agent: Go-http-client/1.1
//		//   Accept-Encoding: gzip
//	},
//}
//// テスト実行
//fmt.Println("GET test")
//for _, t := range tests {
//	t := t
//	t.Method = http.MethodGet
//	t.Do()
//}
//}}
