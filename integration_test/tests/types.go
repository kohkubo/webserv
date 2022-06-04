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

type TestCatergory struct {
	Name      string
	Config    string
	TestCases []TestCase
}

// メソッド, webservの起動~テスト実行まで行う
func (c TestCatergory) ExecuteTests() {
	if IsFatal() {
		return
	}
	if c.Config == "" {
		fmt.Fprintln(os.Stderr, "emtpy config")
		return
	}
	if err := utils.RestartWebserv(c.Config); err != nil {
		fmt.Fprintf(os.Stderr, "could not start webserv: %v\n", err)
		return
	}
	fmt.Println()
	fmt.Println(c.Name)
	fmt.Println("config:", c.Config)
	for _, t := range c.TestCases {
		t.Execute()
	}
}

type TestCase struct {
	Name string
	Test func() (bool, error)
}

func (t *TestCase) Execute() {
	if IsFatal() {
		return
	}

	fmt.Print("[" + t.Name + "] ")
	ok, err := t.Test()
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
