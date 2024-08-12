# Key Sections Relevant for Web Server Development

## 1. HTTP Message Structure:
 ### 1.1 Request and Response Messages
 HTTP messages consist of a start line, headers, an optional message body, and an empty line (CRLF) marking the end of the header section. Understanding the structure is crucial for parsing and generating HTTP messages.

 ### 1.2 Start Line:
Comprises the request line (in requests) or status line (in responses).
- **Request Line:** Method, Request-URI, and HTTP version.
- **Status Line:** HTTP version, status code, and reason phrase.

## 2. Methods:
- HTTP methods (GET, POST, PUT, DELETE, etc.) define the desired action to be performed on the resource.

![http_method](/doc/images/http_methods.png)

Written in [enum Method](https://github.com/LeaYeh/webserver/blob/623b31802186bcf9b4ee6a1f8f1abd2e8eeaea87/include/AHeaderAnalyzer.hpp#L57-L64)

## 3. Headers:
- Headers provide meta-information about the request or response.
- **General Headers:** Apply to both request and response messages (e.g., Date, Cache-Control).
- **Request Headers:** Specific to requests (e.g., Host, User-Agent).
- **Response Headers:** Specific to responses (e.g., Server, WWW-Authenticate).
- **Entity Headers:** Provide information about the body of the [resource](/doc/background_knowledge.md#definitions) (e.g., Content-Type, Content-Length).

## 4. Chunked Transfer Encoding:
- Allows a message body to be sent in chunks, which is useful for dynamically generated content.
- Each chunk is preceded by its size in bytes, and a zero-sized chunk marks the end of the data.

## 5. Caching Mechanisms:
- HTTP caching mechanisms help reduce latency and network traffic.
- Headers such as Cache-Control, Expires, and ETag manage how responses can be cached and reused.

## 6. Content Negotiation:
- Allows the server to serve different versions of a resource based on the client's capabilities or preferences.
- Uses headers like Accept, Accept-Language, and Accept-Encoding.

## 7. Error Handling:
HTTP status codes indicate the result of the request. (For more detail, check [status_code](/doc/background_knowledge.md#status-codes))
- 1xx: Informational
- 2xx: Success (e.g., 200 OK)
- 3xx: Redirection (e.g., 301 Moved Permanently)
- 4xx: Client Error (e.g., 404 Not Found)
- 5xx: Server Error (e.g., 500 Internal Server Error)

## 8. Security Considerations:
It is essential to consider HTTPS for secure data transmission, authentication mechanisms, and proper error handling to prevent information leakage.

# Building a Web Server
## Request Parsing
**Objective:** Efficiently parse incoming HTTP requests to extract method, URL, headers, and body.

### 1. Start Line Parsing:
- Request Line: Extract the HTTP method (e.g., GET, POST), the Request-URI (path and query), and the HTTP version (e.g., HTTP/1.1).
- Case Focus: Handle various methods and malformed start lines. Consider method validity and parse Request-URI accurately to ensure proper routing and resource access.

### 2. Header Parsing:
- Extract Headers: Capture headers as key-value pairs. Common headers include Host, User-Agent, Accept, and [Content-Type](/doc/background_knowledge.md#definitions).
- Case Focus: Ensure case-insensitive parsing of header names and handle duplicate headers according to the specification (e.g., combining values with a comma for Set-Cookie).

### 3. Body Parsing:
- Handle Content Length: Use the Content-Length header to read the correct amount of body data for requests with bodies (e.g., POST, PUT).
- Chunked Transfer: If Transfer-Encoding: chunked is present, read the body in chunks and decode it accordingly.
- Case Focus: Handle both fixed-length and chunked bodies, as well as edge cases where Content-Length or Transfer-Encoding is missing.

## Response Generation

**Objective:** Create appropriate HTTP responses based on request processing outcomes.

### 1. Status Line:
- Construct Status Line: Generate a status line using the HTTP version, status code (e.g., 200, 404), and reason phrase (e.g., OK, Not Found).
- Case Focus: Ensure correct status codes for various situations, such as 200 for successful requests, 404 for not found, and 500 for server errors.

### 2. Headers:
- Standard Headers: Include necessary headers like Date, Server, Content-Type, and Content-Length.
- Custom Headers: Allow the addition of custom headers as needed.
- Case Focus: Ensure headers like Content-Length accurately reflect the body size and consider Connection header values for persistent connections.

### 3. Body:
- Content Generation: Include HTML, JSON, or other data types in the response body, determined by the request and server logic.
- Case Focus: Properly encode and send the response body, ensuring that it matches the Content-Type specified.

## Connection Handling
**Objective:** Manage connections efficiently to handle multiple simultaneous requests.

### 1. Persistent Connections:

- Keep-Alive: Support persistent connections using the Connection: keep-alive header, allowing multiple requests over a single connection.
- Case Focus: Properly handle connection closing using the Connection: close header when necessary, and manage timeouts to release inactive connections.

### 2. Concurrency:

- Multi-threading/Asynchronous I/O: Implement threading or asynchronous techniques to handle multiple client requests simultaneously.
- Case Focus: Optimize server performance under heavy load by managing resources efficiently and preventing blocking operations.

## Error Handling
**Objective:** Provide clear and informative error responses for client and server errors.

### 1. Client Errors:

- 4xx Responses: Return appropriate 4xx status codes (e.g., 400 Bad Request, 404 Not Found) with descriptive error messages.
- Case Focus: Gracefully handle malformed requests and invalid URLs, providing meaningful feedback to the client.

### 2. Server Errors:
- 5xx Responses: Return 5xx status codes (e.g., 500 Internal Server Error, 503 Service Unavailable) for server-side issues.
- Case Focus: Ensure robust exception handling to catch and log server errors, minimizing downtime and improving diagnostics.

## Content Delivery
**Objective:** Deliver content efficiently and accurately based on client requests and server capabilities.

### 1. Content Negotiation:

- Headers: Use headers like Accept, Accept-Language, and Accept-Encoding to determine the best representation of a resource to send to the client.
- Case Focus: Implement logic to select appropriate content types and handle multiple language versions if supported.
### 2. Transfer Encoding:

- Chunked Encoding: Use chunked transfer encoding for dynamically generated or large content that can’t be determined at the start.
- Case Focus: Correctly encode and send chunked data, ensuring proper termination with a zero-length chunk.

## Performance Optimization
**Objective:** Improve server efficiency and response times using HTTP/1.1 features.

### 1. Caching:
- Headers: Utilize Cache-Control, Expires, ETag, and Last-Modified headers to control caching behavior and reduce redundant server requests.
- Case Focus: Implement validation caching to check for resource changes using If-Modified-Since or If-None-Match headers, minimizing data transmission.

### 2. Compression:
- Content-Encoding: Use Content-Encoding: gzip or deflate to compress response bodies and reduce bandwidth usage.
- Case Focus: Ensure correct negotiation and application of compression algorithms based on client capabilities.

### 3. Load Balancing:
- Distribution: Distribute incoming requests across multiple server instances to balance load and improve scalability.
- Case Focus: Implement efficient load balancing strategies to maintain optimal performance under varying traffic conditions.

## Security Considerations
**Objective:** Ensure secure communication and protect the server from common vulnerabilities.

### 1. HTTPS:
- TLS/SSL: Use HTTPS to encrypt data in transit, protecting against eavesdropping and man-in-the-middle attacks.
- Case Focus: Implement SSL/TLS with proper certificate management and configuration to ensure secure connections.

### 2. Authentication and Authorization:
- Mechanisms: Implement authentication schemes like Basic, Digest, or OAuth to verify client identity.
- Case Focus: Ensure secure storage and transmission of credentials, and implement proper access control measures.

### 3. Input Validation:
- Sanitization: Validate and sanitize all inputs to prevent injection attacks and buffer overflows.
- Case Focus: Pay particular attention to request headers, query parameters, and body data to eliminate security vulnerabilities.

