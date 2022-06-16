package tests

import (
	"fmt"
	"integration_test/webserv"
	"io/ioutil"
	"os"
	"strconv"
)

// fileToBytes: fileNameで指定されたパスのファイルの中身を[]byteに詰めて返します.
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

func fileToString(fileName string) string {
	file, err := os.Open(fileName)
	if err != nil {
		webserv.ExitWithKill(fmt.Errorf("FileBytes: %v", err))
	}
	defer file.Close()
	srcBytes, err := ioutil.ReadAll(file)
	if err != nil {
		webserv.ExitWithKill(fmt.Errorf("FileBytes: %v", err))
	}
	return string(srcBytes)
}

func lenStr(body []byte) string {
	return strconv.Itoa(len(body))
}
