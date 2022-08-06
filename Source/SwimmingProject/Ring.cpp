// Fill out your copyright notice in the Description page of Project Settings.


#include "Ring.h"

#include "SwimCharacter.h"
#include "SwimInstance.h"
#include "Components/BoxComponent.h"
#include "Sound/SoundBase.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ARing::ARing()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RingObj = CreateDefaultSubobject<UStaticMesh>(TEXT("Ring"));
	FVector NewRotation = GetActorLocation();
	SetActorRotation(FRotator(0, 0, 90));
	SetActorScale3D(FVector(0.7, 0.7, 0.7));
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> RingAsset(TEXT("StaticMesh'/Game/ImportedAssets/Ring.Ring'"));
	//UStaticMesh* Asset = RingAsset.Object;
	if(RingAsset.Object)
	{
		RingObj = RingAsset.Object;
	}
	//RingObj->SetStaticMesh(Asset);

	//BoxCollision->SetupAttachment(RootComponent);
	
}

// Called when the game starts or when spawned
void ARing::BeginPlay()
{
	Super::BeginPlay();

	BoxCollision = GetOwner()->FindComponentByClass<UBoxComponent>();
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ARing::BeginOverlap);
	
	
	//RingObj.Attach
}

// Called every frame
void ARing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARing::BeginOverlap(UPrimitiveComponent* OtherComp, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASwimCharacter* SwimCharacter = Cast<ASwimCharacter>(OtherActor);
	USwimInstance* GameInstance = Cast<USwimInstance>(GetGameInstance());

	if(SwimCharacter != nullptr)
	{
		SwimCharacter->CollectedRings++;
		GameInstance->InstanceRings = SwimCharacter->CollectedRings;
		UGameplayStatics::SpawnSoundAtLocation(this, CollectedFX, GetActorLocation());
		Destroy();
	}


	
}


