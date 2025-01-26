#include "DXContext.h"

bool DXContext::Init()
{
	if (FAILED(CreateDXGIFactory2(0, IID_PPV_ARGS(&m_dxgiFactory))))
	{ return false; }

	if(FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device))))
	{ return false; }
	
	//After the device we create the command queue, we need to decriptor for the command queue
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
	//we need to setup the descritor before we pass it to CreateCommandQueue function
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; // multi work mode when it can transfer, copy ect. Notice that we passed the command list type because a command queue contains only one type of command list
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH; //used for gaming usually
	cmdQueueDesc.NodeMask = 0; //related to which GPU to be used. 0 points to the default GPU.
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	//create a command queue
	if (FAILED(m_device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&m_cmdQueue))))
	{ return false; }

	//here we can use the m_cmdQueue to excute commands
	//example signal will setup a fence value while wait is check if the fence got a certain value or higher
	//lets create a fence
	if (FAILED(m_device->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence))))
	{ return false; }

	//Cearting a fence event
	m_fenceEvent = CreateEvent(nullptr, false, false, nullptr);
	if(!m_fenceEvent)
	{ return false; }

	//creatinig the command allocator
	if(FAILED(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,IID_PPV_ARGS(&m_cmdAllocator))))
	{ return false; }

	//creating the command list
	if(FAILED(m_device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT,D3D12_COMMAND_LIST_FLAG_NONE ,IID_PPV_ARGS(&m_cmdList))))
	{ return false; }

	return true;
}

void DXContext::Shutdown()
{
	//releasing what we have created in a reverse order of the creation
	m_cmdList.Release();
	m_cmdAllocator.Release();
	if (m_fenceEvent)
	{
		CloseHandle(m_fenceEvent);
	}
	m_fence.Release();
	m_cmdQueue.Release();
	m_device.Release();
	m_dxgiFactory.Release();
}

void DXContext::SignalAndWait()
{
	//Command queue is incrementing the fence value by 1
	//This takes place on the GPU
	m_cmdQueue->Signal(m_fence, ++m_fenceValue);

	//the fence set the event as soon as it is completed  when it get that from the GPU
	if (SUCCEEDED(m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent)))
	{
		//wiat for the event that is triggered when the m_fence complete
		if (WaitForSingleObject(m_fenceEvent, 20000) != WAIT_OBJECT_0) {
			exit(-1);
		}


	}
	else {
		exit(-1);
	}
	//Now we need to check from the CPU side to see if GPU has finished
	

	//We did not do this in the example but left notes for further reference
	//chekcing if the last complted fence value is less that the current one
	//if it is then the gpu is still not finished
	//still not that clear but I will continue
}

ID3D12GraphicsCommandList7* DXContext::InitCommandList()
{
	m_cmdAllocator->Reset();
	m_cmdList->Reset(m_cmdAllocator, nullptr);
	return m_cmdList;
}

void DXContext::ExeCommandList()
{
	//need to close command list before we use it
	if (SUCCEEDED(m_cmdList->Close())) {
		ID3D12CommandList* lists[] = { m_cmdList };
		m_cmdQueue->ExecuteCommandLists(1, lists);
		SignalAndWait();
	}
}
