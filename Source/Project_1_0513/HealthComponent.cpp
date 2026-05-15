#include "HealthComponent.h"

UHealthComponent::UHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
    Super::BeginPlay();
    // 游戏开始时，当前血量等于最大血量
    CurrentHealth = MaxHealth;
}

void UHealthComponent::TakeDamage(float Amount)
{
    if (Amount <= 0.0f) return;

    // 记录受伤前的血量，用于广播
    float OldHealth = CurrentHealth;
    CurrentHealth = FMath::Clamp(CurrentHealth - Amount, 0.0f, MaxHealth);
    float ActualDamage = OldHealth - CurrentHealth;

    // 广播血量变化事件
    OnHealthChanged.Broadcast(CurrentHealth, MaxHealth, ActualDamage);

    // 可在调试信息中打印
    UE_LOG(LogTemp, Warning, TEXT("Health: %f / %f (Damage: %f)"), CurrentHealth, MaxHealth, ActualDamage);
}