#pragma once

#include <Support/WinInclude.h>
#include <Support/ComPointer.h>

class DXContext {

public:
	bool Init();
	void Shutdown();

	void SignalAndWait();

	ID3D12GraphicsCommandList7* InitCommandList();
	void ExeCommandList();

	//Helper functions
	inline void Flush(size_t count) {
		for (size_t i = 0; i < count; i++)
			SignalAndWait();
	}

	inline ComPointer<IDXGIFactory7>& GetFactory() {
		return m_dxgiFactory;
	}

	inline ComPointer<ID3D12Device10>& GetDevice (){
		return m_device;
	}

	inline ComPointer< ID3D12CommandQueue>& GetCommandQueue() {
		return m_cmdQueue;
	}

private:
	//note the order these com pointers are created
	ComPointer<IDXGIFactory7> m_dxgiFactory;
	ComPointer<ID3D12Device10> m_device;
	ComPointer<ID3D12CommandQueue> m_cmdQueue;
	ComPointer<ID3D12CommandAllocator> m_cmdAllocator;
	ComPointer<ID3D12GraphicsCommandList7> m_cmdList;

	ComPointer<ID3D12Fence1> m_fence;
	UINT64 m_fenceValue = 0;

	//pointer to windows event. This event will be used to signal inactive GPU for more that a certain time
	HANDLE m_fenceEvent = nullptr;

	//Singelton
	//This is a structure to create a single instace from a certain class. It was used in multiple classes in this example.
public:
	DXContext(const DXContext&) = delete;
	DXContext& operator=(const DXContext&) = delete;

	inline static DXContext& Get() {
		static DXContext instance;
		return instance;
	}

private:
	DXContext() = default;

};
