# Analyzing requests from clients

## 1. Types of files
The file extensions in the URLs of the HTTP **`GET`** requests:

- HTML Files: `.html`, `.htm`
- CSS Files: `.css`
- JavaScript Files: `.js`
- Image Files: `.jpg`, `.jpeg`, `.png`, `.gif`, `.svg`
- Video Files: ``mp4`, `.avi`, `.webm`
- Audio Files: `.mp3`, `.wav`, `.ogg`
- Document Files: `.pdf`, `.doc`, `.docx`, `.xls`, `.xlsx`
- Data Files: `.json`, `.xml`, `.csv`
- Font Files: `.woff`, `.woff2`, `.ttf`, `.otf`

## 2. Upload Files
File uploads are using HTTP **`POST`** requests:

- **POST Requests**: Examine the server logs or requests to identify POST requests to endpoints designed to handle file uploads.
- **Content-Type Headers**: Check for `Content-Type: multipart/form-data`, which indicates file upload forms.
- **Request Body**: Uploaded files are usually included in the body of the request. The presence of boundary markers in the body can indicate multipart data, which is used in file uploads.

## 3. Access Files
File access can be inferred from **`GET`** requests where clients request to retrieve specific files. Analyze:

- **GET Requests**: Look for GET requests targeting specific file paths.
- **Referrer Headers**: This can show where the request is coming from, helping to understand if the file is being accessed directly or via a page.
- **Authentication/Authorization Logs**: Accessing certain files might require authentication. Review logs to see if access is granted based on user roles or permissions.

## 4. CGI
CGI scripts are used to execute programs on the server and dynamically generate web content. To determine if CGI is being used:

- **Script Execution**: Look for URLs ending with `.cgi` .pl, .py, etc., which indicate CGI scripts.
- **POST/GET Requests to CGI Directories**: Requests targeting directories typically used for CGI, such as /cgi-bin/.
- **Execution Logs**: Server logs might indicate script execution, showing request methods and script paths.
- **Configuration Files**: Check web server configuration files (like `httpd.conf` for Apache) for CGI settings and handlers.

## How to Analyze?
To perform this analysis, we use:

- **Server Logs**: Analyze logs like Apache's access.log or Nginx's access.log to see raw request data.
- **Monitoring Tools**: Use tools like Google Analytics, New Relic, or server monitoring software to track file access and uploads.
- **Custom Scripts**: Write scripts to parse logs and identify patterns relevant to file types, uploads, and CGI use.
