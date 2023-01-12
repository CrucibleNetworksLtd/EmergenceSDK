// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "AssetTypeCategories.h"
/**
 * 
 */
class EMERGENCEEDITOR_API FEmergenceDeploymentAssetTypeActions : public FAssetTypeActions_Base
{
public:
	FEmergenceDeploymentAssetTypeActions(EAssetTypeCategories::Type Category);
	UClass* GetSupportedClass() const override;
	FText GetName() const override;
	FColor GetTypeColor() const override;
	uint32 GetCategories() override;
	bool CanLocalize() const override;
private:
	EAssetTypeCategories::Type MyAssetCategory;
};