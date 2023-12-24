// HttpClient.cpp
#include "HttpClient.h"

#include <string.h>
#include <string>
using std::string;

#define SPACE_CHARS     " \t\r\n"
static string trim(const string& str) {
    string::size_type pos1 = str.find_first_not_of(SPACE_CHARS);
    if (pos1 == string::npos)   return "";

    string::size_type pos2 = str.find_last_not_of(SPACE_CHARS);
    return str.substr(pos1, pos2 - pos1 + 1);
}

static inline bool is_unambiguous(char c) {
    return (c >= '0' && c <= '9') ||
        (c >= 'A' && c <= 'Z') ||
        (c >= 'a' && c <= 'z') ||
        c == '-' ||
        c == '_' ||
        c == '.' ||
        c == '~';
}

static string escape(const string& istr) {
    string ostr;
    const char* p = istr.c_str();
    int len = istr.size();
    char szHex[4] = { 0 };
    for (int i = 0; i < len; ++i) {
        if (is_unambiguous(p[i])) {
            ostr += p[i];
        }
        else {
            sprintf_s(szHex, "%%%02X", p[i]);
            ostr += szHex;
        }
    }
    return ostr;
}

#define DEFAULT_TIMEOUT     10
HttpClient::HttpClient() {
    m_timeout = DEFAULT_TIMEOUT;
}

HttpClient::~HttpClient() {

}

int HttpClient::Send(const HttpRequest& req, HttpResponse* res) {
    return curl(req, res);
}

static size_t s_formget_cb(void* arg, const char* buf, size_t len) {
    return len;
}

static size_t s_header_cb(char* buf, size_t size, size_t cnt, void* userdata) {
    if (buf == NULL || userdata == NULL)    return 0;

    HttpResponse* res = (HttpResponse*)userdata;

    string str(buf);
    string::size_type pos = str.find_first_of(':');
    if (pos == string::npos) {
        if (res->version.empty() && strncmp(buf, "HTTP", 4) == 0) {
            // status line
            // HTTP/1.1 200 OK\r\n
            char* space1 = strchr(buf, ' ');
            char* space2 = strchr(space1 + 1, ' ');
            if (space1 && space2) {
                *space1 = '\0';
                *space2 = '\0';
                res->version = buf;
                res->status_code = atoi(space1 + 1);
                res->status_message = trim(space2 + 1);
            }
        }
    }
    else {
        // headers
        string key = trim(str.substr(0, pos));
        string value = trim(str.substr(pos + 1));
        res->headers[key] = value;
    }
    return size * cnt;
}

static size_t s_body_cb(char* buf, size_t size, size_t cnt, void* userdata) {
    if (buf == NULL || userdata == NULL)    return 0;

    HttpResponse* res = (HttpResponse*)userdata;
    res->body.append(buf, size * cnt);
    return size * cnt;
}

