import cgi
import os


print("<html><head><title>CGI Response</title></head><body>")
print("<body><h1>CGI Response</h1>")

print("HTTP/1.1 200 OK")
print("Content-type: text/html")
print()

form = cgi.FieldStorage()

if os.environ['REQUEST_METHOD'] == 'POST':
    print("<p>POST request</p>")
    post_data = form.getvalue('data', 'No data sent')
    print(f"<p>POST data: {post_data}</p>")
else:
    print("<p>GET request</p>")
    print("<p>GET data: {}</p>".format(form.getvalue('data')))

print("</body></html>")