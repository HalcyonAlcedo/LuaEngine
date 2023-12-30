#include "kiero.h"
#include <iostream>
#include <future>


#include <d3d11.h>
#include <assert.h>
#include <d3d12.h>
#include <dxgi1_4.h>

#include "win32_impl.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include <imgui/imgui_impl_dx12.h>
#include <chrono>
#include <atlbase.h>
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")

#define USE_MINHOOK

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

typedef HRESULT(__stdcall* Present) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);
typedef uintptr_t PTR;
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

Present oPresent;
using fResizeBuffers = HRESULT(__fastcall*)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);
fResizeBuffers oResize = NULL;
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
	LuaCore::luaframe = false;
	imgui_logger->info("初始化ImGui设备");
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	auto fonts = ImGui::GetIO().Fonts;
	imgui_logger->info("从{}加载字体数据", "c:/windows/fonts/simhei.ttf");
	fonts->AddFontFromFileTTF(
		"c:/windows/fonts/simhei.ttf",
		13.0f,
		NULL,
		fonts->GetGlyphRangesChineseFull()
	);
	imgui_logger->info("初始化ImGui渲染");
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);
}
HRESULT __stdcall hkResize(IDXGISwapChain* pChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT Flags)
{
	imgui_logger->info("重置d3d11数据");
	ImGui_ImplDX11_InvalidateDeviceObjects();
	if (nullptr != mainRenderTargetView) {
		mainRenderTargetView->Release();
		mainRenderTargetView = nullptr;
	}
	HRESULT toReturn = oResize(pChain, BufferCount, Width, Height, NewFormat, Flags);
	ImGui_ImplDX11_CreateDeviceObjects();
	return toReturn;
}
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (!ViewInit)
	{
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
		{
			imgui_logger->info("初始化d3d11视图");
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

	//recreate rendertarget on reset
	if (mainRenderTargetView == NULL)
	{
		imgui_logger->warn("d3d11视图已移除，正在重新加载");
		ID3D11Texture2D* pBackBuffer = NULL;
		pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
		pBackBuffer->Release();
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
		LuaCore::run("on_imgui");
	}
	//ImGui::EndFrame();
	ImGui::Render();
	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return oPresent(pSwapChain, SyncInterval, Flags);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
{
	framework_logger->info("加载图片{}到纹理", filename);
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

template<typename T>
static void SafeRelease(T*& res) {
	if (res)
		res->Release();
	res = NULL;
}

//https://github.com/ocornut/imgui/blob/master/examples/example_win32_directx12/main.cpp
struct FrameContext {
	CComPtr<ID3D12CommandAllocator> command_allocator = NULL;
	CComPtr<ID3D12Resource> main_render_target_resource = NULL;
	D3D12_CPU_DESCRIPTOR_HANDLE main_render_target_descriptor;
};

// Data
static std::vector<FrameContext> g_FrameContext;
static UINT						g_FrameBufferCount = 0;

static CComPtr<ID3D12DescriptorHeap> g_pD3DRtvDescHeap = NULL;
static CComPtr<ID3D12DescriptorHeap> g_pD3DSrvDescHeap = NULL;
static CComPtr<ID3D12CommandQueue> g_pD3DCommandQueue = NULL;
static CComPtr<ID3D12GraphicsCommandList> g_pD3DCommandList = NULL;

LRESULT APIENTRY WndProc12(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

typedef long(__fastcall* Present12) (IDXGISwapChain3* pSwapChain, UINT SyncInterval, UINT Flags);
static Present12 OriginalPresent;

typedef void(*ExecuteCommandLists)(ID3D12CommandQueue* queue, UINT NumCommandLists, ID3D12CommandList* ppCommandLists);
static ExecuteCommandLists OriginalExecuteCommandLists;

typedef long(__fastcall* ResizeBuffers)(IDXGISwapChain3* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
static ResizeBuffers OriginalResizeBuffers;

static WNDPROC OriginalWndProc;
static HWND Window = nullptr;

static uint64_t* g_MethodsTable = NULL;
static bool g_Initialized = false;

enum class Status {
	UnknownError = -1,
	NotSupportedError = -2,
	ModuleNotFoundError = -3,

	AlreadyInitializedError = -4,
	NotInitializedError = -5,

	Success = 0,
};

long __fastcall HookPresent(IDXGISwapChain3* pSwapChain, UINT SyncInterval, UINT Flags) {
	if (g_pD3DCommandQueue == nullptr) {
		return OriginalPresent(pSwapChain, SyncInterval, Flags);
	}
	if (!g_Initialized) {
		framework_logger->info("初始化DX12交换链");
		LuaCore::luaframe = false;
		ID3D12Device* pD3DDevice;

		if (FAILED(pSwapChain->GetDevice(__uuidof(ID3D12Device), (void**)&pD3DDevice))) {
			return OriginalPresent(pSwapChain, SyncInterval, Flags);
		}

		{
			DXGI_SWAP_CHAIN_DESC desc;
			pSwapChain->GetDesc(&desc);
			Window = desc.OutputWindow;
			if (!OriginalWndProc) {
				OriginalWndProc = (WNDPROC)SetWindowLongPtr(Window, GWLP_WNDPROC, (__int3264)(LONG_PTR)WndProc12);
			}
			g_FrameBufferCount = desc.BufferCount;
			g_FrameContext.clear();
			g_FrameContext.resize(g_FrameBufferCount);
		}

		{
			D3D12_DESCRIPTOR_HEAP_DESC desc = {};
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			desc.NumDescriptors = g_FrameBufferCount;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

			if (pD3DDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pD3DSrvDescHeap)) != S_OK) {
				return OriginalPresent(pSwapChain, SyncInterval, Flags);
			}
		}

		{
			D3D12_DESCRIPTOR_HEAP_DESC desc;
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			desc.NumDescriptors = g_FrameBufferCount;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			desc.NodeMask = 1;

			if (pD3DDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pD3DRtvDescHeap)) != S_OK) {
				return OriginalPresent(pSwapChain, SyncInterval, Flags);
			}

			const auto rtvDescriptorSize = pD3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_pD3DRtvDescHeap->GetCPUDescriptorHandleForHeapStart();

			for (UINT i = 0; i < g_FrameBufferCount; i++) {

				g_FrameContext[i].main_render_target_descriptor = rtvHandle;
				pSwapChain->GetBuffer(i, IID_PPV_ARGS(&g_FrameContext[i].main_render_target_resource));
				pD3DDevice->CreateRenderTargetView(g_FrameContext[i].main_render_target_resource, nullptr, rtvHandle);
				rtvHandle.ptr += rtvDescriptorSize;
			}

		}

		{
			ID3D12CommandAllocator* allocator;
			if (pD3DDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator)) != S_OK) {
				return OriginalPresent(pSwapChain, SyncInterval, Flags);
			}

			for (size_t i = 0; i < g_FrameBufferCount; i++) {
				if (pD3DDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_FrameContext[i].command_allocator)) != S_OK) {
					return OriginalPresent(pSwapChain, SyncInterval, Flags);
				}
			}

			if (pD3DDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_FrameContext[0].command_allocator, NULL, IID_PPV_ARGS(&g_pD3DCommandList)) != S_OK || g_pD3DCommandList->Close() != S_OK) {
				return OriginalPresent(pSwapChain, SyncInterval, Flags);
			}
		}

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = nullptr;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

			// Setup Platform/Renderer backends
		auto fonts = ImGui::GetIO().Fonts;
		imgui_logger->info("从{}加载字体数据", "c:/windows/fonts/simhei.ttf");
		fonts->AddFontFromFileTTF(
			"c:/windows/fonts/simhei.ttf",
			13.0f,
			NULL,
			fonts->GetGlyphRangesChineseFull()
		);
		imgui_logger->info("初始化ImGui渲染");
		ImGui_ImplWin32_Init(Window);
		ImGui_ImplDX12_Init(pD3DDevice, g_FrameBufferCount,
			DXGI_FORMAT_R8G8B8A8_UNORM, g_pD3DSrvDescHeap,
			g_pD3DSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
			g_pD3DSrvDescHeap->GetGPUDescriptorHandleForHeapStart());

		g_Initialized = true;

		pD3DDevice->Release();
	}

	ImGui_ImplWin32_NewFrame();
	ImGui_ImplDX12_NewFrame();
	ImGui::NewFrame();

	if (LuaCore::luaframe) {
		LuaCore::run("on_imgui");
	}

	FrameContext& currentFrameContext = g_FrameContext[pSwapChain->GetCurrentBackBufferIndex()];
	currentFrameContext.command_allocator->Reset();

	D3D12_RESOURCE_BARRIER barrier;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = currentFrameContext.main_render_target_resource;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	g_pD3DCommandList->Reset(currentFrameContext.command_allocator, nullptr);
	g_pD3DCommandList->ResourceBarrier(1, &barrier);
	g_pD3DCommandList->OMSetRenderTargets(1, &currentFrameContext.main_render_target_descriptor, FALSE, nullptr);
	g_pD3DCommandList->SetDescriptorHeaps(1, &g_pD3DSrvDescHeap);
	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_pD3DCommandList);
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	g_pD3DCommandList->ResourceBarrier(1, &barrier);
	g_pD3DCommandList->Close();

	g_pD3DCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList**)&g_pD3DCommandList);
	return OriginalPresent(pSwapChain, SyncInterval, Flags);
}

