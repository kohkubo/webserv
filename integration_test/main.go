package main

import (
	"integration_test/colorprint"
	"integration_test/tests"
	"integration_test/webserv"
	"os"
	"time"
)

var optionalTest bool

func main() {
	if len(os.Args) != 1 {
		if os.Args[1] == "-o" {
			optionalTest = true
		}
	}
	var status int
	select {
	case <-time.After(5 * time.Minute):
		colorprint.Stderr("itest: unexptected timeout")
		status = 1
	case ok := <-test():
		if ok {
			colorprint.Stdout("All ok")
		} else {
			colorprint.Stderr("Fail")
			status = 1
		}
	}
	webserv.Kill()
	os.Exit(status)
}

func test() chan bool {
	result := make(chan bool)
	go func() {
		t := tests.Generate(optionalTest)
		result <- t.Test()
	}()
	return result
}
