// Fill out your copyright notice in the Description page of Project Settings.

#include "Magic.h"
#include "SpellSystem.h"
#include "MagicPlayerController.h"
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

/** Sets the owner of this spell system**/
void ASpellSystem::SetOwningPawn(AActor* NewOwner)
{
	if (Caster != NewOwner)
	{
		Instigator = Cast<APawn>(NewOwner);
		Caster = NewOwner;
		SetOwner(NewOwner);
	}
}

void ASpellSystem::AddSpell(int32 SpellID)
{
	bool Valid;

	//Get the Spell that was cast from the Database
	struct FSpellStruct Spell = USingletonFunctionLib::GetDatabase(Valid)->Spells[SpellID];

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
	{
		CurrentSpell = 0;
	}
	else
	{
		++CurrentSpell;
	}
}

void ASpellSystem::SelectPrevious()
{
	if (CurrentSpell - 1 < 0)
	{
		CurrentSpell = AvailableSpells.Num();
	}
	else
	{
		--CurrentSpell;
	}
}

void ASpellSystem::ReleaseSpell()
{
	if (CurrentSpellObject)
	{
		CurrentSpellObject->bRelease = true;
		bRelease = true;
	}
}

void ASpellSystem::CastSpell(USkeletalMeshComponent* CasterMesh, FRotator CastRotation,
	FVector CastPositionOffset, AActor* InTarget, bool bUseLiveRotation)
{
	bSuccess = false;
	Rotator = CastRotation;

	//If in Casting State or The Current Cooldown is still active then no cast.
	if (!bCasting && !IsCurrentSpellOnCooldown())
	{
		//Reset Release state and Set Casting state
		bRelease = false;
		bCasting = true;

		//Get the Struct of the current spell from the database.
		struct FSpellStruct TempCurrentSpell = GetCurrentSpell();

		//Set a new cooldown for the cast spell
		Cooldowns[CurrentSpell] = TempCurrentSpell.Cooldown;
		CastTime = TempCurrentSpell.CastTime;

		//Get The location of the spell by adding the cast position offset and spell offset
		FVector Location = (CastPositionOffset + TempCurrentSpell.CastOffset);
		Location = Rotator.RotateVector(Location);
		Location += Caster->GetActorLocation();

		FTransform SpawnTM(Rotator, Location);

		CurrentSpellObject = Cast<ASpellBase>(UGameplayStatics::BeginSpawningActorFromClass(this, TempCurrentSpell.SpellClass, SpawnTM, true));
		if (CurrentSpellObject)
		{
			CurrentSpellObject->Instigator = Instigator;
			CurrentSpellObject->SetOwner(this);
			CurrentSpellObject->bUseLiveRotation = bUseLiveRotation;
			CurrentSpellObject->SetTarget(InTarget);

			UGameplayStatics::FinishSpawningActor(CurrentSpellObject, SpawnTM);
			CurrentSpellObject->CastSpell(TempCurrentSpell);
		}

		if (CasterMesh)
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

FVector ASpellSystem::GetAdjustedAim() const
{
	AMagicPlayerController* const PlayerController = Instigator ? Cast<AMagicPlayerController>(Instigator->Controller) : NULL;
	FVector FinalAim = FVector::ZeroVector;

	if (PlayerController)
	{
		FVector CamLoc;
		FRotator CamRot;
		PlayerController->GetPlayerViewPoint(CamLoc, CamRot);
		FinalAim = CamRot.Vector();
	}

	return FinalAim;
}