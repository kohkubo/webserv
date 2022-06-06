package webserv

import (
	"fmt"
	"integration_test/colorprint"
	"io"
	"os"
)

func ExitWithKill(err error) {
	colorprint.Stderr("Exit by unexpeted error!")
	fmt.Fprintf(os.Stderr, "error content: %v", err)
	Kill()
	fmt.Println()
	fmt.Println("===webserv===")
	fmt.Printf("%s", log)
	fmt.Println("=============")
	os.Exit(1)
}

func Kill() {
	if current_process == nil {
		return
	}
	current_process.Process.Kill()
	str, _ := io.ReadAll(stderr)
	log = fmt.Sprintf("%s%s\n", log, str)
	current_process.Wait()
	current_process = nil
}
