// Fill out your copyright notice in the Description page of Project Settings.

#include "Magic.h"
#include "SpellSystem.h"
#include "SingletonFunctionLib.h"

// Sets default values
ASpellSystem::ASpellSystem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CurrentSpell = 0;
	CastTime = 0;
	bCasting = false;
	bRelease = true;
}

// Called when the game starts or when spawned
void ASpellSystem::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASpellSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Reduce all cooldowns by elapsed time since last frame
	for (auto& Cooldown : Cooldowns)
	{
		if (Cooldown > 0.f)
		{
			Cooldown -= DeltaTime;
		}
	}

	// If the cast time is still greater than 0 then delta elapsed time
	if (CastTime > 0)
	{
		CastTime -= DeltaTime;
	}
	else
	{
		bCasting = !bRelease;
	}

}

void ASpellSystem::AddSpell(int32 SpellID)
{
	bool Valid;

	//Get the Spell that was cast from the Database
	FSpellStruct Spell = USingletonFunctionLib::GetDatabase(Valid)->Spells[SpellID];

	//Add Current Spell to available spells list and add a cooldown slot
	AvailableSpells.Emplace(Spell);
	Cooldowns.Emplace(0.f);
}

void ASpellSystem::RemoveSpell()
{
	AvailableSpells.RemoveAt(CurrentSpell);
	Cooldowns.RemoveAt(CurrentSpell);
}

void ASpellSystem::CancelSpellCast()
{
	if (bCasting)
	{
		CurrentSpellObject->Destroy();
		bCasting = false;
		CastTime = 0.f;
	}
}

void ASpellSystem::SelectNext()
{
	if (CurrentSpell + 1 > AvailableSpells.Num())
		CurrentSpell = 0;
	else
		++CurrentSpell;
}

void ASpellSystem::SelectPrevious()
{
	if (CurrentSpell - 1 < 0)
		CurrentSpell = AvailableSpells.Num();
	else 
		--CurrentSpell;
}

void ASpellSystem::ReleaseSpell()
{
	if (CurrentSpellObject != NULL)
	{
		CurrentSpellObject->bRelease = true;
		bRelease = true;
	}
}

void ASpellSystem::InitSpellCast(FVector CastPositionOffset, FVector& Location, FSpellStruct& SpellStruct)
{
	//Reset Release state and Set Casting state
	bRelease = false;
	bCasting = true;

	//Get the Struct of the current spell from the database.
	SpellStruct = GetCurrentSpell();

	//Set a new cooldown for the cast spell
	Cooldowns[CurrentSpell] = SpellStruct.Cooldown;
	CastTime = SpellStruct.CastTime;

	//Get The location of the spell by adding the cast position offset and spell offset
	Location = (CastPositionOffset + SpellStruct.CastOffset);
	//Rotate the spell forward
	Location = Rotator.RotateVector(Location);
	//Add to the actor location
	Location += Caster->GetActorLocation();
}

void ASpellSystem::Cast(AActor* InCaster, USkeletalMeshComponent* CasterMesh, FRotator CastRotation,
	FVector CastPositionOffset, AActor* InTarget, bool bUseLiveRotation)
{
	bSuccess = false;
	Caster = InCaster;
	Rotator = CastRotation;

	//If in Casting State or The Current Cooldown is still active then no cast.
	if (!bCasting && !IsCurrentSpellOnCooldown())
	{

		UE_LOG(LogTemp, Warning, TEXT("Cast confirmed."));

		FVector Location;
		FSpellStruct TempCurrentSpell;
		InitSpellCast(CastPositionOffset, Location, TempCurrentSpell);

		//Set Spawn Parameters
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = InCaster;
		SpawnParams.Name = TempCurrentSpell.Name;

		CurrentSpellObject = GetWorld()->SpawnActor<ASpellBase>(TempCurrentSpell.SpellClass, Location, Rotator, SpawnParams);
		CurrentSpellObject->bUseLiveRotation = bUseLiveRotation;
		CurrentSpellObject->SetTarget(InTarget);
		CurrentSpellObject->Cast(TempCurrentSpell);

		if (CasterMesh != NULL)
		{
			CurrentSpellObject->AttachRootComponentTo(CasterMesh, NAME_None, EAttachLocation::KeepWorldPosition);
		}

		bSuccess = (CurrentSpellObject != NULL);
		LastSpell = TempCurrentSpell;
	}
}

bool ASpellSystem::IsCurrentSpellOnCooldown() const
{
	if (Cooldowns.Num() < 0)
	{
		return false;
	}

	return Cooldowns[CurrentSpell] > 0.f;
}