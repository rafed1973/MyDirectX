#pragma once

#include <iostream>
#include <Support/WinInclude.h>
#include <Support/ComPointer.h>

class DXWindow {

public:
	bool Init();
	void Update();
	void Shutdown();

	inline bool ShouldClose() const {
		return m_shouldClose;
	}

private:
	static LRESULT CALLBACK OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);
	//This is an integer number and it is a type returned by the window register and we need it to stor that
	ATOM m_wndClass = 0;
	//This is a window handle to store window instance when we create it
	HWND m_window = nullptr;

	bool m_shouldClose = false;

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