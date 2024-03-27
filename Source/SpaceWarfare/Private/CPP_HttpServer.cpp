// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_HttpServer.h"

#include "HttpPath.h"
#include "IHttpRouter.h"
#include "HttpServerHttpVersion.h"
#include "HttpServerModule.h"
#include "HttpServerResponse.h"
#include "JsonUtilities.h"
#include "Kismet/KismetMathLibrary.h"

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
}

void ACPP_HttpServer::StartServer()
{
	FHttpServerModule& HttpServerModule = FHttpServerModule::Get();
	TSharedPtr<IHttpRouter> HttpRouter = HttpServerModule.GetHttpRouter(ServerPort);

	// If port already binded by another process, then this check must be failed
	// !!! BUT !!!
	// This check is always true
	// I don't no why...
	if (HttpRouter.IsValid())
	{
		// Bind as many routes as you need
		HttpRouter->BindRoute(FHttpPath(SatelliteListPath), EHttpServerRequestVerbs::VERB_GET,
			[this](const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete) { return GetSatelliteList(Request, OnComplete); });
		HttpRouter->BindRoute(FHttpPath(HttpPathPOST), EHttpServerRequestVerbs::VERB_POST,
			[this](const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete) { return RequestPOST(Request, OnComplete); });

		HttpServerModule.StartAllListeners();

		UE_LOG(LogTemp, Log, TEXT("Web server started on port = %d"), ServerPort);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Could not start web server on port = %d"), ServerPort);
	}
}

void ACPP_HttpServer::StopServer()
{
    FHttpServerModule& httpServerModule = FHttpServerModule::Get();
	httpServerModule.StopAllListeners();
}

bool ACPP_HttpServer::GetSatelliteList(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
{
    RequestPrint(Request);

    FSatellitesResponse Satellites;
    for (int i = 0; i < 5; i++)
    {
        FSatelliteResponse Satellite;
        Satellite.OwnerID = UKismetMathLibrary::RandomIntegerInRange(0, 3);
        Satellite.Label = "ISS Test Satellite Name";
        Satellite.Position = FVector(
            UKismetMathLibrary::RandomFloatInRange(6500.0f, 20000.0f), 
            UKismetMathLibrary::RandomFloatInRange(6500.0f, 20000.0f), 
            UKismetMathLibrary::RandomFloatInRange(6500.0f, 20000.0f)
        );
        Satellite.Rotation = FRotator(
            UKismetMathLibrary::RandomFloatInRange(0.0f, 360.0f), 
            UKismetMathLibrary::RandomFloatInRange(0.0f, 360.0f), 
            UKismetMathLibrary::RandomFloatInRange(0.0f, 360.0f)
        );
        Satellite.Velocity = FVector(
            UKismetMathLibrary::RandomFloatInRange(0.0f, 200.0f), 
            UKismetMathLibrary::RandomFloatInRange(0.0f, 200.0f), 
            UKismetMathLibrary::RandomFloatInRange(0.0f, 200.0f)
        );

        Satellites.Satellites.Add(Satellite);
    }

    FString JsonSatellites;
    FJsonObjectConverter::UStructToJsonObjectString<FSatellitesResponse>(Satellites, JsonSatellites);

	TUniquePtr<FHttpServerResponse> Response = FHttpServerResponse::Create(JsonSatellites, TEXT("application/json"));
    
	OnComplete(MoveTemp(Response));
	return true;
}

bool ACPP_HttpServer::RequestPOST(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
{
    RequestPrint(Request);
    // NOTE: Use application/json content type later
	TUniquePtr<FHttpServerResponse> response = FHttpServerResponse::Create(TEXT("HttpServerExample POST"), TEXT("text/html"));
	OnComplete(MoveTemp(response));
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
