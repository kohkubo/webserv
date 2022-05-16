package main

import (
	"bufio"
	"fmt"
	"io"
	"log"
	"net/http"
	"os"
	"reflect"
)

// for color print
const (
	red   = "\033[31m"
	green = "\033[32m"
	reset = "\033[0m"
)

// 実行するテストの名前と関数を渡してその結果に合わせたメッセージを出力する関数です
func testHandler(name string, test func() (bool, error)) {
	fmt.Print("[ " + name + " ] ")
	ok, err := test()
	if err != nil {
		log.Fatalf("erro occured!: %v", err)
	}
	if ok {
		fmt.Println(green, "ok", reset)
	} else {
		fmt.Println(red, "error", reset)
		os.Exit(1) // TODO: テスト全体でエラーがあれば最後にexit(1)する
	}
}

// src(conn)からリクエストを受け取りパースする
// TODO: タイムアウト実装
func getResponse(src io.Reader, method string) (*http.Response, error) {
	r := bufio.NewReader(src)
	req := &http.Request{
		Method: method,
	}
	resp, err := http.ReadResponse(r, req)
	if err != nil {
		return nil, fmt.Errorf("failt to read response: %w", err)
	}
	return resp, nil
}

// レスポンスが期待するヘッダーとボディを持っているか確認
func compareResponse(resp *http.Response, expectHeader http.Header, expectBody []byte) (int, error) {
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
			return 0, fmt.Errorf("failt to read response: %v", err)
		}
		if !reflect.DeepEqual(body, expectBody) {
			fmt.Printf("body diff: actual=%v, expect=%v\n", body, expectBody)
			diff_flag++
		}
	}
	return diff_flag, nil
}
