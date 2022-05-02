package main

import (
	"bytes"
	"io"
	"io/ioutil"
	"log"
	"os"
)

// FileToBytes: fileNameで指定されたパスのファイルの中身を[]byteに詰めて返します.
func FileToBytes(fileName string) []byte {
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

// FileToReader: fileNameの中身をリクエストで送信するbodyの型(io.Reader)にして返します。
func FileToReader(fileName string) io.Reader {
	return bytes.NewReader(FileToBytes(fileName))
}
