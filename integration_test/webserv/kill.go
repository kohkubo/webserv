package webserv

import (
	"fmt"
	"integration_test/colorprint"
	"os"
)

func ExitWithKill(err error) {
	colorprint.Stderr("Exit by unexpeted error!")
	fmt.Fprintf(os.Stderr, "error : %v", err)
	Kill()
	os.Exit(1)
}

func Kill() {
	if currentProcess == nil {
		return
	}
	currentProcess.Process.Kill()
	//str, _ := io.ReadAll(stdErr)
	//log = fmt.Sprintf("%s%s\n", log, str)
	currentProcess.Wait()
	//fmt.Println()
	//fmt.Println("===webserv===")
	//fmt.Printf("%s", log)
	//fmt.Println("=============")
	currentProcess = nil
}
