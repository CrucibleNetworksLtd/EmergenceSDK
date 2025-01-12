// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "WalletConnectCode.h"

TSharedRef<SWidget> UWalletConnectCode::RebuildWidget() {
	Singleton = UEmergenceSingleton::GetEmergenceManager(this->GetOwningPlayer());
	if (Singleton) {
		//start the ticking for the countdown timer. Keep in mind, this is visual ONLY and doesn't affact the requests themselves
		this->GetOwningPlayer()->GetWorld()->GetTimerManager().SetTimer(TimeRemainingTimerHandle, this, &UWalletConnectCode::UpdateTimeRemaining, 1.0F, true, 1.0F);
		if (ActivateOnConstruct) {
			this->StartAttempt();
		}
	}
	return Super::RebuildWidget();
}

void UWalletConnectCode::QRCodeCompleted(UTexture2D* _Icon, FString _WalletConnectString, EErrorCode _StatusCode) {
	Singleton->OnGetQRCodeCompleted.RemoveDynamic(this, &UWalletConnectCode::QRCodeCompleted);
	if (_StatusCode == EErrorCode::EmergenceOk) {
		this->SetBrushFromTexture(_Icon, false);
		this->WalletConnectString = _WalletConnectString;
		Singleton->GetHandshake(this->ConnectionRefreshTime);
	}
	else {
		this->OnSignInFailure.Broadcast(EEmergenceWalletConnectStepError::QRCodeFail);
		this->CancelAllAndRestartAttempt();
	}
}

void UWalletConnectCode::AccessTokenCompleted(EErrorCode StatusCode) {
	Singleton->OnLoginFinished.RemoveDynamic(this, &UWalletConnectCode::AccessTokenCompleted);
	if (StatusCode == EErrorCode::EmergenceOk) { //EVERYTHING IS DONE, CLEANUP AND SAY SUCCESS
		this->GetOwningPlayer()->GetWorld()->GetTimerManager().ClearTimer(TimeRemainingTimerHandle);
		Singleton->OnGetHandshakeCompleted.RemoveDynamic(this, &UWalletConnectCode::GetHandshakeCompleted);
		Singleton->OnGetQRCodeCompleted.RemoveDynamic(this, &UWalletConnectCode::QRCodeCompleted);
		Singleton->OnReinitializeWalletConnectCompleted.RemoveDynamic(this, &UWalletConnectCode::ReinitializeWalletConnectCompleted);
		this->OnSignInSuccess.Broadcast(); //call sign in success
	}
	else {
		this->OnSignInFailure.Broadcast(EEmergenceWalletConnectStepError::AccessTokenFail);
		this->CancelAllAndRestartAttempt();
	}
}

void UWalletConnectCode::CancelAllAndRestartAttempt() {
	//Singleton->OnReinitializeWalletConnectCompleted.AddDynamic(this, &UWalletConnectCode::ReinitializeWalletConnectCompleted);
	Singleton->CancelSignInRequest();
	//Singleton->ReinitializeWalletConnect();
	this->StartAttempt();
}

void UWalletConnectCode::GetHandshakeCompleted(FString Address, EErrorCode StatusCode)
{
	Singleton->OnGetHandshakeCompleted.RemoveDynamic(this, &UWalletConnectCode::GetHandshakeCompleted);
	if (StatusCode != EErrorCode::EmergenceOk) {
		this->OnSignInFailure.Broadcast(EEmergenceWalletConnectStepError::HandshakeFail);
		this->CancelAllAndRestartAttempt();
	}
}

void UWalletConnectCode::CancelAll()
{
	Singleton->CancelSignInRequest();
	this->GetOwningPlayer()->GetWorld()->GetTimerManager().ClearTimer(TimeRemainingTimerHandle);
}

void UWalletConnectCode::StartAll()
{
	Singleton = UEmergenceSingleton::GetEmergenceManager(this->GetOwningPlayer());
	if (Singleton) {
		this->GetOwningPlayer()->GetWorld()->GetTimerManager().SetTimer(TimeRemainingTimerHandle, this, &UWalletConnectCode::UpdateTimeRemaining, 1.0F, true, 1.0F);
		this->StartAttempt();
	}
}

void UWalletConnectCode::StartAttempt()
{
	this->TimeRemaining = ConnectionRefreshTime;

	Singleton->OnGetQRCodeCompleted.RemoveDynamic(this, &UWalletConnectCode::QRCodeCompleted);
	Singleton->OnLoginFinished.RemoveDynamic(this, &UWalletConnectCode::AccessTokenCompleted);
	Singleton->OnGetHandshakeCompleted.RemoveDynamic(this, &UWalletConnectCode::GetHandshakeCompleted);

	Singleton->OnGetQRCodeCompleted.AddDynamic(this, &UWalletConnectCode::QRCodeCompleted);
	Singleton->GetQRCode();
	Singleton->OnLoginFinished.AddDynamic(this, &UWalletConnectCode::AccessTokenCompleted);	
	Singleton->OnGetHandshakeCompleted.AddDynamic(this, &UWalletConnectCode::GetHandshakeCompleted);
}

void UWalletConnectCode::UpdateTimeRemaining()
{
	if (this->TimeRemaining > 0) {
		this->TimeRemaining--;
	}
}

void UWalletConnectCode::ReinitializeWalletConnectCompleted(EErrorCode StatusCode) {
	Singleton->OnReinitializeWalletConnectCompleted.RemoveDynamic(this, &UWalletConnectCode::ReinitializeWalletConnectCompleted);
	if (StatusCode == EErrorCode::EmergenceOk) {
		this->StartAttempt();
	}
}