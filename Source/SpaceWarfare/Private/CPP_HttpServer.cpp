// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_HttpServer.h"

#include "HttpPath.h"
#include "IHttpRouter.h"
#include "HttpServerHttpVersion.h"
#include "HttpServerModule.h"
#include "HttpServerResponse.h"

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
	// This check always true
	// I don't no why...
	if (HttpRouter.IsValid())
	{
		// Bind as many routes as you need
		HttpRouter->BindRoute(FHttpPath(HttpPathGET), EHttpServerRequestVerbs::VERB_GET,
			[this](const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete) { return RequestGET(Request, OnComplete); });
		HttpRouter->BindRoute(FHttpPath(HttpPathPOST), EHttpServerRequestVerbs::VERB_POST,
			[this](const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete) { return RequestPOST(Request, OnComplete); });
		HttpRouter->BindRoute(FHttpPath(HttpPathPUT), EHttpServerRequestVerbs::VERB_PUT,
			[this](const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete) { return RequestPUT(Request, OnComplete); });

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

bool ACPP_HttpServer::RequestGET(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
{
    RequestPrint(Request);
    // NOTE: Use application/json content type later
	TUniquePtr<FHttpServerResponse> Response = FHttpServerResponse::Create(TEXT("HttpServerExample GET"), TEXT("text/html"));
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

bool ACPP_HttpServer::RequestPUT(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
{
    RequestPrint(Request);
    // NOTE: Use application/json content type later
	TUniquePtr<FHttpServerResponse> response = FHttpServerResponse::Create(TEXT("HttpServerExample PUT"), TEXT("text/html"));
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
	case EHttpServerRequestVerbs::VERB_PUT:
		StrRequestType = TEXT("PUT");
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