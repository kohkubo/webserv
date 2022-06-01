package tests

import (
	"integration_test/tester"
	"net/http"
)

func T() {
	testHandler("simple_5000", func() (bool, error) {
		clientA, err := tester.NewClient(&tester.Client{
			Port: "5000",
			ReqPayload: []string{
				"GET / HTTP/1.1\r\n",
				"Host: localhost:5000\r\n",
				"User-Agent: curl/7.79.1\r\n",
				`Accept: */*` + "\r\n",
				"\r\n",
			},
			ExpectStatusCode: http.StatusOK,
			ExpectHeader:     nil,
			ExpectBody:       fileToBytes("../html/index.html"),
		})
		if err != nil {
			return false, err
		}
		return clientA.Test()
	})
}
