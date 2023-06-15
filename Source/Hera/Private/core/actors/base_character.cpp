// Copyright Final Fall Games. All Rights Reserved.

#include "core/actors/base_character.h"
#include "core/actors/projectile_actor.h"
#include "core/gas/life_attribute_set.h"
#include "core/gas/abilities/base_ability.h"
#include "core/gas/base_asc.h"
#include "core/gas/tags.h"
#include "core/ui/healthbar_widget.h"
#include "core/base_player_controller.h"

#include "AbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include <GameplayEffectTypes.h>
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

//---------------------------------------------------------------------------------------------------------------------
/// MARK: - Character
//---------------------------------------------------------------------------------------------------------------------

ACharacterBaseValid::ACharacterBaseValid()
{
	PrimaryActorTick.bCanEverTick = false;
	bAlwaysRelevant = true;
	CharacterName = FText::FromString("Default");

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Overlap);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	GetThirdPersonMesh()->SetOwnerNoSee(true);
	GetThirdPersonMesh()->SetOnlyOwnerSee(false);
	GetThirdPersonMesh()->SetupAttachment(GetCapsuleComponent());
	GetThirdPersonMesh()->bCastDynamicShadow = true;
	GetThirdPersonMesh()->CastShadow = true;
	GetThirdPersonMesh()->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));
	GetThirdPersonMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Overlap);
	GetThirdPersonMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	FloatingHealthbarComponent = CreateDefaultSubobject<UWidgetComponent>(FName("FloatingHealthbarComponent"));
	FloatingHealthbarComponent->SetupAttachment(RootComponent);
	FloatingHealthbarComponent->SetWidgetSpace(EWidgetSpace::Screen);
	FloatingHealthbarComponent->SetDrawSize(FVector2D(150, 150));

	// We load in the blueprint widget in c++ so it's not up to each subclass to perform the boilerplate setup
	HealthbarWidgetClass = StaticLoadClass(
		UObject::StaticClass(), 
		nullptr,

		// There seems to be a make-believe "Game" directory created at the project's root 
		// that mirrors the Content directory structure. 
		TEXT("/Game/Hera/UI/UMG_Healthbar.UMG_Healthbar_C")
	);
	if (!HealthbarWidgetClass)
	{
		UE_LOG(
			LogTemp, 
			Error, 
			TEXT("%s() Failed to find HealthbarWidgetClass. If it was moved please update the reference location in C++."),
			*FString(__FUNCTION__)
		);
	}

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponentBase>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AbilitySystemComponent->DamageReceivedDelegate.AddDynamic(this, &ACharacterBaseValid::OnDamageReceived);
	AbilitySystemComponent->HealingReceivedDelegate.AddDynamic(this, &ACharacterBaseValid::OnHealingReceived);

	// Initializing the AttributeSet in the Owner Actor's constructor automatically registers
	// it with the AbilitySystemComponent
	LifeAttributes = CreateDefaultSubobject<ULifeAttributeSet>("LifeAttributeSet");

	const auto Movement = GetCharacterMovement();
	Movement->GravityScale = 2.0f;
	Movement->Mass = 100.f;
	Movement->AirControl = DefaultAirControl();
	Movement->MaxWalkSpeed = DefaultWalkSpeed();
	Movement->MaxWalkSpeedCrouched = DefaultCrouchSpeed();
	Movement->BrakingFrictionFactor = 1.0f;
	Movement->BrakingDecelerationWalking = 10000.0f;
	Movement->MaxAcceleration = 10000.0f;
	Movement->bMaintainHorizontalGroundVelocity = false;
	Movement->bCanWalkOffLedgesWhenCrouching = false;
	Movement->bUseFlatBaseForFloorChecks = true;
	// Movement->bCanEverCrouch = true;
	// Movement->CanCrouch = true;
}

