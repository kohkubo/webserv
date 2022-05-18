package tests

import (
	"fmt"
	"io/ioutil"
	"log"
	"os"
)

var (
	CountTestFail uint
	HELLO_WORLD   = fileToBytes("../html/index.html")
	NOT_FOUND     = fileToBytes("../html/not_found.html")
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
		CountTestFail++
	}
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
