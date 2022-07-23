// Fill out your copyright notice in the Description page of Project Settings.


#include "Ocean.h"

#include "Kismet/KismetSystemLibrary.h"
/*
AOcean::AOcean()
{

}*/

/*
void AOcean::EnterWater_Implementation()
{
	
}

void AOcean::ExitWater_Implementation()
{
	
}
*/


// When player overlaps the water, "Entered water + Player"
void AOcean::BeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Entered water"));
	FString ActorName = UKismetSystemLibrary::GetDisplayName(OtherActor);
	printf("Entered water");
	AppendName(ActorName);
	//EnterWater_Implementation();

	//EnterWaterMessage();
};

void AOcean::EndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	FString ActorName = UKismetSystemLibrary::GetDisplayName(OtherActor);
	printf("Exited water");
	AppendName(ActorName);
	//ExitWater_Implementation();
};

/*
void AOcean::ActorBeginOverlap(AActor* OtherActor)
{
	FString ActorName = UKismetSystemLibrary::GetDisplayName(OtherActor);
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, ActorName);
	EnterWater_Implementation();
};
*/
/*
void AOcean::EnterWaterMessage()
{
	EnterWater();
}*/

void AOcean::EnterWater_Implementation()
{
	EnterWater();
}



