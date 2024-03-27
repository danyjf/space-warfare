// Fill out your copyright notice in the Description page of Project Settings.

#include "JsonReadWrite.h"
#include "FileReadWrite.h"

TSharedPtr<FJsonObject> UJsonReadWrite::ReadJson(FString JsonPath)
{
	FString JsonString = UFileReadWrite::ReadFile(JsonPath);

	TSharedPtr<FJsonObject> RetJsonObject;

	if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(JsonString), RetJsonObject))
	{
		UE_LOG(LogTemp, Error, TEXT("Read json failed - Was not able to deserialize string - '%s'"), *JsonPath);
		return nullptr;
	}

	return RetJsonObject;
}

void UJsonReadWrite::WriteJson(FString JsonPath, TSharedPtr<FJsonObject> JsonObject)
{
	FString JsonString;

	if (!FJsonSerializer::Serialize(JsonObject.ToSharedRef(), TJsonWriterFactory<>::Create(&JsonString, 0)))
	{
		UE_LOG(LogTemp, Error, TEXT("Write json failed - Was not able to serialize to string - '%s'"), *JsonPath);
		return;
	}

	UFileReadWrite::WriteFile(JsonPath, JsonString);
}
