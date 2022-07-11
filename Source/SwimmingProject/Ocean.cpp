// Fill out your copyright notice in the Description page of Project Settings.


#include "Ocean.h"

#include "Kismet/KismetSystemLibrary.h"

/*AOcean::AOcean()
{

	
}*/



// Implement EnterWater from Interface
bool AOcean::EnterWater_Implementation()
{
	
	return false;
};

bool AOcean::ExitWater_Implementation()
{
	
	return false;
};

// When player overlaps the water, "Entered water + Player"
void AOcean::BeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FString ActorName = UKismetSystemLibrary::GetDisplayName(OtherActor);
	printf("Entered water");
	AppendName(ActorName);
	EnterWater_Implementation();
};

void AOcean::EndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	FString ActorName = UKismetSystemLibrary::GetDisplayName(OtherActor);
	printf("Exited water");
	AppendName(ActorName);
	ExitWater_Implementation();
}


