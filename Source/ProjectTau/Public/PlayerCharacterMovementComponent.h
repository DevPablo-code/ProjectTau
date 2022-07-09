// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerCharacterMovementComponent.generated.h"

class APlayerCharacterBase;

/**
 * 
 */
UCLASS()
class PROJECTTAU_API UPlayerCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
		 
protected:
	/** Character movement component belongs to */
	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<APlayerCharacterBase> PlayerCharacterOwner;

public:
	UFUNCTION(BlueprintCallable, Category = "Player Character Movement")
	FORCEINLINE APlayerCharacterBase* GetPlayerCharacterOwner() const { return PlayerCharacterOwner; };

	virtual void PostLoad() override;
	virtual void SetUpdatedComponent(USceneComponent* NewUpdatedComponent) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Character Movement|Grounded", Meta = (AllowPrivateAccess = "true"))
	float RunSpeed = 300.0f;
	// The ground speed when sprinting
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Character Movement|Grounded", Meta = (AllowPrivateAccess = "true"))
	float SprintSpeed = 600.0f;
	// The acceleration when running
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Character Movement|Grounded", Meta = (AllowPrivateAccess = "true"))
	float RunAcceleration = 2000.0f;
	// The acceleration when sprinting
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Character Movement|Grounded", Meta = (AllowPrivateAccess = "true"))
	float SprintAcceleration = 2000.0f;

public:
	virtual float GetMaxSpeed() const override;
	virtual float GetMaxAcceleration() const override;

	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	virtual void UpdateCharacterStateAfterMovement(float DeltaSeconds) override;

	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

public:   
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Character Movement")
	uint32 bCanSprint:1;

	FORCEINLINE bool CanEverSprint() const { return bCanSprint; };

	bool CanSprintInCurrentState() const;

	UFUNCTION(BlueprintCallable, Category = "Player Character Movement")
	void Sprint(bool bClientSimulation);

	UFUNCTION(BlueprintCallable, Category = "Player Character Movement")
	void UnSprint(bool bClientSimulation);

	UFUNCTION(BlueprintCallable, Category = "Player Character Movement")
	bool IsSprinting() const;

public:
	UPROPERTY(Category = "Player Character Movement (General Settings)", VisibleInstanceOnly, BlueprintReadOnly)
	uint32 bWantsToSprint:1;
};

class FSavedMove_PlayerCharacter : public FSavedMove_Character
{
public:
	typedef FSavedMove_Character Super;

	APlayerCharacterBase* PlayerCharacterOwner;

	// Resets all saved variables.
	virtual void Clear() override;
	// Store input commands in the compressed flags.
	virtual uint8 GetCompressedFlags() const override;
	// This is used to check whether or not two moves can be combined into one.
	// Basically you just check to make sure that the saved variables are the same.
	virtual bool CanCombineWith(const FSavedMovePtr& NewMovePtr, ACharacter* Character, float MaxDelta) const override;
	// Sets up the move before sending it to the server. 
	virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;
	// Sets variables on character movement component before making a predictive correction.
	virtual void PrepMoveFor(class ACharacter* Character) override;

	uint32 bWantsToSprint:1;
};

class FNetworkPredictionData_Client_PlayerCharacter : public FNetworkPredictionData_Client_Character
{
public:
	typedef FNetworkPredictionData_Client_Character Super;

	// Constructor
	FNetworkPredictionData_Client_PlayerCharacter(const UCharacterMovementComponent& ClientMovement);

	//brief Allocates a new copy of our custom saved move
	virtual FSavedMovePtr AllocateNewMove() override;
};
