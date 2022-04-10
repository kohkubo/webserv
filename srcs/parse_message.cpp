/*
message format
syntax notation: ABNF

- HTTP メッセージ全体の文法
HTTP-message	= start-line *( header-field CRLF ) CRLF [ message-body ]

- start-line
start-line		= request-line / status-line
request-line	= method SP request-target SP HTTP-version CRLF
method			= token
tokenはなんなのか謎

status-line		= HTTP-version SP status-code SP reason-phrase CRLF
status-code 	= 3DIGIT
reason-phrase	= *( HTAB / SP / VCHAR / obs-text )
obs-textは謎

-  header-field
header-field	= field-name ":" OWS field-value OWS
field-name		= token
field-value		= *( field-content / obs-fold )
field-content	= field-vchar [ 1*( SP / HTAB ) field-vchar ]
field-vchar		= VCHAR / obs-text
obs-fold		= CRLF 1*( SP / HTAB ; obsolete line folding ; see Section 3.2.4
*/