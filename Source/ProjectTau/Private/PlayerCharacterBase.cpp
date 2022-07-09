// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacterBase.h"
#include "PlayerCharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogPlayerCharacter, Log, All);

// Sets default values
APlayerCharacterBase::APlayerCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UPlayerCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void APlayerCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APlayerCharacterBase, bIsSprinting, COND_SimulatedOnly);
}

// Called when the game starts or when spawned
void APlayerCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APlayerCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PlayerCharacterMovement = Cast<UPlayerCharacterMovementComponent>(GetMovementComponent());
}

void APlayerCharacterBase::Sprint()
{
	if (PlayerCharacterMovement)
	{
		if (CanSprint())
		{
			PlayerCharacterMovement->bWantsToSprint = true;
		}
	#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		else if (!PlayerCharacterMovement->CanEverSprint())
		{
			UE_LOG(LogPlayerCharacter, Log, TEXT("%s is trying to sprint, but crouching is disabled on this character! (check Player Character Movement)"), *GetName());
		}
	#endif
	}
}

void APlayerCharacterBase::UnSprint()
{
	if (PlayerCharacterMovement) 
	{
		PlayerCharacterMovement->bWantsToSprint = false;
	}
}

bool APlayerCharacterBase::CanSprint() const
{
	return !PlayerCharacterMovement->bWantsToSprint && PlayerCharacterMovement && PlayerCharacterMovement->CanEverSprint() && GetRootComponent() && !GetRootComponent()->IsSimulatingPhysics();
}

void APlayerCharacterBase::OnRep_IsSprinting()
{
	if (PlayerCharacterMovement)
	{
		if (bIsSprinting)
		{
			PlayerCharacterMovement->bWantsToSprint = true;
		}
		else
		{
			PlayerCharacterMovement->bWantsToSprint = false;
		}
		PlayerCharacterMovement->bNetworkUpdateReceived = true;
	}
}

bool APlayerCharacterBase::CanJumpInternal_Implementation() const
{
	return Super::CanJumpInternal_Implementation() && !bIsSprinting;
}


