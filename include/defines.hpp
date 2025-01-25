#pragma once
#include <string>

#define PROGRAM_NAME "webserv"
#define SUCCESS      0
#define FAILURE      1

#define STY_RES        "\e[0m"
#define STY_BOL        "\e[1m"
#define STY_ITA        "\e[3m"
#define STY_UND        "\e[4m"
#define STY_RED        "\e[31m"
#define STY_GRE        "\e[32m"
#define STY_YEL        "\e[33m"
#define STY_BLU        "\e[34m"
#define STY_MAG        "\e[35m"
#define STY_CYA        "\e[36m"
#define STY_WHI        "\e[37m"
#define STY_GRA        "\e[90m"
#define STY_WHI_BRI    "\e[97m"
#define STY_BLA_BG     "\e[41m"
#define STY_RED_BG     "\e[41m"
#define STY_GRE_BG     "\e[42m"
#define STY_YEL_BG     "\e[43m"
#define STY_BLU_BG     "\e[44m"
#define STY_MAG_BG     "\e[45m"
#define STY_CYA_BG     "\e[46m"
#define STY_WHI_BG     "\e[47m"
#define STY_GRA_BG     "\e[100m"
#define STY_WHI_BRI_BG "\e[107m"

#define MAX_PATH_LENGTH 128

#define OK_MSG                    "OK"
#define CREATED_MSG               "Created"
#define ACCEPTED_MSG              "Accepted"
#define NO_CONTENT_MSG            "No Content"
#define MOVED_PERMANENTLY_MSG     "Moved Permanently"
#define FOUND_MSG                 "Found"
#define SEE_OTHER_MSG             "See Other"
#define NOT_MODIFIED_MSG          "Not Modified"
#define BAD_REQUEST_MSG           "Bad Request"
#define UNAUTHORIZED_MSG          "Unauthorized"
#define FORBIDDEN_MSG             "Forbidden"
#define NOT_FOUND_MSG             "Not Found"
#define METHOD_NOT_ALLOWED_MSG    "Method Not Allowed"
#define REQUEST_TIMEOUT_MSG       "Request Timeout"
#define NO_CONTENT_LENGTH_MSG     "No Content Length"
#define INTERNAL_SERVER_ERROR_MSG "Internal Server Errror"
#define NOT_IMPLEMENTED_MSG       "Not Implemented"
#define BAD_GATEWAY_MSG           "Bad Gateway"
#define SERVICE_UNAVAILABLE_MSG   "Service Unavaliable"

