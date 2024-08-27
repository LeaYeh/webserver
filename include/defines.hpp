#pragma once

#define PROGRAM_NAME "webserv"
#define SUCCESS 0
#define FAILURE 1

#define STY_RES "\e[0m"
#define STY_BOL "\e[1m"
#define STY_ITA "\e[3m"
#define STY_UND "\e[4m"
#define STY_RED "\e[31m"
#define STY_GRE "\e[32m"
#define STY_YEL "\e[33m"
#define STY_BLU "\e[34m"
#define STY_MAG "\e[35m"
#define STY_CYA "\e[36m"
#define STY_WHI "\e[37m"
#define STY_GRA "\e[90m"
#define STY_WHI_BRI "\e[97m"
#define STY_BLA_BG "\e[41m"
#define STY_RED_BG "\e[41m"
#define STY_GRE_BG "\e[42m"
#define STY_YEL_BG "\e[43m"
#define STY_BLU_BG "\e[44m"
#define STY_MAG_BG "\e[45m"
#define STY_CYA_BG "\e[46m"
#define STY_WHI_BG "\e[47m"
#define STY_GRA_BG "\e[100m"
#define STY_WHI_BRI_BG "\e[107m"

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

}

namespace webshell
{

enum RequestMethod
{
    GET = 0,
    HEAD,
    POST,
    PUT,
    DELETE
};

enum RequestLineState
{
    START = 0,
    METHOD,
    SPACE_BEFORE_URI,
    URI,
    URI_SCHEME,
    URI_DIRECTORY_SLASH,
    URI_CLOSE_SSLASH,
    URI_QUERY,
    SPACE_BEFORE_VERSION,
    VERSION_HTTP,
    VERSION_SLASH,
    VERSION_DIGIT,
    END_LINE_1,
    HEADER_FIELD,
    END_LINE_2,
    ERROR_STATE,
    END_REQUEST_PARSER
};

enum RequestHeaderState
{
    START_HEADER,
    HOST,
    HOST_COLON,
    HOST_NAME,
    NEW_LINE_BEFORE_ACCEPT,
    ACCEPT,
    ACCEPT_COLON,
    ACCEPT_TYPE,
    NEW_LINE_BEFORE_ACCEPT_ENCODING,
    ACCEPT_ENCODING,
    ACCEPT_ENCODING_COLON,
    ACCEPT_ENCODING_TYPE,
    NEW_LINE_BEFORE_CONNECTION,
    CONNECTION,
    CONNECTION_COLON,
    CONNECTION_TYPE,
    NEW_LINE_BEFORE_CT,
    CONTENT_TYPE,
    CONTENT_TYPE_COLON,
    CONTENT_TYPE_NAME,
    NEW_LINE_BEFORE_CL,
    CONTENT_LENGTH,
    CONTENT_LENGTH_COLON,
    CONTENT_LENGTH_NBR,
    INVALID_HEADER,
    NEW_LINE_BEFORE_THE_REST,
    THE_REST,
    END_HEADERS
};

enum StatusCode
{
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

enum Encoding
{
    GZIP,
    DEFLATE,
    BR,
    IDENTITY,
    CHUNKED,
    COMPRESS
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

} // namespace webkernel
