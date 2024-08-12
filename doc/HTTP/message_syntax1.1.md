# Protocol Versioning
The version of an HTTP message is indicated by an HTTP-version field in the first line of the message.

```c++
HTTP-version  = HTTP-name "/" MAJOR "." MINOR
```
- HTTP-name `%x48.54.54.50` (ASCII hex) represents the characters "H", "T", "T", "P".
- The HTTP version number consists of two decimal digits separated by a
   "."
- The first digit ("major version") indicates the HTTP messaging syntax, whereas the second digit ("minor version") indicates the highest minor version within that major version to which the sender is conformant and able to understand for future communication.
- Exp for HTTP-version: HTTP/1.1 added features like persistent connections over HTTP/1.0, which improves performance.

# Message Format
## 1. Request Message Format
An HTTP/1.1 request message includes the following components:

```c++
<method> <request-target> <HTTP-version>
<headers>

<body>
```

- `Method`: This specifies the action to be performed. Common methods include:

    ![http_method](/doc/images/http_methods.png)

- `Request Target`: This is the path to the resource you want to access. It usually starts with a /.

- `HTTP-Version`: Specifies the version of HTTP being used, such as HTTP/1.1.

- `Headers`: These provide metadata about the request, such as:

    - **Host**: Specifies the domain name of the server (required in HTTP/1.1).
    - **User-Agent**: Identifies the client software making the request.
    - **Accept**: Specifies the media types the client can process.
- `Body`: This part is optional and is used mainly with POST and PUT methods to send data like form submissions or file uploads.

### Example Request

```c+++
Copy code
GET /index.html HTTP/1.1
Host: www.example.com
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64)
```

```c++
#include <iostream>
#include <string>
#include <sstream>
#include <boost/asio.hpp>
#include <boost/array.hpp>

// Function to create and send an HTTP request
void sendHttpRequest() {
    // Using Boost.Asio for networking
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::resolver resolver(io_service);
    boost::asio::ip::tcp::socket socket(io_service);

    try {
        // Resolve the host name to an IP address
        boost::asio::ip::tcp::resolver::query query("www.example.com", "80");
        boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

        // Connect to the server
        boost::asio::connect(socket, endpoint_iterator);

        // Create the HTTP request
        std::ostringstream request_stream;
        request_stream << "GET /index.html HTTP/1.1\r\n";
        request_stream << "Host: www.example.com\r\n";
        request_stream << "User-Agent: MyClient/1.0\r\n";
        request_stream << "Accept: */*\r\n";
        request_stream << "Connection: close\r\n";
        request_stream << "\r\n"; // End of headers, no body for GET

        // Convert the request to a string
        std::string request = request_stream.str();

        // Send the HTTP request
        boost::asio::write(socket, boost::asio::buffer(request));

        // Read the response
        boost::asio::streambuf response;
        boost::asio::read_until(socket, response, "\r\n");

        // Check the response status
        std::istream response_stream(&response);
        std::string http_version;
        unsigned int status_code;
        std::string status_message;

        response_stream >> http_version;
        response_stream >> status_code;
        std::getline(response_stream, status_message);

        if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
            std::cout << "Invalid response\n";
            return;
        }

        if (status_code != 200) {
            std::cout << "Response returned with status code: " << status_code << "\n";
            return;
        }

        // Read headers
        std::string header;
        while (std::getline(response_stream, header) && header != "\r") {
            std::cout << header << "\n";
        }
        std::cout << "\n";

        // Write the remaining data to output
        if (response.size() > 0) {
            std::cout << &response;
        }

        // Read until EOF
        boost::system::error_code error;
        while (boost::asio::read(socket, response, boost::asio::transfer_at_least(1), error)) {
            std::cout << &response;
        }

        if (error != boost::asio::error::eof) {
            throw boost::system::system_error(error);
        }

    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}
```

## 2. Response Message Format

```c++
<HTTP-version> <status-code> <reason-phrase>
<headers>

<body>
```

- `HTTP-Version`: Indicates the HTTP version used by the server.

- `Status Code`: A three-digit code indicating the result of the request. Common status codes include:

    - **200 OK**: The request was successful.
    - **404 Not Found**: The requested resource could not be found.
    - **500 Internal Server Error**: The server encountered an error.
- `Reason-Phrase`: A short description of the status code. A client SHOULD ignore the reason-phrase content.

- `Headers`: A server **MUST** reject any received request message that contains whitespace between a `header field-name and colon` with a response code of 400 (Bad Request).These provide metadata about the response, such as:

    - **Content-Type**: Specifies the media type of the response body, like text/html or application/json.
    - **Content-Length**: Indicates the size of the response body in bytes.
    - **Set-Cookie**: Sends cookies from the server to the client.

      `Note`: In practice, the "Set-Cookie" header field often appears multiple times in a response message and does not use the list syntax, violating the above requirements on multiple header fields with the same name. Since it cannot be combined into a single field-value recipients ought to handle "Set-Cookie" as a special case while processing header fields.

- `Body`: This contains the content being sent back to the client, such as the HTML of a web page, an image, or JSON data.

### Example Request

```c+++
HTTP/1.1 200 OK
Content-Type: text/html
Content-Length: 1234

<html>
  <head><title>Example Page</title></head>
  <body><h1>Welcome to Example</h1></body>
</html>
```

In C++ code:
```c++
#include <iostream>
#include <string>

// A simple function to generate and print an HTTP response
void sendHttpResponse() {
    std::string response;
    
    // Start Line
    response += "HTTP/1.1 200 OK\r\n";

    // Headers
    response += "Content-Type: text/html\r\n";
    response += "Content-Length: 1234\r\n";
    response += "\r\n"; // Blank line to separate headers from body

    // Body
    response += "<html>\n";
    response += "  <head><title>Example Page</title></head>\n";
    response += "  <body><h1>Welcome to Example</h1></body>\n";
    response += "</html>";

    // Print the HTTP response
    std::cout << response;
}
```

## A server 
### MUST NOT:
- Send Content-Length in such a response unless its field-value equals the decimal number of octets that would have been sent in the payload body of a response if the same request had used the GET method.
- send a Transfer-Encoding header field in any response with a status code of 1xx (Informational) or 204 (No Content).
- send a Transfer-Encoding header field in any 2xx (Successful) response to a CONNECT request.
- send a response containing Transfer-Encoding unless the corresponding request indicates HTTP/1.1 (or later).
- send a Content-Length header field in any message that contains a Transfer-Encoding header field.

### MAY:
- send a Content-Length header field in a response to a HEAD request.
- send a Content-Length header field in a 304 (Not Modified) response to a conditional GET request.
- reject a request that contains a message body but not a Content-Length by responding with 411 (Length Required).
- A server that receives an incomplete request message, usually due to a canceled request or a triggered timeout exception, MAY send an error response prior to closing the connection.

## A Reciepent
### MUST:
- a recipient MUST anticipate potentially large decimal numerals and prevent parsing errors due to integer conversion overflows.
- either reject the message as invalid or replace the duplicated field-values with a single valid Content-Length field containing that decimal value prior to determining the message body length or forwarding the message.
