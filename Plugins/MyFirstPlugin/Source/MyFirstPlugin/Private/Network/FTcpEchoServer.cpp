#include "FTcpEchoServer.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h"

FTcpEchoServer::FTcpEchoServer()
{
}

FTcpEchoServer::~FTcpEchoServer()
{
    Stop();
}

bool FTcpEchoServer::Start(int32 Port)
{
    if (bIsRunning) return false;

    ListenPort = Port;

    // 获取 Socket 子系统
    ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
    if (!SocketSubsystem) return false;

    // 创建监听 Socket (TCP)
    ListenerSocket = SocketSubsystem->CreateSocket(NAME_Stream, TEXT("TcpEchoListener"), false);
    if (!ListenerSocket) return false;

    // 绑定地址和端口
    FIPv4Address IPv4Address = FIPv4Address::Any; // 监听所有网络接口
    TSharedRef<FInternetAddr> Addr = SocketSubsystem->CreateInternetAddr();
    Addr->SetIp(IPv4Address.Value);
    Addr->SetPort(Port);

    if (!ListenerSocket->Bind(*Addr))
    {
        UE_LOG(LogTemp, Error, TEXT("FTcpEchoServer: Failed to bind to port %d"), Port);
        SocketSubsystem->DestroySocket(ListenerSocket);
        ListenerSocket = nullptr;
        return false;
    }

    // 开始监听，最大等待连接数 5
    if (!ListenerSocket->Listen(5))
    {
        UE_LOG(LogTemp, Error, TEXT("FTcpEchoServer: Failed to listen on port %d"), Port);
        SocketSubsystem->DestroySocket(ListenerSocket);
        ListenerSocket = nullptr;
        return false;
    }

    bIsRunning = true;
    ServerThread = TUniquePtr<FRunnableThread>(FRunnableThread::Create(this, TEXT("TcpEchoServerThread")));
    if (!ServerThread)
    {
        UE_LOG(LogTemp, Error, TEXT("FTcpEchoServer: Failed to create thread"));
        return false;
    }
    UE_LOG(LogTemp, Warning, TEXT("FTcpEchoServer: Started on port %d"), Port);
    return true;
}

void FTcpEchoServer::Stop()
{
    bIsRunning = false;

    // 关闭监听 Socket，从而让阻塞的 Accept 返回
    if (ListenerSocket)
    {
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ListenerSocket);
        ListenerSocket = nullptr;
    }

    // 等待线程结束
    if (ServerThread)
    {
        ServerThread->WaitForCompletion();
        ServerThread.Reset();
    }

    UE_LOG(LogTemp, Warning, TEXT("FTcpEchoServer: Stopped"));
}

bool FTcpEchoServer::Init()
{
    return true;
}

uint32 FTcpEchoServer::Run()
{
    while (bIsRunning)
    {
        // 接受客户端连接（阻塞模式，可通过设置超时避免一直阻塞）
        bool bHasPendingConnection = false;
        if (ListenerSocket && ListenerSocket->HasPendingConnection(bHasPendingConnection) && bHasPendingConnection)
        {
            FSocket* NewClientSocket = ListenerSocket->Accept(TEXT("TcpEchoClient"));
            if (NewClientSocket)
            {
                // 为每个客户端创建独立处理（这里简单处理：单客户端，阻塞模式）
                HandleClient(NewClientSocket);
                // 处理完关闭客户端 Socket
                ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(NewClientSocket);
            }
        }
        FPlatformProcess::Sleep(0.1f); // 避免忙等
    }
    return 0;
}

void FTcpEchoServer::Exit()
{
}

void FTcpEchoServer::HandleClient(FSocket* InClientSocket)
{
    uint8 Buffer[1024];
    int32 BytesRead = 0;

    // 接收数据
    if (InClientSocket->Recv(Buffer, sizeof(Buffer), BytesRead))
    {
        if (BytesRead > 0)
        {
            // 原样返回
            int32 BytesSent = 0;
            InClientSocket->Send(Buffer, BytesRead, BytesSent);
            FString Received = FString(UTF8_TO_TCHAR(reinterpret_cast<char*>(Buffer)));
            UE_LOG(LogTemp, Warning, TEXT("FTcpEchoServer: Received '%s', echoed back."), *Received);
        }
    }
}