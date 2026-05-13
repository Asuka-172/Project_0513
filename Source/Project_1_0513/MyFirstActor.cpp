#include "MyFirstActor.h"
#include "Components/StaticMeshComponent.h"

AMyFirstActor::AMyFirstActor()
{
    // 创建静态网格组件并设为根组件
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = MeshComponent;

    // 允许每帧 Tick
    PrimaryActorTick.bCanEverTick = true;
}

void AMyFirstActor::MoveActor(FVector Direction)
{
    // 根据方向和速度移动
    FVector NewLocation = GetActorLocation() + Direction * MoveSpeed * GetWorld()->GetDeltaSeconds();
    SetActorLocation(NewLocation);
}