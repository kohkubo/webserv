package main

import (
	"integrationtest/colorprint"
	"integrationtest/tests"
	"integrationtest/webserv"
	"os"
	"time"
)

func main() {
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
		option_ok := true
		if len(os.Args) != 1 && os.Args[1] == "-all" {
			option_ok = tests.OptionalTests().Test()
		}
		result <- nomal_ok && option_ok
	}()
	return result
}
