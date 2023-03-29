// Copyright Final Fall Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
// #include <GameplayEffectTypes.h>
#include "character_actor.generated.h"

class UHeroAbilitySystemComponent;

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class USpringArmComponent;
class UAnimMontage;
class USoundBase;
class UBaseAbility;

UCLASS(config=Game)
class AHeraCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

	// MARK: - Gamplay Ability System

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=GAS, meta = (AllowPrivateAccess = "true"))
	class UHeroAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(BlueprintReadWrite, Category=GAS, meta = (AllowPrivateAccess = "true"))
	class ULifeAttributeSet* LifeAttributes;

	// We need to initialize the Ability System on the server and client
	// This function is called on the server and is a convenient place to 
	// init the Ability System there. 
	virtual void PossessedBy(AController* NewController) override;
	// This function is called on the client and is a convenient place to 
	// init the Ability System there. 
	virtual void OnRep_PlayerState() override;

	void AssignInputBindings();

	// MARK: - Character

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	TObjectPtr<USkeletalMeshComponent> Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FirstPersonCameraComponent;

	/** Third person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> ThirdPersonCameraComponent;

	/** Third person camera spring arm */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> ThirdPersonCameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	bool IsCameraChangeAllowed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	bool bCameraIsChangingPov;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	bool bCameraIsFirstPerson;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Crouch Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* CrouchAction;

	
public:
	AHeraCharacter();

	// MARK: - Gamplay Ability System

	// The Ability System uses this via the IAbilitySystemInterface. 
	// It should return a reference to this OwningActor's ASC 
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void InitializeAttributes();

	// Grants the abilities set in the DefaultAbilities property to the Character when that
	// that Character is possessed by a Controller.
	// These abilities are usually set in the derived Blueprint 'Class Defaults' panel.
	virtual void GiveAbilities();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=GAS)
	TSubclassOf<class UGameplayEffect> DefaultAttributeEffect;

	// These abilities are usually set in the derived Blueprint 'Class Defaults' panel.
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=GAS)
	TArray<TSubclassOf<class UBaseAbility>> DefaultAbilities;

	// MARK: - Character

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Bool for AnimBP to switch to another animation set */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	bool bHasRifle;

	/** Setter to set the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void SetHasRifle(bool bNewHasRifle);

	/** Getter for the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool GetHasRifle();

	/** Setter to set the bool */
	UFUNCTION(BlueprintCallable, Category = Camera)
	void SetCameraIsChangingPov(bool bNewIsChanging);

	/** Getter for the bool */
	UFUNCTION(BlueprintCallable, Category = Camera)
	bool GetCameraIsChangingPov();

	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { 
		return FirstPersonCameraComponent;
	}
	
	/** Returns ThirdPersonCameraComponent subobject **/
	UCameraComponent* GetThirdPersonCameraComponent() const { 
		return ThirdPersonCameraComponent;
	}

	UFUNCTION(BlueprintCallable, Category = Camera)
	void SetCameraToFPV();

	UFUNCTION(BlueprintCallable, Category = Camera)
	void SetCameraToTPV();

	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }


protected:
	virtual void BeginPlay();

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for Crouch input */
	UFUNCTION(BlueprintCallable, Category=Character)
	void Duck(const FInputActionValue& Value);

	/** Called for Crouch input */
	UFUNCTION(BlueprintCallable, Category=Character)
	void UnDuck(const FInputActionValue& Value);

	// MARK: - APawn interface

	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
};