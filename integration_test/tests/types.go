package tests

import (
	"fmt"
	"integration_test/tests/utils"
	"os"
)

var (
	CountTestFatal uint
	CountTestFail  uint
)

// for color print
const (
	red   = "\033[31m"
	green = "\033[32m"
	reset = "\033[0m"
)

func IsFatal() bool {
	return CountTestFatal != 0
}

func IsFail() bool {
	return CountTestFail != 0
}

type testCatergory struct {
	name      string
	config    string
	testCases []testCase
}

// メソッド, webservの起動~テスト実行まで行う
func (c testCatergory) runTests() {
	if IsFatal() {
		return
	}
	if c.config == "" {
		fmt.Fprintln(os.Stderr, "emtpy config")
		return
	}
	if err := utils.RestartWebserv(c.config); err != nil {
		fmt.Fprintf(os.Stderr, "could not start webserv: %v\n", err)
		return
	}
	fmt.Println()
	fmt.Println(c.name)
	fmt.Println("config:", c.config)
	for _, t := range c.testCases {
		if IsFatal() {
			return
		}

		fmt.Print("[" + t.name + "] ")
		ok, err := t.test()
		switch {
		case err != nil:
			fmt.Fprintf(os.Stderr, "fatal error : %v", err)
			CountTestFatal++
		case ok:
			fmt.Println(green, "ok", reset)
		default:
			fmt.Println(red, "error", reset)
			CountTestFail++
		}
	}
}

type testCase struct {
	name string
	test func() (bool, error)
}
