package tests

import (
	"fmt"
	"integration_test/webserv"
	"io/ioutil"
	"os"
)

// FileToBytes: fileNameで指定されたパスのファイルの中身を[]byteに詰めて返します.
func fileToBytes(fileName string) []byte {
	file, err := os.Open(fileName)
	if err != nil {
		fmt.Println(fileName)
		webserv.ExitWithKill(fmt.Errorf("FileBytes: %v", err))
	}
	defer file.Close()
	srcBytes, err := ioutil.ReadAll(file)
	if err != nil {
		webserv.ExitWithKill(fmt.Errorf("FileBytes: %v", err))
	}
	return srcBytes
}
