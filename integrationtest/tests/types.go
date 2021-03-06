package tests

import (
	"fmt"
	"integrationtest/colorprint"
	"integrationtest/webserv"
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
	}
	fmt.Println()
	fmt.Println("============================================================")
	fmt.Println("Category:", c.categoryName)
	fmt.Println("Config  :", c.config)
	fmt.Println("============================================================")
	if err := webserv.Restart(c.config); err != nil {
		webserv.ExitWithKill(fmt.Errorf("could not start webserv: %v\n", err))
		return
	}
	for _, t := range c.testCases {
		fmt.Println("[ RUN      ] ------->" + t.caseName + "<-------")
		switch ok := t.test(); {
		case ok:
			colorprint.Stdout("[ OK       ] " + t.caseName)
		default:
			colorprint.Stderr("[ KO       ] " + t.caseName)
			countTestFail++
		}
	}
}
