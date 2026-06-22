#pragma once

#include "CoreMinimal.h"

class UTexture2D;

/**
 * 纹理压缩自动化决策器
 * 根据纹理名称后缀推断最佳压缩格式、sRGB、Mip 生成设置
 */
class FTextureCompressionAutomator
{
public:
    /** 分析纹理并返回推荐的压缩设置 */
    static TextureCompressionSettings GetRecommendedCompression(const UTexture2D* Texture);

    /** 分析纹理并返回推荐的 sRGB 设置 */
    static bool GetRecommendedSRGB(const UTexture2D* Texture);

    /** 分析纹理并返回推荐的 Mip 生成设置 */
    static TextureMipGenSettings GetRecommendedMipGen(const UTexture2D* Texture);

    /**
     * 根据推荐压缩设置转换为 UI 中的下拉索引
     * @param Compression 压缩枚举
     * @return 索引：0=BC1, 1=BC3, 2=BC5, 3=BC7，默认 0
     */
    static int32 GetCompressionIndexFromSettings(TextureCompressionSettings Compression);

    /**
     * 根据推荐 Mip 设置转换为 UI 中的下拉索引
     * @param MipGen Mip 枚举
     * @return 索引：0=FromTextureGroup, 1=NoMipmaps
     */
    static int32 GetMipGenIndexFromSettings(TextureMipGenSettings MipGen);
};