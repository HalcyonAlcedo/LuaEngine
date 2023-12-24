#pragma once
#include "HttpClient.h"
#include "easywsclient.hpp"
#include <assert.h>
#include "md5.h"

namespace game_network {
	static easywsclient::WebSocket::pointer ws = NULL;
	queue<string> MessageCache;

	void handle_message(const string& message)
	{
		MessageCache.push(message);
	}

	static void LinkWS(string link) {
		ws = easywsclient::WebSocket::from_url(link);
		assert(ws);
	}

	static void WSSendMessage(string Message) {
		ws->send(Message);
	}

	static void WSHandle() {
		if (ws != NULL and ws->getReadyState() != easywsclient::WebSocket::CLOSED) {
			ws->poll();
			ws->dispatch(handle_message);
		}
	}

	static bool WSState() {
		if (ws != NULL) {
			if (
				ws->getReadyState() == easywsclient::WebSocket::CONNECTING or
				ws->getReadyState() == easywsclient::WebSocket::OPEN
				)
				return true;
		}
		return  false;
	}

	static string GetHttpData(string url) {
		HttpClient session;
		HttpRequest req;
		req.method = "GET";
		req.url = url;
		HttpResponse res;
		int ret = session.Send(req, &res);
		string retString = "";
		if (ret != 0) {
			retString = HttpClient::strerror(ret);
		}
		else {
			retString = res.body;
		}
		return retString;
	}

	static bool DownloadFile(string url, string file) {
		HttpClient session;
		return session.dl_curl_get_req(url, file);
	}

	static string getFileMD5(string file)
	{
		ifstream in(file.c_str(), ios::binary);
		if (!in)
			return "";

		MD5 md5;
		std::streamsize length;
		char buffer[1024];
		while (!in.eof()) {
			in.read(buffer, 1024);
			length = in.gcount();
			if (length > 0)
				md5.update(buffer, length);
		}
		in.close();
		return md5.toString();
	}

	static void Registe(lua_State* L) {
		engine_logger->info("注册网络相关函数");
		//获取网络数据
		lua_register(L, "GetHttpData", [](lua_State* pL) -> int
			{
				string httpUrl = (string)lua_tostring(pL, -1);
				lua_pushstring(pL, GetHttpData(httpUrl).c_str());
				return 1;
			});
		//下载文件
		lua_register(L, "DownloadFile", [](lua_State* pL) -> int
			{
				string url = (string)lua_tostring(pL, 1);
				string file = (string)lua_tostring(pL, 2);
				lua_pushboolean(pL, DownloadFile(url, file));
				return 1;
			});
		//获取文件Md5
		lua_register(L, "GetFileMD5", [](lua_State* pL) -> int
			{
				string file = (string)lua_tostring(pL, 1);
				lua_pushstring(pL, getFileMD5(file).c_str());
				return 1;
			});
		//连接到websocket服务器
		lua_register(L, "LinkWSServer", [](lua_State* pL) -> int
			{
				string WSSLink = (string)lua_tostring(pL, -1);
				LinkWS(WSSLink);
				return 0;
			});
		//发送信息至WS服务器
		lua_register(L, "SendWSMessage", [](lua_State* pL) -> int
			{
				string Message = (string)lua_tostring(pL, -1);
				WSSendMessage(Message);
				return 0;
			});
		//获取服务器消息堆栈中的一条数据并弹出该数据
		lua_register(L, "GetWSMessage", [](lua_State* pL) -> int
			{
				if (!MessageCache.empty()) {
					lua_pushstring(pL, MessageCache.front().c_str());
					MessageCache.pop();
				}
				else
					lua_pushstring(pL, "");
				return 1;
			});
		//获取服务器连接状态
		lua_register(L, "GetWSLinkState", [](lua_State* pL) -> int
			{
				lua_pushboolean(pL, WSState());
				return 1;
			});
	}
}