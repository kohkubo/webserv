package main

import (
	"integration_test/colorprint"
	"integration_test/tests"
	"integration_test/webserv"
	"os"
	"time"
)

var doOptionalTest bool
var option_ok = true

func main() {
	if len(os.Args) != 1 {
		if os.Args[1] == "-all" {
			doOptionalTest = true
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
		nomal_ok := tests.NormalTests().Test()
		if doOptionalTest {
			option_ok = tests.OptionalTests().Test()
		}
		result <- nomal_ok && option_ok
	}()
	return result
}
