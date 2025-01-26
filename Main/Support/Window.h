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

	inline bool ShouldClose() const {
		return m_shouldClose;
	}

	inline bool ShouldResize() const {
		return m_shouldResize;
	}

	static constexpr size_t GetFrameCount()
	{
		return 2;
	}

private:
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

	//swap chain com pointer
	ComPointer<IDXGISwapChain3> m_swapChain;

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