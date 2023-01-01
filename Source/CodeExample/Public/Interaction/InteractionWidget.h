// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionWidget.generated.h"

/**
 * 
 */
UCLASS()
class CODEEXAMPLE_API UInteractionWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UPROPERTY( meta = ( BindWidget )  )
	class UTextBlock* HiTextBlock;

};
