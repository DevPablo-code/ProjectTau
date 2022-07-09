// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "PlayerCharacterBase.h"

void UPlayerCharacterMovementComponent::PostLoad()
{
	Super::PostLoad();

	PlayerCharacterOwner = Cast<APlayerCharacterBase>(CharacterOwner);
}

void UPlayerCharacterMovementComponent::SetUpdatedComponent(USceneComponent* NewUpdatedComponent)
{
	Super::SetUpdatedComponent(NewUpdatedComponent);

	PlayerCharacterOwner = Cast<APlayerCharacterBase>(CharacterOwner);
}

float UPlayerCharacterMovementComponent::GetMaxSpeed() const
{
	switch (MovementMode)
	{
		case MOVE_Walking:
		case MOVE_NavWalking:
		{
			if (IsCrouching())
			{
				return MaxWalkSpeedCrouched;
			}
			else
			{
				if (bWantsToSprint)
				{
					return SprintSpeed;
				}

				return RunSpeed;
			}
		}
		case MOVE_Falling:
			return RunSpeed;
		case MOVE_Swimming:
			return MaxSwimSpeed;
		case MOVE_Flying:
			return MaxFlySpeed;
		case MOVE_Custom:
			return MaxCustomMovementSpeed;
		case MOVE_None:
		default:
			return 0.f;
	}
}

float UPlayerCharacterMovementComponent::GetMaxAcceleration() const
{
	if (IsMovingOnGround())
	{
		if (bWantsToSprint)
		{
			return SprintAcceleration;
		}

		return RunAcceleration;
	}

	return Super::GetMaxAcceleration();
}

void UPlayerCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);

	if (IsSprinting() && !CanSprintInCurrentState())
	{
		PlayerCharacterOwner->SetSprinting(false);
	}
}

void UPlayerCharacterMovementComponent::UpdateCharacterStateAfterMovement(float DeltaSeconds)
{
	Super::UpdateCharacterStateAfterMovement(DeltaSeconds);

	if (IsSprinting() && !CanSprintInCurrentState())
	{
		PlayerCharacterOwner->SetSprinting(false);
	}
}

FNetworkPredictionData_Client* UPlayerCharacterMovementComponent::GetPredictionData_Client() const
{
	if (ClientPredictionData == nullptr)
	{
		UPlayerCharacterMovementComponent* MutableThis = const_cast<UPlayerCharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_PlayerCharacter(*this);
	}

	return ClientPredictionData;
}

bool UPlayerCharacterMovementComponent::CanSprintInCurrentState() const
{
	if (!CanEverSprint())
	{
		return false;
	}

	return (IsFalling() || IsMovingOnGround()) && UpdatedComponent && !UpdatedComponent->IsSimulatingPhysics() && !bWantsToCrouch && PlayerCharacterOwner->IsMovingForward();
}

bool UPlayerCharacterMovementComponent::IsSprinting() const
{
	return PlayerCharacterOwner && PlayerCharacterOwner->bIsSprinting;
}

void FSavedMove_PlayerCharacter::Clear()
{
	Super::Clear();

	bWantsToSprint = false;
}

uint8 FSavedMove_PlayerCharacter::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	if (bWantsToSprint) 
	{
		Result |= FLAG_Custom_0;
	}

	return Result;
}

bool FSavedMove_PlayerCharacter::CanCombineWith(const FSavedMovePtr& NewMovePtr, ACharacter* Character, float MaxDelta) const
{
	const FSavedMove_PlayerCharacter* NewMove = static_cast<FSavedMove_PlayerCharacter*>(NewMovePtr.Get());

	if (bWantsToSprint == NewMove->bWantsToSprint) 
	{
		return true;
	}

	return Super::CanCombineWith(NewMovePtr, Character, MaxDelta);
}

void FSavedMove_PlayerCharacter::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	PlayerCharacterOwner = Cast<APlayerCharacterBase>(CharacterOwner);

	UPlayerCharacterMovementComponent* CharacterMovement = static_cast<UPlayerCharacterMovementComponent*>(Character->GetCharacterMovement());
	if (CharacterMovement) 
	{
		bWantsToSprint = CharacterMovement->bWantsToSprint;
	}
}

void FSavedMove_PlayerCharacter::PrepMoveFor(ACharacter* Character)
{
	Super::PrepMoveFor(Character);

	UPlayerCharacterMovementComponent* CharacterMovement = static_cast<UPlayerCharacterMovementComponent*>(Character->GetCharacterMovement());
	if (CharacterMovement)
	{
		CharacterMovement->bWantsToSprint = bWantsToSprint;
	}
}

FNetworkPredictionData_Client_PlayerCharacter::FNetworkPredictionData_Client_PlayerCharacter(const UCharacterMovementComponent& ClientMovement)
	: Super(ClientMovement)
{
}

FSavedMovePtr FNetworkPredictionData_Client_PlayerCharacter::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_PlayerCharacter());
}
