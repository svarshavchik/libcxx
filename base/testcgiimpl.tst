#! /bin/bash

./testcgiimpl 1
./testcgiimpl
./testcgiimpl 2
./testcgiimpl 6

echo ""

(export SCRIPT_NAME=/cgi-bin/test
./testcgiimpl 1
./testcgiimpl
./testcgiimpl 2
./testcgiimpl 6

echo ""

export PATH_INFO=/subpath
./testcgiimpl 1
./testcgiimpl
./testcgiimpl 2
./testcgiimpl 6

echo ""
export HTTP_HOST='www.example.com:80'
export QUERY_STRING='foo=bar&b%41r=baz&foo=foobar'
./testcgiimpl 1
./testcgiimpl
./testcgiimpl 2

export SSL_PROTOCOL=TLS1
./testcgiimpl 6
./testcgiimpl 5
)

echo ''
echo 'foo=bar&b%41r=baz&foo2=foob%40r' | tr -d '\012' >testcgiimpl.tmp
HTTP_HOST=post.example.com REQUEST_METHOD=POST CONTENT_TYPE='APPLICATION/X-WWW-FORM-URLENCODED' SCRIPT_NAME=/test.cgi CONTENT_LENGTH=`wc -c <testcgiimpl.tmp` ./testcgiimpl 6 <testcgiimpl.tmp
echo ''
HTTP_HOST=post.example.com REQUEST_METHOD=POST CONTENT_TYPE='APPLICATION/X-WWW-FORM-URLENCODED' SCRIPT_NAME=/test.cgi QUERY_STRING=extra1=value1 CONTENT_LENGTH=`wc -c <testcgiimpl.tmp` ./testcgiimpl 6 <testcgiimpl.tmp

echo "The quick brown fox jumped over the lazy dog's tail" >testcgiimpl.tmp
echo ''
HTTP_HOST=post.example.com REQUEST_METHOD=POST CONTENT_TYPE='text/plain; charset=utf-8' SCRIPT_NAME=/test.cgi CONTENT_LENGTH=`wc -c <testcgiimpl.tmp` ./testcgiimpl 6 <testcgiimpl.tmp
rm -f testcgiimpl.tmp

./testcgiimpl -- - foo bar Foo "foo bar" baz 'foo!'

echo ''
echo 'foo=bar&b%41r=baz&foo2=foob%40r' | tr -d '\012' >testcgiimpl.tmp
HTTP_HOST=post.example.com REQUEST_METHOD=POST CONTENT_TYPE='APPLICATION/X-WWW-FORM-URLENCODED' SCRIPT_NAME=/test.cgi CONTENT_LENGTH=`wc -c <testcgiimpl.tmp` LANG=en_US.UTF-8 ./testcgiimpl --set-property x::http::form::maxsize=10 6 <testcgiimpl.tmp 2>&1

rm -f testcgiimpl.tmp

