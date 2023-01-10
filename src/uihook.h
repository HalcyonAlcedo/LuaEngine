#include "kiero.h"

#if KIERO_INCLUDE_D3D11

#include <d3d11.h>
#include <assert.h>

#include "win32_impl.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

typedef HRESULT(__stdcall* Present) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);
typedef uintptr_t PTR;
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

Present oPresent;
Present oResize;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;

//纹理
struct Texture {
	int width = 0;
	int height = 0;
	ID3D11ShaderResourceView* texture = NULL;
	Texture(
		int width = 0,
		int height = 0,
		ID3D11ShaderResourceView* texture = NULL
	) :width(width), height(height), texture(texture) { };
};
map<string, Texture> TextureList;

static bool ViewInit = false;
static bool GameInit = false;

void InitImGui()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	auto fonts = ImGui::GetIO().Fonts;
	fonts->AddFontFromFileTTF(
		"c:/windows/fonts/simhei.ttf",
		13.0f,
		NULL,
		fonts->GetGlyphRangesChineseFull()
	);
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);
}

HRESULT __stdcall hkResize(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	mainRenderTargetView->Release();
	mainRenderTargetView = nullptr;
	/*
	ID3D11Texture2D* pBuffer;
	pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBuffer);
	// Perform error handling here!

	pDevice->CreateRenderTargetView(pBuffer, NULL, &mainRenderTargetView);
	// Perform error handling here!
	pBuffer->Release();

	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	*/
	LOG(ERR) << utils::UTF8_To_string("d3d11发生异常，已禁用图形绘制系统，此问题已知不用联系作者，重启游戏即可修复！");
	return oResize(pSwapChain, SyncInterval, Flags);
}
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (!ViewInit)
	{
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
		{
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc(&sd);
			window = sd.OutputWindow;
			ID3D11Texture2D* pBackBuffer;
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
			pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
			pBackBuffer->Release();
			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
			InitImGui();
			ViewInit = true;
		}
		else
			return oPresent(pSwapChain, SyncInterval, Flags);
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	const ImGuiIO& io = ImGui::GetIO();
	if (!io.WantCaptureMouse) {
		ImGui::SetMouseCursor(ImGuiMouseCursor_None);
	}
	else
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
	}
	//impl::showExampleWindow("D3D11");
	if (LuaCore::luaframe) {
		LuaCore::luaframe = false;
		LuaCore::run("on_imgui");
	}

	ImGui::EndFrame();
	ImGui::Render();
	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return oPresent(pSwapChain, SyncInterval, Flags);
}

DWORD WINAPI MainThread(LPVOID lpReserved)
{
	bool init_hook = false;
	do
	{
		if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
		{
			kiero::bind(8, (void**)&oPresent, hkPresent);
			kiero::bind(13, (void**)&oResize, hkResize);
			init_hook = true;
		}
	} while (!init_hook);
	return TRUE;
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
{
	// Load from disk into a raw RGBA buffer
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
	if (image_data == NULL)
		return false;

	// Create texture
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = image_width;
	desc.Height = image_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	ID3D11Texture2D* pTexture = NULL;
	D3D11_SUBRESOURCE_DATA subResource;
	subResource.pSysMem = image_data;
	subResource.SysMemPitch = desc.Width * 4;
	subResource.SysMemSlicePitch = 0;
	pDevice->CreateTexture2D(&desc, &subResource, &pTexture);

	// Create texture view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	pDevice->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
	pTexture->Release();

	*out_width = image_width;
	*out_height = image_height;
	stbi_image_free(image_data);

	return true;
}

#endif // KIERO_INCLUDE_D3D11
namespace uihook {
	bool thread = false;
	HMODULE hMod;
	void init() {
		if (!thread) {
			thread = true;
			CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);
		}
		GameInit = true;
	}
	static void LuaRegister(lua_State* L) {
		lua_register(L, "LoadTexture", [](lua_State* pL) -> int
		{
			string file = (string)lua_tostring(pL, 1);
			string name = (string)lua_tostring(pL, 2);
			void* ret = nullptr;
			TextureList[name] = Texture(0,0, NULL);
			bool texture = LoadTextureFromFile(file.c_str(), &TextureList[name].texture, &TextureList[name].width, &TextureList[name].height);
			IM_ASSERT(texture);
			ret = (void*)TextureList[name].texture;
			lua_pushinteger(pL, (long long)ret);
			lua_pushinteger(pL, TextureList[name].width);
			lua_pushinteger(pL, TextureList[name].height);
			return 3;
		});
		lua_register(L, "TextureList", [](lua_State* pL) -> int
		{
			lua_newtable(pL);//创建一个表格，放在栈顶
			for (auto [name, texture] : TextureList) {
				lua_pushstring(pL, name.c_str());
				lua_newtable(pL);//压入编号信息表
				lua_pushstring(pL, "texture");
				lua_pushinteger(pL, (long long)texture.texture);
				lua_settable(pL, -3);
				lua_pushstring(pL, "width");
				lua_pushinteger(pL, texture.width);
				lua_settable(pL, -3);
				lua_pushstring(pL, "height");
				lua_pushinteger(pL, texture.height);
				lua_settable(pL, -3);
				lua_settable(pL, -3);//弹出到顶层
			}
			return 1;
		});
	}
}