package tests

import (
	"fmt"
	"integration_test/colorprint"
	"integration_test/webserv"
)

type testCase struct {
	name string
	test func() bool
}

type testCatergory struct {
	name      string
	config    string
	testCases []testCase
}

var (
	countTestFail uint
)

// メソッド, webservの起動~テスト実行まで行う
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
	fmt.Println(c.name)
	fmt.Println("config:", c.config)
	for _, t := range c.testCases {
		fmt.Print("[" + t.name + "] ")
		ok := t.test()
		switch {
		case ok:
			colorprint.Stdout("ok")
		default:
			colorprint.Stderr("ko")
			countTestFail++
		}
	}
}
