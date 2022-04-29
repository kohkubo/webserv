package main

import (
	"bytes"
	"fmt"
	"net/http"
	"webtest/testcase"
)

// TODO: ヘッダの情報が書き換えられるか検証
//       Header.set()とかで元々あるやつ書き換えられるかも
// TODO: TestCase struct に直接書き込むのがわかりにくかったら間にデータ型用意
func testPOST() {
	tests := []testcase.TestCase{
		{ // sample test
			Name: "sample",
			Port: PORT_5500,
			Url:  ROOT,
			Body: bytes.NewReader(FileToBytes(HELLO_WORLD_PAGE)),
			AddFields: map[string]string{
				CONTENT_TYPE: "txt/html", // フィールドにこれがなくても送れた
			},
			WantStatusCode: http.StatusOK,

			// リクエスト内容:
			//   POST / HTTP/1.1
			//   Host: localhost:5500
			//   User-Agent: Go-http-client/1.1
			//   Content-Length: 127
			//   Content-Type: txt/html
			//   Accept-Encoding: gzip
			//
			//   [ body省略 ]
		},
	}
	// テスト実行
	fmt.Println("POST test")
	for _, t := range tests {
		t := t
		t.Method = http.MethodPost
		//t.Do() // おそらく今はwevservの返答が変なのでエラー出てる
	}
}
