#include "UIBackend.hpp"
#include <imgui.h>
#include "imgui_impl_win32.h"
#include "imgui_impl_dx10.h"
#include "imgui_impl_dx11.h"
#include "../logger/Logger.hpp"
#include "Kiero.hpp"

#include <d3d11.h>
#include <d3d10.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND window, UINT message, WPARAM wideParameter, LPARAM longParameter);

namespace UIBackend
{
	HWND g_hWnd = nullptr;
	WNDPROC g_wndProcOriginal = nullptr;
	std::function<void()> g_renderCallback;
	bool g_initialized = false;

	LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
			return true;

		return CallWindowProc(g_wndProcOriginal, hWnd, msg, wParam, lParam);
	}

	// DirectX 10 Present Hook
	HRESULT WINAPI Present10(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
	{
		static ID3D10Device* pDevice = nullptr;
		static ID3D10RenderTargetView* pRenderTargetView = nullptr;

		if (!g_initialized)
		{
			if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D10Device), (void**)&pDevice)))
			{
				DXGI_SWAP_CHAIN_DESC sd;
				pSwapChain->GetDesc(&sd);
				g_hWnd = sd.OutputWindow;

				ID3D10Texture2D* pBackBuffer = nullptr;
				pSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&pBackBuffer);
				pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pRenderTargetView);
				pBackBuffer->Release();

				g_wndProcOriginal = (WNDPROC)SetWindowLongPtr(g_hWnd, GWLP_WNDPROC, (LONG_PTR)WndProc);

				ImGui::CreateContext();
				ImGui_ImplWin32_Init(g_hWnd);
				ImGui_ImplDX10_Init(pDevice);

				g_initialized = true;
				Logger::Log(Logger::LogTag::Info, "[UIBackend] DirectX 10 initialized");
			}
			else
			{
				return orig_Present10(pSwapChain, SyncInterval, Flags);
			}
		}

		ImGui_ImplDX10_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		if (g_renderCallback)
			g_renderCallback();

		ImGui::Render();

		pDevice->OMSetRenderTargets(1, &pRenderTargetView, nullptr);
		ImGui_ImplDX10_RenderDrawData(ImGui::GetDrawData());

		return orig_Present10(pSwapChain, SyncInterval, Flags);
	}

	// DirectX 11 Present Hook
	HRESULT WINAPI Present11(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
	{
		static ID3D11Device* pDevice = nullptr;
		static ID3D11DeviceContext* pContext = nullptr;
		static ID3D11RenderTargetView* pRenderTargetView = nullptr;

		if (!g_initialized)
		{
			if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
			{
				pDevice->GetImmediateContext(&pContext);

				DXGI_SWAP_CHAIN_DESC sd;
				pSwapChain->GetDesc(&sd);
				g_hWnd = sd.OutputWindow;

				ID3D11Texture2D* pBackBuffer = nullptr;
				pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
				pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pRenderTargetView);
				pBackBuffer->Release();

				g_wndProcOriginal = (WNDPROC)SetWindowLongPtr(g_hWnd, GWLP_WNDPROC, (LONG_PTR)WndProc);

				ImGui::CreateContext();
				ImGui_ImplWin32_Init(g_hWnd);
				ImGui_ImplDX11_Init(pDevice, pContext);

				g_initialized = true;
				Logger::Log(Logger::LogTag::Info, "[UIBackend] DirectX 11 initialized");
			}
			else
			{
				return orig_Present11(pSwapChain, SyncInterval, Flags);
			}
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		if (g_renderCallback)
			g_renderCallback();

		ImGui::Render();

		pContext->OMSetRenderTargets(1, &pRenderTargetView, nullptr);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		return orig_Present11(pSwapChain, SyncInterval, Flags);
	}

	bool Initialize(const std::function<void()>& renderCallback)
	{
		g_renderCallback = renderCallback;

		// 尝试初始化 Kiero（支持 D3D10 和 D3D11）
		if (kiero::init(kiero::RenderType::D3D10) == kiero::Status::Success)
		{
			if (kiero::bind(8, reinterpret_cast<void**>(&orig_Present10), reinterpret_cast<void*>(Present10)) == kiero::Status::Success)
			{
				Logger::Log(Logger::LogTag::Info, "[UIBackend] Kiero D3D10 bound successfully");
				return true;
			}
		}

		if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
		{
			if (kiero::bind(8, reinterpret_cast<void**>(&orig_Present11), reinterpret_cast<void*>(Present11)) == kiero::Status::Success)
			{
				Logger::Log(Logger::LogTag::Info, "[UIBackend] Kiero D3D11 bound successfully");
				return true;
			}
		}

		Logger::Log(Logger::LogTag::Error, "[UIBackend] Failed to initialize Kiero");
		return false;
	}

	void Shutdown()
	{
		if (!g_initialized) return;

		if (g_wndProcOriginal)
		{
			SetWindowLongPtr(g_hWnd, GWLP_WNDPROC, (LONG_PTR)g_wndProcOriginal);
		}

		ImGui_ImplDX11_Shutdown();
		ImGui_ImplDX10_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		g_initialized = false;
	}
}
