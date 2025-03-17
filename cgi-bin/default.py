#!/usr/bin/env python3
import os
import sys

# Force stdout to use line buffering if possible
if hasattr(sys.stdout, "reconfigure"):
    sys.stdout.reconfigure(line_buffering=True)
else:
    # Fallback for older Python versions: use os.fdopen with buffering=1
    sys.stdout = os.fdopen(sys.stdout.fileno(), "w", buffering=1)

def debug_log(message):
    # Output the message to stderr and flush immediately
    print("DEBUG: " + message, file=sys.stderr, flush=True)

def debug_environment():
    # Log each environment variable
    for key, value in os.environ.items():
        debug_log(f"{key}: {value}")

def main():
    debug_environment()
    # Output HTTP headers and HTML content (each print is immediately flushed)
    print("HTTP/1.1 200 OK")
    print("Content-Type: text/html")
    print("Content-Length: 490")
    print("")  # Empty line indicates the end of headers
    print("<html>")
    print("<head>")
    print("<title>CGI Python</title>")
    print("</head>")
    print("<body>")
    print("<h1>CGI Python</h1>")
    print(f"<p>Python version: {sys.version}</p>")
    print(f"<p>Python path: {sys.path}</p>")
    print(f"<p>Python executable: {sys.executable}</p>")
    print("</body>")
    print("</html>")

if __name__ == '__main__':
    main()
