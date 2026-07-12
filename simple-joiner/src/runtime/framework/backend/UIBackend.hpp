#pragma once

#include <functional>
#include <d3d10_1.h>
#include <d3d11.h>

namespace UIBackend
{
	// 原始 Present 函数指针
	extern HRESULT(WINAPI* orig_Present10)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
	extern HRESULT(WINAPI* orig_Present11)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);

	// 初始化渲染框架（会自动 Hook DirectX Present 函数）
	// renderCallback: 每帧调用的渲染回调函数
	bool Initialize(const std::function<void()>& renderCallback);

	// 关闭渲染框架
	void Shutdown();
}
