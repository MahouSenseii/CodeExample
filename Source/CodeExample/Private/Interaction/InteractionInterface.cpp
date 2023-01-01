// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/InteractionInterface.h"

// Add default functionality here for any IInteractionInterface functions that are not pure virtual.

AInteractionParent* IInteractionInterface::AddInteraction(AInteractionParent* InteractableObject)
{

	return InteractableObject;
}

AInteractionParent* IInteractionInterface::RemoveInteraction(AInteractionParent* InteractableObject)
{

	return InteractableObject;
}
