#pragma once

#include "CoreMinimal.h"

class FAssetInfoCollector
{
public:
    FAssetInfoCollector();

    // 刷新统计数据，扫描整个项目
    void Refresh();

    // 各类型统计
    int32 GetTextureCount() const { return TextureCount; }
    int32 GetMaterialCount() const { return MaterialCount; }
    int32 GetBlueprintCount() const { return BlueprintCount; }
    int32 GetStaticMeshCount() const { return StaticMeshCount; }
    int32 GetTotalCount() const { return TotalCount; }

private:
    int32 TextureCount = 0;
    int32 MaterialCount = 0;
    int32 BlueprintCount = 0;
    int32 StaticMeshCount = 0;
    int32 TotalCount = 0;
};