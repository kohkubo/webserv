package tests

import (
	"fmt"
	"io/ioutil"
	"log"
	"os"
)

var (
	CountTestFatal uint
	CountTestFail  uint
	HELLO_WORLD    = fileToBytes("../html/index.html")
	content_404    = []byte(`<!DOCTYPE html>
<html>
    <head>
        <title>404</title>
    </head>
    <body>
<h2>404 Not Found</h2>
default error page
    </body>
</html>`)
)

// for color print
const (
	red   = "\033[31m"
	green = "\033[32m"
	reset = "\033[0m"
)

// 実行するテストの名前と関数を渡してその結果に合わせたメッセージを出力する関数です
func testHandler(name string, test func() (bool, error)) {
	if IsFatal() {
		return
	}
	fmt.Print("[ " + name + " ] ")
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
