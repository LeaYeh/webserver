#!/bin/bash

# Variables
file="$1"        # File to upload
url="$2"         # Target server URL

if [[ -z "$file" || -z "$url" ]]; then
    echo "Usage: $0 <file> <url>"
    exit 1
fi

# Ensure the file exists
if [[ ! -f "$file" ]]; then
    echo "File not found: $file"
    exit 1
fi

# Read file size
file_size=$(stat -c%s "$file")
echo "Uploading $file ($file_size bytes) to $url with default chunk size"
echo "Please wait..."

# Use curl to directly stream the file with chunked encoding
curl -X POST \
     -H "Transfer-Encoding: chunked" \
     -H "Content-Type: application/octet-stream" \
     --data-binary "@$file" \
     "$url"

# Check if curl succeeded
if [[ $? -ne 0 ]]; then
    echo "Error: File upload failed with curl. Shutting down Shell."
    exit 1 # Exit with error status
fi

echo "Upload complete."
