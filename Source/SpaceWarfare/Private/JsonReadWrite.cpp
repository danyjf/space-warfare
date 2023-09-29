// Fill out your copyright notice in the Description page of Project Settings.


#include "JsonReadWrite.h"
#include "FileReadWrite.h"

TSharedPtr<FJsonObject> UJsonReadWrite::ReadJson(FString JsonPath, bool& bOutSuccess, FString& OutInfoMessage)
{
	FString JsonString = UFileReadWrite::ReadFile(JsonPath, bOutSuccess, OutInfoMessage);
	if (!bOutSuccess)
	{
		return nullptr;
	}

	TSharedPtr<FJsonObject> RetJsonObject;

	if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(JsonString), RetJsonObject))
	{
		bOutSuccess = false;
		OutInfoMessage = FString::Printf(TEXT("Read json failed - Was not able to deserialize string - '%s'"), *JsonPath);
		return nullptr;
	}

	bOutSuccess = true;
	OutInfoMessage = FString::Printf(TEXT("Read json succeeded - '%s'"), *JsonPath);
	return RetJsonObject;
}

void UJsonReadWrite::WriteJson(FString JsonPath, TSharedPtr<FJsonObject> JsonObject, bool& bOutSuccess, FString& OutInfoMessage)
{
	FString JsonString;

	if (!FJsonSerializer::Serialize(JsonObject.ToSharedRef(), TJsonWriterFactory<>::Create(&JsonString, 0)))
	{
		bOutSuccess = false;
		OutInfoMessage = FString::Printf(TEXT("Write json failed - Was not able to serialize to string - '%s'"), *JsonPath);
		return;
	}

	UFileReadWrite::WriteFile(JsonPath, JsonString, bOutSuccess, OutInfoMessage);
	if (!bOutSuccess)
	{
		return;
	}

	bOutSuccess = true;
	OutInfoMessage = FString::Printf(TEXT("Write json succeeded - '%s'"), *JsonPath);
}
