package main

const (
	ROOT         = "/"
	NO_SUCH_FILE = "/no_such_file"
	TMP_FILE     = "/html/tmp.html"
	PORT_5500    = "5500"
	PORT_5001    = "5001"
)

// Resourse pass
const (
	HELLO_WORLD_PAGE           = "../html/index.html"
	FORBIDDEN_PAGE             = "../html/forbidden.html"
	NOT_FOUND_PAGE             = "../html/not_found.html"
	BAD_REQUEST_PAGE           = "../html/bad_request.html"
	NOT_IMPLEMENTED_PAGE       = "../html/not_implemented.html"
	INTERNAL_SERVER_ERROR_PAGE = "../html/internal_server_error.html"
	UNKNOWN_ERROR_PAGE         = "../html/unknown_error.html"
)

// Http message field name
const (
	HOST          = "Host"
	USERAGENT     = "User-Agent"
	ACCEPT        = "Accept"
	CONTENT_LEN   = "Content-Length"
	CONTENT_TYPE  = "Content-Type"
	CONNECTION    = "Connection"
	AUTHORIZATION = "Authorization"
)
