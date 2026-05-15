#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

// 声明动态多播委托，带三个 float 参数
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
    FOnHealthChanged,
    float, CurrentHealth,
    float, MaxHealth,
    float, Damage
);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECT_1_0513_API UHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UHealthComponent();

    // 最大生命值，可在蓝图中调整默认值
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
    float MaxHealth = 100.0f;

    // 当前生命值，只读，运行时由逻辑更新
    UPROPERTY(BlueprintReadOnly, Category = "Health")
    float CurrentHealth;

    // 健康变化事件，可在蓝图中绑定
    UPROPERTY(BlueprintAssignable, Category = "Health")
    FOnHealthChanged OnHealthChanged;

    // 受到伤害，可在蓝图中调用
    UFUNCTION(BlueprintCallable, Category = "Health")
    void TakeDamage(float Amount);

    virtual void BeginPlay() override;
};