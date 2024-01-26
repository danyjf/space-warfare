// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "JsonReadWrite.generated.h"


/**
 * 
 */
UCLASS()
class FILEREADWRITE_API UJsonReadWrite : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static TSharedPtr<FJsonObject> ReadJson(FString JsonPath);

	static void WriteJson(FString JsonPath, TSharedPtr<FJsonObject> JsonObject);
};
