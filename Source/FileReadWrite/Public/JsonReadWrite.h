// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "JsonObjectConverter.h"	// JsonUtilities module

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

    template<typename OutStructType>
    static void ReadStructFromJsonFile(FString JsonPath, OutStructType* OutStruct)
    {
    	 TSharedPtr<FJsonObject> JsonObject = ReadJson(JsonPath);

    	 if (!FJsonObjectConverter::JsonObjectToUStruct<OutStructType>(JsonObject.ToSharedRef(), OutStruct))
    	 {
    		  UE_LOG(LogTemp, Error, TEXT("Read struct json failed - Was not able to convert the json object to the desired structure. Is it the right format / struct?"));
    	 }
    }
};
