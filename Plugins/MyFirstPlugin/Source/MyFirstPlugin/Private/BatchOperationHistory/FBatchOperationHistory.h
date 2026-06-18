#pragma once

#include "CoreMinimal.h"
#include "Misc/DateTime.h"

/**
 * 单条操作历史记录
 */
struct FBatchOperationEntry
{
    FDateTime Timestamp;       // 操作时间
    FString Operation;         // 操作类型（Copy / Rename / Compress）
    FString AssetName;         // 资产名称
    FString SourcePath;        // 源路径
    FString TargetName;        // 目标名称或新名称
    bool bSuccess;             // 是否成功
};

/**
 * 批量操作历史记录管理器
 * 负责记录每次批量操作的详细信息，并支持导出为 CSV 文件
 */
class FBatchOperationHistory
{
public:
    FBatchOperationHistory();

    /**
     * 添加一条历史记录
     * @param Operation 操作类型
     * @param AssetName 资产名称
     * @param SourcePath 源路径
     * @param TargetName 目标名称
     * @param bSuccess 是否成功
     */
    void AddEntry(const FString& Operation, const FString& AssetName,
        const FString& SourcePath, const FString& TargetName, bool bSuccess);

    /**
     * 导出所有历史记录到 CSV 文件
     * @param FilePath 文件名（不含目录，仅文件名，如 "BatchReport_20240617.csv"）
     * @return 完整导出文件路径（空字符串表示失败）
     */
    FString ExportToCSV(const FString& FilePath) const;

    /**
     * 获取所有历史记录
     */
    const TArray<FBatchOperationEntry>& GetEntries() const { return Entries; }

    /**
     * 清空历史记录
     */
    void Clear();

private:
    TArray<FBatchOperationEntry> Entries;
};