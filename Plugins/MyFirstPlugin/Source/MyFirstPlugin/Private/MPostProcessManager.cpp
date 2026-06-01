#include "MPostProcessManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

void UMPostProcessManager::Initialize(APostProcessVolume* Volume)
{
    PostProcessVolume = Volume;
    if (!PostProcessVolume) return;

    // 加载材质资源（路径根据实际调整）
    UMaterial* GrayscaleMat = LoadObject<UMaterial>(nullptr, TEXT("/Game/Materials/M_PP_Grayscale"));
    UMaterial* InvertMat = LoadObject<UMaterial>(nullptr, TEXT("/Game/Materials/M_PP_Invert"));
    UMaterial* FilmMat = LoadObject<UMaterial>(nullptr, TEXT("/Game/Materials/M_PP_Film"));

    if (GrayscaleMat) GrayscaleMID = UMaterialInstanceDynamic::Create(GrayscaleMat, this);
    if (InvertMat) InvertMID = UMaterialInstanceDynamic::Create(InvertMat, this);
    if (FilmMat) FilmMID = UMaterialInstanceDynamic::Create(FilmMat, this);

    // 应用初始状态
    ApplyBlendables();
}

void UMPostProcessManager::Shutdown()
{
    if (PostProcessVolume)
    {
        PostProcessVolume->Settings.WeightedBlendables.Array.Empty();
    }
    GrayscaleMID = nullptr;
    InvertMID = nullptr;
    FilmMID = nullptr;
}

void UMPostProcessManager::ApplyBlendables()
{
    if (!PostProcessVolume) return;

    PostProcessVolume->Settings.WeightedBlendables.Array.Empty();

    if (bFiltersEnabled[0] && GrayscaleMID)
    {
        PostProcessVolume->Settings.WeightedBlendables.Array.Add(
            FWeightedBlendable(1.0f, GrayscaleMID)
        );
    }
    if (bFiltersEnabled[1] && InvertMID)
    {
        PostProcessVolume->Settings.WeightedBlendables.Array.Add(
            FWeightedBlendable(1.0f, InvertMID)
        );
    }
    if (bFiltersEnabled[2] && FilmMID)
    {
        PostProcessVolume->Settings.WeightedBlendables.Array.Add(
            FWeightedBlendable(1.0f, FilmMID)
        );
    }
}

void UMPostProcessManager::SetGrayscaleBlendWeight(float Value)
{
    if (GrayscaleMID) GrayscaleMID->SetScalarParameterValue(FName("BlendWeight"), Value);
}

void UMPostProcessManager::SetGrayscaleContrast(float Value)
{
    if (GrayscaleMID) GrayscaleMID->SetScalarParameterValue(FName("Contrast"), Value);
}

void UMPostProcessManager::SetGrayscaleBrightness(float Value)
{
    if (GrayscaleMID) GrayscaleMID->SetScalarParameterValue(FName("Brightness"), Value);
}

void UMPostProcessManager::SetInvertWeight(float Value)
{
    if (InvertMID) InvertMID->SetScalarParameterValue(FName("InvertWeight"), Value);
}

void UMPostProcessManager::SetInvertThreshold(float Value)
{
    if (InvertMID) InvertMID->SetScalarParameterValue(FName("Threshold"), Value);
}

void UMPostProcessManager::SetSepiaStrength(float Value)
{
    if (FilmMID) FilmMID->SetScalarParameterValue(FName("SepiaStrength"), Value);
}

void UMPostProcessManager::SetNoiseStrength(float Value)
{
    if (FilmMID) FilmMID->SetScalarParameterValue(FName("NoiseStrength"), Value);
}

void UMPostProcessManager::SetVignetteStrength(float Value)
{
    if (FilmMID) FilmMID->SetScalarParameterValue(FName("VignetteStrength"), Value);
}

void UMPostProcessManager::SetFilterEnabled(int32 FilterIndex, bool bEnabled)
{
    if (FilterIndex >= 0 && FilterIndex < 3)
    {
        bFiltersEnabled[FilterIndex] = bEnabled;
        ApplyBlendables();
    }
}