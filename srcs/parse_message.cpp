/*
message format
syntax notation: ABNF
説明が抜けている単語や記法はABNFのwikipediaか, RFC7230内で該当単語を検索するとヒットするはずです.


- HTTP メッセージ全体の文法
HTTP-message	= start-line *( header-field CRLF ) CRLF [ message-body ]


- start-line
start-line		= request-line / status-line

- start-line: request
request-line	= method SP request-target SP HTTP-version CRLF
method			= token
token			= 1*tchar
tchar			= "!" / "#" / "$" / "%" / "&" / "'" / "*" / "+" / "-" / "." / "^" / "_" / "`" / "|" / "~" / DIGIT / ALPHA ; any VCHAR, except delimiters

- start-line: response
status-line		= HTTP-version SP status-code SP reason-phrase CRLF
status-code 	= 3DIGIT
reason-phrase	= *( HTAB / SP / VCHAR / obs-text )
obs-text		= %x80-FF


-  header-field ※行頭に空白を挟むことで行の拡張ができるという話がありましたが, message/httpメディアタイプ(content-type?)を除いて, 新仕様からは廃止されていました
header-field	= field-name ":" OWS field-value OWS
field-name		= token
field-value		= *( field-content / obs-fold )
field-content	= field-vchar [ 1*( SP / HTAB ) field-vchar ]
field-vchar		= VCHAR / obs-text
obs-fold		= CRLF 1*( SP / HTAB ) ; obsolete line folding ; see Section 3.2.4
OWS				= *( SP / HTAB ) ; optional whitespace


- message-body
message-body = *OCTET
*/