package tests

import (
	"fmt"
	"integration_test/colorprint"
	"integration_test/webserv"
)

type testCase struct {
	caseName string
	test     func() bool
}

type testCatergory struct {
	categoryName string
	config       string
	testCases    []testCase
}

var (
	countTestFail uint
)

func (c testCatergory) runTests() {
	if c.config == "" {
		webserv.ExitWithKill(fmt.Errorf("emtpy config"))
		return
	}
	if err := webserv.Restart(c.config); err != nil {
		webserv.ExitWithKill(fmt.Errorf("could not start webserv: %v\n", err))
		return
	}
	fmt.Println()
	fmt.Println(c.categoryName)
	fmt.Println("config:", c.config)
	for _, t := range c.testCases {
		fmt.Print("[" + t.caseName + "] ")
		switch ok := t.test(); {
		case ok:
			colorprint.Stdout("ok")
		default:
			colorprint.Stderr("ko")
			countTestFail++
		}
	}
}