int HttpClient::curl(const HttpRequest& req, HttpResponse* res) {
    CURL* handle = curl_easy_init();

    // SSL
    curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);

    // method
    curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, req.method.c_str());

    // url
    curl_easy_setopt(handle, CURLOPT_URL, req.url.c_str());

    // header
    struct curl_slist* headers = NULL;
    if (m_headers.size() != 0) {
        for (auto& pair : m_headers) {
            string header = pair.first;
            header += ": ";
            header += pair.second;
            headers = curl_slist_append(headers, header.c_str());
        }
    }
    const char* psz = "text/plain";
    switch (req.content_type) {
#define CASE_CONTENT_TYPE(id, str)  \
    case id: psz = str;    break;

        FOREACH_CONTENT_TYPE(CASE_CONTENT_TYPE)
#undef  CASE_CONTENT_TYPE
    }
    string strContentType("Content-type: ");
    strContentType += psz;
    headers = curl_slist_append(headers, strContentType.c_str());
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
    //hlogd("%s %s", req.method.c_str(), req.url.c_str());
    //hlogd("%s", strContentType.c_str());

    // body or params
    struct curl_httppost* httppost = NULL;
    struct curl_httppost* lastpost = NULL;
    switch (req.content_type) {
    case FORM_DATA:
    {
        for (auto& pair : req.form) {
            CURLformoption opt = pair.second.type == FormData::FILENAME ? CURLFORM_FILE : CURLFORM_COPYCONTENTS;
            curl_formadd(&httppost, &lastpost,
                CURLFORM_COPYNAME, pair.first.c_str(),
                opt, pair.second.data.c_str(),
                CURLFORM_END);
        }
        if (httppost) {
            curl_easy_setopt(handle, CURLOPT_HTTPPOST, httppost);
            curl_formget(httppost, NULL, s_formget_cb);
        }
    }
    break;
    case QUERY_STRING:
    case X_WWW_FORM_URLENCODED:
    {
        string params;
        auto iter = req.kvs.begin();
        while (iter != req.kvs.end()) {
            if (iter != req.kvs.begin()) {
                params += '&';
            }
            params += escape(iter->first);
            params += '=';
            params += escape(iter->second);
            iter++;
        }
        if (req.content_type == QUERY_STRING) {
            string url_with_params(req.url);
            url_with_params += '?';
            url_with_params += params;
            curl_easy_setopt(handle, CURLOPT_URL, url_with_params.c_str());
            //hlogd("%s", url_with_params.c_str());
        }
        else {
            curl_easy_setopt(handle, CURLOPT_POSTFIELDS, params.c_str());
            //hlogd("%s", params.c_str());
        }
    }
    break;
    default:
    {
        if (req.text.size() != 0) {
            curl_easy_setopt(handle, CURLOPT_POSTFIELDS, req.text.c_str());
            //hlogd("%s", req.text.c_str());
        }
    }
    break;
    }

    if (m_timeout != 0) {
        curl_easy_setopt(handle, CURLOPT_TIMEOUT, m_timeout);
    }

    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, s_body_cb);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, res);

    curl_easy_setopt(handle, CURLOPT_HEADER, 0);
    curl_easy_setopt(handle, CURLOPT_HEADERFUNCTION, s_header_cb);
    curl_easy_setopt(handle, CURLOPT_HEADERDATA, res);

    int ret = curl_easy_perform(handle);
    if (ret != 0) {
        //hloge("%d: %s", ret, curl_easy_strerror((CURLcode)ret));
    }

    //if (res->body.length() != 0) {
        //hlogd("Response:%s", res->body.c_str());
    //}
    //double total_time, name_time, conn_time, pre_time;
    //curl_easy_getinfo(handle, CURLINFO_TOTAL_TIME, &total_time);
    //curl_easy_getinfo(handle, CURLINFO_NAMELOOKUP_TIME, &name_time);
    //curl_easy_getinfo(handle, CURLINFO_CONNECT_TIME, &conn_time);
    //curl_easy_getinfo(handle, CURLINFO_PRETRANSFER_TIME, &pre_time);
    //hlogd("TIME_INFO: %lf,%lf,%lf,%lf", total_time, name_time, conn_time, pre_time);

    if (headers) {
        curl_slist_free_all(headers);
    }
    if (httppost) {
        curl_formfree(httppost);
    }

    curl_easy_cleanup(handle);

    return ret;
}

const char* HttpClient::strerror(int errcode) {
    return curl_easy_strerror((CURLcode)errcode);
}

//下载文件数据接收函数
size_t dl_req_reply(void* buffer, size_t size, size_t nmemb, void* user_p)
{
    FILE* fp = (FILE*)user_p;
    size_t return_size = fwrite(buffer, size, nmemb, fp);
    //cout << (char *)buffer << endl;
    return return_size;
}

//http GET请求文件下载  
CURLcode HttpClient::dl_curl_get_req(const std::string& url, std::string filename)
{
    const char* file_name = filename.c_str();
    char* pc = new char[1024];//足够长
    strcpy_s(pc, strlen(file_name) + 1, file_name);

    FILE* fp;
    fopen_s(&fp, pc, "wb");

    //curl初始化  
    CURL* curl = curl_easy_init();
    // curl返回值 
    CURLcode res;
    if (curl)
    {
        //设置curl的请求头
        struct curl_slist* header_list = NULL;
        header_list = curl_slist_append(header_list, "User-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64; Trident/7.0; rv:11.0) like Gecko");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);

        //不接收响应头数据0代表不接收 1代表接收
        curl_easy_setopt(curl, CURLOPT_HEADER, 0);

        //设置请求的URL地址 
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        //设置ssl验证
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);

        //CURLOPT_VERBOSE的值为1时，会显示详细的调试信息
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);

        curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);

        //设置数据接收函数
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &dl_req_reply);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);

        //设置超时时间
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 6); // set transport and time out time  
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 6);

        // 开启请求  
        res = curl_easy_perform(curl);
    }
    // 释放curl 
    curl_easy_cleanup(curl);
    //释放文件资源
    fclose(fp);

    return res;
}