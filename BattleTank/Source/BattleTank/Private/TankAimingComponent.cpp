// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleTank.h"
#include "TankTurret.h"
#include "TankBarrel.h"
#include "Projectile.h"
#include "TankAimingComponent.h"

UTankAimingComponent::UTankAimingComponent()
{
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
}

void UTankAimingComponent::BeginPlay()
{
	Super::BeginPlay();
	// So that first fire is after initial reload
	LastFireTime = FPlatformTime::Seconds();
}

void UTankAimingComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if ((FPlatformTime::Seconds() - LastFireTime) < ReloadTimeInSeconds)
	{
		FiringState = EFiringState::Reloading;
	}
	else if (IsBarrelMoving())
	{
		FiringState = EFiringState::Aiming;
	}
	else if (RoundsLeft < 1)
	{
		FiringState = EFiringState::OutOfAmmo;
	}
	else
	{
		FiringState = EFiringState::Locked;
	}
}

// Sets default values for this component's properties
void UTankAimingComponent::Initialise(UTankTurret* TurretToSet, UTankBarrel* BarrelToSet)
{
	Turret = TurretToSet;
	Barrel = BarrelToSet;
}

void UTankAimingComponent::AimAt(FVector HitLocation)
{
	if (!ensure(Turret && Barrel)) { return; }
	FVector OutLaunchVelocity;
	FVector ProjectileStart = Barrel->GetSocketLocation(FName("Projectile")); // returns location of barrel if it fails
    // Calculate the OutLaunchVelocity
	bool bHaveAimSolution = UGameplayStatics::SuggestProjectileVelocity
	(
		this,
		OutLaunchVelocity,
		ProjectileStart,
		HitLocation,
		LaunchSpeed,
		false, // this and following two could be removed, as they are set to the default values anyway
		0,
		0,
		ESuggestProjVelocityTraceOption::DoNotTrace
	);
	if (bHaveAimSolution)
	{
		FVector AimDirection = OutLaunchVelocity.GetSafeNormal();
		MoveBarrelTowards(AimDirection);
	}
}

void UTankAimingComponent::MoveBarrelTowards(FVector AimDirection)
{
	if (!ensure(Turret && Barrel)) { return; }
	this->AimDirection = AimDirection;
	// Work out difference between current barrel rotation and AimDirection
	auto BarrelRotator = Barrel->GetForwardVector().Rotation();
	auto AimAsRotator = AimDirection.Rotation();
	auto DeltaRotator = AimAsRotator - BarrelRotator;

	Barrel->Elevate(DeltaRotator.Pitch);
	// Always rotate the shortest distance
	if (FMath::Abs(DeltaRotator.Yaw) >= 180)
	{
		Turret->Rotate(DeltaRotator.Yaw * -1);
	}
	else
	{
		Turret->Rotate(DeltaRotator.Yaw);
	}
}

bool UTankAimingComponent::IsBarrelMoving() const
{
	if (!ensure(Barrel)) { return false; } // in other cases, you might want to return an enum

	auto CurrentDirection = Barrel->GetForwardVector();

	return !CurrentDirection.Equals(AimDirection, 0.1f);
}

void UTankAimingComponent::Fire()
{
	if (FiringState == EFiringState::Aiming || FiringState == EFiringState::Locked)
	{
		if (!Barrel) { return; }
		if (!ProjectileBlueprint) { return; }
		// Spawn a projectile at the socket location
		auto Projectile = GetWorld()->SpawnActor<AProjectile>(
			ProjectileBlueprint,
			Barrel->GetSocketLocation(FName("Projectile")),
			Barrel->GetSocketRotation(FName("Projectile"))
			);

		Projectile->Launch(LaunchSpeed);
		LastFireTime = FPlatformTime::Seconds();
		RoundsLeft--;
	}
}

EFiringState UTankAimingComponent::GetFiringState() const
{
	return FiringState;
}

int UTankAimingComponent::GetRoundsLeft() const
{
	return RoundsLeft;
}

