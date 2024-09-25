 // Fill out your copyright notice in the Description page of Project Settings.

// UPARAM(ref) to take something as a reference, inside functions
// Use "int &myInt" to return something, use "Const int" to input something

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/Vector.h"
#include "cInventorySystem.generated.h"

USTRUCT(Blueprintable)
struct FSlot
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString ItemID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int Quantity;
		
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class INVENTORYSYSTEMNEW_API UcInventorySystem : public UActorComponent
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Sets default values for this component's properties
	UcInventorySystem();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
	//---Variables---\\
	
	// Sets the desired amount of slots for the Player's Inventory
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int cInventorySize = 32;

	// Current actor being viewed
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* cLookatActor = NULL;

	// Current Contents stored by the Player
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSlot> cContents;

	
    //---Custom Functions---\\
	
	// Finds the Player's Camera 
	UFUNCTION(BlueprintCallable, Category="C++ Functions")
	void cGetPlayerCameraManager(APlayerCameraManager* &CameraManager);

	// Calculates Line in front of Camera
	UFUNCTION(BlueprintCallable, Category="C++ Functions")
	void cCameraLineTrace(const float interactionRange, FVector3f &startPoint, FVector3f &endPoint);

	// Traces a Sphere across a given Vector
	UFUNCTION(BlueprintCallable, Category="C++ Functions")
	void cCalculateSphereTrace(const FVector3f startPos, const FVector3f endPos, const float sphereRadius, const bool debug, FHitResult &hitResult, bool &hitSomething);

	// Sets a Hit Actor once, to allow for a text popup
	UFUNCTION(BlueprintCallable, Category="C++ Functions")
	void cSetHitActor(const FHitResult hitResult, const bool hitSomething, bool &continueCode, FString &name);

	// Checks whether to call the "InteractWith" function of the item currently being focused on
	UFUNCTION(BlueprintCallable, Category="C++ Functions")
	void cCheckInteraction();

	// Checks whether a slot containing this item already exists
	UFUNCTION(BlueprintCallable, Category="C++ Functions")
	void cFindSlot(const FString itemID, const int maxStackSize, int &index, bool &foundSlot);

	// Adds to a currently existing stack
	UFUNCTION(BlueprintCallable, Category="C++ Functions")
	void cAddtoStack(const int index, const int quantity);

	// Checks if any slots are empty
	UFUNCTION(BlueprintCallable, Category="C++ Functions")
	void cAnyEmptySlotsAvailabe(int &index, bool &hasEmptySlot);

	// Sets a slot to this itemID, if there is an empty slot available
	UFUNCTION(BlueprintCallable, Category="C++ Functions")
	void cCreateNewStack(const FString itemID, const int quantity, bool &Success);

	// Allows for the user to move items from one slot to another
	UFUNCTION(BlueprintCallable, Category="C++ Functions")
	void cTransferSlots(const int maxStackSize, const int sourceIndex, const int destinationIndex, UcInventorySystem *sourceComponent);
	
	// Attempts to add more items to the inventory system
	UFUNCTION(BlueprintCallable, Category="C++ Functions")
	void cAddToInventory(const FString itemID, const int quantity, const int maxStackSize, bool &success, int &quantityRemaining);

	// Attempts to remove an item from the inventory system
	UFUNCTION(BlueprintCallable, Category="C++ Functions")
    void cRemoveFromInventory(const bool removeWholeStack, const int index, FString &localID, int &localQuantity, bool &stackIsEmpty);
};
