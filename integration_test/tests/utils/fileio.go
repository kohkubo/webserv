package utils

import (
	"fmt"
	"io/ioutil"
	"os"
)

// FileToBytes: fileNameで指定されたパスのファイルの中身を[]byteに詰めて返します.
func FileToBytes(fileName string) []byte {
	file, err := os.Open(fileName)
	if err != nil {
		ExitWithKillServer(fmt.Errorf("FileBytes: %v", err))
	}
	defer file.Close()
	srcBytes, err := ioutil.ReadAll(file)
	if err != nil {
		ExitWithKillServer(fmt.Errorf("FileBytes: %v", err))
	}
	return srcBytes
}
