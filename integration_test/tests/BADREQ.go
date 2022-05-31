package tests

import (
	"integration_test/tester"
	"net/http"
	"strings"
)

func TestBADREQ() {
	testHandler("toolongreq", func() (bool, error) {
		longline := strings.Repeat("a", 8192)
		clientA, err := tester.NewClient(&tester.Client{
			Port: "5500",
			ReqPayload: []string{
				"GET / HTTP/1.1\r\n",
				longline,
			},
			ExpectStatusCode: http.StatusBadRequest,
			ExpectHeader:     nil,
			ExpectBody:       nil,
		})
		if err != nil {
			return false, err
		}
		return clientA.Test()
	})
}
