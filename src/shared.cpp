#define _CRT_SECURE_NO_WARNINGS

#include "shared.h"
#include <stdio.h>
#include "imgui/imgui.h"

void impl::showExampleWindow(const char* comment)
{
	char buffer[128];
	::memset(buffer, 0, 128);
	::sprintf(buffer, "LuaEngine (%s)", comment);

	ImGui::Begin(buffer);

	ImGui::Text(u8"欢迎使用LuaEngine，当前为内部测试版本，如果发现Bug请及时反馈。");

	ImGui::End();
}