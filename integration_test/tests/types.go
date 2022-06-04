package tests

import (
	"fmt"
	"integration_test/exe"
	"os"
)

// 一つ一つのテストに関する構造体
type TestCase struct {
	Name string
	Test func() (bool, error)
}

// メソッド, テスト結果の合否を出力
func (t *TestCase) Execute() {
	if exe.IsFatal() {
		return
	}

	fmt.Print("[" + t.Name + "] ")
	ok, err := t.Test()
	switch {
	case err != nil:
		fmt.Fprintf(os.Stderr, "fatal error : %v", err)
		exe.CountTestFatal++
	case ok:
		fmt.Println("\033[32m", "ok", "\033[0m")
	default:
		fmt.Println("\033[31m", "error", "\033[0m")
		exe.CountTestFail++
	}
}

// 大分類のテストケース
type Catergory struct {
	Name      string
	Config    string
	TestCases []TestCase
}

// メソッド, webservの起動~テスト実行まで行う
func (c Catergory) ExecuteTests() {
	if c.Config == "" {
		fmt.Fprintln(os.Stderr, "emtpy config") // とりあえず
		return
	}
	exe.RestartWebserv(c.Config)
	fmt.Println()
	fmt.Println(c.Name)
	fmt.Println("config:", c.Config)
	for _, t := range c.TestCases {
		t.Execute()
	}
}
