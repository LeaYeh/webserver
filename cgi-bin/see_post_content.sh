#!/bin/bash
if [ -z "$CONTENT_LENGTH" ]; then
    post_content=""
else
    echo "try to read content length char: ${CONTENT_LENGTH:-0}" >&2
    post_content=$(dd bs=1 count="$CONTENT_LENGTH")
    echo "read content: $post_content" >&2
fi

total_length=$(( 61 + ${CONTENT_LENGTH:-0} ))
total_length_hex=$(printf '%x' "$total_length")

echo "total_length=$total_length" >&2
echo "total_length_hex=$total_length_hex" >&2
# hex_length=${#total_length_hex}

echo "HTTP/1.1 200 OK"
echo "Content-type: text/html"
echo "Transfer-encoding: chunked"
echo "Connection: close"
echo ""
printf "%s\r\n" "$total_length_hex"
printf "<html><body><h1>See What You Post</h1><p> %s </p></body></html>\r\n" "$post_content"
printf "0\r\n\r\n"
