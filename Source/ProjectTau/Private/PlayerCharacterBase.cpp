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

void APlayerCharacterBase::SetSprinting(bool Sprint)
{
	bIsSprinting = Sprint;
	PlayerCharacterMovement->bWantsToSprint = Sprint;

	if (GetLocalRole() < ROLE_Authority)
	{
		ServerSetSprinting(Sprint);
	}
}

void APlayerCharacterBase::ServerSetSprinting_Implementation(bool Sprint)
{
	SetSprinting(Sprint);
}

bool APlayerCharacterBase::ServerSetSprinting_Validate(bool Sprint)
{
	return true;
}

void APlayerCharacterBase::Sprint()
{
	if(CanSprint()) 
	{
		SetSprinting(true);
	}
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	else if (!PlayerCharacterMovement->CanEverCrouch())
	{
		UE_LOG(LogPlayerCharacter, Log, TEXT("%s is trying to sprint, but sprinting is disabled on this character! (check PlayerCharacterMovement)"), *GetName());
	}
#endif
}

void APlayerCharacterBase::UnSprint()
{
	SetSprinting(false);
}

bool APlayerCharacterBase::CanSprint() const
{
	return !PlayerCharacterMovement->bWantsToSprint && PlayerCharacterMovement && PlayerCharacterMovement->CanEverSprint() && GetRootComponent() && !GetRootComponent()->IsSimulatingPhysics();
}

bool APlayerCharacterBase::CanJumpInternal_Implementation() const
{
	return Super::CanJumpInternal_Implementation() && !bIsSprinting;
}