void HookExecuteCommandLists(ID3D12CommandQueue* queue, UINT NumCommandLists, ID3D12CommandList* ppCommandLists) {
	if (!g_pD3DCommandQueue && queue->GetDesc().Type == D3D12_COMMAND_LIST_TYPE_DIRECT) {
		g_pD3DCommandQueue = queue;
	}

	OriginalExecuteCommandLists(queue, NumCommandLists, ppCommandLists);
}

void ResetState() {
	if (g_Initialized) {
		g_Initialized = false;
		ImGui_ImplWin32_Shutdown();
		ImGui_ImplDX12_Shutdown();
	}
	g_pD3DCommandQueue = nullptr;
	g_FrameContext.clear();
	g_pD3DCommandList = nullptr;
	g_pD3DRtvDescHeap = nullptr;
	g_pD3DSrvDescHeap = nullptr;
}

long HookResizeBuffers(IDXGISwapChain3* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
	ResetState();
	return OriginalResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
}

Status D3D12Init() {
	WNDCLASSEX windowClass;
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = DefWindowProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandle(NULL);
	windowClass.hIcon = NULL;
	windowClass.hCursor = NULL;
	windowClass.hbrBackground = NULL;
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = L"Fake Window";
	windowClass.hIconSm = NULL;

	::RegisterClassEx(&windowClass);

	HWND window = ::CreateWindow(windowClass.lpszClassName, L"Fake DirectX Window", WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, NULL, NULL, windowClass.hInstance, NULL);

	HMODULE libDXGI;
	HMODULE libD3D12;

	if ((libDXGI = ::GetModuleHandle(L"dxgi.dll")) == NULL) {
		::DestroyWindow(window);
		::UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
		return Status::ModuleNotFoundError;
	}

	if ((libD3D12 = ::GetModuleHandle(L"d3d12.dll")) == NULL) {
		::DestroyWindow(window);
		::UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
		return Status::ModuleNotFoundError;
	}

	void* CreateDXGIFactory;
	if ((CreateDXGIFactory = ::GetProcAddress(libDXGI, "CreateDXGIFactory")) == NULL) {
		::DestroyWindow(window);
		::UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
		return Status::UnknownError;
	}

	CComPtr<IDXGIFactory> factory;
	if (((long(__stdcall*)(const IID&, void**))(CreateDXGIFactory))(__uuidof(IDXGIFactory), (void**)&factory) < 0) {
		::DestroyWindow(window);
		::UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
		return Status::UnknownError;
	}

	CComPtr<IDXGIAdapter> adapter;
	if (factory->EnumAdapters(0, &adapter) == DXGI_ERROR_NOT_FOUND) {
		::DestroyWindow(window);
		::UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
		return Status::UnknownError;
	}

	void* D3D12CreateDevice;
	if ((D3D12CreateDevice = ::GetProcAddress(libD3D12, "D3D12CreateDevice")) == NULL) {
		::DestroyWindow(window);
		::UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
		return Status::UnknownError;
	}

	CComPtr<ID3D12Device> device;
	if (((long(__stdcall*)(IUnknown*, D3D_FEATURE_LEVEL, const IID&, void**))(D3D12CreateDevice))(adapter, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), (void**)&device) < 0) {
		::DestroyWindow(window);
		::UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
		return Status::UnknownError;
	}

	D3D12_COMMAND_QUEUE_DESC queueDesc;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Priority = 0;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.NodeMask = 0;

	CComPtr<ID3D12CommandQueue> commandQueue;
	if (device->CreateCommandQueue(&queueDesc, __uuidof(ID3D12CommandQueue), (void**)&commandQueue) < 0) {
		::DestroyWindow(window);
		::UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
		return Status::UnknownError;
	}

	CComPtr<ID3D12CommandAllocator> commandAllocator;
	if (device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&commandAllocator) < 0) {
		::DestroyWindow(window);
		::UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
		return Status::UnknownError;
	}

	CComPtr<ID3D12GraphicsCommandList> commandList;
	if (device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&commandList) < 0) {
		::DestroyWindow(window);
		::UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
		return Status::UnknownError;
	}

	DXGI_RATIONAL refreshRate;
	refreshRate.Numerator = 60;
	refreshRate.Denominator = 1;

	DXGI_MODE_DESC bufferDesc;
	bufferDesc.Width = 100;
	bufferDesc.Height = 100;
	bufferDesc.RefreshRate = refreshRate;
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	DXGI_SAMPLE_DESC sampleDesc;
	sampleDesc.Count = 1;
	sampleDesc.Quality = 0;

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferDesc = bufferDesc;
	swapChainDesc.SampleDesc = sampleDesc;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.OutputWindow = window;
	swapChainDesc.Windowed = 1;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	CComPtr<IDXGISwapChain> swapChain;
	if (factory->CreateSwapChain(commandQueue, &swapChainDesc, &swapChain) < 0) {
		::DestroyWindow(window);
		::UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
		return Status::UnknownError;
	}

	g_MethodsTable = (uint64_t*)::calloc(150, sizeof(uint64_t));
	::memcpy(g_MethodsTable, *(uint64_t**)(void*)device, 44 * sizeof(uint64_t));
	::memcpy(g_MethodsTable + 44, *(uint64_t**)(void*)commandQueue, 19 * sizeof(uint64_t));
	::memcpy(g_MethodsTable + 44 + 19, *(uint64_t**)(void*)commandAllocator, 9 * sizeof(uint64_t));
	::memcpy(g_MethodsTable + 44 + 19 + 9, *(uint64_t**)(void*)commandList, 60 * sizeof(uint64_t));
	::memcpy(g_MethodsTable + 44 + 19 + 9 + 60, *(uint64_t**)(void*)swapChain, 18 * sizeof(uint64_t));

	::DestroyWindow(window);
	::UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
	return Status::Success;
}

