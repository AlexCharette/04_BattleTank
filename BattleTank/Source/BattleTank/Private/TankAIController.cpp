// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleTank.h"
#include "Tank.h"
#include "TankAIController.h"

void ATankAIController::BeginPlay()
{
	Super::BeginPlay();

}

void ATankAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ATank* PlayerTank = Cast<ATank>(GetWorld()->GetFirstPlayerController()->GetPawn());

	if (PlayerTank)
	{
		// TODO Move towards the player

		// Aim at the player
		ATank* PossessedTank = Cast<ATank>(GetPawn());

		PossessedTank->AimAt(PlayerTank->GetActorLocation());
		PossessedTank->Fire(); // TODO Limit firing rate
	}
}


