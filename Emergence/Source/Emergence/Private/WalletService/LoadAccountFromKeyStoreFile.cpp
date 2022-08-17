// Copyright Crucible Networks Ltd 2022. All Rights Reserved.


#include "WalletService/LoadAccountFromKeyStoreFile.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"

ULoadAccountFromKeyStoreFile* ULoadAccountFromKeyStoreFile::LoadAccountFromKeyStoreFile(const UObject* WorldContextObject, const FString &Name, const FString &Password, const FString &Path, const FString &NodeURL, const FString& ChainID)
{
	ULoadAccountFromKeyStoreFile* BlueprintNode = NewObject<ULoadAccountFromKeyStoreFile>();
	BlueprintNode->Name = Name;
	BlueprintNode->Password = Password;
	BlueprintNode->Path = Path;
	BlueprintNode->NodeURL = NodeURL;
	BlueprintNode->ChainID = ChainID;
	BlueprintNode->WorldContextObject = WorldContextObject;
	return BlueprintNode;
}

void ULoadAccountFromKeyStoreFile::Activate()
{
	Path = Path.Replace(TEXT(" "), TEXT("%20"));

	auto Emergence = UEmergenceSingleton::GetEmergenceManager(WorldContextObject);
	FString AccessToken = Emergence->GetCurrentAccessToken();

	TSharedPtr<FJsonObject> Json = MakeShareable(new FJsonObject);
	Json->SetStringField("name", this->Name);
	Json->SetStringField("password", this->Password);
	Json->SetStringField("path", this->Path);
	Json->SetStringField("nodeURL", this->NodeURL);
	Json->SetStringField("ChainID", this->ChainID);

	FString OutputString;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(Json.ToSharedRef(), Writer);

	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"Content-Type", "application/json"});
	Headers.Add(TPair<FString, FString>{"Authorization", AccessToken});
	UHttpHelperLibrary::ExecuteHttpRequest<ULoadAccountFromKeyStoreFile>(
		this,
		&ULoadAccountFromKeyStoreFile::LoadAccountFromKeyStoreFile_HttpRequestComplete,
		UHttpHelperLibrary::APIBase + "loadAccount",
		"POST",
		60.0F,
		Headers,
		OutputString);
	UE_LOG(LogEmergenceHttp, Display, TEXT("LoadAccountFromKeyStoreFile request started with JSON, calling LoadAccountFromKeyStoreFile_HttpRequestComplete on request completed. Json sent as part of the request: "));
	UE_LOG(LogEmergenceHttp, Display, TEXT("%s"), *OutputString);
}

void ULoadAccountFromKeyStoreFile::LoadAccountFromKeyStoreFile_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	UE_LOG(LogEmergenceHttp, Display, TEXT("LoadAccountFromKeyStoreFile_HttpRequestComplete: %s"), *HttpResponse->GetContentAsString());
	if (StatusCode == EErrorCode::EmergenceOk) {
		OnLoadAccountFromKeyStoreFileCompleted.Broadcast(HttpResponse->GetContentAsString(), EErrorCode::EmergenceOk);
	}
	else {
		OnLoadAccountFromKeyStoreFileCompleted.Broadcast(FString(), StatusCode);
		UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("LoadAccountFromKeyStoreFile", StatusCode);
	}
}