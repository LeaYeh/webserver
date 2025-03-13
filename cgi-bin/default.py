import os
import sys


def debug_log(message):
    sys.stderr.write("DEBUG: " + message + "\n")
    sys.stderr.flush()

def debug_environment():
    for key, value in os.environ.items():
        debug_log("{}: {}".format(key, value))

def application(environ):
    debug_environment()

    response = ''
    response += 'HTTP/1.1 200 OK\r\n'
    response += 'Content-Type: text/html\r\n'
    response += '\r\n'
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
    debug_log("Starting CGI script")
    sys.stdout.write(application(os.environ))
