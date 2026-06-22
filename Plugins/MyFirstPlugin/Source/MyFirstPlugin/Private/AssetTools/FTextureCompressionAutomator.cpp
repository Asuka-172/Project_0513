#include "FTextureCompressionAutomator.h"
#include "Engine/Texture2D.h"

TextureCompressionSettings FTextureCompressionAutomator::GetRecommendedCompression(const UTexture2D* Texture)
{
    if (!Texture) return TC_Default;

    FString TextureName = Texture->GetName();

    // 法线贴图：_N, _Normal → BC5
    if (TextureName.Contains(TEXT("_N")) || TextureName.Contains(TEXT("_Normal")))
    {
        return TC_Normalmap;  // UE 用 TC_Normalmap 表示 BC5
    }

    // 粗糙度：_R, _Roughness → BC4 (单通道)，这里近似为 TC_Grayscale 或 TC_Displacementmap
    // 注意：BC4 在 UE 中没有直接对应的 TC_ 枚举，通常使用 TC_Grayscale 或 TC_Displacementmap
    if (TextureName.Contains(TEXT("_R")) || TextureName.Contains(TEXT("_Roughness")))
    {
        return TC_Grayscale;  // 灰度图，近似单通道压缩
    }

    // 金属度：_M, _Metallic → BC4
    if (TextureName.Contains(TEXT("_M")) || TextureName.Contains(TEXT("_Metallic")))
    {
        return TC_Grayscale;
    }

    // 高度贴图：_H, _Height
    if (TextureName.Contains(TEXT("_H")) || TextureName.Contains(TEXT("_Height")))
    {
        return TC_Grayscale;
    }

    // 颜色贴图：_D, _Albedo, _Diffuse → BC7 或 BC1
    if (TextureName.Contains(TEXT("_D")) || TextureName.Contains(TEXT("_Albedo")) || TextureName.Contains(TEXT("_Diffuse")))
    {
        return TC_Default;  // BC1/BC3，默认设置通常就是 BC1
    }

    // 默认：BC1/BC3
    return TC_Default;
}

bool FTextureCompressionAutomator::GetRecommendedSRGB(const UTexture2D* Texture)
{
    if (!Texture) return true;

    FString TextureName = Texture->GetName();

    // 法线、粗糙度、金属度、高度等数据贴图不需要 sRGB
    if (TextureName.Contains(TEXT("_N")) || TextureName.Contains(TEXT("_Normal")) ||
        TextureName.Contains(TEXT("_R")) || TextureName.Contains(TEXT("_Roughness")) ||
        TextureName.Contains(TEXT("_M")) || TextureName.Contains(TEXT("_Metallic")) ||
        TextureName.Contains(TEXT("_H")) || TextureName.Contains(TEXT("_Height")))
    {
        return false;
    }

    // 其他情况（颜色贴图）开启 sRGB
    return true;
}

TextureMipGenSettings FTextureCompressionAutomator::GetRecommendedMipGen(const UTexture2D* Texture)
{
    if (!Texture) return TMGS_FromTextureGroup;

    FString TextureName = Texture->GetName();

    // UI 纹理推荐无 Mip
    if (TextureName.Contains(TEXT("_UI")) || TextureName.Contains(TEXT("_Widget")))
    {
        return TMGS_NoMipmaps;
    }

    // 默认使用纹理组设置
    return TMGS_FromTextureGroup;
}

int32 FTextureCompressionAutomator::GetCompressionIndexFromSettings(TextureCompressionSettings Compression)
{
    // 映射到当前 UI 选项索引：0=BC1, 1=BC3, 2=BC5, 3=BC7
    // 注意：CompressionOptions 顺序固定
    switch (Compression)
    {
    case TC_Normalmap:  return 2; // BC5
    case TC_Grayscale:
    case TC_Displacementmap:
        return 0; // 近似为 BC1（单通道）
    case TC_BC7: return 3; // BC7
    case TC_Default:
    default:
        return 0; // BC1
    }
}

int32 FTextureCompressionAutomator::GetMipGenIndexFromSettings(TextureMipGenSettings MipGen)
{
    // 当前 MipGen 选项共用 CompressionOptions 数据源（只有4项），但可以扩展为独立数据源
    // 这里简化：索引0=FromTextureGroup, 1=NoMipmaps
    switch (MipGen)
    {
    case TMGS_NoMipmaps: return 1;
    default: return 0;
    }
}