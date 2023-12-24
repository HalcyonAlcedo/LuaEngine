// HttpRequest.h
#ifndef HTTP_REQUEST_H_
#define HTTP_REQUEST_H_

#include <string>
#include <vector>
#include <map>

using std::string;
using std::vector;
using std::map;

typedef std::map<std::string, std::string> KeyValue;

// F(id, str)
#define FOREACH_CONTENT_TYPE(F) \
    F(TEXT_PLAIN,               "text/plain")   \
    F(TEXT_HTML,                "text/html")    \
    F(TEXT_XML,                 "text/xml")     \
    F(APPLICATION_JSON,         "application/json") \
    F(APPLICATION_XML,          "application/xml")  \
    F(APPLICATION_JAVASCRIPT,   "application/javascript")   \
    \
    F(FORM_DATA,                "multipart/form-data")  \
    \
    F(X_WWW_FORM_URLENCODED,    "application/x-www-form-urlencoded")    \
    F(QUERY_STRING,             "text/plain")

#define ENUM_CONTENT_TYPE(id, _)    id,
enum ContentType {
    FOREACH_CONTENT_TYPE(ENUM_CONTENT_TYPE)
};

struct FormData {
    enum FormDataType {
        CONTENT,
        FILENAME
    } type;
    string       data;
    FormData() {
        type = CONTENT;
    }
    FormData(const char* data, FormDataType type = CONTENT) {
        this->type = type;
        this->data = data;
    }
    FormData(const string& str, FormDataType type = CONTENT) {
        this->type = type;
        this->data = str;
    }
    FormData(int n) {
        this->type = CONTENT;
        this->data = std::to_string(n);
    }
    FormData(long long n) {
        this->type = CONTENT;
        this->data = std::to_string(n);
    }
    FormData(float f) {
        this->type = CONTENT;
        this->data = std::to_string(f);
    }
    FormData(double lf) {
        this->type = CONTENT;
        this->data = std::to_string(lf);
    }
};

typedef std::multimap<std::string, FormData>     Form;

struct HttpRequest {
    // request line
    string              method;
    string              url;
    string              version;

    // headers
    KeyValue            headers;

    // body
    ContentType     content_type;
    string          text;
    KeyValue        kvs; // QUERY_STRING,X_WWW_FORM_URLENCODED
    Form            form; // FORM_DATA
};

struct HttpResponse {
    // status line
    string version;
    int    status_code;
    string status_message;

    // headers
    KeyValue headers;

    // body
    string body;
};

#endif // HTTP_REQUEST_H_
