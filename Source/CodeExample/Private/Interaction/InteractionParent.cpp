// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/InteractionParent.h"

// Sets default values
AInteractionParent::AInteractionParent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Base setup for interaction  actor 
	_RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));

	RootComponent = _RootComponent;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));

	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Game/LevelPrototyping/Meshes/SM_ChamferCube.SM_ChamferCube'"));

	Mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	UStaticMesh* Asset = MeshAsset.Object;

	Mesh->SetStaticMesh(Asset);

	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Interaction Widget"));
	
	InteractionWidget->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	InteractionWidget->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	// Sets interaction Sphere default values
	triggerSphere = CreateDefaultSubobject<USphereComponent>("InteractSphereTrigger");
	triggerSphere->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	triggerSphere->SetSphereRadius(200.0f);
	triggerSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	triggerSphere->OnComponentBeginOverlap.AddDynamic(this, &AInteractionParent::OnOverlapBegin);
	triggerSphere->OnComponentEndOverlap.AddDynamic(this, &AInteractionParent::OnOverlapEnd);

}

// Called when the game starts or when spawned
void AInteractionParent::BeginPlay()
{
	Super::BeginPlay();
	/* on begin play set interaction to false */
	InteractionWidget->SetVisibility(false);
	
}

// Called every frame
void AInteractionParent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// call to show interaction widget
void AInteractionParent::ShowInteractionWidget()
{
	InteractionWidget->SetVisibility(true);
}

// call to hide interaction widget
void AInteractionParent::HideInteractionWidget()
{
	InteractionWidget->SetVisibility(false);
}

AInteractionParent* AInteractionParent::AddInteraction(AInteractionParent* InteractableObject)
{
	return  InteractableObject;
}

AInteractionParent* AInteractionParent::RemoveInteraction(AInteractionParent* InteractableObject)
{
	return  InteractableObject;
}


void AInteractionParent::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Interface = Cast<IInteractionInterface>(OtherActor);
	{
		Interface->AddInteraction(this);
	};

}

void AInteractionParent::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Interface = Cast<IInteractionInterface>(OtherActor);
	{
		Interface->RemoveInteraction(this);
	};
}