Status Hook(uint16_t _index, void** _original, void* _function) {
	void* target = (void*)g_MethodsTable[_index];
	if (MH_CreateHook(target, _function, _original) != MH_OK || MH_EnableHook(target) != MH_OK) {
		return Status::UnknownError;
	}
	return Status::Success;
}

Status Unhook(uint16_t _index, void** _original, void* _function) {
	void* target = (void*)g_MethodsTable[_index];
	MH_DisableHook(target);
	return Status::Success;
}

LRESULT APIENTRY WndProc12(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (g_Initialized) {
		ImGuiIO& io = ImGui::GetIO();
		switch (msg) {
		case WM_LBUTTONDOWN:
			io.MouseDown[0] = true;
			return io.WantCaptureMouse ? 0 : CallWindowProc(OriginalWndProc, hWnd, msg, wParam, lParam);
		case WM_LBUTTONUP:
			io.MouseDown[0] = false;
			return io.WantCaptureMouse ? 0 : CallWindowProc(OriginalWndProc, hWnd, msg, wParam, lParam);
		case WM_RBUTTONDOWN:
			io.MouseDown[1] = true;
			return io.WantCaptureMouse ? 0 : CallWindowProc(OriginalWndProc, hWnd, msg, wParam, lParam);
		case WM_RBUTTONUP:
			io.MouseDown[1] = false;
			return io.WantCaptureMouse ? 0 : CallWindowProc(OriginalWndProc, hWnd, msg, wParam, lParam);
		case WM_MBUTTONDOWN:
			io.MouseDown[2] = true;
			return io.WantCaptureMouse ? 0 : CallWindowProc(OriginalWndProc, hWnd, msg, wParam, lParam);
		case WM_MBUTTONUP:
			io.MouseDown[2] = false;
			return io.WantCaptureMouse ? 0 : CallWindowProc(OriginalWndProc, hWnd, msg, wParam, lParam);
		case WM_MOUSEWHEEL:
			io.MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f;
			return io.WantCaptureMouse ? 0 : CallWindowProc(OriginalWndProc, hWnd, msg, wParam, lParam);
		case WM_MOUSEMOVE:
			io.MousePos.x = (signed short)(lParam);
			io.MousePos.y = (signed short)(lParam >> 16);
			return io.WantCaptureMouse ? 0 : CallWindowProc(OriginalWndProc, hWnd, msg, wParam, lParam);
		case WM_KEYDOWN:
			if (wParam < 256)
				io.KeysDown[wParam] = 1;
			return io.WantCaptureKeyboard ? 0 : CallWindowProc(OriginalWndProc, hWnd, msg, wParam, lParam);
		case WM_KEYUP:
			if (wParam < 256)
				io.KeysDown[wParam] = 0;
			return io.WantCaptureKeyboard ? 0 : CallWindowProc(OriginalWndProc, hWnd, msg, wParam, lParam);
		case WM_CHAR:
			// You can also use ToAscii()+GetKeyboardState() to retrieve characters.
			if (wParam > 0 && wParam < 0x10000)
				io.AddInputCharacter((unsigned short)wParam);
			return io.WantCaptureKeyboard ? 0 : CallWindowProc(OriginalWndProc, hWnd, msg, wParam, lParam);
		}
	}
	return CallWindowProc(OriginalWndProc, hWnd, msg, wParam, lParam);
}

