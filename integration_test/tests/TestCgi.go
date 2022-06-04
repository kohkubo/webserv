package tests

import (
	"integration_test/tester"
	"net/http"
)

func TestCgi() {
	testHandler("5000_cgi_get_normal", func() (bool, error) {
		Port := "5000"
		Path := "/cgi.sh"
		clientA, err := tester.NewClient(&tester.Client{
			Port: Port,
			ReqPayload: []string{
				"GET " + Path + " HTTP/1.1\r\n",
				"Host: localhost:" + Port + "\r\n",
				"User-Agent: curl/7.79.1\r\n",
				`Accept: */*` + "\r\n",
				"\r\n",
			},
			ExpectStatusCode: http.StatusOK,
			ExpectHeader:     nil,
			ExpectBody: []byte(
				`###########################
name=
blood=
text=
example=
###########################
`),
		})
		if err != nil {
			return false, err
		}
		return clientA.Test()
	})

}
