#include "GameWorld.h"
#include "GameObject.h"

UGameWorld::UGameWorld() {}

void UGameWorld::BeginPlay()
{
    // 通知所有对象 BeginPlay
    for (UGameObject* Obj : ActiveObjects)
    {
        if (Obj) Obj->BeginPlay();
    }
}

void UGameWorld::Tick(float DeltaTime)
{
    // 更新所有对象
    for (UGameObject* Obj : ActiveObjects)
    {
        if (Obj) Obj->Tick(DeltaTime);
    }
}

UGameObject* UGameWorld::SpawnObject(TSubclassOf<UGameObject> Class, const FTransform& SpawnTransform)
{
    if (!Class) return nullptr;

    UGameObject* NewObj = NewObject<UGameObject>(this, Class);
    if (NewObj)
    {
        NewObj->WorldTransform = SpawnTransform;
        ActiveObjects.Add(NewObj);
        NewObj->BeginPlay();
    }
    return NewObj;
}

void UGameWorld::DestroyObject(UGameObject* Object)
{
    if (Object)
    {
        Object->EndPlay();
        ActiveObjects.Remove(Object);
        Object->ConditionalBeginDestroy();
    }
}