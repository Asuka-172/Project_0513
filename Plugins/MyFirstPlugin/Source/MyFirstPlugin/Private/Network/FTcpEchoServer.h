#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "Sockets.h"

class FTcpEchoServer : public FRunnable
{
public:
    FTcpEchoServer();
    virtual ~FTcpEchoServer();

    bool Start(int32 Port);
    virtual void Stop() override;
    bool IsRunning() const { return bIsRunning; }

    // FRunnable interface
    virtual bool Init() override;
    virtual uint32 Run() override;
    virtual void Exit() override;

private:
    FSocket* ListenerSocket = nullptr;
    FSocket* ClientSocket = nullptr;
    TUniquePtr<FRunnableThread> ServerThread;
    FThreadSafeBool bIsRunning = false;
    int32 ListenPort = 0;

    void HandleClient(FSocket* InClientSocket);
};