package tests

var testCgi = testCatergory{
	//	categoryName: "cgi",
	//	config:       "integration_test/conf/test.conf",
	//	testCases: []testCase{
	//		{
	//			caseName: "5000_cgi_get_normal",
	//			test: func() bool {

	//				expectBody, contentLen := bytesAndLen("../html/index.html")
	//				expectStatusCode := 200
	//				Port := "50000"
	//				clientA := tester.NewClient(tester.Client{
	//					Port: Port,
	//					ReqPayload: []string{
	//						"GET /cgi_test.py?name=taro&blood=A&text=string HTTP/1.1\r\n",
	//						"Host: localhost:" + Port + "\r\n",
	//						"User-Agent: curl/7.79.1\r\n",
	//						`Accept: */*` + "\r\n",
	//						"\r\n",
	//					ExpectStatusCode: expectStatusCode,
	//					ExpectHeader: http.Header{
	//						"Connection":     {"close"},
	//						"Content-Length": {contentLen},
	//						"Content-Type":   {"text/html"},
	//					},
	//					ExpectBody: []byte(
	//						`name= taro
	//blood= A
	//text= string
	//`,
	//					),
	//				})
	//				return clientA.DoAndCheck()
	//			},
	//		},
	//	},
}
