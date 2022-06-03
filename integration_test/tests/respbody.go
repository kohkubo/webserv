package tests

var (
	content_400 = []byte(`<!DOCTYPE html>
<html>
    <head>
        <title>400</title>
    </head>
    <body>
<h2>400 Bad Request</h2>
default error page
    </body>
</html>`)
	content_403 = []byte(`<!DOCTYPE html>
<html>
    <head>
        <title>403</title>
    </head>
    <body>
<h2>403 Forbidden</h2>
default error page
    </body>
</html>`)
	content_404 = []byte(`<!DOCTYPE html>
<html>
    <head>
        <title>404</title>
    </head>
    <body>
<h2>404 Not Found</h2>
default error page
    </body>
</html>`)
	content_405 = []byte(`<!DOCTYPE html>
<html>
    <head>
        <title>405</title>
    </head>
    <body>
<h2>405 Method Not Allowed</h2>
default error page
    </body>
</html>`)
	content_408 = []byte(`<!DOCTYPE html>
<html>
    <head>
        <title>408</title>
    </head>
    <body>
<h2>408 Request Timeout</h2>
default error page
    </body>
</html>`)
	content_500 = []byte(`<!DOCTYPE html>
<html>
    <head>
        <title>500</title>
    </head>
    <body>
<h2>500 Internal Server Error</h2>
default error page
    </body>
</html>`)
	content_501 = []byte(`<!DOCTYPE html>
<html>
    <head>
        <title>501</title>
    </head>
    <body>
<h2>501 Not Implemented</h2>
default error page
    </body>
</html>`)
	content_505 = []byte(`<!DOCTYPE html>
<html>
    <head>
        <title>505</title>
    </head>
    <body>
<h2>505 HTTP Version Not Supported</h2>
default error page
    </body>
</html>`)
	content_520 = []byte(`<!DOCTYPE html>
<html>
    <head>
        <title>520</title>
    </head>
    <body>
<h2>520 Unknown Error</h2>
default error page
    </body>
</html>`)
)