Status InstallHooks() {
#ifdef USE_MINHOOK
	MH_Initialize();
#endif

	Hook(54, (void**)&OriginalExecuteCommandLists, HookExecuteCommandLists);
	Hook(140, (void**)&OriginalPresent, HookPresent);
	Hook(145, (void**)&OriginalResizeBuffers, HookResizeBuffers);

	return Status::Success;
}

Status RemoveHooks() {
	Unhook(54, (void**)&OriginalExecuteCommandLists, HookExecuteCommandLists);
	Unhook(140, (void**)&OriginalPresent, HookPresent);
	Unhook(145, (void**)&OriginalResizeBuffers, HookResizeBuffers);

	if (Window && OriginalWndProc) {
		SetWindowLongPtr(Window, GWLP_WNDPROC, (__int3264)(LONG_PTR)OriginalWndProc);
	}

	ResetState();
	ImGui::DestroyContext();
	MH_Uninitialize();
	return Status::Success;
}
namespace hook_ui {

	HMODULE hMod;
	bool dx12API = false;
	bool imgui = true;
	void init() {
		if (GameInit || !hook_ui::imgui) return;
		if (dx12API) {
			if (D3D12Init() == Status::Success) {
				InstallHooks();
				engine_logger->info("注入d3d12");
			}
			else {
				engine_logger->error("注入d3d12错误");
			}
		}
		else {
			bool init_hook = false;
			do
			{
				if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
				{
					framework_logger->info("创建on_imgui钩子");
					kiero::bind(8, (void**)&oPresent, hkPresent);
					kiero::bind(13, (void**)&oResize, hkResize);
					init_hook = true;
				}
			} while (!init_hook);
		}
		GameInit = true;
	}
	void removeUI() {
		RemoveHooks();
	}
	static void LuaRegister(lua_State* L) {
		engine_logger->info("注册ImGui相关函数");
		lua_register(L, "LoadTexture", [](lua_State* pL) -> int
			{
				string file = (string)lua_tostring(pL, 1);
				string name = (string)lua_tostring(pL, 2);
				void* ret = nullptr;
				TextureList[name] = Texture(0, 0, NULL);
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