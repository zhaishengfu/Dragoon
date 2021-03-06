// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DragoonCharacter.h"
#include "EnemyAgent.generated.h"

/**
 * 
 */
UCLASS()
class DRAGOON_API AEnemyAgent : public ADragoonCharacter
{
	GENERATED_BODY()
	
public:
	// array of waypoints used to have agent patrol a certain path
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = PatrolState )
	TArray<FVector> waypoints;

	// position which a guard should protect
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = GuardState )
	FVector guardPost = FVector::ZeroVector;

	// should the agent wait once they arrive at waypoints
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = PatrolState )
	bool bIsPatrolContinuous = true;

	// particle system to spawn when the agent dies
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Particles )
	UParticleSystem* emitter;

private:
	/**
	 * A number to reflect the enemie's strength when joining the attack circle of a player
	 */
	int enemyScore;

	// Number to reflect the attack strength of a strong attack
	int strongAttackScore;
	// Number to reflect the attack strength of a quick attack
	int quickAttackScore;
	// Number to reflect the attack strength of a feint attack
	int feintAttackScore;
	// Whether agent is in combat/attack circle
	bool bIsInCombat = false;
	
public:
	// default c-tor needed for all UObject classes
	AEnemyAgent();
	/**
	 * Sets up agent to have specified enemy score
	 * @param score	Value to to which enemy score should be set
	 */
	AEnemyAgent( int score );

	/** Returns enemyScore **/
	UFUNCTION( BlueprintCallable, Category = EnemyAgent )
	FORCEINLINE int GetEnemyScore() const { return enemyScore; }

	/** Returns bIsInCombat **/
	UFUNCTION( BlueprintCallable, Category = EnemyAgent )
	FORCEINLINE bool GetIsInCombat() const { return bIsInCombat; }

	/**
	 * Wrapper function for equipping/unequipping sword
	 */
	void DrawSword();

	/**
	* Sets bIsInCombat to true and draws sword
	*/
	void JoinCombat();

	/**
	* Sets bIsInCombat to false and sheathes sword
	*/
	void LeaveCombat();

	/**
	 * Enemy agents overriden take damage function
	 */
	virtual void MyTakeDamage( int val ) override;

	/**
	 * Randomly choose which type of attack to perform.
	 */
	int ChooseAttack();

	/**
	* Call the respective attack function for attackToPerform
	* @param attackToPerform	The int score of the type of attack to be performed
	*/
	void PerformAttack( int attackToPerform );

	/**
	* Perform a parry in the specified direction
	*/
	void ParryAttack( EAttackDirection dir );

	/**
	* Get whether an action is in progress for this agent
	*/
	bool IsBusy();

	/**
	* Dodge in a direction determined by the supplied direction of incoming attack
	*/
	void DodgeAttack( EAttackDirection dir );

	/**
	 * Sends message to attack circle that current attack is finished
	 */ 
	virtual void FinishedAttacking() override;

protected:
	/**
	* Currently just calls parent's begin play event
	*/
	virtual void BeginPlay() override;

	virtual void BasicAttack() override;

	virtual void Tick( float deltaSeconds ) override;

	// get random attack direction
	virtual void AttackDirectionChosen() override;

	/**
	 * Removes agent from blackboard, disables collision, and spawns the particle emitter.
	 */
	UFUNCTION( BlueprintCallable, Category = EnemyAgent )
	void AgentDied();
};

// struct to represent attacks in the N-grams used by DragoonAIBlackboard
USTRUCT()
struct FAttack {
	GENERATED_BODY()

	UPROPERTY()
	EAttackDirection direction;	// holds the attack's direction

	UPROPERTY()
	EAttackType type;	// holds what kind of attack is being performed

	UPROPERTY()
	int id;	// id is derived from direction and type. Serves to uniquely identify each of the 27 possible attacks

	UPROPERTY()
	AEnemyAgent* target;	// enemy that is being attacked

	// default c-tor. needed for USTRUCT
	FAttack() {
		direction = EAttackDirection::AD_DownwardRightSlash;
		type = EAttackType::AT_Quick;
		id = NULL;
		target = nullptr;
	}

	/**
	 * Constructor to be used when creating attacks
	 * @param dir		the direction of the attack being performed
	 * @param atkType	the type of attack being performed (strong, quick, feint)
	 * @param enemy		the target of the attack being performed
	 */
	FAttack( EAttackDirection dir, EAttackType atkType, AEnemyAgent* enemy ) {
		direction = dir;
		type = atkType;
		id = ( uint8 )direction + ( uint8 )type;
		target = enemy;
	}
};
