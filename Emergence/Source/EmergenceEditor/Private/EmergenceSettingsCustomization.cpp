// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#include "EmergenceSettingsCustomization.h"
#include "EmergencePluginSettings.h"
#include "PropertyEditing.h"
#include "Widgets/Text/SRichTextBlock.h"
#include "Styling/SlateColor.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "MyEditorModule"

TSharedRef< IDetailCustomization > FEmergenceSettingsCustomization::MakeInstance()
{
    return MakeShareable(new FEmergenceSettingsCustomization);
}

void FEmergenceSettingsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
    IDetailCategoryBuilder& APKPackagingCategory = DetailBuilder.EditCategory(TEXT("Chain"));
	APKPackagingCategory.AddCustomRow(LOCTEXT("UpgradeInfo", "Upgrade Info"), false)
		.WholeRowWidget
		[
			SNew(SBorder)
			.Padding(1)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.Padding(FMargin(10, 10, 10, 10))
				.FillWidth(1.0f)
				[
					SNew(SRichTextBlock)
					.Text(LOCTEXT("ChainText", "<RichTextBlock.TextHighlight>Note on chains selected here</>: This selection only set the chain used by the WalletConnect wallet, e.g. the wallet used by the overlay. It doesn't set the chains used by local wallets, e.g. the system you may use if you are trying to implement a server-side wallet.\n\n<RichTextBlock.TextHighlight>Also note</>: The preset Node URLs provided by Crucible are pretty much just for testing as they only have a limited amount of requests per day, and these are shared by everyone using Emergence. You probably shouldn't be using it in shipping."))
					.TextStyle(FEditorStyle::Get(), "MessageLog")
					.DecoratorStyleSet(&FEditorStyle::Get())
					.AutoWrapText(true)
				]
			]
		];
}

#undef LOCTEXT_NAMESPACE