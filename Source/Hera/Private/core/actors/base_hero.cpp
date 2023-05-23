// Copyright Final Fall Games. All Rights Reserved.

#include "core/actors/base_hero.h"
#include "core/gas/abilities/base_ability.h"
#include "core/gas/base_asc.h"
#include "core/base_player_controller.h"

#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/TimelineComponent.h"
#include "Curves/CurveFloat.h"

//---------------------------------------------------------------------------------------------------------------------
/// MARK: - Character
//---------------------------------------------------------------------------------------------------------------------

AHeroBase::AHeroBase()
   // : ACharacterBaseValid()
{
	//// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponents
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FPVCamera"));
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
	FirstPersonCamera->SetRelativeLocation(FVector(-10.f, 0.f, 60.f));
	FirstPersonCamera->bUsePawnControlRotation = true;
	FirstPersonCamera->SetFieldOfView(90);
	FirstPersonCamera->SetAutoActivate(true);
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("TPVCameraBoom"));
	CameraBoom->SetupAttachment(GetCapsuleComponent());
	CameraBoom->SetRelativeLocation(FVector(-10.f, 0.f, 60.f));
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->TargetArmLength = 400.f;
	
	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TPVCamera"));
	ThirdPersonCamera->SetupAttachment(CameraBoom);
	ThirdPersonCamera->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	ThirdPersonCamera->bUsePawnControlRotation = true;
	ThirdPersonCamera->SetFieldOfView(90);
	ThirdPersonCamera->SetAutoActivate(true);

   // Arms
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FPVMesh"));
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->SetupAttachment(FirstPersonCamera);
	FirstPersonMesh->bCastDynamicShadow = false;
	FirstPersonMesh->CastShadow = false;
	FirstPersonMesh->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	// Camera POV toggling
	// PovTimelineCurve = LoadObject<UCurveFloat>(this, TEXT("/Game/Hera/Characters/Data/CURVE_ChangePov"));
	// if (!PovTimelineCurve)
	// {
	// 	UE_LOG(
	// 		LogTemp, 
	// 		Error, 
	// 		TEXT("%s() Failed to load CURVE_ChangePov. If it was moved please update the reference location in C++."),
	// 		*FString(__FUNCTION__)
	// 	);
	// }
}

void AHeroBase::BeginPlay()
{
   Super::BeginPlay();

   //Add Input Mapping Context
	if (auto PlayerController = Cast<APlayerControllerBase>(Controller))
	{
		auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
			PlayerController->GetLocalPlayer()
		);
		if (Subsystem)
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

// Client-only
void AHeroBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	AssignInputBindings();
}

//---------------------------------------------------------------------------------------------------------------------
/// MARK: - Inputs
//---------------------------------------------------------------------------------------------------------------------

void AHeroBase::AssignInputBindings() 
{
	if (!bInputsBoundToASC && AbilitySystemComponent && InputComponent)
	{
		auto AbilityEnumPath = FTopLevelAssetPath(FName("/Script/Hera"), FName("EAbilityInputID"));
		const auto Bindings = FGameplayAbilityInputBinds(
			// Confirm and Cancel are special bindings and are required to be explicitly named here
			FString("Confirm"), 
			FString("Cancel"),

			// All the enum values will be given bindings and must have the same name as in the Config/DefaultInput.ini
			AbilityEnumPath, 

			// This matches the Confirm/Cancel names with their enum values
			static_cast<int32>(EAbilityInputID::Confirm), 
			static_cast<int32>(EAbilityInputID::Cancel)
		);

		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, Bindings);

      bInputsBoundToASC = true;
	}
}

void AHeroBase::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (auto Input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Moving
		Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AHeroBase::Move);

		// Looking
		Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &AHeroBase::Look);

		// Crouching
		Input->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &AHeroBase::Duck);
		Input->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AHeroBase::UnDuck);

		// Toggle camera POV
		Input->BindAction(TogglePovAction, ETriggerEvent::Started, this, &AHeroBase::ToggleCameraPOV);
	}

	AssignInputBindings();
}


void AHeroBase::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	auto MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AHeroBase::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AHeroBase::Duck(const FInputActionValue& Value) 
{
	ACharacter::Crouch(true);
}

void AHeroBase::UnDuck(const FInputActionValue& Value) 
{
	ACharacter::UnCrouch(true);
}

void AHeroBase::ToggleCameraPOV(const FInputActionValue& Value)
{
	// Check if the game is disallowing camera switching
	if (!bCameraChangeIsAllowed)
	{
		return;
	}

	// Create a timeline and bind the update function
	if (!PovTimeline)
	{
		FOnTimelineFloat PovUpdateCallback;
		PovUpdateCallback.BindUFunction(this, TEXT("CameraPovUpdate"));

		FOnTimelineEvent PovFinishedCallback;
		PovFinishedCallback.BindUFunction(this, TEXT("CameraPovFinished"));

		PovTimeline = NewObject<UTimelineComponent>(this, FName("Change Camera POV Timeline"));
		PovTimeline->AddInterpFloat(PovTimelineCurve, PovUpdateCallback);
		PovTimeline->SetTimelineFinishedFunc(PovFinishedCallback);
		PovTimeline->SetTimelineLength(PovChangeDuration);
		PovTimeline->SetLooping(false);
	}

	if (PovTimeline->IsPlaying())
	{
		PovTimeline->Reverse();
	}
	else 
	{
		// The timeline was either stopped or just created.
		// If we're currently first person then we want to play the timeline forward from the start
		if (bCameraIsFirstPerson)
		{
			PovTimeline->PlayFromStart();
		}
		// If we're currently third person then we want to play the timeline reversed from the end
		else 
		{
			PovTimeline->ReverseFromEnd();
		}
	}
}

//---------------------------------------------------------------------------------------------------------------------
/// MARK: - Camera
//---------------------------------------------------------------------------------------------------------------------

void AHeroBase::CameraPovUpdate(float Scale)
{
	CameraBoom->TargetArmLength = CameraBoomMaxLength * Scale;
}

void AHeroBase::CameraPovFinished()
{
	PovTimeline->Stop();
	PovTimeline->MarkAsGarbage();
}

// void AHeroBase::SetCameraBoomMaxLength(float Length)
// {
// 	PovTimelineCurve = NewObject<UCurveFloat>();
// 	PovTimelineCurve->PovTimelineCurve.AddKey(0.f, Length);
// 	PovTimelineCurve->FloatCurve
// }

// float AHeroBase::GetCameraBoomMaxLength()
// {
// 	return CameraBoomMaxLength;
// }

void AHeroBase::SetCameraToFPV()
{
	if (!bCameraIsFirstPerson)
	{
		bCameraIsFirstPerson = true;
		FirstPersonCamera->SetActive(true);
		ThirdPersonCamera->SetActive(false);
	}
}

void AHeroBase::SetCameraToTPV()
{
	if (bCameraIsFirstPerson)
	{
		bCameraIsFirstPerson = false;
		ThirdPersonCamera->SetActive(true);
		FirstPersonCamera->SetActive(false);
	}
}

bool AHeroBase::GetCameraIsChangingPov()
{
	if (PovTimeline)
	{
		return PovTimeline->IsPlaying();
	}

	return false;
}