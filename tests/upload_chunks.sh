#!/bin/bash

# Variables
file="$1"        # File to upload
url="$2"         # Target server URL
chunk_size="$3"  # Chunk size in bytes

if [[ -z "$file" || -z "$url" || -z "$chunk_size" ]]; then
    echo "Usage: $0 <file> <url> <chunk_size>"
    exit 1
fi

# Ensure the file exists
if [[ ! -f "$file" ]]; then
    echo "File not found: $file"
    exit 1
fi

# Read file size
file_size=$(stat -c%s "$file")
echo "Uploading $file ($file_size bytes) to $url with chunk size $chunk_size bytes"

# Offset to track how much data has been read
offset=0

# Process the first chunk with headers
chunk=$(dd if="$file" bs=1 count="$chunk_size" skip=$offset 2>/dev/null)
if [[ -n "$chunk" ]]; then
    echo "Sending first chunk with headers..."
    curl -X POST \
         -H "X-File-Name: $(basename "$file")" \
         -H "Transfer-Encoding: chunked" \
         -H "Content-Type: application/octet-stream" \
         --data-binary "$chunk" \
         "$url"
    offset=$((offset + chunk_size))
fi

# Send subsequent chunks without headers
while [[ $offset -lt $file_size ]]; do
    chunk=$(dd if="$file" bs=1 count="$chunk_size" skip=$offset 2>/dev/null)

    if [[ -z "$chunk" ]]; then
        break
    fi

    echo "Sending subsequent chunk from offset $offset..."
    curl -X POST \
         --data-binary "$chunk" \
         "$url"

    offset=$((offset + chunk_size))
done

echo "Upload complete."
