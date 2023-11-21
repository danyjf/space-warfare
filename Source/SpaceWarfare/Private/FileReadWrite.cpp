// Fill out your copyright notice in the Description page of Project Settings.


#include "FileReadWrite.h"

#include "HAL/PlatformFileManager.h"


FString UFileReadWrite::ReadFile(FString FilePath)
{
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*FilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("Read string from file failed - File doesn't exist - '%s'"), *FilePath);
		return "";
	}

	FString RetString = "";

	if (!FFileHelper::LoadFileToString(RetString, *FilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("Read string from file failed - Was not able to read file - '%s'"), *FilePath);
		return "";
	}

	return RetString;
}

void UFileReadWrite::WriteFile(FString FilePath, FString String, bool ShouldAppend)
{
	bool bSuccess = false;

	if (ShouldAppend)
	{
		bSuccess = FFileHelper::SaveStringToFile(
			String, 
			*FilePath, 
			FFileHelper::EEncodingOptions::AutoDetect, 
			&IFileManager::Get(), 
			FILEWRITE_Append
		);
	}
	else
	{
		bSuccess = FFileHelper::SaveStringToFile(String, *FilePath);
	}

	if (!bSuccess)
	{
		UE_LOG(LogTemp, Error, TEXT("Write string to file failed - Was not able to write to file - '%s'"), *FilePath);
		return;
	}
}
