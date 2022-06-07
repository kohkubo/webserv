package tests

import (
	"fmt"
	"integration_test/colorprint"
	"integration_test/webserv"
	"os"
)

type testCase struct {
	name string
	test func() (bool, error)
}

type testCatergory struct {
	name      string
	config    string
	testCases []testCase
}

// メソッド, webservの起動~テスト実行まで行う
func (c testCatergory) runTests() {
	if isFatal() {
		return
	}
	if c.config == "" {
		fmt.Fprintln(os.Stderr, "emtpy config")
		return
	}
	if err := webserv.Restart(c.config); err != nil {
		fmt.Fprintf(os.Stderr, "could not start webserv: %v\n", err)
		return
	}
	fmt.Println()
	fmt.Println(c.name)
	fmt.Println("config:", c.config)
	for _, t := range c.testCases {
		if isFatal() {
			return
		}

		fmt.Print("[" + t.name + "] ")
		ok, err := t.test()
		switch {
		case err != nil:
			fmt.Fprintf(os.Stderr, "fatal error : %v", err)
			CountTestFatal++
		case ok:
			colorprint.Stdout("ok")
		default:
			colorprint.Stderr("ko")
			CountTestFail++
		}
	}
}
