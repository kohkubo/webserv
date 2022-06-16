package tests

import (
	"fmt"
	"integration_test/httpresp"
	"integration_test/webserv"
	"io/ioutil"
	"os"
	"strconv"
)

// bytesAndLen: fileNameで指定されたパスのファイルの中身を[]byteに詰めて返します.
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

func bytesAndLen(fileName string) ([]byte, string) {
	bytes := fileToBytes(fileName)
	return bytes, strconv.Itoa(len(bytes))
}

func errBytesAndLen(statusCode int) ([]byte, string) {
	bytes := httpresp.ErrorBody(statusCode)
	return bytes, strconv.Itoa(len(bytes))
}
