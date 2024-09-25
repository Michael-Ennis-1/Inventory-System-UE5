// Fill out your copyright notice in the Description page of Project Settings.
#define COLLISION_INTERACTIVE ECC_GameTraceChannel1

#include "cInventorySystem.h"
#include "cItemComponent.h"
#include "ContentBrowserDataSource.h"
#include "GraphAStar.h"
#include "K2Node_InputActionEvent.h"
#include "MeshPassProcessor.h"
#include "Algo/ForEach.h"
#include "GameFramework/GameSession.h"

//-----Standard Functions-----\\

// Sets default values for this component's properties
UcInventorySystem::UcInventorySystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UcInventorySystem::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UcInventorySystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}



//-----Custom Functions-----\\

// Grabs the Player's Camera
void UcInventorySystem::cGetPlayerCameraManager(APlayerCameraManager* &CameraManager)
{
	CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
}

// Creates a Trace Line from the Forward Vector of the Camera
void UcInventorySystem::cCameraLineTrace(const float interactionRange, FVector3f &startPoint, FVector3f &endPoint)
{
	// Gets the Camera that the Player is using
	APlayerCameraManager* PlayerCameraManager;
	cGetPlayerCameraManager(PlayerCameraManager);

	// Sets the Start Point to the location of Camera
	startPoint = FVector3f(PlayerCameraManager->GetCameraLocation());

	// Sets the End Point to the forward vector of the Camera, multiplied by the "Interaction Range", adding the Start Point
	endPoint = FVector3f(PlayerCameraManager->GetActorForwardVector());
	endPoint *= interactionRange;
	endPoint += startPoint;
}

// Performs a Sphere RayTrace based on User Parameters
void UcInventorySystem::cCalculateSphereTrace(const FVector3f startPos, const FVector3f endPos, const float sphereRadius, const bool debug, FHitResult &hitResult, bool &hitSomething)
{
	// Ignores the Actor attempting the Trace
	TArray<AActor*> actorsToIgnore;
	actorsToIgnore.Add(GetOwner());

	// Attempts a Sphere Trace along a given line, showing Debug Spheres if required
	if (debug)
	{
		hitSomething = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), FVector(startPos), FVector(endPos), sphereRadius,
													   UEngineTypes::ConvertToTraceType(COLLISION_INTERACTIVE),
													   false, actorsToIgnore,
													   EDrawDebugTrace::ForDuration, hitResult, true,
													   FLinearColor::Red, FLinearColor::Blue, 0);
	}
	else
	{
		hitSomething = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), FVector(startPos), FVector(endPos), sphereRadius,
											   UEngineTypes::ConvertToTraceType(COLLISION_INTERACTIVE),
											   false, actorsToIgnore,
											   EDrawDebugTrace::None, hitResult, true,
											   FLinearColor::Red, FLinearColor::Blue, 0);
	}
	
}

// Sets the Actor currently being Hit by a Raycast 
void UcInventorySystem::cSetHitActor(const FHitResult hitResult, const bool hitSomething, bool &continueCode, FString &name)
{
	
	// If the Raycast was successful, and not looking at the same item, sends a message to the screen
	if (hitSomething)
	{
		if (hitResult.GetActor() != cLookatActor)
		{
			cLookatActor = hitResult.GetActor();
		}
		if (cLookatActor->FindComponentByClass<UcItemComponent>())
		{
			name = cLookatActor->FindComponentByClass<UcItemComponent>()->cName;
		}
	}
	else
	{
		// Resets the previously viewed Actor, so that it can be viewed again
		cLookatActor = NULL;
	}
}

// Checks whether to call the "InteractWith" function of the item currently being focused on
void UcInventorySystem::cCheckInteraction()
{
	// If both cLookatActor and it's ItemComponent exist, then call the "InteractWith" function within the ItemComponent
	if(cLookatActor)
	{
		if(cLookatActor->FindComponentByClass<UcItemComponent>())
		{
			cLookatActor->FindComponentByClass<UcItemComponent>()->InteractWith();
		}
	}
}

// Checks whether a slot containing this item already exists
void UcInventorySystem::cFindSlot(const FString itemID, const int maxStackSize, int &index, bool &foundSlot)
{
	int _index = 0;

	// Searchs all FSlots in cContents struct
	for (auto f : cContents)
	{
		// If the slot's item ID aligns and the max stack size for this slot is not reached
		if (itemID == f.ItemID && f.Quantity < maxStackSize)
		{
			// Return the correct slot index and returns true
			index = _index;
			foundSlot = true;
			return;
		}
		_index++;
	}

	// If no slot is found with this particular item, return false
	index = -1;
	foundSlot = false;
}

// Adds to a currently existing stack
void UcInventorySystem::cAddtoStack(const int index, const int quantity)
{
	cContents[index].Quantity += quantity;
}

// Checks if any slots are empty
void UcInventorySystem::cAnyEmptySlotsAvailabe(int &index, bool &hasEmptySlot)
{
    int _index = 0;

	// Searchs all FSlots in cContents struct
	for (auto f : cContents)
	{
		// If the current slot has no items, returns true and returns the index of the empty slot
		if (f.Quantity == 0)
		{
			index = _index;
			hasEmptySlot = true;
			return;
		}
		_index++;
	}

	// If no empty slot found, return false
	hasEmptySlot = false;
	index = -1;
}

