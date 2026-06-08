#pragma once

#include "CoreMinimal.h"
#include "Sockets.h"

class FTcpEchoClient
{
public:
    FTcpEchoClient();
    ~FTcpEchoClient();

    bool Connect(const FString& Address, int32 Port);
    void Disconnect();
    bool SendMessage(const FString& Message);

private:
    FSocket* Socket = nullptr;
    bool bConnected = false;
};