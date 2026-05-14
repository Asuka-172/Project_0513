#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableBase.generated.h"

class USphereComponent;

UCLASS(Blueprintable)
class PROJECT_1_0513_API AInteractableBase : public AActor
{
    GENERATED_BODY()

public:
    AInteractableBase();

    // 蓝图实现交互行为
    UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
    void OnInteract(AActor* Interactor);

    // C++默认返回 true，蓝图可覆写
    UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
    bool CanInteract(AActor* Interactor);
    virtual bool CanInteract_Implementation(AActor* Interactor);

    // 触发区域回调（6 参数版，匹配 OnComponentBeginOverlap 委托）
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USphereComponent* TriggerSphere;
};