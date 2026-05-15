#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageTrigger.generated.h"

class UBoxComponent;

UCLASS(Blueprintable)
class PROJECT_1_0513_API ADamageTrigger : public AActor
{
    GENERATED_BODY()

public:
    ADamageTrigger();

    // 每次进入触发器造成的伤害量
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    float DamageAmount = 20.0f;

    // 触发区域
    UPROPERTY(VisibleAnywhere)
    UBoxComponent* TriggerBox;

    // 重叠回调
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);
};