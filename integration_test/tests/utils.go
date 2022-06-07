package tests

import (
	"fmt"
	"integration_test/webserv"
	"io/ioutil"
	"os"
)

var (
	CountTestFatal uint
	CountTestFail  uint
)

func isFatal() bool {
	return CountTestFatal != 0
}

func isFail() bool {
	return CountTestFail != 0
}

// FileToBytes: fileNameで指定されたパスのファイルの中身を[]byteに詰めて返します.
func fileToBytes(fileName string) []byte {
	file, err := os.Open(fileName)
	if err != nil {
		webserv.ExitWithKill(fmt.Errorf("FileBytes: %v", err))
	}
	defer file.Close()
	srcBytes, err := ioutil.ReadAll(file)
	if err != nil {
		webserv.ExitWithKill(fmt.Errorf("FileBytes: %v", err))
	}
	return srcBytes
}
