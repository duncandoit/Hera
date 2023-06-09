// Copyright Final Fall Games. All Rights Reserved.

#include "core/gas/abilities/base_ability.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayTagContainer.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

UAbilityBase::UAbilityBase()
{
	// Default to Instance Per Actor
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	// Default tags that block this ability from activating
	// ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Dead")));
	// ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun")));
}

void UAbilityBase::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilitySpec& Spec
)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (ActivateAbilityOnGranted)
	{
		ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
	}
}

bool UAbilityBase::HitscanTrace(
	FHitResult& OutHit,
	const FVector StartLocation, // World location
	const FVector Direction,
	const float Distance,        // Centemeters
	// const FCollisionQueryParams QueryParams,
	const bool bDrawDebug
) 
{
	const auto World = GetWorld();
	check(World);

	FColor DebugLineColor = FColor::Green;
	float ImpulseForce = 500.f;
	const FVector EndLocation = StartLocation + Direction * Distance;

	FCollisionQueryParams &Params = FCollisionQueryParams::DefaultQueryParam;
	FCollisionResponseParams& CollisionResponse = FCollisionResponseParams::DefaultResponseParam;

	bool bDidHit = World->LineTraceSingleByChannel(
		OutHit,                           // Hit result of the trace
		StartLocation,                    // Start vector
		EndLocation,                      // End vector
		ECollisionChannel::ECC_Visibility // Collision channel
		// QueryParams
	);
	
	if (bDidHit) 
	{
		if ((OutHit.bBlockingHit == true) && (OutHit.Component != nullptr))
		{
			// Only run on server
			// Note: Any mode < NM_Client is some type of server
			if (World->GetNetMode() < NM_Client)
			{
				if (const auto Component = OutHit.GetActor()->FindComponentByClass(
					UCharacterMovementComponent::StaticClass())
				) 
				{
					if (const auto MovementComponent = Cast<UCharacterMovementComponent>(Component))
					{
						Multicast_BoopCharacter(Direction, 500.f, MovementComponent);
					}
				}

				if (const auto MeshComponent = Cast<UStaticMeshComponent>(OutHit.GetActor()->GetRootComponent()))
				{
					Multicast_BoopStaticMesh(StartLocation, Direction, 500.f, MeshComponent);
				}
			}
		}
		else 
		{
			bDidHit = false;
			DebugLineColor = FColor::White;
		}
	}
	else 
	{
		DebugLineColor = FColor::White;
	}

	if (bDrawDebug)
	{
		DrawDebugLine(
			World,         // World
			StartLocation, // Start point
			EndLocation,   // End point
			DebugLineColor,// Color
			false,         // Is it persistent?
			5.f,           // Lifetime in seconds
			(uint8)0U, 
			0.3f           // Thickness
		);
	}

	return bDidHit;
}

void UAbilityBase::Multicast_BoopCharacter_Implementation(
	const FVector Direction, 
	const float Intensity, 
	UCharacterMovementComponent* MovementComponent
)
{
	float Mass = FMath::Max<float>(MovementComponent->Mass, 1);
	MovementComponent->AddImpulse((Direction * Intensity) / Mass);
}

void UAbilityBase::Multicast_BoopStaticMesh_Implementation(
	const FVector ImpactLocation,
	const FVector Direction, 
	const float Intensity, 
	UStaticMeshComponent* MeshComponent
)
{
	if (!MeshComponent->IsSimulatingPhysics())
	{
		return;
	}

	float Mass = FMath::Max<float>(MeshComponent->GetMass(), 1);
	MeshComponent->AddImpulseAtLocation(
		(Direction * Intensity) / Mass, 
		ImpactLocation
	);
}