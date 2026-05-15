#include "DamageTrigger.h"
#include "Components/BoxComponent.h"
#include "HealthComponent.h"

ADamageTrigger::ADamageTrigger()
{
    // 创建并设置碰撞盒
    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    RootComponent = TriggerBox;
    TriggerBox->SetBoxExtent(FVector(100.0f));
    TriggerBox->SetCollisionProfileName(TEXT("Trigger"));

    // 绑定重叠事件
    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ADamageTrigger::OnOverlapBegin);
}

void ADamageTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || OtherActor == this) return;

    // 尝试获取 OtherActor 的 HealthComponent
    UHealthComponent* HealthComp = OtherActor->FindComponentByClass<UHealthComponent>();
    if (HealthComp)
    {
        HealthComp->TakeDamage(DamageAmount);
    }
}