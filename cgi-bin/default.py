import os


def application(environ) -> str:
    response = ''

    response += 'HTTP/1.1 200 OK\n'
    response += 'Content-Type: text/html\n'
    response += '\n'
    response += '<html>\n'
    response += '<head>\n'
    response += '<title>CGI Python</title>\n'
    response += '</head>\n'
    response += '<body>\n'
    response += '<h1>CGI Python</h1>\n'
    response += '<p>Python version: ' + str(os.sys.version) + '</p>\n'
    response += '<p>Python path: ' + str(os.sys.path) + '</p>\n'
    response += '<p>Python executable: ' + str(os.sys.executable) + '</p>\n'
    response += '</body>\n'
    response += '</html>\n'
    return response

if __name__ == '__main__':
    print(application({}))
