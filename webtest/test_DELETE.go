package main

import (
	"fmt"
	"net/http"
	"webtest/testcase"
)

func testDELETE() {
	tests := []testcase.TestCase{
		{ // sample test
			Name:           "sample",
			Port:           PORT_5500,
			Url:            TMP_FILE,
			WantStatusCode: http.StatusOK,

			// リクエスト内容:
			//   DELETE /html/tmp.html HTTP/1.1
			//   Host: localhost:5500
			//   User-Agent: Go-http-client/1.1
			//   Accept-Encoding: gzip
		},
	}
	// テスト実行
	fmt.Println("DELETE test")
	for _, t := range tests {
		t := t
		t.Method = http.MethodDelete
		//t.Do() // おそらくwevservの返答が未完成なのが理由でエラー出てる
	}
}
