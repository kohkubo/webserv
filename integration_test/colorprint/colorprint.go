package colorprint

import (
	"fmt"
	"os"
)

// for color print
const (
	red   = "\033[31m"
	green = "\033[32m"
	reset = "\033[0m"
)

func NotGood(s string) {
	fmt.Fprintf(os.Stderr, "%s%s%s\n", red, s, reset)
}

func Good(s string) {
	fmt.Printf("%s%s%s\n", green, s, reset)
}
