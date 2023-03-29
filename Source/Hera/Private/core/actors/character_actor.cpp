// Copyright Final Fall Games. All Rights Reserved.

#include "core/actors/character_actor.h"
#include "core/actors/projectile_actor.h"
#include "core/gas/life_attribute_set.h"
#include "core/gas/abilities/base_ability.h"
#include "core/gas/hero_asc.h"

#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include <GameplayEffectTypes.h>

//////////////////////////////////////////////////////////////////////////
// AHeraCharacter

AHeraCharacter::AHeraCharacter() 
	: IsCameraChangeAllowed(true)
	, bCameraIsChangingPov(false)
	, bCameraIsFirstPerson(true)
	, bHasRifle(false)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponents
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FPV Camera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->SetFieldOfView(90);
	FirstPersonCameraComponent->SetAutoActivate(true);
	
	ThirdPersonCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("TPV Camera Boom"));
	ThirdPersonCameraBoom->SetupAttachment(GetCapsuleComponent());
	ThirdPersonCameraBoom->SetRelativeLocation(FVector(-10.f, 0.f, 60.f));
	ThirdPersonCameraBoom->bUsePawnControlRotation = true;
	ThirdPersonCameraBoom->TargetArmLength = 400.f;
	
	ThirdPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TPV Camera"));
	ThirdPersonCameraComponent->SetupAttachment(ThirdPersonCameraBoom);
	ThirdPersonCameraComponent->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	ThirdPersonCameraComponent->bUsePawnControlRotation = true;
	ThirdPersonCameraComponent->SetFieldOfView(90);
	ThirdPersonCameraComponent->SetAutoActivate(true);

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	AHeraCharacter::AbilitySystemComponent = CreateDefaultSubobject<UHeroAbilitySystemComponent>("AbilitySystemComponent");
	AHeraCharacter::AbilitySystemComponent->SetIsReplicated(true);
	AHeraCharacter::AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	// Initializing the AttributeSet in the Owner Actor's constructor automatically registers
	// it with the ASC
	LifeAttributes = CreateDefaultSubobject<ULifeAttributeSet>("LifeAttributeSet");
}

void AHeraCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

}

class UAbilitySystemComponent* AHeraCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AHeraCharacter::InitializeAttributes()
{
	if (AHeraCharacter::AbilitySystemComponent && DefaultAttributeEffect)
	{
		FGameplayEffectContextHandle EffectContext = AHeraCharacter::AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		FGameplayEffectSpecHandle SpecHandle = AHeraCharacter::AbilitySystemComponent->MakeOutgoingSpec(
			DefaultAttributeEffect,	// GameplayEffect class
			1, 							// Level
			EffectContext				// Context
		);
		if (SpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle EffectHandle = AHeraCharacter::AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(
				*SpecHandle.Data.Get() // GameplayEffect
			);
		}
	}
}

void AHeraCharacter::GiveAbilities() 
{
	if (HasAuthority() && AHeraCharacter::AbilitySystemComponent)
	{
		for (TSubclassOf<UBaseAbility>& Ability : DefaultAbilities)
		{
			AHeraCharacter::AbilitySystemComponent->GiveAbility(
				FGameplayAbilitySpec(
					Ability, 
					1, 
					static_cast<int32>(Ability.GetDefaultObject()->AbilityInputID), 
					this
				)
			);
		}
	}
}

void AHeraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	// Server GAS init
	AHeraCharacter::AbilitySystemComponent->InitAbilityActorInfo(this, this); // (Avatar, Owner)
	InitializeAttributes();
	GiveAbilities();
}

void AHeraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Client GAS init
	AHeraCharacter::AbilitySystemComponent->InitAbilityActorInfo(this, this); // (Avatar, Owner)
	InitializeAttributes();
	AssignInputBindings();
}

void AHeraCharacter::AssignInputBindings() 
{
	if (AHeraCharacter::AbilitySystemComponent && InputComponent)
	{
		const FGameplayAbilityInputBinds Bindings = FGameplayAbilityInputBinds(
			// Confirm and Cancel are special bindings and are required to be explicitly named here
			"Confirm", 
			"Cancel", 

			// All the enum values will be given bindings and must have the same name as in the Config/DefaultInput.ini
			"EAbilityInputID", 

			// This matches the Confirm/Cancel names with their enum values
			static_cast<int32>(EAbilityInputID::Confirm), 
			static_cast<int32>(EAbilityInputID::Cancel)
		);
		AHeraCharacter::AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, Bindings);
	}
}

//////////////////////////////////////////////////////////////////////////// Input

void AHeraCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AHeraCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AHeraCharacter::Look);

		//Crouching
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &AHeraCharacter::Duck);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AHeraCharacter::UnDuck);
	}

	AssignInputBindings();
}


void AHeraCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AHeraCharacter::Look(const FInputActionValue& Value)
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

void AHeraCharacter::Duck(const FInputActionValue& Value) 
{
	ACharacter::Crouch(true);
}

void AHeraCharacter::UnDuck(const FInputActionValue& Value) 
{
	ACharacter::UnCrouch(true);
}

void AHeraCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool AHeraCharacter::GetHasRifle()
{
	return bHasRifle;
}

void AHeraCharacter::SetCameraToFPV()
{
	if (!bCameraIsFirstPerson)
	{
		bCameraIsFirstPerson = true;
		FirstPersonCameraComponent->SetActive(true);
		ThirdPersonCameraComponent->SetActive(false);
	}
}

void AHeraCharacter::SetCameraToTPV()
{
	if (bCameraIsFirstPerson)
	{
		bCameraIsFirstPerson = false;
		ThirdPersonCameraComponent->SetActive(true);
		FirstPersonCameraComponent->SetActive(false);
	}
}

void AHeraCharacter::SetCameraIsChangingPov(bool bNewIsChanging)
{
	bCameraIsChangingPov = bNewIsChanging;
}

bool AHeraCharacter::GetCameraIsChangingPov()
{
	return bCameraIsChangingPov;
}