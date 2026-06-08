#include "FUdpChatRoom.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"

FUdpChatRoom::FUdpChatRoom() {}

FUdpChatRoom::~FUdpChatRoom()
{
    Shutdown();
}

bool FUdpChatRoom::Initialize(int32 LocalPort)
{
    UdpSocket = FUdpSocketBuilder(TEXT("UdpChatSocket"))
        .AsNonBlocking()          // 非阻塞模式，RecvFrom 不会死等
        .AsReusable()             // 允许端口复用
        .BoundToPort(LocalPort)   // 绑定端口
        .Build();

    if (!UdpSocket)
    {
        UE_LOG(LogTemp, Error, TEXT("FUdpChatRoom: Failed to create UDP socket via builder"));
        return false;
    }

    BoundPort = LocalPort;
    bIsRunning = true;

    ReceiveThread.Reset(FRunnableThread::Create(this, TEXT("UdpChatReceiver"), 0, TPri_Normal));
    UE_LOG(LogTemp, Warning, TEXT("FUdpChatRoom: Started on port %d"), LocalPort);
    return true;
}

void FUdpChatRoom::Shutdown()
{
    bIsRunning = false;
    if (UdpSocket)
    {
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(UdpSocket);
        UdpSocket = nullptr;
    }
    if (ReceiveThread)
    {
        ReceiveThread->WaitForCompletion();
        ReceiveThread.Reset();
    }
    UE_LOG(LogTemp, Warning, TEXT("FUdpChatRoom: Shutdown"));
}

bool FUdpChatRoom::SendMessage(const FString& Message, const FString& RemoteIP, int32 RemotePort)
{
    if (!UdpSocket) return false;

    ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
    TSharedRef<FInternetAddr> TargetAddr = SocketSubsystem->CreateInternetAddr();
    bool bIsValid = false;
    TargetAddr->SetIp(*RemoteIP, bIsValid);
    if (!bIsValid)
    {
        UE_LOG(LogTemp, Error, TEXT("FUdpChatRoom: Invalid IP '%s'"), *RemoteIP);
        return false;
    }
    TargetAddr->SetPort(RemotePort);

    FTCHARToUTF8 Converter(*Message);
    int32 BytesSent = 0;
    bool bSuccess = UdpSocket->SendTo(reinterpret_cast<const uint8*>(Converter.Get()), Converter.Length(), BytesSent, *TargetAddr);  // <-- 修复点
    if (bSuccess)
    {
        UE_LOG(LogTemp, Log, TEXT("FUdpChatRoom: Sent '%s' to %s:%d"), *Message, *RemoteIP, RemotePort);
    }
    return bSuccess;
}

bool FUdpChatRoom::GetNextMessage(FString& OutMessage, FString& OutSender)
{
    TPair<FString, FString> Pair;
    if (IncomingMessages.Dequeue(Pair))
    {
        OutMessage = Pair.Key;
        OutSender = Pair.Value;
        return true;
    }
    return false;
}

bool FUdpChatRoom::Init() { return true; }
void FUdpChatRoom::Exit() {}

uint32 FUdpChatRoom::Run()
{
    TSharedRef<FInternetAddr> SenderAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
    UE_LOG(LogTemp, Warning, TEXT("FUdpChatRoom: Receive thread started"));
    while (bIsRunning)
    {
        uint8 Buffer[1024];
        int32 BytesRead = 0;
        bool bSuccess = UdpSocket->RecvFrom(Buffer, sizeof(Buffer), BytesRead, *SenderAddr);
        if (!bSuccess)
        {
            ESocketErrors Error = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLastErrorCode();
            // 忽略无数据错误（非阻塞模式的正常情况）
            if (Error != SE_EWOULDBLOCK && Error != SE_NO_ERROR)
            {
                UE_LOG(LogTemp, Warning, TEXT("FUdpChatRoom: RecvFrom error %d"), (int32)Error);
            }
            FPlatformProcess::Sleep(0.001f); // 短暂让出CPU
            continue;
        }
        if (BytesRead > 0)
        {
            FString Message = FString(UTF8_TO_TCHAR(reinterpret_cast<const char*>(Buffer))).Left(BytesRead);
            FString Sender = SenderAddr->ToString(true);
            UE_LOG(LogTemp, Log, TEXT("FUdpChatRoom: Received '%s' from %s"), *Message, *Sender);
            IncomingMessages.Enqueue(TPair<FString, FString>(Message, Sender));
        }
    }
    return 0;
}