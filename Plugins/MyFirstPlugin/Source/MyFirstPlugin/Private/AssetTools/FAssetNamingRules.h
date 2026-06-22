#pragma once

#include "CoreMinimal.h"
#include "AssetRegistry/AssetData.h"

class UTexture2D;

class FAssetNamingRules
{
public:
    /** 根据资产类返回推荐前缀 */
    static FString GetRecommendedPrefix(UClass* AssetClass);

    /** 根据纹理设置推断推荐后缀（仅对 UTexture2D 有效） */
    static FString GetRecommendedSuffix(UTexture2D* Texture);

    /** 检查资产名称是否符合命名规范 */
    static bool IsCompliant(const FAssetData& Asset);

    /**
     * 生成符合规范的资产名称
     * @param Asset 资产数据
     * @return 建议的新名称（不含路径）
     */
    static FString SuggestNewName(const FAssetData& Asset);
};