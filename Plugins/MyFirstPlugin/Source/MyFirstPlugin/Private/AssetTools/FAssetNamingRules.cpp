#include "FAssetNamingRules.h"
#include "Engine/Texture2D.h"
#include "Materials/Material.h"
#include "Engine/StaticMesh.h"
#include "Engine/Blueprint.h"

FString FAssetNamingRules::GetRecommendedPrefix(UClass* AssetClass)
{
    if (!AssetClass) return TEXT("");
    if (AssetClass->IsChildOf(UTexture2D::StaticClass()))  return TEXT("T_");
    if (AssetClass->IsChildOf(UMaterial::StaticClass()))    return TEXT("M_");
    if (AssetClass->IsChildOf(UStaticMesh::StaticClass()))  return TEXT("SM_");
    if (AssetClass->IsChildOf(UBlueprint::StaticClass()))   return TEXT("BP_");
    return TEXT("");
}

FString FAssetNamingRules::GetRecommendedSuffix(UTexture2D* Texture)
{
    if (!Texture) return TEXT("");

    // 根据压缩设置推断用途
    if (Texture->CompressionSettings == TC_Normalmap)
        return TEXT("_N");
    if (Texture->CompressionSettings == TC_Displacementmap || Texture->CompressionSettings == TC_DistanceFieldFont)
        return TEXT("_H"); // 高度图之类

    // 其他情况看 sRGB
    if (Texture->SRGB)
        return TEXT("_D");   // 颜色贴图
    else
        return TEXT("_M");   // 粗糙度/金属度等遮罩
}

bool FAssetNamingRules::IsCompliant(const FAssetData& Asset)
{
    FString AssetName = Asset.AssetName.ToString();
    UClass* AssetClass = Asset.GetClass();
    FString ExpectedPrefix = GetRecommendedPrefix(AssetClass);

    // 检查前缀
    if (!ExpectedPrefix.IsEmpty() && !AssetName.StartsWith(ExpectedPrefix))
        return false;

    // 对于纹理，进一步检查后缀（如果纹理已加载）
    if (AssetClass && AssetClass->IsChildOf(UTexture2D::StaticClass()))
    {
        UTexture2D* Texture = Cast<UTexture2D>(Asset.GetAsset());
        if (Texture)
        {
            FString ExpectedSuffix = GetRecommendedSuffix(Texture);
            if (!ExpectedSuffix.IsEmpty() && !AssetName.EndsWith(ExpectedSuffix))
                return false;
        }
    }
    return true;
}

FString FAssetNamingRules::SuggestNewName(const FAssetData& Asset)
{
    FString OriginalName = Asset.AssetName.ToString();
    UClass* AssetClass = Asset.GetClass();
    FString Prefix = GetRecommendedPrefix(AssetClass);

    // 构建新名称：先去掉可能已有的错误前缀（简单处理：如果以任何常见前缀开头就截断）
    // 这里直接添加正确前缀，但为了避免重复，检查是否已有正确前缀
    FString NewName = OriginalName;
    if (!Prefix.IsEmpty() && !OriginalName.StartsWith(Prefix))
    {
        // 尝试截断可能存在的旧前缀（如 T_, M_, SM_, BP_, MI_）
        static const TArray<FString> KnownPrefixes = { TEXT("T_"), TEXT("M_"), TEXT("MI_"), TEXT("SM_"), TEXT("BP_") };
        for (const FString& OldPrefix : KnownPrefixes)
        {
            if (OriginalName.StartsWith(OldPrefix))
            {
                NewName = OriginalName.Mid(OldPrefix.Len());
                break;
            }
        }
        NewName = Prefix + NewName;
    }

    // 纹理后缀处理
    if (AssetClass && AssetClass->IsChildOf(UTexture2D::StaticClass()))
    {
        UTexture2D* Texture = Cast<UTexture2D>(Asset.GetAsset());
        if (Texture)
        {
            FString Suffix = GetRecommendedSuffix(Texture);
            if (!Suffix.IsEmpty() && !NewName.EndsWith(Suffix))
            {
                // 移除已有的其他后缀（如 _D, _N, _R 等）简化处理：直接加后缀，不删除旧后缀
                NewName += Suffix;
            }
        }
    }

    return NewName;
}