package response

const (
	STATUS_200_PHRASE = "200 OK"
	STATUS_201_PHRASE = "201 Created"
	STATUS_202_PHRASE = "202 Accepted"
	STATUS_204_PHRASE = "204 No Content"
	STATUS_206_PHRASE = "206 Partial Content"
	STATUS_301_PHRASE = "301 Moved Permanently"
	STATUS_302_PHRASE = "302 Moved Temporarily"
	STATUS_303_PHRASE = "303 See Other"
	STATUS_304_PHRASE = "304 Not Modified"
	STATUS_307_PHRASE = "307 Temporary Redirect"
	STATUS_308_PHRASE = "308 Permanent Redirect"
	STATUS_400_PHRASE = "400 Bad Request"
	STATUS_401_PHRASE = "401 Unauthorized"
	STATUS_402_PHRASE = "402 Payment Required"
	STATUS_403_PHRASE = "403 Forbidden"
	STATUS_404_PHRASE = "404 Not Found"
	STATUS_405_PHRASE = "405 Not Allowed"
	STATUS_406_PHRASE = "406 Not Acceptable"
	STATUS_408_PHRASE = "408 Request Time-out"
	STATUS_409_PHRASE = "409 Conflict"
	STATUS_410_PHRASE = "410 Gone"
	STATUS_411_PHRASE = "411 Length Required"
	STATUS_412_PHRASE = "412 Precondition Failed"
	STATUS_413_PHRASE = "413 Request Entity Too Large"
	STATUS_414_PHRASE = "414 Request-URI Too Large"
	STATUS_415_PHRASE = "415 Unsupported Media Type"
	STATUS_416_PHRASE = "416 Requested Range Not Satisfiable"
	STATUS_421_PHRASE = "421 Misdirected Request"
	STATUS_429_PHRASE = "429 Too Many Requests"
	STATUS_500_PHRASE = "500 Internal Server Error"
	STATUS_501_PHRASE = "501 Not Implemented"
	STATUS_502_PHRASE = "502 Bad Gateway"
	STATUS_503_PHRASE = "503 Service Temporarily Unavailable"
	STATUS_504_PHRASE = "504 Gateway Time-out"
	STATUS_505_PHRASE = "505 HTTP Version Not Supported"
	STATUS_507_PHRASE = "507 Insufficient Storage"
	STATUS_520_PHRASE = "520 Unknown Error"
)

var Statuses = map[int]string{
	200: STATUS_200_PHRASE,
	204: STATUS_204_PHRASE,
	301: STATUS_301_PHRASE,
	304: STATUS_304_PHRASE,
	400: STATUS_400_PHRASE,
	403: STATUS_403_PHRASE,
	404: STATUS_404_PHRASE,
	405: STATUS_405_PHRASE,
	500: STATUS_500_PHRASE,
	501: STATUS_501_PHRASE,
	520: STATUS_520_PHRASE,
}