void ACharacterBaseValid::BeginPlay()
{
	Super::BeginPlay();

	float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FloatingHealthbarComponent->SetRelativeLocation(FVector(0, 0, CapsuleHalfHeight)); // Top of the capsule

	GetCharacterMovement()->JumpZVelocity = VelocityForJumpHeight(100.f);

	// Only needed for Heroes placed in world when the player is the Server.
	// On respawn, they are set up in PossessedBy.
	// When the player is a client, the floating healthbars are all set up in OnRep_PlayerState.
	InitializeFloatingHealthbar();
}

void ACharacterBaseValid::PossessedBy(AController* NewController)
{
	// Note: Server-only

	Super::PossessedBy(NewController);

	AbilitySystemComponent->InitAbilityActorInfo(this, this); // (Avatar, Owner)
	InitializeAttributes();
	InitializeAbilities();

	// If player is host on listen server, the floating status bar would have been created   
	// for them from BeginPlay before player possession, hide it
	if (IsLocallyControlled() && IsPlayerControlled() && FloatingHealthbarComponent && FloatingHealthbarWidget)
	{
		FloatingHealthbarComponent->SetVisibility(false, true);
	}
}

void ACharacterBaseValid::OnRep_PlayerState()
{
	// Note: Client-only

	Super::OnRep_PlayerState();

	AbilitySystemComponent->InitAbilityActorInfo(this, this); // (Avatar, Owner)
	InitializeAttributes();

	// Simulated on proxies don't have their PlayerStates yet when 
	// BeginPlay is called so we call it again here
	InitializeFloatingHealthbar();
}

float ACharacterBaseValid::VelocityForJumpHeight(const float Height/* cm */) const
{
	const auto MovementComponent = GetCharacterMovement();
	const auto World = GetWorld();
	if (!IsValid(MovementComponent) || !IsValid(World))
	{
		return 0.f;
	}

	// We convert to m from cm
	const float JumpHeight = Height / 100.f;

	// Gravity is in Newtons by default in the engine. We convert to m/s.
	const float Gravity = MovementComponent->GravityScale > 0 
	                         ? FMath::Abs(World->GetDefaultGravityZ() * MovementComponent->GravityScale) / 100.f
	                         : 0;
	// Final velocity is in cm/s
	const float JumpVelocity = FMath::Sqrt(2.f * Gravity * JumpHeight) * 100;
	
	// GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Yellow, FString::Printf(
	// 	TEXT("Gravity (m/s): %f"), Gravity)
	// );
	
	// GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Yellow, FString::Printf(
	// 	TEXT("Jump Height (m): %f"), JumpHeight)
	// );

	// GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Yellow, FString::Printf(
	// 	TEXT("Velocity: %f\n\n"), JumpVelocity)
	// );

	return JumpVelocity;
}

void ACharacterBaseValid::DeathRagdoll() const
{
	if (const auto MovementComponent = GetMovementComponent())
	{
		// Disable the character's movement
		GetCharacterMovement()->DisableMovement();
		GetCharacterMovement()->SetComponentTickEnabled(false);
	}

	if (const auto ThirdPersonMesh = GetMesh())
	{
		// Set the skeletal mesh to physics simulation mode
		ThirdPersonMesh->SetSimulatePhysics(true);
		ThirdPersonMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

		// Apply a force or impulse to trigger the ragdoll physics
		FVector Force = FVector::UpVector * 1000.f;
		ThirdPersonMesh->AddForce(Force, NAME_None, true);
	}

	if (const auto Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Capsule->DestroyComponent();
	}

	if (FloatingHealthbarComponent)
	{
		FloatingHealthbarComponent->SetHiddenInGame(true);
		FloatingHealthbarComponent->DestroyComponent();
	}
}

//---------------------------------------------------------------------------------------------------------------------
/// MARK: - Floating Healthbar
//---------------------------------------------------------------------------------------------------------------------

UHealthbarWidget* ACharacterBaseValid::GetFloatingHealthbar()
{
	return FloatingHealthbarWidget;
}