// Sets a slot to this itemID, if there is an empty slot available
void UcInventorySystem::cCreateNewStack(const FString itemID, const int quantity, bool& Success)
{
	int _index;
	bool _hasEmptySlot;

	// Sends _index and _hasEmptySlot variables as Reference, to be set then returned after the function is complete
	cAnyEmptySlotsAvailabe(_index, _hasEmptySlot);

	// Checks if an empty slot exists
	if (_hasEmptySlot)
	{
		// Sets the first empty slot found to this Item's ID and its quantity, then returns True
		cContents[_index].ItemID = itemID;
		cContents[_index].Quantity = quantity;
		Success = true;
		return;
	}

	// If no emtpy slot exists, return False
	Success = false;
	return;
	
}

// Allows for the user to move items from one slot to another
void UcInventorySystem::cTransferSlots(const int maxStackSize, const int sourceIndex, const int destinationIndex, UcInventorySystem *sourceComponent)
{
	// Saved a local slot from the referenced inventory system
	FSlot localSlot = sourceComponent->cContents[sourceIndex];

	// Continues if the Destination Index does not equal -1, as that means an error has occured
	if (!(destinationIndex < 0))
	{
		// If the slot destination does not contain this item already
		if (!(localSlot.ItemID == cContents[destinationIndex].ItemID))
		{
			// Swaps the slot that the player dragged from with the slot that the player dragged to
			sourceComponent->cContents[sourceIndex] = cContents[destinationIndex];

			// Swaps the slot that the player dragged to with the slot that the player dragged from
			cContents[destinationIndex] = localSlot;
		}
		else
		{
			// Adds the quantities from the Target slot and the Previous slot, subtracts the stack size and clamps to between a minimum of 0 and max of stack limit
			// Means that only reasonable quantities are added to a slot
			int temp1 = FMath::Clamp((cContents[destinationIndex].Quantity + localSlot.Quantity) - maxStackSize, 0, maxStackSize);
			FString tempString; 

			// Makes sure that there is something in the slot, otherwise it is empty
			if (temp1 > 0)
			{
				tempString = localSlot.ItemID;
			}

			// Adds the old slot to the new slot, only if it doesn't go over the maximum quantity. Otherwise, adds any spillover back to the old slot
			sourceComponent->cContents[sourceIndex].ItemID = tempString;
			sourceComponent->cContents[sourceIndex].Quantity = temp1;
			
			cContents[destinationIndex].ItemID = localSlot.ItemID;
			cContents[destinationIndex].Quantity = FMath::Clamp(cContents[destinationIndex].Quantity + localSlot.Quantity, 0, maxStackSize);
		}
	}
}



// Attempts to add new items to the inventory system
void UcInventorySystem::cAddToInventory(const FString itemID, const int quantity, const int maxStackSize, bool &success, int &quantityRemaining)
{
	// While loop variables
	int remaining = quantity;
    bool hasFailed = false;

	// Other local variables
	int foundSlotIndex, emptySlotIndex;
	bool foundSlot, emptySlot, newStack;

	// Attempts to add items to the inventory, until all items have been added or no more space
	while (remaining > 0 && !hasFailed)
	{
		
		// Attempts to find a slot with the item currently inside
		cFindSlot(itemID, maxStackSize, foundSlotIndex, foundSlot);
		if (foundSlot)
		{
			cAddtoStack(foundSlotIndex, 1);
			remaining--;
		}
		else
		{
			// Attempts to find an empty slot in the inventory system
			cAnyEmptySlotsAvailabe(emptySlotIndex, emptySlot);
			if (emptySlot)
			{

				// If found, attempts to create a new stack
				cCreateNewStack(itemID, 1, newStack);
				if (newStack)
				{
					remaining--;
				}
				else
				{
					hasFailed = true;
				}
			}
			else
			{
				// If no empty slots are available in the inventory system, the function returns failed
				hasFailed = true;
			}
		}
	}

	// If the function succeeded, remaining = 0 and has failed = false
	quantityRemaining = remaining;
	success = !hasFailed;
}

// Attempts to remove an item from the inventory system
void UcInventorySystem::cRemoveFromInventory(const bool removeWholeStack, const int index, FString &localID, int &localQuantity, bool &stackIsEmpty)
{
	// Saves the Quantity and ID for later use
	localQuantity = cContents[index].Quantity;
	localID = cContents[index].ItemID;

	// Checks whether to wipe the slot of items or not
	if (removeWholeStack || localQuantity == 1)
	{
		// Wipes the whole slot empty
		cContents[index].ItemID = "";
		cContents[index].Quantity = 0;

		// Returns true from the function
		stackIsEmpty = true;
	}
	else
	{
		// Lowers the quantity of the specific slot by 1
		cContents[index].Quantity--;

		// Returns false from the function
		stackIsEmpty = false;
	}
}






