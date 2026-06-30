#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameWorld.generated.h"

class UGameObject;

UCLASS()
class GAMEFRAMEWORK_API UGameWorld : public UObject
{
    GENERATED_BODY()

public:
    UGameWorld();
    virtual void BeginPlay();
    virtual void Tick(float DeltaTime);

    // 在指定位置创建一个新的游戏对象
    UGameObject* SpawnObject(TSubclassOf<UGameObject> Class, const FTransform& SpawnTransform);
    // 销毁一个游戏对象
    void DestroyObject(UGameObject* Object);

    // 获取当前世界中所有活跃的对象
    const TArray<UGameObject*>& GetActiveObjects() const { return ActiveObjects; }

protected:
    UPROPERTY()
    TArray<TObjectPtr<UGameObject>> ActiveObjects;
};