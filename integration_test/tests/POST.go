package tests

import (
	"fmt"
	"integration_test/tester"
)

func TestPOST() {
	fmt.Println("POST test")

	testHandler("simple", func() (bool, error) {
		clientA := tester.NewClient(&tester.Client{
			Port: "5500",
			ReqPayload: []string{
				"POST / HTTP/1.1\r\n",
				"Host: localhost:5500\r\n",
				"User-Agent: curl/7.79.1\r\n",
				`Accept: */*` + "\r\n",
				"\r\n",
				/* body */
			},
			ExpectHeader: nil,
			ExpectBody:   nil,
		})
		return clientA.IsTestOK(), nil
	})

}
