// Fill out your copyright notice in the Description page of Project Settings.


#include "MisClases/Widget/LanWidget.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Kismet/GameplayStatics.h"

void ULanWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	Button_Hostear->OnClicked.AddDynamic(this, &ULanWidget::OnButtonHostearClicked);
	Button_Unirse->OnClicked.AddDynamic(this, &ULanWidget::OnButtonUnirseClicked);
	
	FInputModeUIOnly InputMode;
	
	GetOwningPlayer()->SetInputMode(InputMode);
	GetOwningPlayer()->SetShowMouseCursor(true);
}

void ULanWidget::OnButtonHostearClicked()
{
	FInputModeGameOnly InputMode;
	
	GetOwningPlayer()->SetInputMode(InputMode);
	GetOwningPlayer()->SetShowMouseCursor(false);
	
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, Level, true, TEXT("Listen"));
	
}

void ULanWidget::OnButtonUnirseClicked()
{
	FInputModeGameOnly InputMode;
	
	GetOwningPlayer()->SetInputMode(InputMode);
	GetOwningPlayer()->SetShowMouseCursor(false);
	
	
	const FString IP = EditableTextBox_DireccionIP->GetText().ToString();
	UGameplayStatics::OpenLevel(this,*IP);
}