namespace weblog
{

enum LogLevel
{
    DEBUG = 0,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

} // namespace weblog

namespace webshell
{

enum RequestAnalyzerState
{
    PARSING_REQUEST_LINE = 0,
    PARSING_REQUEST_HEADERS,
    PARSING_REQUEST_BODY,
    COMPLETE,
    ERROR
};

enum RequestMethod
{
    UNKNOWN = -1,
    GET = 0,
    POST,
    DELETE,
    CONNECT,
    OPTIONS
};

enum RequestLineState
{
    RQLINE_ERROR = -1,
    PRE_CR = 0,
    PRE_LF,
    METHOD,
    URI,
    VERSION,
    END_RQLINE
};

enum URIState
{
    URI_ERROR = -1,
    URI_START = 0,
    URI_LIMBO,
    URI_SCHEME,
    URI_HOST_TRIAL,
    URI_HOST_IPV4,
    URI_HOST_REGNAME,
    URI_PORT,
    URI_PATH_TRIAL,
    URI_PATH,
    URI_QUERY,
    URI_FRAGMENT,
    END_URI_PARSER
};

enum URIType
{
    UNKNOWN_TYPE = -1,
    ORIGIN = 0,
    ABSOLUTE,
    AUTHORITY,
    ASTERISK
};

enum ChunkState
{
    CHUNKSIZE = 0,
    SIZE_CRLF,
    // SIZE_LF,
    CHUNKBODY,
    BODY_CRLF,
    // BODY_LF
};

enum CacheState
{
    C_ERROR = -1,
    C_DIRECTIVE_START = 0,
    C_DIRECTIVE,
    C_ARGUMENT_START,
    C_ARGUMENT,
    C_QUOTED, // RFC 7234 5.2.1: "A sender SHOULD NOT generate the quoted string
              // form"
    C_INQUOTE_ESCAPE
};

enum CookieState
{
    CO_ERROR = -1,
    CO_OWS = 0,
    CO_NAME,
    CO_VALUE,
    CO_SP
};

enum RequestHeaderState
{
    INVALID_HEADER = -1,
    START_HEADER = 0,
    FIELD_NAME,
    LEADING_WS,
    FIELD_VALUE,
    MIDDLE_OR_END_WS,
    FIELD_END_CRLF,
    CHECK_OBS_FOLD,
    HEADER_END_CRLF,
    END_HEADERS
};

enum StatusCode
{
    UNDEFINED = -1,
    CONTINUE = 100,
    OK = 200,
    CREATED = 201,
    ACCEPTED = 202,
    NO_CONTENT = 204,
    MOVED_PERMANENTLY = 301,
    FOUND = 302,
    SEE_OTHER = 303,
    NOT_MODIFIED = 304,
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    METHOD_NOT_ALLOWED = 405,
    REQUEST_TIMEOUT = 408,
    NO_CONTENT_LENGTH = 411,
    PAYLOAD_TOO_LARGE = 413,
    EXPECTATION_FAILED = 417,
    INTERNAL_SERVER_ERROR = 500,
    NOT_IMPLEMENTED = 501,
    BAD_GATEWAY = 502,
    SERVICE_UNAVAILABLE = 503
};

enum ContentType
{
    TEXT_HTML = 0,
    TEXT_CSS,
    TEXT_PLAIN,
    IMAGE_JPEG,
    IMAGE_PNG,
    IMAGE_GIF,
    APPLICATION_OCTET_STREAM,
    APPLICATION_JSON
};

enum ConnectionType
{
    KEEP_ALIVE = 0,
    CLOSE
};

enum HttpVersion // we only support HTTP/1.1
{
    HTTP_1_0,
    HTTP_1_1,
    HTTP_2_0,
    HTTP_3_0
};

} // namespace webshell

namespace webconfig
{
enum ConfigBlockLevel
{
    GLOBAL = 0,
    HTTP,
    SERVER,
    LOCATION
};

} // namespace webconfig

namespace webkernel
{

// TODO: the MAX_EVENTS is critical for the performance of the server, need to
// be optimized
const int MAX_EVENTS = 10;

// TODO: the CHUNKED_SIZE is critical for the performance of the server, need to
// be optimized (it is the size of the buffer for the body)
const int CHUNKED_SIZE = 1024;
// const char* CHUNKED_END_MARKER = "0\r\n\r\n";
const std::string CHUNKED_END_MARKER = "0\r\n\r\n";

enum ReactorType
{
    UNDEFINED = -1,
    REACTOR = 0,
    DISPATCHER,
    WORKER
};

enum EventType
{
    NEW_CONNECTION = 0,
    COMMUNICATION,
    DATA_RECEIVED,
    DATA_SENT
};

enum EncodingMode
{
    IDENTITY = 0,
    CHUNKED = 0b00000001,
    GZIP = 0b00000010,
};

enum EventProcessingState
{
    INITIAL = 0b00000000,
    PROCESSING = 0b00000001,
    HANDLE_OTHERS_CHUNKED = 0b00000010,
    HANDLE_FIRST_CHUNKED = 0b00000100,
    HANDLE_CHUNKED = 0b00001000,
    COMPELETED = 0b00010000,
    ERROR = 0b01000000,
    UNKNOWN = 0b10000000
};

} // namespace webkernel
