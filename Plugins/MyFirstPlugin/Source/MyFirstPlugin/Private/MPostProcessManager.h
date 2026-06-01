#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/PostProcessVolume.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "MPostProcessManager.generated.h"

UCLASS()
class UMPostProcessManager : public UObject
{
    GENERATED_BODY()

public:
    void Initialize(APostProcessVolume* Volume);
    void Shutdown();

    // 灰度化参数
    void SetGrayscaleBlendWeight(float Value);
    void SetGrayscaleContrast(float Value);
    void SetGrayscaleBrightness(float Value);

    // 反转参数
    void SetInvertWeight(float Value);
    void SetInvertThreshold(float Value);

    // 老电影参数
    void SetSepiaStrength(float Value);
    void SetNoiseStrength(float Value);
    void SetVignetteStrength(float Value);

    // 启用/禁用某个滤镜
    void SetFilterEnabled(int32 FilterIndex, bool bEnabled);

private:
    UPROPERTY()
    UMaterialInstanceDynamic* GrayscaleMID = nullptr;

    UPROPERTY()
    UMaterialInstanceDynamic* InvertMID = nullptr;

    UPROPERTY()
    UMaterialInstanceDynamic* FilmMID = nullptr;

    UPROPERTY()
    APostProcessVolume* PostProcessVolume = nullptr;

    void ApplyBlendables();
    bool bFiltersEnabled[3] = { true, true, true };
};