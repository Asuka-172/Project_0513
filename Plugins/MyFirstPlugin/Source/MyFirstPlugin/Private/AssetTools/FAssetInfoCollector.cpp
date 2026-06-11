#include "FAssetInfoCollector.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/Texture.h"
#include "Materials/Material.h"
#include "Engine/Blueprint.h"
#include "Engine/StaticMesh.h"

FAssetInfoCollector::FAssetInfoCollector()
{
}

void FAssetInfoCollector::Refresh()
{
    // 获取 AssetRegistry 实例
    IAssetRegistry& AssetRegistry =
        FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();

    // 如果正在加载，强制等待完成（编辑器中通常已经加载完毕）
    if (AssetRegistry.IsLoadingAssets())
    {
        AssetRegistry.SearchAllAssets(true); // 强制同步扫描
    }

    // 按类获取资产
    FARFilter Filter;
    Filter.bRecursivePaths = true;
    Filter.PackagePaths.Add(FName("/Game")); // 只扫描游戏内容目录

    TArray<FAssetData> AllAssets;
    AssetRegistry.GetAssets(Filter, AllAssets);

    // 重置计数
    TextureCount = 0;
    MaterialCount = 0;
    BlueprintCount = 0;
    StaticMeshCount = 0;
    TotalCount = AllAssets.Num();

    // 逐个判断资产类型
    for (const FAssetData& Asset : AllAssets)
    {
        UClass* AssetClass = Asset.GetClass(); // 获取原生类
        if (!AssetClass) continue;

        // 判断是否是贴图
        if (AssetClass->IsChildOf(UTexture::StaticClass()))
        {
            TextureCount++;
        }
        // 判断是否是材质
        else if (AssetClass->IsChildOf(UMaterial::StaticClass()))
        {
            MaterialCount++;
        }
        // 判断是否是蓝图
        else if (AssetClass->IsChildOf(UBlueprint::StaticClass()))
        {
            BlueprintCount++;
        }
        // 判断是否是静态网格
        else if (AssetClass->IsChildOf(UStaticMesh::StaticClass()))
        {
            StaticMeshCount++;
        }
    }
}