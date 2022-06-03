package exe

import (
	"fmt"
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

// 実行するテストの名前と関数を渡してその結果に合わせたメッセージを出力する関数です
func SmallHandler(name string, test func() (bool, error)) {

	// Fatalなerrorが起きてる場合はテスト無視
	if IsFatal() {
		return
	}

	// テスト実行
	fmt.Print("[" + name + "] ")
	ok, err := test()
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

func IsFatal() bool {
	return CountTestFatal != 0
}

func IsFail() bool {
	return CountTestFail != 0
}

func BigHandler(name string, bigtest func(), configpath string) {
	RestartWebserv(configpath)
	fmt.Println()
	fmt.Println(name)
	fmt.Println("config:", configpath)
	bigtest()
}
