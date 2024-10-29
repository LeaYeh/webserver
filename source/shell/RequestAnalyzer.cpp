#include "RequestAnalyzer.hpp"
#include "defines.hpp"
#include "ParseException.hpp"

namespace webshell
{

RequestAnalyzer::RequestAnalyzer()
    : _state(PARSING_REQUEST_LINE), _rl_analyzer(), /*_uri_analyzer(),*/
      _header_analyzer(), _status_info(OK, "")
{
}

RequestAnalyzer::RequestAnalyzer(const RequestAnalyzer& other)
    : _state(other._state),
      _rl_analyzer(other._rl_analyzer),
      /*_uri_analyzer(other._uri_analyzer),*/
      _header_analyzer(other._header_analyzer), _status_info(other._status_info)
{
}

RequestAnalyzer::~RequestAnalyzer()
{
}

RequestAnalyzer& RequestAnalyzer::operator=(const RequestAnalyzer& other)
{
    if (this != &other)
    {
        _state = other._state;
        _rl_analyzer = other._rl_analyzer;
        // _uri_analyzer = other._uri_analyzer;
        _header_analyzer = other._header_analyzer;
        _status_info = other._status_info;
    }
    return (*this);
}

void RequestAnalyzer::feed(const char ch)
{
    try
    {
        // throw ParseException(NOT_FOUND, NOT_FOUND_MSG);
        switch (_state)
        {
            case PARSING_REQUEST_LINE:
                if (_rl_analyzer.done())
                {
                    _state = PARSING_REQUEST_HEADERS;
                    _method = _rl_analyzer.method();
                    _target = _rl_analyzer.target();
                    _version = _rl_analyzer.version();
                }
                else
                {
                    _rl_analyzer.feed(ch);
                    break;
                }
            case PARSING_REQUEST_HEADERS:
                if (_header_analyzer.done())
                {
                    // _state = PARSING_REQUEST_BODY;
                    //TODO: extract info from header here
                    _state = COMPLETE;
                }
                else
                {
                    _header_analyzer.feed(ch);
                    break;
                }
            default:
                break;
            //TODO: i cant do this lol what is the point of chunked then
            // case PARSING_REQUEST_BODY:
            //     if (_body_analyzer.done())
            //     {
            //         //extract info from header here
            //         _state = COMPLETE;
            //     }
            //     else
            //     {
            //         _body_analyzer.feed(ch);
            //         break;
            //     }
        }
    }
    catch (ParseException& e)
    {
        _status_info.first = e.code();
        _status_info.second = e.msg();
        _state = ERROR;
        e.displayIssue(); //for debug purposes
    }
}

bool RequestAnalyzer::isComplete(void) const
{
    // TODO: check if received /r/n/r/n or 0 from chunked data or reached the Content-Length or ...
    //MKH - i dont really think we need that
    return (_state == COMPLETE);
}

void RequestAnalyzer::reset(void)
{
    _state = PARSING_REQUEST_LINE;
}

RequestAnalyzerState RequestAnalyzer::state(void) const
{
    return (_state);
}

Request RequestAnalyzer::request(void) const
{
    Request req;
    req.setMethod(_method);
    req.setTarget(_target);
    req.setVersion(_version);
    // req.setHeaders(_headers);
    return (req);
}

std::pair<StatusCode, std::string> RequestAnalyzer::statusInfo()
{
    return (_status_info);
}

bool RequestAnalyzer::hasError()
{
    return (_state == ERROR);
    // return (_status_info.first != OK);
}

} // namespace webshell
