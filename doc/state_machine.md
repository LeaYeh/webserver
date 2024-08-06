## State Machine Diagram
```c++
[Start] → [Request Line] → [Header Field] → [Body] → [End]
                   ↳ [Header Field] ↘ [End]
```
### 1. Start State 
Parse the request line to extract the HTTP method, Request-URI, and HTTP version.

### 2. Header Field Parsing:
- Read each line until an empty line is encountered.
- Split each header line into a key-value pair and store it in a dictionary.

### 3. Body Parsing:
- Determine the presence and length of the body using Content-Length or Transfer-Encoding.
- Read the body based on the determined length or chunk encoding

### 4. Error Handling:
- Implement error handling for each state to manage invalid inputs and malformed requests.
- Ensure proper logging and error response generation for client and server errors.

### 5. State Transitions:
- Use a loop to manage state transitions and parsing flow.
- Ensure that each state performs its function and transitions appropriately based on the input data.

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