#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "ToolMenus.h"

class FMyAssetActions : public FAssetTypeActions_Base
{
public:
    // 显式指定要支持的资产类型
    explicit FMyAssetActions(UClass* InSupportedClass);

    // 菜单显示名称
    virtual FText GetName() const override;

    // 资产类型颜色
    virtual FColor GetTypeColor() const override;

    // 此 Action 支持的资产类型
    virtual UClass* GetSupportedClass() const override;

    // 构建右键菜单
    virtual void GetActions(const TArray<UObject*>& InObjects,
        FToolMenuSection& Section) override;

    // 必须实现的纯虚函数
    virtual uint32 GetCategories() override;


private:
    UClass* SupportedClass = nullptr;

    // 复制纹理到 /Game/CopiedTextures/
    static void ExecuteCopyToDirectory(TArray<FAssetData> Assets);

    // 转换为材质实例（占位功能）
    static void ExecuteConvertToInstance(TArray<FAssetData> Assets);
};