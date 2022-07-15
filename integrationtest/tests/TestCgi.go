package tests

import (
	"integrationtest/httpresp"
	"integrationtest/httptest"
	"net/http"
)

var testCgi = testCatergory{
	categoryName: "cgi",
	config:       "integrationtest/conf/cgi.conf",
	testCases: []testCase{
		{
			caseName: "cgi get normal",
			test: func() bool {
				expectBody := []byte(
					"name= taro\n" +
						"blood= A\n" +
						"text= string\n",
				)

				expectStatusCode := 200
				port := "50000"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET /cgi_test.py?name=taro&blood=A&text=string HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
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
			caseName: "indexでcgi指定",
			test: func() bool {
				expectBody := []byte(
					"name= taro\n" +
						"blood= A\n" +
						"text= string\n",
				)

				expectStatusCode := 200
				port := "50000"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET /html/?name=taro&blood=A&text=string HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
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
		// {
		// 	caseName: "postでcgi",
		// 	test: func() bool {
		// 		expectBody := []byte(
		// 			"name= taro\n" +
		// 				"blood= A\n" +
		// 				"text= string\n",
		// 		)

		// 		expectStatusCode := 200
		// 		port := "50000"
		// 		clientA := httptest.NewClient(httptest.TestSource{
		// 			Port: port,
		// 			Request: "POST /html/cgi_test.py HTTP/1.1\r\n" +
		// 				"Host: localhost:" + port + "\r\n" +
		// 				"Connection: close\r\n" +
		// 				"Content-Length: 29" + "\r\n" +
		// 				"User-Agent: curl/7.79.1\r\n" +
		// 				`Accept: */*` + "\r\n" +
		// 				"\r\n" +
		// 				"name=taro\n" +
		// 				"blood=A\n" +
		// 				"text=string",
		// 			ExpectStatusCode: expectStatusCode,
		// 			ExpectHeader: http.Header{
		// 				"Connection":     {"close"},
		// 				"Content-Length": {lenStr(expectBody)},
		// 				"Content-Type":   {"text/html"},
		// 			},
		// 			ExpectBody: expectBody,
		// 		})
		// 		return clientA.DoAndCheck()
		// 	},
		// },
		{
			caseName: "cgi timeout",
			test: func() bool {
				expectStatusCode := 504
				expectBody := httpresp.ErrorBody(expectStatusCode)
				port := "50000"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET /time_out.py HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
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
			caseName: "cgi header field",
			test: func() bool {
				expectBody := []byte(
					"Hello world",
				)

				expectStatusCode := 200
				port := "50000"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET /cgi_test/add_header.py HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
						"Connection: close\r\n" +
						"User-Agent: curl/7.79.1\r\n" +
						`Accept: */*` + "\r\n" +
						"\r\n",
					ExpectStatusCode: expectStatusCode,
					ExpectHeader: http.Header{
						"Connection":     {"close"},
						"Content-Length": {lenStr(expectBody)},
						"Content-Type":   {"text/html"},
						"hoge":           {"fuga"},
					},
					ExpectBody: expectBody,
				})
				return clientA.DoAndCheck()
			},
		},

		{
			caseName: "cgi status code",
			test: func() bool {
				expectBody := []byte(
					"Hello world",
				)

				expectStatusCode := 418
				port := "50000"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET /cgi_test/status.py HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
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
			caseName: "cgi content length",
			test: func() bool {
				expectBody := []byte(
					"Hello world",
				)

				expectStatusCode := 200
				port := "50000"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET /cgi_test/content_length.py HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
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
			caseName: "cgi too long header",
			test: func() bool {
				expectStatusCode := 500
				expectBody := httpresp.ErrorBody(expectStatusCode)
				port := "50000"
				clientA := httptest.NewClient(httptest.TestSource{
					Port: port,
					Request: "GET /cgi_test/too_long_header.py HTTP/1.1\r\n" +
						"Host: localhost:" + port + "\r\n" +
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
	},
}
