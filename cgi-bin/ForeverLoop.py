import sys
import time

print("Content-Type: text/plain\n")
sys.stdout.flush()

counter = 0
while True:
    print(f"Message {counter}: Streaming data...\n")
    sys.stdout.flush()
    time.sleep(1)
    counter += 1