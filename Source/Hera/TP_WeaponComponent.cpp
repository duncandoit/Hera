// Copyright Epic Games, Inc. All Rights Reserved.


#include "TP_WeaponComponent.h"
#include "HeraCharacter.h"
#include "HeraProjectile.h"
#include "HeraUtil.h"

#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"

// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent()
{
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
}


void UTP_WeaponComponent::Fire()
{
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}

	// Try and fire a projectile
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			// APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
			// const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			// const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);
			//
			//Set Spawn Collision Handling Override
			// FActorSpawnParameters ActorSpawnParams;
			// ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
			//
			// Spawn the projectile at the muzzle
			// World->SpawnActor<AHeraProjectile>(
			// 	ProjectileClass, 
			// 	SpawnLocation, 
			// 	SpawnRotation, 	
			// 	ActorSpawnParams
			// );



			// TODO: Make into seperate Components
			//		- ImpulseComponent
			//		- LineTraceComponent
			//
			// Do line trace for hitscan impact
			FColor DebugLineColor = FColor:: Yellow;
			float TraceDistance = 1000.f;
			float ImpulseForce = 1000.f;
			const UCameraComponent& CharCam = *Character->GetFirstPersonCameraComponent();
			const FVector TraceStart = CharCam.GetComponentLocation();
			const FVector TraceEnd = TraceStart + CharCam.GetForwardVector() * TraceDistance;
			FHitResult Hit;
			const bool bDidHit = World->LineTraceSingleByChannel(
				Hit,							// Hit result of the trace
				TraceStart,					// Start vector
				TraceEnd,						// End vector
				ECollisionChannel::ECC_Visibility	// Collision channel
			);
			
			if (bDidHit) 
			{
				if ((Hit.bBlockingHit == true) && (Hit.Component != nullptr))
				{
					if (Hit.Component->IsSimulatingPhysics())
					{
						UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent>(Hit.GetActor()->GetRootComponent());

						// Hit.Component->AddImpulseAtLocation(Character->GetVelocity() + 100.f, Hit.ImpactPoint);
						MeshComponent->AddImpulse(CharCam.GetForwardVector() * ImpulseForce * MeshComponent->GetMass());
						UHeraUtil::DebugPrint("Line trace hit something physical", DebugLineColor);

						UHeraUtil::DebugPrint(Hit.Component->GetName(), DebugLineColor);
					}
					else 
					{
						UHeraUtil::DebugPrint("Line trace hit something static", DebugLineColor);
					}
				}
				else 
				{
					DebugLineColor = FColor::White;
					UHeraUtil::DebugPrint("Line trace succeeded but didn't make contact", DebugLineColor);
				}
			}
			else 
			{
				DebugLineColor = FColor::Red;
				UHeraUtil::DebugPrint("Line trace fail", DebugLineColor);
			}

			DrawDebugLine(
				World, 			// World
				TraceStart, 		// Start point
				TraceEnd, 		// End point
				DebugLineColor, 	// Color
				false, 			// Is it persistent?
				1.f, 			// LifeTime
				(uint8)0U, 
				1.f				// Thickness
			);
		}
	}
	
	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}
	
	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void UTP_WeaponComponent::AttachWeapon(AHeraCharacter* TargetCharacter)
{
	Character = TargetCharacter;
	if (Character == nullptr)
	{
		return;
	}

	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));
	
	// switch bHasRifle so the animation blueprint can switch to another animation set
	Character->SetHasRifle(true);

	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// Fire
			EnhancedInputComponent->BindAction(
				FireAction,
				ETriggerEvent::Triggered, 
				this, 
				&UTP_WeaponComponent::Fire
			);
		}
	}
}

void UTP_WeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Character == nullptr)
	{
		return;
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}