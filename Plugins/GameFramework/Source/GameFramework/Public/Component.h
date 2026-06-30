#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Component.generated.h"

class UGameObject; // 前置声明

// 组件基类：游戏对象的能力模块
UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew)
class GAMEFRAMEWORK_API UComponent : public UObject
{
    GENERATED_BODY()

public:
    UComponent();
    virtual void BeginPlay();
    virtual void Tick(float DeltaTime);
    virtual void EndPlay();

    // 获取该组件所属的游戏对象
    UGameObject* GetOwner() const { return Owner; }
    // 设置组件所有者（由 GameObject 在 AddComponent 时调用）
    void SetOwner(UGameObject* InOwner) { Owner = InOwner; }

protected:
    // 指向拥有此组件的游戏对象
    UPROPERTY()
    TObjectPtr<UGameObject> Owner;
};