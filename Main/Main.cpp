#include <iostream>

#include <Support/WinInclude.h>
#include <Support/ComPointer.h>
#include <Support/Window.h>
#include <Debug/DXDebugLayer.h>
#include<D3D/DXContext.h>

using namespace std;
int main()
{
    DXDebugLayer::Get().Init();

    if (DXContext::Get().Init() && DXWindow::Get().Init())
    {
        //here where the program starts
        while(!DXWindow::Get().ShouldClose())
        {
            //updating the window so we can interact with it
            DXWindow::Get().Update();

            //creating a command list
            auto* cmdList = DXContext::Get().InitCommandList();
            
            //here we can start adding commands to the list
            // a lot of setup
            // draw

            //Excuting the command list
            DXContext::Get().ExeCommandList();

            //show me on the screen
        }

        DXWindow::Get().Shutdown();
        DXContext::Get().Shutdown();
    }

    DXDebugLayer::Get().Shutdown();
}

