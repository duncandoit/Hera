// Copyright Final Fall Games. All Rights Reserved.

#pragma once

#include "core/actors/base_character.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "AbilitySystemInterface.h"
#include "base_hero.generated.h"

class USkeletalMeshComponent;
class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;
class UTimelineComponent;

UCLASS()
class HERA_API AHeroBase : public ACharacterBaseValid
{
	GENERATED_BODY()

public:
	AHeroBase();

	UCameraComponent* GetFirstPersonCamera() const { return FirstPersonCamera; }
	UCameraComponent* GetThirdPersonCamera() const { return ThirdPersonCamera; }

   UFUNCTION(BlueprintCallable, Category ="Hera|Character")
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	UFUNCTION(BlueprintPure, Category ="Hera|Character|Camera")
	bool GetCameraIsChangingPov();

	UFUNCTION(BlueprintCallable, Category ="Hera|Character|Camera")
	void SetCameraToFPV();

	UFUNCTION(BlueprintCallable, Category ="Hera|Character|Camera")
	void SetCameraToTPV();

   UPROPERTY(
		EditAnywhere, BlueprintReadOnly, Category ="Hera|Character|Input", 
		meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

protected:
   virtual void BeginPlay() override;
   virtual void OnRep_PlayerState() override;
   virtual void AssignInputBindings();

	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category="Hera|Character|Input")
	void Duck(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category="Hera|Character|Input")
	void UnDuck(const FInputActionValue& Value);
	
	UFUNCTION(BlueprintCallable, Category="Hera|Character|Input")
	void ToggleCameraPOV(const FInputActionValue& Value);

	// UFUNCTION(BlueprintCallable, Category="Hera|Character|Camera")
	// void SetCameraBoomMaxLength(float Length);

	// UFUNCTION(BlueprintPure, Category="Hera|Character|Camera")
	// float GetCameraBoomMaxLength();

   bool bInputsBoundToASC;

private:
	void CameraPovUpdate(float Scale);
	void CameraPovFinished();

	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	TObjectPtr<USkeletalMeshComponent> FirstPersonMesh;

	// First person camera
	UPROPERTY(
		VisibleAnywhere, BlueprintReadWrite, Category ="Hera|Character|Camera", 
		meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FirstPersonCamera;

	// Third person camera
	UPROPERTY(
		VisibleAnywhere, BlueprintReadOnly, Category ="Hera|Character|Camera", 
		meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> ThirdPersonCamera;

	// Third person camera spring arm
	UPROPERTY(
		VisibleAnywhere, BlueprintReadOnly, Category ="Hera|Character|Camera", 
		meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(
		VisibleAnywhere, BlueprintReadWrite, Category ="Hera|Character|Camera", 
		meta = (AllowPrivateAccess = "true"))
	bool bCameraChangeIsAllowed = true;

	UPROPERTY(
		VisibleAnywhere, BlueprintReadOnly, Category ="Hera|Character|Camera", 
		meta = (AllowPrivateAccess = "true"))
	bool bCameraIsFirstPerson = true;

	UPROPERTY(
		EditAnywhere, BlueprintReadOnly, Category ="Hera|Character|Input", 
		meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(
		EditAnywhere, BlueprintReadOnly, Category ="Hera|Character|Input", 
		meta=(AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(
		EditAnywhere, BlueprintReadOnly, Category ="Hera|Character|Input", 
		meta=(AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;
	
	UPROPERTY(
		EditAnywhere, BlueprintReadOnly, Category ="Hera|Character|Input", 
		meta=(AllowPrivateAccess = "true"))
	UInputAction* TogglePovAction;

	UTimelineComponent* PovTimeline;
	UCurveFloat* PovTimelineCurve;
	float CameraBoomMaxLength = 500.0f;
	const float PovChangeDuration = 0.2f;

	//------------------------------------------------------------------------------------------------------------------
	/// MARK: - APawn interface
	//------------------------------------------------------------------------------------------------------------------

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
};
