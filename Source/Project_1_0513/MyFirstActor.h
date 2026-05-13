#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyFirstActor.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PROJECT_1_0513_API AMyFirstActor : public AActor
{
    GENERATED_BODY()

public:
    AMyFirstActor();

    // 移动速度，可在编辑器和蓝图中读写
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Actor")
    float MoveSpeed = 100.0f;

    // 显示名称，可在编辑器和蓝图中读写
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Actor")
    FString DisplayName = TEXT("MyFirstActor");

    // 移动函数，可在蓝图中调用
    UFUNCTION(BlueprintCallable, Category = "My Actor")
    void MoveActor(FVector Direction);

protected:
    // 静态网格组件，可见但只读
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    class UStaticMeshComponent* MeshComponent;
};