package tests

import (
	"integrationtest/httpresp"
	"integrationtest/httptest"
	"net/http"
)

var testAvailableMethods = testCatergory{
	categoryName: "available method",
	config:       "integrationtest/conf/available_methods.conf",
	testCases: []testCase{
		{
			caseName: "available_methods ok",
			test: func() bool {
				expectStatusCode := 200
				expectBody := fileToBytes("html/index.html")
				Port := "50000"
				Path := "/"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: Port,
					Request: "GET " + Path + " HTTP/1.1\r\n" +
						"Host: localhost:" + Port + "\r\n" +
						"Connection: close\r\n" +
						"User-Agent: curl/7.79.1\r\n" +
						`Accept: */*` + "\r\n" +
						"\r\n",
					ExpectStatusCode: expectStatusCode,
					ExpectHeader: http.Header{
						"Connection":     {"close"},
						"Content-Length": {lenStr(expectBody)},
						"Content-Type":   {"text/html"},
					},
					ExpectBody: expectBody,
				})
				return clientA.DoAndCheck()
			},
		},
		{
			caseName: "available_methods NG 405",
			test: func() bool {
				expectStatusCode := 405
				expectBody := httpresp.ErrorBody(expectStatusCode)
				Port := "50000"
				Path := "/"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: Port,
					Request: `POST ` + Path + ` HTTP/1.1` + "\r\n" +
						`Host: localhost:` + Port + "\r\n" +
						"Connection: close\r\n" +
						`User-Agent: curl/7.79.1` + "\r\n" +
						`Accept: */*` + "\r\n" +
						`Content-Length: 9` + "\r\n" +
						`Content-Type: application/x-www-form-urlencoded` + "\r\n" +
						"\r\n" +
						"name=hoge",
					ExpectStatusCode: http.StatusMethodNotAllowed,
					ExpectHeader: http.Header{
						"Connection":     {"close"},
						"Content-Length": {lenStr(expectBody)},
						"Content-Type":   {"text/html"},
					},
					ExpectBody: expectBody,
				})
				return clientA.DoAndCheck()
			},
		},
		{
			caseName: "multi available_methods get",
			test: func() bool {
				expectStatusCode := 200
				expectBody := fileToBytes("html/index.html")
				Port := "50001"
				Path := "/"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: Port,
					Request: "GET " + Path + " HTTP/1.1\r\n" +
						"Host: localhost:" + Port + "\r\n" +
						"Connection: close\r\n" +
						"User-Agent: curl/7.79.1\r\n" +
						`Accept: */*` + "\r\n" +
						"\r\n",
					ExpectStatusCode: expectStatusCode,
					ExpectHeader: http.Header{
						"Connection":     {"close"},
						"Content-Length": {lenStr(expectBody)},
						"Content-Type":   {"text/html"},
					},
					ExpectBody: expectBody,
				})
				return clientA.DoAndCheck()
			},
		},
		{
			caseName: "multi available_methods post",
			test: func() bool {
				expectStatusCode := 403
				expectBody := httpresp.ErrorBody(expectStatusCode)
				Port := "50001"
				Path := "/"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: Port,
					Request: `POST ` + Path + ` HTTP/1.1` + "\r\n" +
						`Host: localhost:` + Port + "\r\n" +
						"Connection: close\r\n" +
						`User-Agent: curl/7.79.1` + "\r\n" +
						`Accept: */*` + "\r\n" +
						`Content-Length: 9` + "\r\n" +
						`Content-Type: application/x-www-form-urlencoded` + "\r\n" +
						"\r\n" +
						"name=hoge",
					ExpectStatusCode: expectStatusCode,
					ExpectHeader: http.Header{
						"Connection":     {"close"},
						"Content-Length": {lenStr(expectBody)},
						"Content-Type":   {"text/html"},
					},
					ExpectBody: expectBody,
				})
				return clientA.DoAndCheck()
			},
		},
		{
			caseName: "multi available_methods delete",
			test: func() bool {
				expectStatusCode := 405
				expectBody := httpresp.ErrorBody(expectStatusCode)
				Port := "50001"
				Path := "/"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: Port,
					Request: `DELETE ` + Path + ` HTTP/1.1` + "\r\n" +
						`Host: localhost:` + Port + "\r\n" +
						"Connection: close\r\n" +
						`User-Agent: curl/7.79.1` + "\r\n" +
						`Accept: */*` + "\r\n" +
						`Content-Type: application/x-www-form-urlencoded` + "\r\n" +
						"\r\n",
					ExpectStatusCode: expectStatusCode,
					ExpectHeader: http.Header{
						"Connection":     {"close"},
						"Content-Length": {lenStr(expectBody)},
						"Content-Type":   {"text/html"},
					},
					ExpectBody: expectBody,
				})
				return clientA.DoAndCheck()
			},
		},
	},
}
