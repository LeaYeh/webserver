## State Machine Diagram
```c++
[Start] → [Request Line] → [Header Field] → [Body] → [End]
                   ↳ [Header Field] ↘ [End]
```
### 1. Start State 
The initial state, waiting for the beginning of a request.

### 2. Request Line Parsing:
- This is where the 1st **recursion** occurs, because it reads each letter and accepts a digit or letter.
- In our case `Request line` contains the HTTP method, the request target/path (Request-URI), and the HTTP version:

    ```c++
    GET /index.html HTTP/1.1
    ```

### 3. Header Field Parsing:
- This is where the 2nd **recursion** occurs.
- Split each header line into a key-value pair and store it in a dictionary.
- Read each line until an empty line is encountered. (Blank line).

    ```c++
    Host: example.com
    User-Agent: Mozilla/5.0
    ```

### 4. Body Parsing:
- Before Body part is a `Blank line`: a line separating headers from the body.
- Optional content, typically present in POST requests.
- Determine the presence and length of the body using [Content-Length](/doc/background_knowledge.md#definitions) or [Transfer-Encoding](/doc/HTTP/general_1.1.md#2-transfer-encoding).
- Read the body based on the determined length or chunk encoding

### 5. Complete: 
The final state, indicating that the entire request has been parsed successfully.

### 6. Error: 
- Implement error handling for each state to manage invalid inputs and malformed requests.
- Ensure proper logging and error response generation for client and server errors.

## Transitions:
Use a loop to manage state transitions and parsing flow.

Ensure that each state performs its function and transitions appropriately based on the input data.

- **Start -> Request Line Parsing**: When the first line of the request is received.
- **Request Line Parsing -> Header Parsing**: When the request line is successfully parsed.
- **Header Parsing -> Header Parsing**: For each subsequent header line.
- **Header Parsing -> Body Parsing**: When a blank line is encountered (indicating the end of headers).
- **Body Parsing -> Complete**: When the body is fully read (based on Content-Length or Transfer-Encoding headers).
- **Any State -> Error**: On encountering a malformed line or unexpected input.

## Key Considerations
- **Line Endings**: HTTP uses CRLF (\r\n) as the line separator. Ensure your parser handles this correctly.
- **Headers**: Headers are case-insensitive. Consider normalizing them.
- **Chunked Transfer Encoding**: If using HTTP/1.1, be prepared to handle chunked transfer encoding.
- **Error Handling**: Robust error handling is crucial for security and stability.

## Implementation Details

### 1. Request Line Parsing:

```c++
// Function to split a string by spaces into a vector of strings
std::vector<std::string> split(const std::string &str) {
    std::istringstream iss(str);
    std::vector<std::string> parts;
    std::string part;
    while (iss >> part) {
        parts.push_back(part);
    }
    return parts;
}

// Function to parse the HTTP request line
std::tuple<std::string, std::string, std::string> parse_request_line(const std::string &line) {
    std::vector<std::string> parts = split(line);
    
    // Check that the request line contains exactly 3 parts
    if (parts.size() != 3) {
        throw std::invalid_argument("Invalid request line");
    }
    
    // Extract the method, path, and HTTP version from the parts
    std::string method = parts[0];
    std::string path = parts[1];
    std::string http_version = parts[2];
    
    // Check that the HTTP version starts with "HTTP/"
    if (http_version.substr(0, 5) != "HTTP/") {
        throw std::invalid_argument("Invalid HTTP version");
    }
    
    return std::make_tuple(method, path, http_version);
}
```

### 2. Header Field Parsing:
```c++
std::map<std::string, std::string> parse_headers(const std::vector<std::string> &lines) {

    std::map<std::string, std::string> headers;
    for (const auto &line : lines) {
        // Trim whitespace from both ends of the line
        std::string trimmed_line = line;
        trimmed_line.erase(trimmed_line.begin(), std::find_if(trimmed_line.begin(), trimmed_line.end(), [](unsigned char c) {
            return !std::isspace(c);
        }));
        trimmed_line.erase(std::find_if(trimmed_line.rbegin(), trimmed_line.rend(), [](unsigned char c) {
            return !std::isspace(c);
        }).base(), trimmed_line.end());

        // Break if the line is empty or end of headers
        if (trimmed_line.empty()) {
            break;
        }

        // Split the line into key and value at the first occurrence of ':'
        std::size_t pos = trimmed_line.find(':');
        if (pos == std::string::npos) {
            continue;  // Handle lines without a colon gracefully
        }

        std::string key = trimmed_line.substr(0, pos);
        std::string value = trimmed_line.substr(pos + 1);

        // Trim whitespace from key and value
        key.erase(key.begin(), std::find_if(key.begin(), key.end(), [](unsigned char c) {
            return !std::isspace(c);
        }));
        key.erase(std::find_if(key.rbegin(), key.rend(), [](unsigned char c) {
            return !std::isspace(c);
        }).base(), key.end());

        value.erase(value.begin(), std::find_if(value.begin(), value.end(), [](unsigned char c) {
            return !std::isspace(c);
        }));
        value.erase(std::find_if(value.rbegin(), value.rend(), [](unsigned char c) {
            return !std::isspace(c);
        }).base(), value.end());

        // Convert the key to lowercase for case-insensitive comparison
        std::transform(key.begin(), key.end(), key.begin(), [](unsigned char ch) {
            return std::tolower(ch);
        });

        // Store the header in the map
        headers[key] = value;
    }

    return headers;
}
```