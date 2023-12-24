// HttpClient.h
#ifndef HTTP_CLIENT_H_
#define HTTP_CLIENT_H_
#define CURL_STATICLIB

/***************************************************************
HttpClient based libcurl
***************************************************************/

#include <curl/curl.h>
#pragma comment (lib, "Crypt32.lib")
#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "wldap32.lib")
#pragma comment (lib, "libcurl_a.lib")

#include "HttpRequest.h"

class HttpClient {
public:
    HttpClient();
    ~HttpClient();

    int Send(const HttpRequest& req, HttpResponse* res);
    static const char* strerror(int errcode);

    void SetTimeout(int sec) { m_timeout = sec; }
    void AddHeader(string key, string value) {
        m_headers[key] = value;
    }
    void DelHeader(string key) {
        auto iter = m_headers.find(key);
        if (iter != m_headers.end()) {
            m_headers.erase(iter);
        }
    }
    void ClearHeader() {
        m_headers.clear();
    }

    CURLcode dl_curl_get_req(const std::string& url, std::string filename);

protected:
    int curl(const HttpRequest& req, HttpResponse* res);

private:
    int m_timeout; // unit:s default:10s
    KeyValue m_headers;
};

#endif  // HTTP_CLIENT_H_

