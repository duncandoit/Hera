// Copyright Final Fall Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "base_character_actor.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class USpringArmComponent;
class UAnimMontage;
class USoundBase;
class UAbilityBase;

UCLASS(config=Game)
class HERA_API ACharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

	//------------------------------------------------------------------------------------------------------------------
	/// MARK: - Gamplay Ability System
	//------------------------------------------------------------------------------------------------------------------

public:
	// The Ability System uses this via the IAbilitySystemInterface. 
	// It should return a reference to this OwningActor's AbilitySystemComponent 
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void InitializeAttributes();

	// Grants the abilities set in the DefaultAbilities property to the Character when that
	// that Character is possessed by a Controller.
	// These abilities are usually set in the derived Blueprint 'Class Defaults' panel.
	virtual void GiveAbilities();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Hera|Character")
	TSubclassOf<class UGameplayEffect> DefaultAttributeEffect;

	// These abilities are usually set in the derived Blueprint 'Class Defaults' panel.
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Hera|Character")
	TArray<TSubclassOf<class UAbilityBase>> DefaultAbilities;

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

private:
	UPROPERTY(
		VisibleAnywhere, BlueprintReadOnly, Category="Hera|Character", 
		meta = (AllowPrivateAccess = "true"))
	class UAbilitySystemComponentBase* AbilitySystemComponent;

	UPROPERTY(
		BlueprintReadWrite, Category="Hera|Character|Attributes", 
		meta = (AllowPrivateAccess = "true"))
	class ULifeAttributeSet* LifeAttributes;

	// We need to initialize the Ability System on the server and client
	// This function is called on the server and is a convenient place to 
	// init the Ability System there. 
	virtual void PossessedBy(AController* NewController) override;
	
	// This function is called on the client and is a convenient place to 
	// init the Ability System there. 
	virtual void OnRep_PlayerState() override;

	void AssignInputBindings();

	//------------------------------------------------------------------------------------------------------------------
	/// MARK: - Character
	//------------------------------------------------------------------------------------------------------------------

public:
	ACharacterBase();

	virtual void Landed(const FHitResult& Hit) override;

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;

	UPROPERTY(
		EditAnywhere, BlueprintReadOnly, Category ="Hera|Character|Input", 
		meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	// Bool for AnimBP to switch to another animation set
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category ="Hera|Character|Weapon")
	bool bHasRifle;

	UFUNCTION(BlueprintCallable, Category ="Hera|Character|Weapon")
	void SetHasRifle(bool bNewHasRifle);

	UFUNCTION(BlueprintCallable, Category ="Hera|Character|Weapon")
	bool GetHasRifle();

	UFUNCTION(BlueprintCallable, Category ="Hera|Character|Camera")
	void SetCameraIsChangingPov(bool bNewIsChanging);

	UFUNCTION(BlueprintCallable, Category ="Hera|Character|Camera")
	bool GetCameraIsChangingPov();

	UFUNCTION(BlueprintCallable, Category ="Hera|Character|Camera")
	void SetCameraToFPV();

	UFUNCTION(BlueprintCallable, Category ="Hera|Character|Camera")
	void SetCameraToTPV();

	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
	
	UCameraComponent* GetThirdPersonCameraComponent() const { return ThirdPersonCameraComponent; }

	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }

private:
	// Pawn mesh: 1st person view (arms; seen only by self)
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	TObjectPtr<USkeletalMeshComponent> Mesh1P;

	// First person camera
	UPROPERTY(
		VisibleAnywhere, BlueprintReadWrite, Category ="Hera|Character|Camera", 
		meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FirstPersonCameraComponent;

	// Third person camera
	UPROPERTY(
		VisibleAnywhere, BlueprintReadOnly, Category ="Hera|Character|Camera", 
		meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> ThirdPersonCameraComponent;

	// Third person camera spring arm
	UPROPERTY(
		VisibleAnywhere, BlueprintReadOnly, Category ="Hera|Character|Camera", 
		meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> ThirdPersonCameraBoom;

	UPROPERTY(
		VisibleAnywhere, BlueprintReadOnly, Category ="Hera|Character|Camera", 
		meta = (AllowPrivateAccess = "true"))
	bool IsCameraChangeAllowed;

	UPROPERTY(
		VisibleAnywhere, BlueprintReadOnly, Category ="Hera|Character|Camera", 
		meta = (AllowPrivateAccess = "true"))
	bool bCameraIsChangingPov;

	UPROPERTY(
		VisibleAnywhere, BlueprintReadOnly, Category ="Hera|Character|Camera", 
		meta = (AllowPrivateAccess = "true"))
	bool bCameraIsFirstPerson;

	UPROPERTY(
		EditAnywhere, BlueprintReadOnly, Category ="Hera|Character|Input", 
		meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(
		EditAnywhere, BlueprintReadOnly, Category ="Hera|Character|Input", 
		meta=(AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	UPROPERTY(
		EditAnywhere, BlueprintReadOnly, Category ="Hera|Character|Input", 
		meta=(AllowPrivateAccess = "true"))
	class UInputAction* CrouchAction;

protected:
	virtual void BeginPlay();

	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category="Hera|Character|Input")
	void Duck(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category="Hera|Character|Input")
	void UnDuck(const FInputActionValue& Value);

	//------------------------------------------------------------------------------------------------------------------
	/// MARK: - UI
	//------------------------------------------------------------------------------------------------------------------

public:
	/// The healthbar that floats over characters' heads
	class UHealthbarWidget* GetFloatingHealthbar();

protected:
	UFUNCTION()
	void InitializeFloatingHealthbar();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Hera|UI")
	TSubclassOf<class UHealthbarWidget> HealthbarWidgetClass;

	UPROPERTY()
	class UHealthbarWidget* FloatingHealthbarWidget;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Hera|UI")
	class UWidgetComponent* FloatingHealthbarComponent;

	//------------------------------------------------------------------------------------------------------------------
	/// MARK: - APawn interface
	//------------------------------------------------------------------------------------------------------------------

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
};