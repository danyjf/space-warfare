// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_HttpServer.h"
#include "CPP_GroundStationManager.h"
#include "CPP_GroundStation.h"
#include "CPP_MultiplayerGameMode.h"
#include "CPP_SatelliteCommandManager.h"
#include "CPP_SatelliteCommands.h"
#include "CPP_GameState.h"
#include "SatelliteCommandDataStructs.h"

#include "HttpPath.h"
#include "IHttpRouter.h"
#include "HttpServerHttpVersion.h"
#include "HttpServerModule.h"
#include "HttpServerResponse.h"
#include "JsonUtilities.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACPP_HttpServer::ACPP_HttpServer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACPP_HttpServer::BeginPlay()
{
    StartServer();
	MultiplayerGameMode = Cast<ACPP_MultiplayerGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	Super::BeginPlay();
}

void ACPP_HttpServer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    StopServer();
}

// Called every frame
void ACPP_HttpServer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (!GroundStationManager)
    {
        GroundStationManager = Cast<ACPP_GroundStationManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ACPP_GroundStationManager::StaticClass()));
    }
}

void ACPP_HttpServer::StartServer()
{
	FHttpServerModule& HttpServerModule = FHttpServerModule::Get();

    // This looks pretty dumb but if I don't do it then I can't check if the 
    // port is open, it sets bHttpListenersEnabled to true in the http server module
	HttpServerModule.StartAllListeners();
	TSharedPtr<IHttpRouter> HttpRouter = HttpServerModule.GetHttpRouter(ServerPort, true);

	if (HttpRouter.IsValid())
	{
		HttpRouter->BindRoute(FHttpPath(GroundStationListPath), EHttpServerRequestVerbs::VERB_GET,
			[this](const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete) { return GetGroundStationList(Request, OnComplete); });
		HttpRouter->BindRoute(FHttpPath(SatelliteListPath), EHttpServerRequestVerbs::VERB_GET,
			[this](const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete) { return GetSatelliteList(Request, OnComplete); });
		HttpRouter->BindRoute(FHttpPath(ThrustCommandPath), EHttpServerRequestVerbs::VERB_POST,
			[this](const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete) { return CreateThrustCommand(Request, OnComplete); });

		HttpServerModule.StartAllListeners();

		UE_LOG(LogTemp, Log, TEXT("Web server started on port = %d"), ServerPort);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Could not start web server on port = %d"), ServerPort);
        ServerPort++;
        StartServer();
	}
}

void ACPP_HttpServer::StopServer()
{
    FHttpServerModule& HttpServerModule = FHttpServerModule::Get();
	HttpServerModule.StopAllListeners();
}

