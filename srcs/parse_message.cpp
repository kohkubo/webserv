/*
message format
syntax notation: ABNF

HTTP-message = start-line *( header-field CRLF ) CRLF [ message-body ]

start-line = request-line / status-line
request-line = method SP request-target SP HTTP-version CRLF
method = token
*/