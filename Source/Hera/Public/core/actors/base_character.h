// Copyright Final Fall Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "AbilitySystemInterface.h"
#include "base_character.generated.h"

class USkeletalMeshComponent;
class UAnimMontage;
class USoundCue;
class UAbilityBase;
class UGameplayEffect;
class UAbilitySystemComponent;
class UAbilitySystemComponentBase;
class ULifeAttributeSet;
class UHealthbarWidget;
class UWidgetComponent;

/// TODO: Add a concept of 'Hostility' that tells us how we respond to other characters
//        - Normal, FriendlyToAll, HostileToAll, NeutralToAll, NeutralToOtherAllegiances, HostileToOtherAllegiances

UCLASS(config=Game)
// UCLASS()
class HERA_API ACharacterBaseValid : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

	//------------------------------------------------------------------------------------------------------------------
	/// MARK: - Gamplay Ability System
	//------------------------------------------------------------------------------------------------------------------

public:
	UFUNCTION(BlueprintPure, Category="Hera|Character|Attributes")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintPure, Category="Hera|Character|Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintPure, Category="Hera|Character|Attributes")
	float GetMaxShields() const;

	UFUNCTION(BlueprintPure, Category="Hera|Character|Attributes")
	float GetShields() const;

	UFUNCTION(BlueprintPure, Category="Hera|Character|Attributes")
	float GetMaxArmor() const;

	UFUNCTION(BlueprintPure, Category="Hera|Character|Attributes")
	float GetArmor() const;

	UFUNCTION(BlueprintPure, Category="Hera|Character|Attributes")
	float GetOverHealth() const;

	UFUNCTION(BlueprintPure, Category="Hera|Character|Attributes")
	float GetOverArmor() const;

	UFUNCTION(BlueprintPure, Category="Hera|Character|Attributes")
	bool IsAlive() const;

	UFUNCTION(BlueprintPure, Category="Hera|Character|Attributes")
	float GetMoveSpeed() const;

	UFUNCTION(BlueprintPure, Category="Hera|Character|Attributes")
	float GetMoveSpeedBaseValue() const;

	UFUNCTION(BlueprintPure, Category="Hera|Character|Attributes")
	float GetXP() const;

	UFUNCTION(BlueprintPure, Category="Hera|Character|Attributes")
	float GetRewardXP() const;

	UFUNCTION(BlueprintPure, Category="Hera|Character|Attributes")
	float GetCharacterLevel() const;

	// These abilities are usually set in the derived Blueprint 'Class Defaults' panel.
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Hera|Character")
	TArray<TSubclassOf<UAbilityBase>> DefaultAbilities;

	// Specific GameplayEffect that applies the Character's default attributes
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Hera|Character")
	TSubclassOf<UGameplayEffect> DefaultAttributesEffect;

	// Effects to be applied once when a Character spawns
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Hera|Character")
	TArray<TSubclassOf<UGameplayEffect>> DefaultEffects;

	// The Ability System uses this via the IAbilitySystemInterface. 
	// It should return a reference to this OwningActor's AbilitySystemComponent 
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	// This function is called on the client and is a convenient place to 
	// init the Ability System there. 
	virtual void OnRep_PlayerState() override;

	// Grants the abilities set in the DefaultAbilities property to the Character when that
	// that Character is possessed by a Controller.
	// These abilities are usually set in the derived Blueprint 'Class Defaults' panel.
	virtual void InitializeAbilities();

	// This must run on Server but we run it on Client too  so that we don't have to wait. 
	// The Server's replication to the Client won't matter since the values should be the same.
	virtual void InitializeAttributes();

	virtual void InitializeEffects(); 

	UPROPERTY(
		VisibleAnywhere, BlueprintReadOnly, Category="Hera|Character", 
		meta = (AllowPrivateAccess = "true"))
	UAbilitySystemComponentBase* AbilitySystemComponent;

private:
	// We need to initialize the Ability System on the server and client
	// This function is called on the server and is a convenient place to 
	// init the Ability System there. 
	virtual void PossessedBy(AController* NewController) override;

	UPROPERTY(
		BlueprintReadWrite, Category="Hera|Character|Attributes", 
		meta = (AllowPrivateAccess = "true"))
	ULifeAttributeSet* LifeAttributes;
	
	//------------------------------------------------------------------------------------------------------------------
	/// MARK: - Character
	//------------------------------------------------------------------------------------------------------------------

public:
	ACharacterBaseValid();

	UFUNCTION(BlueprintCallable, Category = "Hera|Character")
	USkeletalMeshComponent* GetThirdPersonMesh() const { return GetMesh(); }

protected:
	virtual void BeginPlay();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Hera|Character|Animation")
	UAnimMontage* DeathMontage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Hera|Character|Audio")
	USoundCue* DeathSound;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Hera|Character")
	FText CharacterName;

	//------------------------------------------------------------------------------------------------------------------
	/// MARK: - UI
	//------------------------------------------------------------------------------------------------------------------

public:
	/// The healthbar that floats over characters' heads
	UHealthbarWidget* GetFloatingHealthbar();

	UPROPERTY(BlueprintReadWrite)
	bool bFloatingHealthbarIsVisible = true;

protected:
	UFUNCTION()
	void InitializeFloatingHealthbar();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Hera|UI")
	TSubclassOf<UHealthbarWidget> HealthbarWidgetClass;

	UPROPERTY()
	UHealthbarWidget* FloatingHealthbarWidget;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Hera|UI")
	UWidgetComponent* FloatingHealthbarComponent;
};