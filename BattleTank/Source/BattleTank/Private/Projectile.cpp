// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleTank.h"
#include "Projectile.h"


// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Collision Mesh"));
	SetRootComponent(CollisionMesh);
	// (Simulation Generates Hit Events in BP)
	CollisionMesh->SetNotifyRigidBodyCollision(true); // if a class is made inheriting from this one, this checkbox will be ticked by default 
	CollisionMesh->SetVisibility(false); 

	LaunchBlast = CreateDefaultSubobject<UParticleSystemComponent>(FName("Launch Blast"));
	// Attach it to the root component so that the mesh deposits smoke behind it
	LaunchBlast->AttachTo(RootComponent);

	ProjectileMovingComponent = CreateDefaultSubobject<UProjectileMovementComponent>(FName("Moving Component"));
	ProjectileMovingComponent->bAutoActivate = false;

	
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectile::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AProjectile::Launch(float Speed)
{
	ProjectileMovingComponent->SetVelocityInLocalSpace(FVector::ForwardVector * Speed); // forward vector is a unit vector
	ProjectileMovingComponent->Activate();
}

