package main

import (
	"bufio"
	"fmt"
	"net"
	"os"
	"strings"
)

func main() {
	if len(os.Args) < 2 {
		fmt.Println("Usage: manual_chunked_upload <url>")
		fmt.Println("Example: manual_chunked_upload http://127.0.0.1:8080/upload")
		return
	}

	url := os.Args[1]
	urlParts := strings.SplitN(url, "/", 4)
	if len(urlParts) < 4 {
		fmt.Println("Invalid URL format. Use format: http://<host>:<port>/<path>")
		return
	}

	hostPort := urlParts[2]
	path := "/" + urlParts[3]

	fmt.Println("Connecting to server:", hostPort)
	conn, err := net.Dial("tcp", hostPort)
	if err != nil {
		fmt.Println("Error connecting to server:", err)
		return
	}
	defer conn.Close()

	fmt.Println("Connected to server.")

	headers := fmt.Sprintf("POST %s HTTP/1.1\r\n", path)
	headers += fmt.Sprintf("Host: %s\r\n", hostPort)
	headers += "Content-Type: application/octet-stream\r\n"
	headers += "Transfer-Encoding: chunked\r\n"
	headers += "X-File-Name: manual_chunked_upload.txt\r\n"
	headers += "Connection: close\r\n\r\n"

	_, err = conn.Write([]byte(headers))
	if err != nil {
		fmt.Println("Error sending headers:", err)
		return
	}

	fmt.Println("Headers sent. Now you can input chunk data.")
	fmt.Println("Type CRLF (press Enter) to mark end of a chunk. Press Enter twice to simulate EOF.")

	scanner := bufio.NewScanner(os.Stdin)
	var chunkData strings.Builder
	emptyLineCount := 0
	for {
		fmt.Print("Input data (or press Enter for CRLF): ")
		if !scanner.Scan() {
			break
		}

		line := scanner.Text()
		if line == "" {
			emptyLineCount++
			if emptyLineCount == 2 {
				break
			}
			if chunkData.Len() > 0 {
				chunk := strings.TrimSuffix(chunkData.String(), "\n")
				chunkSize := fmt.Sprintf("%x\r\n", len(chunk))
				data := []byte(chunkSize + chunk + "\r\n")

				_, err = conn.Write(data)
				if err != nil {
					fmt.Println("Error sending chunk:", err)
					return
				}
				chunkData.Reset()
			}
			continue
		}

		emptyLineCount = 0
		chunkData.WriteString(line)
	}

	if chunkData.Len() > 0 {
		chunk := chunkData.String()
		chunkSize := fmt.Sprintf("%x\r\n", len(chunk))
		_, err = conn.Write([]byte(chunkSize + chunk + "\r\n"))
		if err != nil {
			fmt.Println("Error sending final chunk:", err)
			return
		}
	}

	_, err = conn.Write([]byte("0\r\n\r\n"))
	if err != nil {
		fmt.Println("Error sending final chunk marker:", err)
		return
	}

	fmt.Println("All chunks sent. Waiting for response...")

	response, err := bufio.NewReader(conn).ReadString('\n')
	if err != nil {
		fmt.Println("Error reading response:", err)
		return
	}

	fmt.Println("Server response:")
	fmt.Println(response)
}
