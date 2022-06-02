package tests

var (
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
)
