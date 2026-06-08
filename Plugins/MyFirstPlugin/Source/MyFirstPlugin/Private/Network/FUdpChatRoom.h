#pragma once

#include "CoreMinimal.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "Containers/Queue.h"
#include "Common/UdpSocketBuilder.h"

class FUdpChatRoom : public FRunnable
{
public:
    FUdpChatRoom();
    virtual ~FUdpChatRoom();

    // 初始化：绑定本地端口，启动接收线程
    bool Initialize(int32 LocalPort);
    // 发送消息到指定远程地址
    bool SendMessage(const FString& Message, const FString& RemoteIP, int32 RemotePort);
    // 关闭
    void Shutdown();
    // 获取接收到的消息队列（主线程轮询用）
    bool GetNextMessage(FString& OutMessage, FString& OutSender);
    bool IsRunning() const { return bIsRunning; }

    // FRunnable
    virtual bool Init() override;
    virtual uint32 Run() override;
    virtual void Exit() override;

private:
    FSocket* UdpSocket = nullptr;
    TUniquePtr<FRunnableThread> ReceiveThread;
    bool bIsRunning = false;
    int32 BoundPort = 0;

    // 线程安全的消息队列
    TQueue<TPair<FString, FString>, EQueueMode::Mpsc> IncomingMessages;
};