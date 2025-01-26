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
        //set window to full screen
        DXWindow::Get().SetFullscreen(true);
        
        //the program main loop
        while(!DXWindow::Get().ShouldClose())
        {
            //updating the window so we can interact with it
            DXWindow::Get().Update();

            //Resizing window
            if (DXWindow::Get().ShouldResize())
            {
                //Flushing the command Q so the swapchain resources are let go
                DXContext::Get().Flush(DXWindow::GetFrameCount());
                DXWindow::Get().Resize();
            }

            //============BEGIN DRAWING==================
            //creating a command list
            auto* cmdList = DXContext::Get().InitCommandList();
            
            //inserting a resource barried in the command list of the type present to render target to befing drwing
            DXWindow::Get().BeginFrame(cmdList);
            
            //inserting a resource barried in the command list of the type render target to present to show on screen
            DXWindow::Get().EndFrame(cmdList);

            //============FINISH DRAWING AND PRESENT==================
            //Excuting the command list
            DXContext::Get().ExeCommandList();

            //Present the swap chain
            DXWindow::Get().Present();

            //show me on the screen
        }

        //Flushing the command Q so the swapchain resources are let go
        DXContext::Get().Flush(DXWindow::GetFrameCount());

        DXWindow::Get().Shutdown();
        DXContext::Get().Shutdown();
    }

    DXDebugLayer::Get().Shutdown();
}

