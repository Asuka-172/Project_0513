#include "FBatchOperationHistory.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFileManager.h"

FBatchOperationHistory::FBatchOperationHistory()
{
}

void FBatchOperationHistory::AddEntry(const FString& Operation, const FString& AssetName,
    const FString& SourcePath, const FString& TargetName, bool bSuccess)
{
    FBatchOperationEntry Entry;
    Entry.Timestamp = FDateTime::Now();
    Entry.Operation = Operation;
    Entry.AssetName = AssetName;
    Entry.SourcePath = SourcePath;
    Entry.TargetName = TargetName;
    Entry.bSuccess = bSuccess;
    Entries.Add(Entry);
}

FString FBatchOperationHistory::ExportToCSV(const FString& FilePath) const
{
    // 构建 CSV 内容
    FString CSVContent;
    CSVContent += TEXT("Time,Operation,AssetName,SourcePath,TargetName,Status\n");

    for (const FBatchOperationEntry& Entry : Entries)
    {
        CSVContent += FString::Printf(
            TEXT("%s,%s,%s,%s,%s,%s\n"),
            *Entry.Timestamp.ToString(),
            *Entry.Operation,
            *Entry.AssetName,
            *Entry.SourcePath,
            *Entry.TargetName,
            Entry.bSuccess ? TEXT("Success") : TEXT("Failed")
        );
    }

    // 确保导出目录存在
    FString ExportDir = FPaths::ProjectSavedDir() / TEXT("BatchReports");
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    if (!PlatformFile.DirectoryExists(*ExportDir))
    {
        PlatformFile.CreateDirectoryTree(*ExportDir);
    }

    // 完整文件路径
    FString FullPath = ExportDir / FilePath;

    // 写入文件
    if (FFileHelper::SaveStringToFile(CSVContent, *FullPath))
    {
        UE_LOG(LogTemp, Log, TEXT("Batch report exported to: %s"), *FullPath);
        return FullPath;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to export batch report to: %s"), *FullPath);
        return FString();
    }
}

void FBatchOperationHistory::Clear()
{
    Entries.Empty();
}