package tests

import (
	"fmt"
	"io/ioutil"
	"log"
	"os"
	"runtime"
	"strings"
)

var (
	CountTestFatal uint
	CountTestFail  uint
)

// for color print
const (
	red   = "\033[31m"
	green = "\033[32m"
	reset = "\033[0m"
)

// 実行するテストの名前と関数を渡してその結果に合わせたメッセージを出力する関数です
func testHandler(name string, test func() (bool, error)) {

	// Fatalなerrorが起きてる場合はテスト無視
	if IsFatal() {
		return
	}

	// 呼び出し関数の名前取得
	pc, _, _, ok := runtime.Caller(1)
	if !ok {
		fmt.Fprintf(os.Stderr, "not possible to recover the information")
		CountTestFatal++
	}
	callerFuncInfo := runtime.FuncForPC(pc).Name()
	callerFunc := callerFuncInfo[strings.LastIndex(callerFuncInfo, ".")+1:]

	// テスト実行
	fmt.Print("[" + callerFunc + " " + name + "] ")
	ok, err := test()
	switch {
	case err != nil:
		fmt.Fprintf(os.Stderr, "fatal error : %v", err)
		CountTestFatal++
	case ok:
		fmt.Println(green, "ok", reset)
	default:
		fmt.Println(red, "error", reset)
		CountTestFail++
	}
}

func IsFatal() bool {
	return CountTestFatal != 0
}

func IsFail() bool {
	return CountTestFail != 0
}

// FileToBytes: fileNameで指定されたパスのファイルの中身を[]byteに詰めて返します.
func fileToBytes(fileName string) []byte {
	file, err := os.Open(fileName)
	if err != nil {
		log.Fatalf("FileBytes: %v", err)
	}
	defer file.Close()
	srcBytes, err := ioutil.ReadAll(file)
	if err != nil {
		log.Fatalf("FileBytes: %v", err)
	}
	return srcBytes
}