bool ACPP_HttpServer::GetGroundStationList(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
{
    RequestPrint(Request);

    FGroundStationListResponse GroundStationListResponse;
    GroundStationListResponse.ClientID = GroundStationManager->OwnerPlayerID;
    GroundStationListResponse.Count = GroundStationManager->GroundStations.Num();
    GroundStationListResponse.RequestTime = GetWorld()->GetGameState<ACPP_GameState>()->CurrentEpoch;
    for (ACPP_GroundStation* GroundStation : GroundStationManager->GroundStations)
    {
        FGroundStationResponse GroundStationResponse;
        GroundStationResponse.OwnerID = GroundStation->OwnerPlayerID;
        GroundStationResponse.Label = GroundStation->Name;
        GroundStationResponse.FOV = GroundStation->DetectionFieldOfView;
        GroundStationResponse.Altitude = GroundStation->GeographicCoordinates.Altitude;
        GroundStationResponse.Latitude = GroundStation->GeographicCoordinates.Latitude;
        GroundStationResponse.Longitude = GroundStation->GeographicCoordinates.Longitude;

        GroundStationListResponse.GroundStations.Add(GroundStationResponse);
    }

    FString JsonGroundStationList;
    FJsonObjectConverter::UStructToJsonObjectString<FGroundStationListResponse>(GroundStationListResponse, JsonGroundStationList);

	TUniquePtr<FHttpServerResponse> Response = FHttpServerResponse::Create(JsonGroundStationList, TEXT("application/json"));
    
	OnComplete(MoveTemp(Response));
	return true;
}

bool ACPP_HttpServer::GetSatelliteList(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
{
    RequestPrint(Request);

    FSatelliteListResponse SatelliteListResponse;
    SatelliteListResponse.ClientID = GroundStationManager->OwnerPlayerID;
    SatelliteListResponse.Count = GroundStationManager->TrackedSatellites.Num();
    SatelliteListResponse.RequestTime = GetWorld()->GetGameState<ACPP_GameState>()->CurrentEpoch;
    for (const TPair<int, FSatelliteInfo>& Elem : GroundStationManager->TrackedSatellites)
    {
        const int& SatelliteID = Elem.Key;
        const FSatelliteInfo& SatelliteInfo = Elem.Value;

        FSatelliteResponse SatelliteResponse;
        SatelliteResponse.OwnerID = SatelliteInfo.OwnerID;
        SatelliteResponse.SatelliteID = SatelliteID;
        SatelliteResponse.Label = SatelliteInfo.Label;
        SatelliteResponse.Mass = SatelliteInfo.Mass;
        SatelliteResponse.Position = SatelliteInfo.Position;
        SatelliteResponse.Rotation = SatelliteInfo.Rotation;
        SatelliteResponse.Velocity = SatelliteInfo.Velocity;
        SatelliteResponse.Epoch = SatelliteInfo.Epoch;
        SatelliteResponse.IsTargeted = SatelliteInfo.IsTargeted;

        SatelliteListResponse.Satellites.Add(SatelliteResponse);
    }

    FString JsonSatelliteList;
    FJsonObjectConverter::UStructToJsonObjectString<FSatelliteListResponse>(SatelliteListResponse, JsonSatelliteList);

	TUniquePtr<FHttpServerResponse> Response = FHttpServerResponse::Create(JsonSatelliteList, TEXT("application/json"));
    
	OnComplete(MoveTemp(Response));
	return true;
}

bool ACPP_HttpServer::CreateThrustCommand(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
{
    RequestPrint(Request);

    int SatelliteID = FCString::Atoi(*Request.PathParams.FindRef("id"));

    // Check if satellite exists
    if (!GroundStationManager->TrackedSatellites.Contains(SatelliteID))
    {
        // Generate forbidden access response
        FString JsonResponse = "{\"message\": \"The provided satellite ID does not exist\"}";
	    TUniquePtr<FHttpServerResponse> Response = FHttpServerResponse::Create(JsonResponse, TEXT("application/json"));
        Response->Code = EHttpServerResponseCodes::NotFound;

	    OnComplete(MoveTemp(Response));
        return true;
    }

    // Check if satellite belongs to the player
    if (GroundStationManager->TrackedSatellites[SatelliteID].OwnerID != GroundStationManager->OwnerPlayerID)
    {
        // Generate forbidden access response
        FString JsonResponse = "{\"message\": \"The provided satellite ID corresponds to a satellite that is not owned by the player\"}";
	    TUniquePtr<FHttpServerResponse> Response = FHttpServerResponse::Create(JsonResponse, TEXT("application/json"));
        Response->Code = EHttpServerResponseCodes::Forbidden;

	    OnComplete(MoveTemp(Response));
        return true;
    }

    // Parse request body into command struct
	FUTF8ToTCHAR BodyTCHARData(reinterpret_cast<const ANSICHAR*>(Request.Body.GetData()), Request.Body.Num());
	FString BodyStrData {BodyTCHARData.Length(), BodyTCHARData.Get()};
    FThrustCommandData ThrustCommandData;

    // Check if the request body is valid
    if (!FJsonObjectConverter::JsonObjectStringToUStruct<FThrustCommandData>(BodyStrData, &ThrustCommandData))
    {
        // Generate a bad request response
        FString JsonResponse = "{\"message\": \"The provided request body could not be parsed\"}";
	    TUniquePtr<FHttpServerResponse> Response = FHttpServerResponse::Create(JsonResponse, TEXT("application/json"));
        Response->Code = EHttpServerResponseCodes::Forbidden;

	    OnComplete(MoveTemp(Response));
        return true;
    }

    // Check if the date is valid
    FDateTime ParsedTime;
    if (!FDateTime::ParseIso8601(*ThrustCommandData.ExecutionTime, ParsedTime))
    {
        // Generate a bad request response
        FString JsonResponse = "{\"message\": \"The provided date is not in the correct format, yyyy-mm-ddT14:10:00.000Z\"}";
	    TUniquePtr<FHttpServerResponse> Response = FHttpServerResponse::Create(JsonResponse, TEXT("application/json"));
        Response->Code = EHttpServerResponseCodes::Forbidden;

	    OnComplete(MoveTemp(Response));
        return true;
    }

    // Add the command locally and send it to the server
    if (!HasAuthority())
    {
        UCPP_ThrustCommand* ThrustCommand = NewObject<UCPP_ThrustCommand>();
        ThrustCommand->DeserializeFromStruct(ThrustCommandData);
        GroundStationManager->SatelliteCommandManager->StorePendingSatelliteCommand(SatelliteID, ThrustCommand);
    }
    GroundStationManager->SatelliteCommandManager->ServerSatelliteThrustCommand(SatelliteID, ThrustCommandData);

    // Generate the http success response
    FString JsonResponse;
    FJsonObjectConverter::UStructToJsonObjectString<FThrustCommandData>(ThrustCommandData, JsonResponse);
	TUniquePtr<FHttpServerResponse> Response = FHttpServerResponse::Create(JsonResponse, TEXT("application/json"));
    Response->Code = EHttpServerResponseCodes::Created;

	OnComplete(MoveTemp(Response));
	return true;
}

void ACPP_HttpServer::RequestPrint(const FHttpServerRequest& Request, bool PrintBody)
{
    FString StrRequestType;
	switch (Request.Verb)
	{
	case EHttpServerRequestVerbs::VERB_GET:
		StrRequestType = TEXT("GET");
		break;
	case EHttpServerRequestVerbs::VERB_POST:
		StrRequestType = TEXT("POST");
		break;
	default:
		StrRequestType = TEXT("Invalid");
	}
	UE_LOG(LogTemp, Log, TEXT("RequestType = '%s'"), *StrRequestType);

	HttpVersion::EHttpServerHttpVersion httpVersion{ Request.HttpVersion };
	UE_LOG(LogTemp, Log, TEXT("HttpVersion = '%s'"), *HttpVersion::ToString(httpVersion));

	UE_LOG(LogTemp, Log, TEXT("RelativePath = '%s'"), *Request.RelativePath.GetPath());

	for (const auto& header : Request.Headers)
	{
		FString strHeaderVals;
		for (const auto& val : header.Value)
		{
			strHeaderVals += TEXT("'") + val + TEXT("' ");
		}
		UE_LOG(LogTemp, Log, TEXT("Header = '%s' : %s"), *header.Key, *strHeaderVals);
	}

	for (const auto& pathParam : Request.PathParams)
	{
		UE_LOG(LogTemp, Log, TEXT("PathParam = '%s' : '%s'"), *pathParam.Key, *pathParam.Value);
	}

	for (const auto& queryParam : Request.QueryParams)
	{
		UE_LOG(LogTemp, Log, TEXT("QueryParam = '%s' : '%s'"), *queryParam.Key, *queryParam.Value);
	}

	// Convert UTF8 to FString
	FUTF8ToTCHAR bodyTCHARData(reinterpret_cast<const ANSICHAR*>(Request.Body.GetData()), Request.Body.Num());
	FString strBodyData{ bodyTCHARData.Length(), bodyTCHARData.Get() };

	UE_LOG(LogTemp, Log, TEXT("Body = '%s'"), *strBodyData);
}
