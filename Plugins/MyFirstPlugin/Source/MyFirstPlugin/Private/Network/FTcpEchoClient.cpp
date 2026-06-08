#include "FTcpEchoClient.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h"

FTcpEchoClient::FTcpEchoClient()
{
}

FTcpEchoClient::~FTcpEchoClient()
{
    Disconnect();
}

bool FTcpEchoClient::Connect(const FString& Address, int32 Port)
{
    ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
    if (!SocketSubsystem) return false;

    Socket = SocketSubsystem->CreateSocket(NAME_Stream, TEXT("TcpEchoClient"), false);
    if (!Socket) return false;

    TSharedRef<FInternetAddr> Addr = SocketSubsystem->CreateInternetAddr();
    bool bIsValid = false;
    Addr->SetIp(*Address, bIsValid);
    if (!bIsValid) return false;
    Addr->SetPort(Port);

    if (!Socket->Connect(*Addr))
    {
        UE_LOG(LogTemp, Error, TEXT("FTcpEchoClient: Failed to connect to %s:%d"), *Address, Port);
        SocketSubsystem->DestroySocket(Socket);
        Socket = nullptr;
        return false;
    }

    bConnected = true;
    UE_LOG(LogTemp, Warning, TEXT("FTcpEchoClient: Connected to %s:%d"), *Address, Port);
    return true;
}

void FTcpEchoClient::Disconnect()
{
    if (Socket)
    {
        Socket->Close();
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
        Socket = nullptr;
    }
    bConnected = false;
}

bool FTcpEchoClient::SendMessage(const FString& Message)
{
    if (!Socket || !bConnected) return false;

    FTCHARToUTF8 Converted(*Message);
    int32 BytesSent = 0;
    if (!Socket->Send(reinterpret_cast<const uint8*>(Converted.Get()), Converted.Length(), BytesSent))
    {
        UE_LOG(LogTemp, Error, TEXT("FTcpEchoClient: Failed to send message"));
        return false;
    }

    UE_LOG(LogTemp, Warning, TEXT("FTcpEchoClient: Sent '%s' (%d bytes)"), *Message, BytesSent);

    // µÈ´ý»ØÏÔ
    uint8 Buffer[1024];
    int32 BytesRead = 0;
    if (Socket->Recv(Buffer, sizeof(Buffer), BytesRead))
    {
        if (BytesRead > 0)
        {
            FString Echoed = FString(UTF8_TO_TCHAR(reinterpret_cast<char*>(Buffer)));
            UE_LOG(LogTemp, Warning, TEXT("FTcpEchoClient: Received echo '%s'"), *Echoed);
        }
    }

    return true;
}