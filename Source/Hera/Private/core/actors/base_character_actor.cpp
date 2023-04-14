// Copyright Final Fall Games. All Rights Reserved.

#include "core/actors/base_character_actor.h"
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

//---------------------------------------------------------------------------------------------------------------------
/// MARK: - ACharacterBase
//---------------------------------------------------------------------------------------------------------------------

ACharacterBase::ACharacterBase()
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

	ASC = CreateDefaultSubobject<UHeroAbilitySystemComponent>("AbilitySystemComponent");
	ASC->SetIsReplicated(true);
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	// Initializing the AttributeSet in the Owner Actor's constructor automatically registers
	// it with the ASC
	LifeAttributes = CreateDefaultSubobject<ULifeAttributeSet>("LifeAttributeSet");
}

void ACharacterBase::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (auto PlayerController = Cast<APlayerController>(Controller))
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

//---------------------------------------------------------------------------------------------------------------------
/// MARK: - Attributes
//---------------------------------------------------------------------------------------------------------------------

float ACharacterBase::GetMaxHealth() const
{
	if (IsValid(LifeAttributes))
	{
		return LifeAttributes->GetMaxHealth();
	}

	return 0.0f;
}

float ACharacterBase::GetHealth() const
{
	if (IsValid(LifeAttributes))
	{
		return LifeAttributes->GetHealth();
	}

	return 0.0f;
}

float ACharacterBase::GetMaxShields() const
{
	if (IsValid(LifeAttributes))
	{
		return LifeAttributes->GetMaxShields();
	}

	return 0.0f;
}

float ACharacterBase::GetShields() const
{
	if (IsValid(LifeAttributes))
	{
		return LifeAttributes->GetShields();
	}

	return 0.0f;
}

float ACharacterBase::GetMaxArmor() const
{
	if (IsValid(LifeAttributes))
	{
		return LifeAttributes->GetMaxArmor();
	}

	return 0.0f;
}

float ACharacterBase::GetArmor() const
{
	if (IsValid(LifeAttributes))
	{
		return LifeAttributes->GetArmor();
	}

	return 0.0f;
}

float ACharacterBase::GetOverHealth() const
{
	if (IsValid(LifeAttributes))
	{
		return LifeAttributes->GetOverHealth();
	}

	return 0.0f;
}

float ACharacterBase::GetOverArmor() const
{
	if (IsValid(LifeAttributes))
	{
		return LifeAttributes->GetOverArmor();
	}

	return 0.0f;
}

bool ACharacterBase::IsAlive() const
{
	// We check againtst the floor because that's what the UI will be showing.
	return FMath::Floor(GetHealth()) > 0;
}

float ACharacterBase::GetMoveSpeed() const
{
	if (IsValid(LifeAttributes))
	{
		return LifeAttributes->GetMoveSpeed();
	}

	return 0.0f;
}

float ACharacterBase::GetMoveSpeedBaseValue() const
{
	if (IsValid(LifeAttributes))
	{
		return LifeAttributes->GetMoveSpeedAttribute().GetGameplayAttributeData(LifeAttributes)->GetBaseValue();
	}

	return 0.0f;
}

float ACharacterBase::GetXP() const
{
	if (IsValid(LifeAttributes))
	{
		return LifeAttributes->GetXP();
	}

	return 0.0f;
}

float ACharacterBase::GetRewardXP() const
{
	if (IsValid(LifeAttributes))
	{
		return LifeAttributes->GetRewardXP();
	}

	return 0.0f;
}

//---------------------------------------------------------------------------------------------------------------------
/// MARK: - Abilties
//---------------------------------------------------------------------------------------------------------------------

class UAbilitySystemComponent* ACharacterBase::GetAbilitySystemComponent() const
{
	return ASC;
}

void ACharacterBase::InitializeAttributes()
{
	if (ASC && DefaultAttributeEffect)
	{
		FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(
			DefaultAttributeEffect, // GameplayEffect class
			1,                      // Level
			EffectContext           // Context
		);
		if (SpecHandle.IsValid())
		{
			auto EffectHandle = ASC->ApplyGameplayEffectSpecToSelf(
				*SpecHandle.Data.Get() // GameplayEffect
			);
		}
	}
}

void ACharacterBase::GiveAbilities() 
{
	if (HasAuthority() && ASC)
	{
		for (TSubclassOf<UAbilityBase>& Ability : DefaultAbilities)
		{
			ASC->GiveAbility(FGameplayAbilitySpec(
				Ability, 
				1, 
				static_cast<int32>(Ability.GetDefaultObject()->AbilityInputID), 
				this
			));
		}
	}
}

void ACharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	// Server GAS init
	ASC->InitAbilityActorInfo(this, this); // (Avatar, Owner)
	InitializeAttributes();
	GiveAbilities();
}

void ACharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Client GAS init
	ASC->InitAbilityActorInfo(this, this); // (Avatar, Owner)
	InitializeAttributes();
	AssignInputBindings();
}

//---------------------------------------------------------------------------------------------------------------------
/// MARK: - Inputs
//---------------------------------------------------------------------------------------------------------------------

void ACharacterBase::AssignInputBindings() 
{
	if (ASC && InputComponent)
	{
		const auto Bindings = FGameplayAbilityInputBinds(
			// Confirm and Cancel are special bindings and are required to be explicitly named here
			"Confirm", 
			"Cancel", 

			// All the enum values will be given bindings and must have the same name as in the Config/DefaultInput.ini
			"EAbilityInputID", 

			// This matches the Confirm/Cancel names with their enum values
			static_cast<int32>(EAbilityInputID::Confirm), 
			static_cast<int32>(EAbilityInputID::Cancel)
		);
		ASC->BindAbilityActivationToInputComponent(InputComponent, Bindings);
	}
}

void ACharacterBase::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACharacterBase::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACharacterBase::Look);

		//Crouching
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ACharacterBase::Duck);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ACharacterBase::UnDuck);
	}

	AssignInputBindings();
}


void ACharacterBase::Move(const FInputActionValue& Value)
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

void ACharacterBase::Look(const FInputActionValue& Value)
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

void ACharacterBase::Duck(const FInputActionValue& Value) 
{
	ACharacter::Crouch(true);
}

void ACharacterBase::UnDuck(const FInputActionValue& Value) 
{
	ACharacter::UnCrouch(true);
}

void ACharacterBase::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool ACharacterBase::GetHasRifle()
{
	return bHasRifle;
}

//---------------------------------------------------------------------------------------------------------------------
/// MARK: - Camera
//---------------------------------------------------------------------------------------------------------------------

void ACharacterBase::SetCameraToFPV()
{
	if (!bCameraIsFirstPerson)
	{
		bCameraIsFirstPerson = true;
		FirstPersonCameraComponent->SetActive(true);
		ThirdPersonCameraComponent->SetActive(false);
	}
}

void ACharacterBase::SetCameraToTPV()
{
	if (bCameraIsFirstPerson)
	{
		bCameraIsFirstPerson = false;
		ThirdPersonCameraComponent->SetActive(true);
		FirstPersonCameraComponent->SetActive(false);
	}
}

void ACharacterBase::SetCameraIsChangingPov(bool bNewIsChanging)
{
	bCameraIsChangingPov = bNewIsChanging;
}

bool ACharacterBase::GetCameraIsChangingPov()
{
	return bCameraIsChangingPov;
}