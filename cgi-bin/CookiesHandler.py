import os
import http.cookies

print("<html><head><title>CGI Response</title></head><body>")
print("<body><h1>CGI Response</h1>")

print("HTTP/1.1 200 OK")
print("Content-type: text/html")
print()

cookie = http.cookies.SimpleCookie(os.environ.get("HTTP_COOKIE", ""))

print("\n")  
print(f"""
<html>
<head><title>Cookies</title></head>
<body>
    <h1>Cookie Handling </h1>
    <p> {cookie} </p>
</body>
</html>
""")

#just a draft idea cuz I dont have this variable in env >_<