#include "FMyAssetActions.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "Misc/ScopedSlowTask.h"
#include "Engine/Texture.h"
#include "Materials/Material.h"
#include "Editor/EditorEngine.h"
#include "FileHelpers.h"
#include "AssetTypeCategories.h"

FMyAssetActions::FMyAssetActions(UClass* InSupportedClass)
    : SupportedClass(InSupportedClass)
{
}

FText FMyAssetActions::GetName() const
{
    if (SupportedClass == UTexture::StaticClass())
    {
        return FText::FromString(TEXT("Texture"));   
    }
    else if (SupportedClass == UMaterial::StaticClass())
    {
        return FText::FromString(TEXT("Material"));
    }
    // 其他类型可以继续扩展，或者返回一个通用名称
    return FText::FromString(TEXT("Asset"));
}

FColor FMyAssetActions::GetTypeColor() const
{
    return FColor(255, 128, 0); // 橙色
}

UClass* FMyAssetActions::GetSupportedClass() const
{
    return SupportedClass;
}

uint32 FMyAssetActions::GetCategories()
{
    // 返回基本资产类别，或自定义 EAssetTypeCategories::Misc
    return EAssetTypeCategories::Misc;
}

void FMyAssetActions::GetActions(const TArray<UObject*>& InObjects, FToolMenuSection& Section)
{
    // 将 UObject 数组转为 FAssetData 数组
    TArray<FAssetData> Assets;
    for (UObject* Obj : InObjects)
    {
        if (Obj)
        {
            Assets.Add(FAssetData(Obj));
        }
    }

    // 添加一个子菜单"My Tools"
    Section.AddSubMenu(
        "MyToolsSubMenu",
        FText::FromString("My Tools"),
        FText::FromString("Custom tools for assets"),
        FNewToolMenuDelegate::CreateLambda([Assets](UToolMenu* SubMenu)
            {
                // 在子菜单中添加具体操作
                FToolMenuSection& SubSection = SubMenu->AddSection("MyToolsSection", FText::FromString("Operations"));

                SubSection.AddMenuEntry(
                    "CopyToDirectory",
                    FText::FromString("Copy to /Game/CopiedTextures"),
                    FText::FromString("将所选资源复制到目标目录"),
                    FSlateIcon(),
                    FUIAction(FExecuteAction::CreateLambda([Assets]()
                        {
                            ExecuteCopyToDirectory(Assets);
                        }))
                );

                SubSection.AddMenuEntry(
                    "ConvertToInstance",
                    FText::FromString("Convert to Material Instance"),
                    FText::FromString("将所选材质转换为材质实例"),
                    FSlateIcon(),
                    FUIAction(FExecuteAction::CreateLambda([Assets]()
                        {
                            ExecuteConvertToInstance(Assets);
                        }))
                );
            })
    );
}

void FMyAssetActions::ExecuteCopyToDirectory(TArray<FAssetData> Assets)
{
    FAssetToolsModule& AssetToolsModule =
        FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");

    FScopedSlowTask SlowTask(Assets.Num(), FText::FromString("Copying assets..."));
    SlowTask.MakeDialog();

    for (const FAssetData& Asset : Assets)
    {
        SlowTask.EnterProgressFrame(1);

        FString NewPackageName = TEXT("/Game/CopiedTextures/") + Asset.AssetName.ToString();
        UObject* ExistingAsset = StaticLoadObject(UObject::StaticClass(), nullptr, *NewPackageName);
        if (ExistingAsset)
        {
            NewPackageName += TEXT("_Copy");
        }

        AssetToolsModule.Get().DuplicateAsset(Asset.AssetName.ToString(),
            FPaths::GetPath(NewPackageName),
            Asset.GetAsset());
    }

    SlowTask.Destroy();
}

void FMyAssetActions::ExecuteConvertToInstance(TArray<FAssetData> Assets)
{
    // 这是一个占位函数，仅打印日志
    UE_LOG(LogTemp, Warning, TEXT("FMyAssetActions: ConvertToInstance called for %d assets"), Assets.Num());
    for (const FAssetData& Asset : Assets)
    {
        UE_LOG(LogTemp, Log, TEXT("  -> %s"), *Asset.AssetName.ToString());
    }
}