void ACharacterBaseValid::InitializeFloatingHealthbar()
{
	// Only create once
	if (FloatingHealthbarWidget || !IsValid(AbilitySystemComponent))
	{
		return;
	}

	// Don't create for locally controlled player. We could add a game setting to toggle this later.
	if (IsPlayerControlled() && IsLocallyControlled())
	{
		return;
	}

	// Need a valid PlayerState
	// if (!GetPlayerState())
	// {
	// 	return;
	// }

	// Setup UI for Locally Owned Players only, not AI or the server's copy of the PlayerControllers
	auto PC = Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC && PC->IsLocalPlayerController())
	{
		if (HealthbarWidgetClass)
		{
			// Creating a widget requires that the first arg be derived from one of the following:
			// - UWidget, UWidgetTree, APlayerController, UGameInstance, UWorld
			FloatingHealthbarWidget = CreateWidget<UHealthbarWidget>(PC, HealthbarWidgetClass);
			if (FloatingHealthbarWidget && FloatingHealthbarComponent)
			{
				FloatingHealthbarWidget->SetOwningCharacter(this);
				FloatingHealthbarComponent->SetWidget(FloatingHealthbarWidget);
			}
		}
	}
}

//---------------------------------------------------------------------------------------------------------------------
/// MARK: - Abilty System
//---------------------------------------------------------------------------------------------------------------------

class UAbilitySystemComponent* ACharacterBaseValid::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ACharacterBaseValid::InitializeAbilities() 
{
	// Grant abilities only once and only on the Server	
	if (!HasAuthority() || !IsValid(AbilitySystemComponent) || AbilitySystemComponent->bDefaultAbilitiesInitialized)
	{
		return;
	}

	for (auto& Ability : DefaultAbilities)
	{
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(
			Ability, 
			GetCharacterLevel(), 
			static_cast<int32>(Ability.GetDefaultObject()->AbilityInputID), 
			this
		));
	}
}

void ACharacterBaseValid::InitializeAttributes()
{
	// Ensure ASC is valid
	if (!IsValid(AbilitySystemComponent))
	{
		UE_LOG(
			LogTemp, 
			Error, 
			TEXT("%s() Missing AbilitySystemComponent for %s. Something has removed it from the Character."), 
			*FString(__FUNCTION__), 
			*GetName()
		);

		return;
	}

	// Ensure the effect has been set
	if (!DefaultAttributesEffect)
	{
		UE_LOG(
			LogTemp, 
			Error, 
			TEXT("%s() Missing DefaultAttributesEffect for %s. Please fill in the character's Blueprint."), 
			*FString(__FUNCTION__), 
			*GetName()
		);

		return;
	}

	// Note: Can run on Server and Client

	auto EffectContextHandle = AbilitySystemComponent->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);

	auto EffectSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
		DefaultAttributesEffect, // GameplayEffect class
		GetCharacterLevel(),     // Level
		EffectContextHandle      // Context
	);

	if (EffectSpecHandle.IsValid())
	{
		auto ActiveEffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(
			*EffectSpecHandle.Data.Get() // GameplayEffect
		);
	}
}

void ACharacterBaseValid::InitializeEffects()
{
	// Grant default effects only once and only on the Server 
	if (!HasAuthority() || !IsValid(AbilitySystemComponent) || AbilitySystemComponent->bDefaultEffectsInitialized)
	{
		return;
	}

	auto EffectContextHandle = AbilitySystemComponent->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);

	for (auto& Effect : DefaultEffects)
	{
		auto EffectSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(Effect, GetCharacterLevel(), EffectContextHandle);
		if (EffectSpecHandle.IsValid())
		{
			auto ActiveEffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(
				*EffectSpecHandle.Data.Get(), 
				AbilitySystemComponent
			);
		}
	}

	AbilitySystemComponent->bDefaultEffectsInitialized = true;
}

