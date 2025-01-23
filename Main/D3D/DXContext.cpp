#include "DXContext.h"

bool DXContext::Init()
{
	if(SUCCEEDED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device))))
	{
		//After the device we create the command queue
		//we need to decriptor for the command queue
		D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
		

		//we need to setup the descritor before we pass it to CreateCommandQueue function
		cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; // multi work mode when it can transfer, copy ect. Notice that we passed the command list type because a command queue contains only one type of command list
		cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH; //used for gaming usually
		cmdQueueDesc.NodeMask = 0; //related to which GPU to be used. 0 points to the default GPU.
		cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

		//create a command queue
		if (SUCCEEDED(m_device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&m_cmdQueue))))
		{ 
			//here we can use the m_cmdQueue to excute commands
			//example signal will setup a fence value while wait is check if the fence got a certain value or higher
			//lets create a fence
			if (SUCCEEDED(m_device->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence))))
			{
				return true;
			}
			
		}
	}
	return false;
}

void DXContext::Shutdown()
{
	m_device.Release();
	m_device.Release();
}
