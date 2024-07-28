# Background Knowledge

## HTTP

### What is HTTP?

> **H**yperText **T**ransfer **P**rotocol

A standard for communication between computers (two or more participants) using a language that computers can understand, as well as **various control** and **error-handling** methods (behavior conventions and standards).

### What means protocol?

A set of rules that define how computers communicate with each other.

HTTP is a protocol that defines how computers communicate over the internet.

HTTP is a bi-directional protocol, means both the client and the server need to follow the rules.

### What is HyperText?

**Text**: A sequence of characters.

**HyperText**: More than just text. It can contain links to other texts, images, videos, etc.

### Status Codes

* 1xx: Informational
    * Means the request has been received and the process is continuing.
* 2xx: Success
    * Means the action was successfully received, understood, and accepted.
    * 200: OK
    * 201: Created
    * 204: No Content
    * 206: Partial Content
        - The server is delivering only part of the resource due to a range header sent by the client.
* 3xx: Redirection
    * Means resources are not available at the current location and the client should go to another location.
    * 301: Moved Permanently
        * The resource has been moved permanently to a new location.
    * 302: Found
        * The resource has been found at a different location.
    * 304: Not Modified
        * No need to retransmit the resource. The client can use the same cached resource.
* 4xx: Client Error
    * Means the client has done something wrong. The server cannot process the request.
    * 400: Bad Request
        * The server cannot process the request due to a client error.
    * 401: Unauthorized
        * The client must authenticate itself to get the requested response.
    * 403: Forbidden
        * The client does not have permission to access the requested resource.
    * 404: Not Found
        * The server cannot find the requested resource.
    * 405: Method Not Allowed
        * The method specified in the request is not allowed.
    * 408: Request Timeout
        * The server timed out waiting for the request.
    * 429: Too Many Requests
        * The client has sent too many requests in a given amount of time.
* 5xx: Server Error
    * Means the server has done something wrong. The server cannot process the request.
    * 500: Internal Server Error
        * The server has encountered a situation it doesn't know how to handle.
    * 501: Not Implemented
        * The request method is not supported by the server.
    * 502: Bad Gateway
        * The server works fine, but the server received an invalid response from an upstream server.
    * 503: Service Unavailable
        * The server is busy and not ready to handle the request.
    * 504: Gateway Timeout
        * The server received an invalid response from an upstream server.

### Definitions

* **Host**
    * The server that hosts the website. It can be a domain name or an IP address.
        * Example: `www.google.com`
```http
Host: www.google.com
```

* **Content-Length**
    * The size of the response body in bytes.
```http
Content-Length: 1024
```

* **Content-Type**
    * The type of the response body.
        * Example: `text/html`, `application/json`, `image/png`
```http
Content-Type: text/html; Charset=utf-8
Accept: */*
```

* **Connection**
    * Use to manage the connection between the client and the server.
    * `keep-alive`
        * The connection will be kept alive. The client can send multiple requests.
        * The connectio will be closed after a close request, or the server closes the connection. Otherwise, the connection will be kept alive and can be reused.
    * `close`: The connection will be closed after the response.
```http
Connection: keep-alive
```

* **content-encoding**
    * The encoding of the response body.
        * Example: `gzip`, `deflate`
```http
// Client can accept gzip encoding
Accept-Encoding: gzip, deflate

// Server can choose to send the response in gzip encoding
Content-Encoding: gzip
```

