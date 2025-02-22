import sys
import time

print("<html><head><title>CGI Response</title></head><body>")
print("<body><h1>CGI Response</h1>")

print("HTTP/1.1 200 OK")
print("Content-type: text/html")
print()
sys.stdout.flush()

counter = 0
while True:
    print(f"Message {counter}: Streaming data...\n")
    sys.stdout.flush()
    time.sleep(1)
    counter += 1