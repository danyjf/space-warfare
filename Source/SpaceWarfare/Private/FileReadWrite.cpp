// Fill out your copyright notice in the Description page of Project Settings.


#include "FileReadWrite.h"

#include "HAL/PlatformFileManager.h"

FString UFileReadWrite::ReadFile(FString FilePath, bool& bOutSuccess, FString& OutInfoMessage)
{
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*FilePath))
	{
		bOutSuccess = false;
		OutInfoMessage = FString::Printf(TEXT("Read string from file failed - File doesn't exist - '%s'"), *FilePath);
		return "";
	}

	FString RetString = "";

	if (!FFileHelper::LoadFileToString(RetString, *FilePath))
	{
		bOutSuccess = false;
		OutInfoMessage = FString::Printf(TEXT("Read string from file failed - Was not able to read file - '%s'"), *FilePath);
		return "";
	}

	bOutSuccess = true;
	OutInfoMessage = FString::Printf(TEXT("Read string from file succeeded - '%s'"), *FilePath);
	return RetString;
}

void UFileReadWrite::WriteFile(FString FilePath, FString String, bool& bOutSuccess, FString& OutInfoMessage)
{
	if (!FFileHelper::SaveStringToFile(String, *FilePath))
	{
		bOutSuccess = false;
		OutInfoMessage = FString::Printf(TEXT("Write string to file failed - Was not able to write to file - '%s'"), *FilePath);
		return;
	}

	bOutSuccess = true;
	OutInfoMessage = FString::Printf(TEXT("Write string to file succeeded - '%s'"), *FilePath);
}
