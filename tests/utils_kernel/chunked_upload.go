package main

import (
	"fmt"
	"io"
	"net/http"
	"os"
	"path/filepath"
	"strconv"
)

func ChunkedUpload(url string, filename string, chunkSize int) (*http.Response, error) {
    file, err := os.Open(filename)
    if err != nil {
        return nil, err
    }
    defer file.Close()

    stat, err := file.Stat()
    if err != nil {
        return nil, err
    }

    fileSize := stat.Size()
    chunks := (fileSize + int64(chunkSize) - 1) / int64(chunkSize)

    reader := &ChunkReader{
        File:      file,
        ChunkSize: chunkSize,
    }

    req, err := http.NewRequest("POST", url, reader)
    if err != nil {
        return nil, err
    }

    req.Header.Set("Content-Type", "application/octet-stream")
    req.Header.Set("Transfer-Encoding", "chunked")
    req.Header.Set("X-File-Size", fmt.Sprintf("%d", fileSize))
    req.Header.Set("X-Chunk-Size", fmt.Sprintf("%d", chunkSize))
    req.Header.Set("X-Chunk-Count", fmt.Sprintf("%d", chunks))
    req.Header.Set("X-File-Name", filepath.Base(filename))

    client := &http.Client{}
    return client.Do(req)
}

type ChunkReader struct {
    File      *os.File
    ChunkSize int
}

func (cr *ChunkReader) Read(p []byte) (n int, err error) {
    if len(p) > cr.ChunkSize {
        p = p[:cr.ChunkSize]
    }
    return cr.File.Read(p)
}

func main() {
    if len(os.Args) < 3 {
        fmt.Println("Usage: chunked_upload <url> <file> [chunk_size]")
        fmt.Println("Example: chunked_upload http://127.0.0.1:8080/upload file.txt 1024")
        return
    }

    url := os.Args[1]
    filePath := os.Args[2]
    chunkSize := 1024 // 默認 chunk 大小

    if len(os.Args) > 3 {
        var err error
        chunkSize, err = strconv.Atoi(os.Args[3])
        if err != nil {
            fmt.Println("Invalid chunk size. Using default 1024 bytes.")
            chunkSize = 1024
        }
    }

    resp, err := ChunkedUpload(url, filePath, chunkSize)
    if err != nil {
        fmt.Println("Error uploading file:", err)
        return
    }
    defer resp.Body.Close()

    fmt.Println("Response:", resp.Status)
    io.Copy(os.Stdout, resp.Body)
}
