// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacterBase.generated.h"

class UPlayerCharacterMovementComponent;

UCLASS(BlueprintType)
class PROJECTTAU_API APlayerCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacterBase(const FObjectInitializer& ObjectInitializer);

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const;

private:
	UPROPERTY(Category = "Player Character", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPlayerCharacterMovementComponent> PlayerCharacterMovement;

public:
	UFUNCTION(BlueprintCallable, Category = "Player Chacter Movement")
	FORCEINLINE UPlayerCharacterMovementComponent* GetPlayerCharacterMovement() const { return PlayerCharacterMovement;  };

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PostInitializeComponents() override;
public:
#pragma region Sprint
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Player Character")
	uint32 bIsSprinting:1;

	void SetSprinting(bool Sprint);

	UFUNCTION(BlueprintCallable, Category = "Player Character")
	void Sprint();

	UFUNCTION(BlueprintCallable, Category = "Player Character")
	void UnSprint();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerSetSprinting(bool Sprint);

	UFUNCTION(BlueprintCallable, Category = "Player Character")
	bool CanSprint() const;      
#pragma endregion
#pragma region Can Jump
	virtual bool CanJumpInternal_Implementation() const override;
#pragma endregion
};
