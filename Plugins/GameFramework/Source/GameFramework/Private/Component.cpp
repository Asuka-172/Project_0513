#include "Component.h"
#include "GameObject.h"

UComponent::UComponent() {}

void UComponent::BeginPlay()
{
    // 子类可覆写
}

void UComponent::Tick(float DeltaTime)
{
    // 子类可覆写
}

void UComponent::EndPlay()
{
    // 子类可覆写
}