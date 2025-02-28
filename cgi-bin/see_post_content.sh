#!/bin/sh
if [ -z "$CONTENT_LENGTH" ]; then
    post_content=""
else
    read -n "$CONTENT_LENGTH" post_content
fi

total_length=$(( 62 + ${CONTENT_LENGTH:-0} ))
total_length_hex=$(printf '%x' "$total_length")

echo "HTTP/1.1 200 OK"
echo "Content-type: text/html"
echo "Transfer-encoding: chunked"
echo "Connection: close"
echo ""
echo "$total_length_hex"
echo "<html><body><h1>See What You Post</h1><p> $post_content </p></body></html>"
echo "0"
echo ""
