#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Component.h"
#include "GameObject.generated.h"

// 游戏对象基类：场景中的实体
UCLASS(Blueprintable)
class GAMEFRAMEWORK_API UGameObject : public UObject
{
    GENERATED_BODY()

public:
    UGameObject();
    virtual void BeginPlay();
    virtual void Tick(float DeltaTime);
    virtual void EndPlay();

    // 添加组件（模板方法，实例化并挂载到当前对象）
    template<typename T>
    T* AddComponent()
    {
        T* NewComp = NewObject<T>(this);
        if (NewComp)
        {
            NewComp->SetOwner(this);
            Components.Add(NewComp);
            NewComp->BeginPlay();
        }
        return NewComp;
    }

    // 获取该对象的所有组件
    const TArray<UComponent*>& GetComponents() const { return Components; }

    // 世界变换（位置、旋转、缩放）
    UPROPERTY(EditAnywhere, Category = "Transform")
    FTransform WorldTransform;

protected:
    // 挂载在此对象上的所有组件
    UPROPERTY(EditAnywhere, Instanced)
    TArray<TObjectPtr<UComponent>> Components;
};