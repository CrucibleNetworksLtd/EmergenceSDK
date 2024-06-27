// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/NamedSlot.h"
#include "ErrorCodeFunctionLibrary.h"
#include "AvatarService/AvatarByOwner.h"
#include "EmergenceUI.generated.h"

UENUM(BlueprintType)
enum class  EmergenceConfirmationType : uint8
{
	Normal     UMETA(ToolTip = "For normal 'are you sure' type questions"),
	Error      UMETA(ToolTip = "To be implemented as bright red, for when you really need the users attention")
};

UENUM(BlueprintType)
enum class EmergenceLoginType : uint8
{
	WalletConnect     UMETA(ToolTip = "A normal wallet connect flow."),
	Futureverse      UMETA(ToolTip = "Login with the Futureverse version of WalletConnect. This requires the user to make a Futurepass once they connect if they don't already have one linked to that address. Useful if you're making a game with Futureverse.")
};

UCLASS(HideDropdown)
class EMERGENCE_API UEmergenceUI : public UUserWidget
{
	GENERATED_BODY()
private:
	TMap<FName, FText> LoadingMessages;

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScreenSwitched, UUserWidget*, NewScreen);

	UFUNCTION(BlueprintPure)
	EmergenceLoginType GetProjectLoginType();

	template<typename T>
	inline static T StringToEnum(const FString& Name) {
		UEnum* EnumClass = StaticEnum<T>();
		if (!EnumClass) {
			UE_LOG(LogTemp, Fatal, TEXT("StringToEnum Enum not found: %s"), *Name);
		}
		return (T)EnumClass->GetIndexByName(FName(*Name), EGetByNameFlags::ErrorIfNotFound);
	}

	//Called whenever the screen is changed
	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers|Emergence UI")
	FOnScreenSwitched OnScreenSwitched;

	UFUNCTION(BlueprintCallable, Category = "Emergence Internal|UI")
	void SetUserHasLoggedInBefore(bool HasLoggedInBefore);

	UFUNCTION(BlueprintPure, Category = "Emergence Internal|UI")
	bool GetUserHasLoggedInBefore();

	//Switches the active screen to a new widget object
	UFUNCTION(BlueprintCallable, Category = "Emergence Internal|UI")
	void SwitchCurrentScreen(UUserWidget* NewScreen);

	//Switches the active screen to a new widget class
	UFUNCTION(BlueprintCallable, Category = "Emergence Internal|UI")
	void SwitchCurrentScreenByClass(TSubclassOf<UUserWidget> NewScreenClass);

	//The current screen slot sub-component. This is a C++ "BindWidget"'d widget, DO NOT RENAME IN UMG BLUEPRINTS!
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget), Category = "C++ Bind Widgets")
	UNamedSlot* CurrentScreenSlotBoundWidget;

	//Adds a message with the given Id to the LoadingMessages table
	UFUNCTION(BlueprintCallable, Category = "Emergence Internal|UI")
	void ShowLoadingMessage(FName MessageId, FText Reason);

	//Removes the message with the given Id from the LoadingMessages table
	UFUNCTION(BlueprintCallable, Category = "Emergence Internal|UI")
	void HideLoadingMessage(FName MessageId);

	//Gets the most recent loading message
	UFUNCTION(BlueprintPure, Category = "Emergence Internal|UI")
	bool GetMostRecentLoadingMessage(FText& Message);

	//Shows an error code for the given error
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Emergence Internal|UI")
	void ShowServerErrorMessage(const EErrorCode& ErrorCode);

	//Closes the EmergenceUI.
	UFUNCTION(BlueprintCallable, Category = "Emergence|Overlay")
	void Close();
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClosed);

	//Called when the UI is closed.
	UPROPERTY(BlueprintAssignable, Category = "Emergence|Overlay")
	FOnClosed Closed;

	//Called when the UI reaches the home screen.
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Emergence Internal|UI")
	FOnClosed OpeningFinished;

	//Gets the default avatar icon from the plugin settings
	UFUNCTION(BlueprintPure, Category = "Emergence Internal|UI")
	static TSoftObjectPtr<UTexture2D> GetDefaultAvatarIcon();
};
