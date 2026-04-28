<div align="center">

# Webserv | @42Vienna
### A Non-Blocking HTTP/1.1 Web Server in C++98

<br>

[![GitHub release (latest by date including pre-releases)](https://img.shields.io/github/v/release/LeaYeh/webserver?include_prereleases&style=flat&color=blue)](https://github.com/LeaYeh/webserver/releases)
[![GitHub tag (latest SemVer)](https://img.shields.io/github/v/tag/LeaYeh/webserver?label=version&style=flat&color=yellow)](https://github.com/LeaYeh/webserver/tags)
[![Visits Badge](https://visitor-badge.laobi.icu/badge?page_id=LeaYeh.webserver&style=flat&right_color=navy)](https://github.com/LeaYeh/webserver)
[![GitHub](https://img.shields.io/github/license/LeaYeh/webserver?style=flat&color=darkred)](https://github.com/LeaYeh/webserver/blob/main/LICENSE)

<br>

This project implements a production-style HTTP/1.1 web server from scratch in C++98, drawing design inspiration from nginx. It features a non-blocking, event-driven I/O model built on the Reactor pattern, nginx-style hierarchical configuration, full CGI/1.1 support, chunked transfer encoding, and dynamic template rendering. The architecture is deliberately minimal — no threads, no third-party libraries — to demonstrate mastery of POSIX sockets, `epoll`, process management, and the HTTP/1.1 specification.

</div>

---

# Table of Contents

* [Supported Features](#supported-features)
* [Quick Start](#quick-start)
* [Configuration System](#configuration-system)
    * [Directive Reference](#directive-reference)
    * [Example Configuration](#example-configuration)
* [Architecture](#architecture)
    * [Reactor Pattern and Event Loop](#reactor-pattern-and-event-loop)
    * [Connection Lifecycle](#connection-lifecycle)
    * [Request Processing State Machine](#request-processing-state-machine)
* [HTTP Request Parsing](#http-request-parsing)
    * [Request Line Analyzer](#request-line-analyzer)
    * [URI Analyzer](#uri-analyzer)
    * [Header Analyzer](#header-analyzer)
    * [Chunked Transfer Decoding](#chunked-transfer-decoding)
* [HTTP Handlers](#http-handlers)
    * [GET — Static File Serving and Directory Listing](#get--static-file-serving-and-directory-listing)
    * [POST — File Upload](#post--file-upload)
    * [DELETE — File Removal](#delete--file-removal)
* [CGI Implementation](#cgi-implementation)
    * [Process Flow](#process-flow)
    * [Environment Variables](#environment-variables)
    * [Timeout Enforcement](#timeout-enforcement)
* [Error Handling](#error-handling)
* [Build System](#build-system)

---

# Supported Features

| Category        | Feature                           | Description                                                              | Status |
|-----------------|-----------------------------------|--------------------------------------------------------------------------|--------|
| **Server**      | Non-blocking I/O                  | All sockets are non-blocking; event-driven via `epoll`                   | ✅     |
|                 | Keep-Alive connections            | Configurable idle timeout per server block                               | ✅     |
|                 | Virtual hosts                     | Multiple `server` blocks on different `listen` addresses                 | ✅     |
|                 | Signal handling                   | Graceful shutdown on `SIGINT`                                            | ✅     |
| **Config**      | nginx-style config file           | Global → HTTP → server → location hierarchy                              | ✅     |
|                 | Location routing                  | Longest-prefix matching across `route` directives                        | ✅     |
|                 | `limit_except`                    | Per-location HTTP method allowlist                                        | ✅     |
|                 | `client_max_body_size`            | Enforces request body size limit (returns 413)                           | ✅     |
|                 | `root` / `alias`                  | File system path mapping per location                                    | ✅     |
|                 | `redirect`                        | HTTP redirect with configurable target URL                               | ✅     |
|                 | `autoindex`                       | Directory listing with HTML template                                     | ✅     |
|                 | `error_page`                      | Custom error page HTML template                                          | ✅     |
| **HTTP**        | GET                               | Static files with MIME detection                                         | ✅     |
|                 | POST                              | File upload to configurable directory                                    | ✅     |
|                 | DELETE                            | Remove files within upload-enabled directories                           | ✅     |
|                 | Chunked Transfer-Encoding         | Decode chunked request bodies; chunk response for large files            | ✅     |
|                 | Cookie parsing                    | Parses `Cookie` request header into key-value map                        | ✅     |
|                 | Percent-encoding                  | RFC 3986 URI decoding                                                    | ✅     |
| **CGI**         | Script execution via `fork/execve`| Isolated child process per request                                       | ✅     |
|                 | POST body piping                  | Passes request body to CGI stdin via temp file                           | ✅     |
|                 | Response header parsing           | Strips CGI headers from output before forwarding to client               | ✅     |
|                 | Timeout                           | Kills CGI process after 3 seconds via monitor process                    | ✅     |
| **Templates**   | Error page rendering              | Substitutes `{{status_code}}` and `{{message}}` in HTML template        | ✅     |
|                 | Autoindex rendering               | Renders directory listing with clickable file links                      | ✅     |
| **Not Implemented** | HTTPS / TLS                   | No SSL support                                                           | ✗      |
|                 | HTTP/2                            | HTTP/1.1 only                                                            | ✗      |
|                 | Multi-worker processes            | Single reactor process only                                              | ✗      |

---

# Quick Start

```sh
# Build
make

# Run with a config file
./webserv configs/default.conf

# Fetch a page
curl http://127.0.0.1:8080/

# Upload a file
curl -X POST http://127.0.0.1:8080/upload \
     --data-binary @myfile.txt \
     -H "Content-Type: text/plain"

# Delete a file
curl -X DELETE http://127.0.0.1:8080/upload/myfile.txt
```

---

# Configuration System

The configuration file follows an nginx-style hierarchical structure: directives cascade from outer blocks into inner ones, with inner blocks taking precedence.

```
global {
    http {
        server {
            location { ... }
        }
    }
}
```

## Directive Reference

### Global Block

| Directive           | Example              | Description                                 |
|---------------------|----------------------|---------------------------------------------|
| `worker_processes`  | `worker_processes 1;`| Number of worker processes (only 1 active)  |
| `worker_connections`| `worker_connections 1024;`| Max simultaneous connections          |

### HTTP Block

| Directive              | Example                                   | Description                              |
|------------------------|-------------------------------------------|------------------------------------------|
| `client_max_body_size` | `client_max_body_size 1M;`                | Maximum allowed request body size        |
| `default_type`         | `default_type text/html;`                 | Fallback MIME type                       |
| `error_page`           | `error_page ./www/html/error_page.html;`  | HTML template for error responses        |
| `autoindex_page`       | `autoindex_page ./www/html/autoindex.html;`| HTML template for directory listing     |

### Server Block

| Directive            | Example                         | Description                                   |
|----------------------|---------------------------------|-----------------------------------------------|
| `listen`             | `listen 127.0.0.1:8080;`        | IP address and port to bind                   |
| `server_name`        | `server_name localhost;`        | Hostname for virtual host matching            |
| `keep_alive_timeout` | `keep_alive_timeout 65;`        | Idle connection timeout in seconds            |
| `error_log`          | `error_log log/error.log error;`| Log file path and log level (error/info)      |

### Location Block

| Directive        | Example                    | Description                                             |
|------------------|----------------------------|---------------------------------------------------------|
| `route`          | `route /upload;`           | URL prefix this block matches (longest prefix wins)     |
| `limit_except`   | `limit_except GET POST;`   | Allowed HTTP methods; others get 405                    |
| `root`           | `root ./www/html;`         | Base filesystem path for request URI                    |
| `alias`          | `alias ./www/images;`      | Replace full route with this path (vs. append for root) |
| `index`          | `index index.html;`        | Default file served for directory requests              |
| `redirect`       | `redirect /new-page;`      | HTTP redirect target                                    |
| `autoindex`      | `autoindex on;`            | Enable directory listing                                |
| `enable_upload`  | `enable_upload on;`        | Allow POST file uploads to this location                |
| `cgi_path`       | `cgi_path ./cgi-bin;`      | Directory containing CGI scripts                        |
| `cgi_extension`  | `cgi_extension .sh;`       | File extension triggering CGI execution                 |

## Example Configuration

```nginx
worker_processes 1;
worker_connections 1024;

http {
    client_max_body_size 1M;
    default_type text/html;
    error_page ./www/html/error_page.html;
    autoindex_page ./www/html/autoindex.html;

    server {
        listen 127.0.0.1:8080;
        server_name localhost;
        keep_alive_timeout 65;
        error_log log/error.log error;

        location {
            route /;
            limit_except GET;
            root ./www/html;
            index index.html;
        }

        location {
            route /upload;
            limit_except GET POST DELETE;
            autoindex on;
            enable_upload on;
        }

        location {
            route /cgi;
            limit_except GET POST;
            cgi_path ./cgi-bin;
            cgi_extension .sh;
        }
    }
}
```

---

# Architecture

## Why Non-Blocking I/O?

Before deciding on the design, we analysed three common concurrency models and their trade-offs (see [`doc/non-blocking IO.md`](doc/non-blocking%20IO.md)):

| Model | Mechanism | Problem |
|-------|-----------|---------|
| **Fork per connection** | `fork()` after `accept()` | High cost: page-table copy, scheduler overhead, memory per process |
| **Thread pool** | Queue + mutex + thread pool | Lock contention on the shared queue; bounded by pool size (~10k connections impractical) |
| **I/O Multiplexing** | `select` / `poll` / `epoll` | Single process handles N sockets; cost paid only when data is ready |

Among the multiplexing syscalls, `epoll` was chosen over `select` and `poll`:

- **`select`** uses a fixed bitmap — hard limit of 1024 fds; O(n) kernel scan + O(n) userspace scan + two copies of `fd_set` per call.
- **`poll`** lifts the 1024 limit but retains the O(n) scan and double copy.
- **`epoll`** stores fds in a kernel-side Red-Black Tree (O(log n) registration); only *ready* fds are returned via a linked list — no scan, no copy.

This project uses `epoll` in **level-triggered (LT)** mode — the default and the only mode permitted by the 42 subject constraints.

That said, we researched **edge-triggered (ET)** mode and document the trade-off here, because understanding *why* ET is more performant motivates much of the overall non-blocking design:

| | Level-Triggered (LT) | Edge-Triggered (ET) |
|--|--|--|
| **When notified** | Every `epoll_wait` call, as long as data remains unread | Once, at the moment data *arrives* |
| **Missed reads** | Cannot miss data — kernel keeps re-notifying | Must drain socket fully on each wakeup (loop until `EAGAIN`), or data is silently lost |
| **Syscall overhead** | Higher — each buffered byte generates a wakeup | Lower — one wakeup per burst of data, regardless of size |
| **Typical use** | Easier to implement correctly | Nginx, high-performance servers |

With LT, if a handler reads only part of the available data, `epoll_wait` wakes up again on the next call — correct, but at the cost of redundant wakeups under high load. ET removes those redundant wakeups entirely: the kernel fires once per state change (new data arriving), so a single wakeup covers an entire burst. The handler *must* read until `EAGAIN` to avoid stalling the connection — a discipline this server's `ConnectionHandler` already enforces via its drain loop, meaning the code would require minimal changes to switch to ET if the constraint were lifted.

## Reactor Pattern — Nginx Architecture Reference

The Reactor pattern comes in four combinations, each with different scalability and complexity trade-offs (see [`doc/web-kernal-design.md`](doc/web-kernal-design.md)):

| Variant | How it works | Bottleneck / Trade-off | Real-world example |
|---------|-------------|------------------------|--------------------|
| **Single Reactor Single Worker** (SRSW) | One event loop handles accept + read/write + business logic in the same process | Handler stalls block the acceptor; single CPU only | Redis ≤ 6.0 |
| **Single Reactor Multiple Workers** (SRMW) | One event loop dispatches I/O events; worker threads handle business logic | The single reactor becomes the bottleneck under massive connection rates; shared queue needs locking | — |
| **Multiple Reactor Multiple Workers** (MRMW, processes) | Main reactor accepts and hands fds to sub-reactors; each sub-reactor owns its own `epoll` loop and worker | No shared state between workers → no locks; main reactor stays lightweight | **Nginx** |
| **Multiple Reactor Multiple Workers** (MRMW, threads) | Same topology but with threads instead of processes | Shared memory simplifies fd handoff; needs careful synchronisation | Netty, Memcached |

Nginx's MRMW (multi-process) design is the reference: the main process only calls `accept()` and distributes connections to worker processes via a shared listening socket. Each worker runs its own independent `epoll` loop with no shared mutable state, eliminating lock contention entirely.

This project implements the simpler **Single Reactor Single Worker** variant — the same three-role structural separation (Reactor / Acceptor / Handler) as nginx, but collapsed into a single process. This is sufficient for the project scope and keeps the implementation auditable without multi-process synchronisation complexity.

```
This server (SRSW):
  Reactor ──new connection──▶ Acceptor ──▶ creates ConnectionHandler
          ──read/write──────▶ ConnectionHandler ──▶ RequestProcessor ──▶ HTTP Handler
          ──CGI pipe ready──▶ CgiHandler
```

All three handler types implement the same `IHandler` interface, so the Reactor dispatches events without knowing what kind of fd it is serving.

```
┌─────────────────────────────────────────────┐
│                  Reactor                     │
│  while (!stop_flag) {                        │
│    epoll_wait(events, MAX_EVENTS, timeout)   │
│    for each event → fd_map[fd]->handle_event │
│  }                                           │
└────────────┬────────────────────────────────┘
             │ dispatches by fd
  ┌──────────┴──────────────────────────────┐
  │         IHandler  (interface)           │
  ├─────────────────────────────────────────┤
  │ Acceptor          — new TCP connections │
  │ ConnectionHandler — client read / write │
  │ CgiHandler        — CGI stdout pipe     │
  └─────────────────────────────────────────┘
```

**Key components:**

| Class | File | Responsibility |
|-------|------|----------------|
| `Reactor` | `kernel/Reactor.hpp` | `epoll` event loop, fd → `IHandler*` map, SIGINT shutdown |
| `Acceptor` | `kernel/Acceptor.hpp` | Accepts TCP connections, registers new `ConnectionHandler` with epoll |
| `ConnectionHandler` | `kernel/ConnectionHandler.hpp` | 4096-byte read buffer, write buffer flushing, keep-alive timer |
| `RequestProcessor` | `kernel/RequestProcessor.hpp` | Per-connection parse state, routes completed requests to HTTP handlers |
| `CgiHandler` | `kernel/CgiHandler.hpp` | Reads CGI process stdout from pipe, relays data to client write buffer |

## Connection Lifecycle

```
Client connects
  → Acceptor::handle_event()
      accept() → new fd
      Create ConnectionHandler, register EPOLLIN with epoll (LT mode)

  → EPOLLIN fires (LT: re-fires until buffer is empty)
      ConnectionHandler reads up to 4096 bytes per iteration until EAGAIN
      RequestProcessor::feed() → RequestAnalyzer state machine
      On COMPLETE → dispatch to GetHandler / PostHandler / DeleteHandler / CgiExecutor

  → Handler builds response into ConnectionHandler write buffer
      Register EPOLLOUT

  → EPOLLOUT fires
      ConnectionHandler flushes write buffer to socket until EAGAIN

  → Keep-alive: reset RequestAnalyzer state, re-arm EPOLLIN
  → Idle timeout: close fd, remove from epoll, destroy handler
```

## Request Processing State Machine

`RequestProcessor` tracks per-connection state using a bitmask so that multiple orthogonal conditions (e.g. chunked body + CGI in progress) can be represented simultaneously:

| State | Bitmask | Meaning |
|-------|---------|---------|
| `INITIAL` | 0 | Fresh request, nothing parsed yet |
| `WAITING_SESSION` | 1 | Waiting for virtual host resolution |
| `PROCESSING` | 2 | Handler is building the response |
| `WAITING_CGI` | 4 | Blocked on CGI child process output |
| `HANDLE_REQ_CHUNKED` | 8 | Decoding incoming chunked request body |
| `HANDLE_RES_CHUNKED` | 16 | Streaming large response file in chunks |
| `CONSUME_BODY` | 128 | Draining leftover request body before responding |
| `COMPLETED` | 64 | Response fully written; evaluate keep-alive |
| `ERROR` | 256 | Unrecoverable error; tear down connection |

---

# HTTP Request Parsing

Request parsing is handled by a chain of state machines that process the byte stream incrementally. This design handles partial TCP segments and pipelined requests naturally.

## Request Line Analyzer

`shell/RequestLineAnalyzer.hpp` parses `METHOD SP URI SP HTTP/1.1 CRLF`.

**States:** `METHOD` → `SPACE_BEFORE_URI` → `URI` → `SPACE_BEFORE_VERSION` → `VERSION` → `CRLF`

Supported methods: `GET`, `POST`, `DELETE`, `OPTIONS`, `CONNECT`.

## URI Analyzer

`shell/UriAnalyzer.hpp` implements RFC 3986 URI parsing.

**Supported URI forms:**

| Form | Example | Usage |
|------|---------|-------|
| origin-form | `/path?query` | Normal HTTP requests |
| absolute-form | `http://host/path` | Proxy requests |
| authority-form | `host:port` | CONNECT method |
| asterisk-form | `*` | OPTIONS |

**Validation:** IPv6 address syntax, percent-decoded characters, path normalization (resolves `..` and `.` segments to prevent directory traversal).

## Header Analyzer

`shell/HeaderAnalyzer.hpp` parses HTTP header fields per RFC 7230.

- Validates `field-name: field-value CRLF` format
- Handles obsolete line folding (multi-line headers)
- Extracts cookies from the `Cookie` header into a key-value map
- Signals completion on the empty `CRLF` line

## Chunked Transfer Decoding

`ChunkedCodec` (`kernel/ChunkedCodec.hpp`) implements RFC 7230 §4.1 chunked encoding.

```
State machine:
  CHUNKSIZE → (hex digits) → SIZE_CRLF → CHUNKBODY → BODY_CRLF → (loop / COMPLETE)
```

- Decodes request bodies for uploads
- Encodes large response files to avoid loading them fully into memory
- `ConnectionHandler` tracks per-fd file offsets in `_chunked_file_records` for streaming

---

# HTTP Handlers

All handlers extend `ARequestHandler` (`kernel/ARequestHandler.hpp`), which provides:
- `HttpException`-based error propagation with automatic error response generation
- `ResponseBuilder` factory for 2xx/3xx/4xx/5xx responses
- Template rendering via `TemplateEngine`

## GET — Static File Serving and Directory Listing

**File resolution pipeline:**

```
URI path
  → longest-prefix match on location routes
  → apply root / alias mapping
  → normalize path (resolve .., .)
  → check existence and read permission
  → detect MIME type from extension
  → serve file (chunked if large) or render directory listing
```

**Directory listing:** When `autoindex on` and the request targets a directory, `GetHandler` loads the template from `autoindex_page`, enumerates directory entries, and substitutes file links. When `index` is configured, it appends the index filename before attempting directory listing.

**MIME detection:** File extension → Content-Type map covers HTML, CSS, JavaScript, images (JPEG, PNG, GIF, SVG, ICO), fonts, JSON, XML, PDF, and binary fallback (`application/octet-stream`).

**Large file streaming:** Files above the chunked threshold are streamed using `Transfer-Encoding: chunked`. The `HANDLE_RES_CHUNKED` state persists a file offset across multiple EPOLLOUT events until the file is fully sent.

## POST — File Upload

**Upload flow:**

1. Validate `enable_upload on` for the matched location; otherwise 403.
2. Enforce `client_max_body_size`; exceed → 413.
3. Require `Content-Length` or `Transfer-Encoding: chunked`; missing → 411.
4. Stream body into a temp file in `/tmp/` (random name).
5. Determine final filename from `Content-Disposition` header or request URI.
6. Normalize final path to prevent directory traversal.
7. Rename temp file to upload directory → 201 Created (or 200 OK if overwrite).
8. Temp file is cleaned up on any error.

## DELETE — File Removal

- Resolves target path using the same root/alias mapping as GET.
- Only files inside upload-enabled directories are deletable (403 otherwise).
- Directory deletion is rejected (403).
- Returns 204 No Content on success.

---

# CGI Implementation

CGI scripts are triggered when the request URI matches a location with `cgi_path` and `cgi_extension` configured.

## Process Flow

```
Request matched as CGI
  → CgiExecutor::execute()
      1. Extract script name from URI
      2. Build CGI environment variables
      3. pipe(stdout_pipe)
      4. fork()
          Child:
            dup2(stdout_pipe[write], STDOUT_FILENO)
            Read POST body from temp file → pass as stdin via pipe
            execve(script_path, argv, envp)
          Parent:
            Close write end of pipe
            Register CgiHandler(stdout_pipe[read]) with epoll
      5. Fork monitor process → kills CGI after 3 s timeout
  → CgiHandler::handle_event() reads output, writes to client buffer
```

## Environment Variables

Standard CGI/1.1 environment passed to the script:

| Variable | Source |
|----------|--------|
| `REQUEST_METHOD` | Parsed request method |
| `SCRIPT_NAME` | URI path of the script |
| `PATH_INFO` | URI path after script name |
| `QUERY_STRING` | URI query component |
| `SERVER_NAME` | `server_name` directive |
| `SERVER_PORT` | Bound port |
| `CONTENT_TYPE` | `Content-Type` request header |
| `CONTENT_LENGTH` | `Content-Length` request header |
| `HTTP_*` | All other request headers (`-` → `_`, uppercased) |

## Timeout Enforcement

A second `fork()` creates a monitor process that `sleep(3)` then sends `SIGKILL` to the CGI PID. The monitor exits immediately if the CGI finishes first (detected via `waitpid` with `WNOHANG`). This prevents hung CGI scripts from blocking the event loop.

---

# Error Handling

`HttpException` (`utils/HttpException.hpp`) carries an HTTP status code and detail message. Handlers `throw HttpException(STATUS_CODE, "detail")` and `ARequestHandler::_handle_exception()` catches it, builds an error response, and renders the configured `error_page` template.

## Supported Status Codes

| Range | Codes |
|-------|-------|
| 2xx | 200 OK, 201 Created, 202 Accepted, 204 No Content |
| 3xx | 301 Moved Permanently, 302 Found, 303 See Other, 304 Not Modified |
| 4xx | 400 Bad Request, 401 Unauthorized, 403 Forbidden, 404 Not Found, 405 Method Not Allowed, 408 Request Timeout, 411 Length Required, 413 Payload Too Large, 417 Expectation Failed |
| 5xx | 500 Internal Server Error, 501 Not Implemented, 502 Bad Gateway, 503 Service Unavailable |

## Error Page Template

The error template at `www/html/error_page.html` uses two placeholders:

```html
<h1>{{status_code}}</h1>
<p>{{message}}</p>
```

`TemplateEngine` (`kernel/TemplateEngine.hpp`) performs simple string substitution before serializing the response.

---

# Build System

The project uses **CMake** wrapped by a convenience `Makefile`.

```sh
make          # configure + build → ./webserv
make clean    # remove build/ directory
make fclean   # clean + remove webserv binary, tmp/, www/upload/
make re       # fclean then rebuild
```

**Compiler requirements:** `c++` with `-std=c++98 -Wall -Wextra -Werror`

The `Makefile` fingerprints source files (`CMakeLists.txt`, `source/`, `include/`) against the binary timestamp — if nothing changed, the build is skipped without invoking CMake.

Runtime directories (`tmp/` and `www/upload/`) are created automatically on first `make`.

---

# Liked it?

Hope you liked this project, don't forget to give it a star ⭐.
<a href="https://github.com/LeaYeh/webserver">
    <img alt="chart" src="https://starchart.cc/LeaYeh/webserver.svg?variant=adaptive" width="600px">
</a>
