// Copyright Final Fall Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffect.h"
#include "projectile_actor.generated.h"

// UCLASS()
// class HERA_API AProjectileBase : public AActor
// {
// 	GENERATED_BODY()
	
// public:	
// 	// Sets default values for this actor's properties
// 	AProjectileBase();

// 	UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta = (ExposeOnSpawn = true))
// 	float Range;

// 	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
// 	FGameplayEffectSpecHandle DamageEffectSpecHandle;

// 	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
// 	class UProjectileMovementComponent* ProjectileMovement;

// protected:
// 	// Called when the game starts or when spawned
// 	virtual void BeginPlay() override;
// };

class USphereComponent;
class UProjectileMovementComponent;

UCLASS(config=Game)
class AHeraProjectile : public AActor
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

public:
	AHeraProjectile();

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
	int damage;
};

