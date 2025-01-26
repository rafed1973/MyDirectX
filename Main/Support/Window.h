#pragma once

#include <iostream>
#include <Support/WinInclude.h>
#include <Support/ComPointer.h>
#include <D3D/DXContext.h>

class DXWindow {

public:
	bool Init();
	void Update();
	void Present();
	void Shutdown();
	void Resize();
	void SetFullscreen(bool enabled);

	//dealing with barriers and using commandlist
	void BeginFrame(ID3D12GraphicsCommandList7* cmdlist); //this will make sure that we can draw on the resource
	void EndFrame(ID3D12GraphicsCommandList7* cmdlist); //this will make sure that we are on the right stat to get present

	inline bool ShouldClose() const {
		return m_shouldClose;
	}

	inline bool ShouldResize() const {
		return m_shouldResize;
	}

	inline bool IsFullscreen() const {
		return m_isFullscreen;
	}

	//declaing the number of swapchain frames or buffers and get them
	static constexpr size_t FrameCount = 2;
	static constexpr size_t GetFrameCount()
	{
		return FrameCount;
	}

private:
	//getting and realsing buffers
	//before resizing a swap chaing buffer we need to release the buffers and then get them
	bool GetBuffers();
	void ReleaseBuffers();
	
	static LRESULT CALLBACK OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	//This is an integer number and it is a type returned by the window register and we need it to stor that
	ATOM m_wndClass = 0;
	//This is a window handle to store window instance when we create it
	HWND m_window = nullptr;
	// a bool that keep track of the window state
	bool m_shouldClose = false;
	//a bool to track if the window size change
	bool m_shouldResize = false;

	UINT m_width = 1280;
	UINT m_height = 720;

	bool m_isFullscreen = false;

	//swap chain com pointer
	ComPointer<IDXGISwapChain3> m_swapChain;

	//com pointer to the swap chain bufferes
	ComPointer<ID3D12Resource2> m_buffers[FrameCount];

	//current buffer
	size_t m_currentBufferIndex = 0;

	//Singelton.
public:
	DXWindow(const DXWindow&) = delete;
	DXWindow& operator=(const DXWindow&) = delete;

	inline static DXWindow& Get() {
		static DXWindow instance;
		return instance;
	}

private:
	DXWindow() = default;

};