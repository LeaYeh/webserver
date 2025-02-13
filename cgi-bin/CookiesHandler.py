import os
import http.cookies

print("Content-Type: text/html")

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