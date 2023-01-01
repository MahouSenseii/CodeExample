// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "Interaction/InteractionInterface.h"
#include "InteractionParent.generated.h"

UCLASS()
class CODEEXAMPLE_API AInteractionParent : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractionParent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS|Component")
	USphereComponent* triggerSphere;

	UPROPERTY(EditAnyWhere)
	USceneComponent* _RootComponent;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	AActor* overlappingActor = nullptr;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Destroy On Interaction")
	bool destroy;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnyWhere)
	UWidgetComponent* InteractionWidget;
	void ShowInteractionWidget();
	void HideInteractionWidget();
	virtual AInteractionParent* AddInteraction(AInteractionParent* InteractableObject);
	virtual AInteractionParent* RemoveInteraction(AInteractionParent* InteractableObject);

private:

	IInteractionInterface* Interface = nullptr;
	/*This will get information when overlapped */
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
