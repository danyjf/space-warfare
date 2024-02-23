// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FileReadWrite.generated.h"


/**
 * 
 */
UCLASS()
class FILEREADWRITE_API UFileReadWrite : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static FString ReadFile(FString FilePath);

	static void WriteFile(FString FilePath, FString String, bool ShouldAppend = false);
};
