#include "InteractableBase.h"
#include "Components/SphereComponent.h"

AInteractableBase::AInteractableBase()
{
    TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerSphere"));
    RootComponent = TriggerSphere;
    TriggerSphere->SetSphereRadius(150.0f);

    // 绑定委托，现在签名完全匹配
    TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &AInteractableBase::OnOverlapBegin);
}

bool AInteractableBase::CanInteract_Implementation(AActor* Interactor)
{
    return true; // 默认任何人都可以交互
}

void AInteractableBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    // 防止自己触发自己
    if (OtherActor == this) return;

    // 检查交互条件
    if (CanInteract(OtherActor))
    {
        // 调用蓝图实现的具体行为
        OnInteract(OtherActor);
    }
}