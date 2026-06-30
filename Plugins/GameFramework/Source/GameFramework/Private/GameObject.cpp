#include "Component.h"
#include "GameObject.h"

UGameObject::UGameObject() {}

void UGameObject::BeginPlay()
{
    // 子类可覆写
}

void UGameObject::Tick(float DeltaTime)
{
    // 更新所有组件
    for (UComponent* Comp : Components)
    {
        if (Comp)
        {
            Comp->Tick(DeltaTime);
        }
    }
}

void UGameObject::EndPlay()
{
    // 通知各组件结束
    for (UComponent* Comp : Components)
    {
        if (Comp) Comp->EndPlay();
    }
    Components.Empty();
}