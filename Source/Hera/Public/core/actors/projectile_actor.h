// Copyright Final Fall Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffect.h"
#include "projectile_actor.generated.h"

UCLASS()
class HERA_API AProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectileBase();

	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
	FGameplayEffectSpecHandle EffectSpecHandle;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovement;
};

UCLASS(config=Game)
class AHeraProjectile : public AProjectileBase
{
	GENERATED_BODY()

	/// Sphere collision component
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	class USphereComponent* CollisionComp;

public:
	AHeraProjectile();

	/// called when projectile hits something
	UFUNCTION()
	void OnHit(
		UPrimitiveComponent* HitComp, 
		AActor* OtherActor, UPrimitiveComponent* 
		OtherComp, FVector NormalImpulse, 
		const FHitResult& Hit
	);

	/// Returns CollisionComp Component
	USphereComponent* GetCollisionComp() const { return CollisionComp; }

	/// Returns ProjectileMovement Component
	// UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
	int damage;
};

