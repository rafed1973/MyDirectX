#include <iostream>

#include <Support/WinInclude.h>
#include <Support/ComPointer.h>
#include <Debug/DXDebugLayer.h>
#include<D3D/DXContext.h>

using namespace std;
int main()
{
    DXDebugLayer::Get().Init();

    if (DXContext::Get().Init())
    {
        //here where the program starts
        while(true)
        {
            //creating a command list
            auto* cmdList = DXContext::Get().InitCommandList();
            
            //here we can start adding commands to the lisgt
            //**************

            //Excuting the command list
            DXContext::Get().ExeCommandList();
        }

        DXContext::Get().Shutdown();
    }

    DXDebugLayer::Get().Shutdown();
}