void ACharacterBaseValid::OnDamageReceived_Implementation(
	UAbilitySystemComponentBase* SourceASC, 
	float FinalDamage,
	bool bIsDead
)
{
	if (!IsAlive())
	{
		DeathRagdoll();
	}
}

void ACharacterBaseValid::OnHealingReceived_Implementation(
	UAbilitySystemComponentBase* SourceASC, 
	float FinalHealing
)
{

}

void ACharacterBaseValid::OnMoveSpeedScaleChanged(
	UAbilitySystemComponentBase* SourceASC, 
	float NewScale
)
{
	if (const auto Movement = GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = DefaultWalkSpeed() * NewScale;
		Movement->MaxWalkSpeedCrouched = DefaultCrouchSpeed() * NewScale;
	}
}

//---------------------------------------------------------------------------------------------------------------------
/// MARK: - Attributes
//---------------------------------------------------------------------------------------------------------------------

float ACharacterBaseValid::GetMaxHealth() const
{
	if (IsValid(LifeAttributes))
	{
		return LifeAttributes->GetMaxHealth();
	}

	return 0.0f;
}

float ACharacterBaseValid::GetHealth() const
{
	if (IsValid(LifeAttributes))
	{
		return LifeAttributes->GetHealth();
	}

	return 0.0f;
}

float ACharacterBaseValid::GetMaxShields() const
{
	if (IsValid(LifeAttributes))
	{
		return LifeAttributes->GetMaxShields();
	}

	return 0.0f;
}

float ACharacterBaseValid::GetShields() const
{
	if (IsValid(LifeAttributes))
	{
		return LifeAttributes->GetShields();
	}

	return 0.0f;
}

float ACharacterBaseValid::GetMaxArmor() const
{
	if (IsValid(LifeAttributes))
	{
		return LifeAttributes->GetMaxArmor();
	}

	return 0.0f;
}

float ACharacterBaseValid::GetArmor() const
{
	if (IsValid(LifeAttributes))
	{
		return LifeAttributes->GetArmor();
	}

	return 0.0f;
}

float ACharacterBaseValid::GetOverHealth() const
{
	if (IsValid(LifeAttributes))
	{
		return LifeAttributes->GetOverHealth();
	}

	return 0.0f;
}

float ACharacterBaseValid::GetOverArmor() const
{
	if (IsValid(LifeAttributes))
	{
		return LifeAttributes->GetOverArmor();
	}

	return 0.0f;
}

float ACharacterBaseValid::GetCurrentHealthPool() const
{
	return GetHealth() + GetShields() + GetArmor() + GetOverHealth() + GetOverArmor();
}

float ACharacterBaseValid::GetMaxHealthPool() const
{
	return GetMaxHealth() + GetMaxShields() + GetMaxArmor() + GetOverHealth() + GetOverArmor();
}

bool ACharacterBaseValid::IsAlive() const
{
	return FMath::CeilToInt(GetHealth()) > 0;
}

float ACharacterBaseValid::GetMoveSpeed() const
{
	if (IsValid(LifeAttributes))
	{
		return LifeAttributes->GetMoveSpeed();
	}

	return 0.0f;
}

float ACharacterBaseValid::GetMoveSpeedBaseValue() const
{
	if (IsValid(LifeAttributes))
	{
		return LifeAttributes->GetMoveSpeedAttribute().GetGameplayAttributeData(LifeAttributes)->GetBaseValue();
	}

	return 0.0f;
}

float ACharacterBaseValid::GetXP() const
{
	if (IsValid(LifeAttributes))
	{
		return LifeAttributes->GetXP();
	}

	return 0.0f;
}

float ACharacterBaseValid::GetRewardXP() const
{
	if (IsValid(LifeAttributes))
	{
		return LifeAttributes->GetRewardXP();
	}

	return 0.0f;
}

float ACharacterBaseValid::GetCharacterLevel() const
{
	if (IsValid(LifeAttributes))
	{
		return LifeAttributes->GetLevel();
	}

	return 1.0f;
}
