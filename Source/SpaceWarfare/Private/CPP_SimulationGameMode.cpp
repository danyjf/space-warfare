// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_SimulationGameMode.h"
#include "JsonReadWrite.h"

#include "JsonObjectConverter.h"	// JsonUtilities module
#include "Kismet/KismetSystemLibrary.h"


void ACPP_SimulationGameMode::BeginPlay()
{
	bool bSuccess;
	FString InfoMessage;
	TSharedPtr<FJsonObject> JsonObject = UJsonReadWrite::ReadJson("C:/Dev/SpaceWarfare/Content/SpaceWarfare/Data/SimulationConfig.json", bSuccess, InfoMessage);

	SimulationConfigStruct SimulationConfig;

	if (!FJsonObjectConverter::JsonObjectToUStruct<SimulationConfigStruct>(JsonObject.ToSharedRef(), &SimulationConfig))
	{
		UKismetSystemLibrary::PrintString(this, "Read struct json failed - Was not able to convert the json object to the desired structure. Is it the right format / struct?");
	}


}
