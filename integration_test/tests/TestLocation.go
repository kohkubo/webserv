package tests

import (
	"integration_test/tester"
	"net/http"
)

func TestLocation() {
	testHandler("locationでdir1を指定できているか", func() (bool, error) {
		clientA, err := tester.NewClient(&tester.Client{
			Port: "5000",
			ReqPayload: []string{
				"GET /dir1/ HTTP/1.1\r\n",
				"Host: localhost:5000\r\n",
				"User-Agent: curl/7.79.1\r\n",
				`Accept: */*` + "\r\n",
				"\r\n",
			},
			ExpectStatusCode: http.StatusOK,
			ExpectHeader:     nil,
			ExpectBody:       fileToBytes("../html/dir1/index.html"),
		})
		if err != nil {
			return false, err
		}
		return clientA.Test()
	})
	testHandler("rootディレクティブが反映されるか", func() (bool, error) {
		Port := "5001"
		Path := "/"
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
			ExpectBody:       fileToBytes("../html/dir1/index.html"),
		})
		if err != nil {
			return false, err
		}
		return clientA.Test()
	})
	testHandler("index指定ができているか", func() (bool, error) {
		Port := "5002"
		Path := "/"
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
			ExpectBody:       fileToBytes("../html/dir1/index2.html"),
		})
		if err != nil {
			return false, err
		}
		return clientA.Test()
	})

	testHandler("index指定ができているか", func() (bool, error) {
		Port := "5002"
		Path := "/"
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
			ExpectBody:       fileToBytes("../html/dir1/index2.html"),
		})
		if err != nil {
			return false, err
		}
		return clientA.Test()
	})

}
