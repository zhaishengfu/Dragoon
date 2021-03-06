// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DragoonCharacter.h"
#include "EnemyAgent.h"


// enum for naming the 8 attack circle slots with regard to the player's orientation
UENUM()
enum class EAttackCircleSlot : uint8 {
	ACS_Front,
	ACS_FrontRight,
	ACS_Right,
	ACS_BackRight,
	ACS_Back,
	ACS_BackLeft,
	ACS_Left,
	ACS_FrontLeft
};

/**
 * 
 */
class DRAGOON_API AttackCircle
{
public:
	// Holds the maximum enemy score that can be within the circle at any given moment
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = AttackCircle )
	int maxEnemyScore = 10;

	// Holds the maximum enemy attack score that can be taking place within the circle at any given moment
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = AttackCircle )
	int maxAttackScore = 10;

	// Holds the amount in cm that the slots should be away from the center of the circle
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = AttackCircle )
	float offsetScale = 150;

private:
	// The score available to place new enemies in the circle
	UPROPERTY( VisibleAnywhere, Category = AttackCirlce )
	int availableEnemyScore;

	// The score available to start new attacks in the circle
	UPROPERTY( VisibleAnywhere, Category = AttackCircle )
	int availableAttackScore;

	// The origin of the attack circle which is based off the location of the player
	FVector centerOfCircle;

	// Pointer to the player
	UPROPERTY( VisibleAnywhere, Category = AttackCircle )
	ADragoonCharacter* player;

	// An array of all enemies currently within the circle
	UPROPERTY( VisibleAnywhere, Category = AttackCircle )
	TArray<AEnemyAgent*> enemiesInCircle;

	// A hashtable for keeping the pointer for which enemy is in each slot. Is set to nullptr if no enemy is in slot.
	UPROPERTY( VisibleAnywhere, Category = AttackCircle )
	TMap<EAttackCircleSlot, AEnemyAgent*> circleSlotOccupant;

	// A hashtable for keeping the offset values of the attack circle slots from the center of the circle
	UPROPERTY( VisibleAnywhere, Category = AttackCircle )
	TMap<EAttackCircleSlot, FVector> circleSlotOffset;

	// A hashtable for keeping if a slot is taken by an agent
	UPROPERTY( VisibleAnywhere, Category = AttackCircle )
	TMap<EAttackCircleSlot, bool> circleSlotOccupied;

public:
	AttackCircle();
	~AttackCircle();
	/**
	* Creates an instance of the AttackCircle class.
	* @param playerCharacter	a pointer to the DragoonCharacter that will be set as the player
	*/
	AttackCircle( ADragoonCharacter* playerCharacter );

	/** Returns availableAttackScore **/
	UFUNCTION( BlueprintCallable, Category = AttackCircle )
	FORCEINLINE int GetAvailableAttackScore() const { return availableAttackScore; }
	/** Returns availableEnemyScore **/
	UFUNCTION( BlueprintCallable, Category = AttackCircle )
	FORCEINLINE int GetAvailableEnemyScore() const { return availableEnemyScore; }
	/** Returns player **/
	UFUNCTION( BlueprintCallable, Category = AttackCircle )
	FORCEINLINE ADragoonCharacter* GetPlayer() const { return player; }
	/** Returns enemiesInCircle **/
	UFUNCTION( BlueprintCallable, Category = AttackCircle )
	FORCEINLINE TArray<AEnemyAgent*> GetEnemiesInCircle() const { return enemiesInCircle; }
	/** Returns centerOfCircle **/
	UFUNCTION( BlueprintCallable, Category = AttackCircle )
	FORCEINLINE FVector GetCenterOfCircle() const { return centerOfCircle; }

	/**
	* Handles request from an attacker to join the attack circle
	* @param attacker	ADragoonCharacter that wants to be able to attack the player
	*/
	UFUNCTION( BlueprintCallable, Category = AttackCircle )
	void JoinCircle( AEnemyAgent* attacker );

	/**
	* Sets centerOfCirlce to be the same as the player's location
	*/
	void UpdateCircleLocation();

	/**
	* Sets the slot with the agent as an occupant to have a nullptr occupant.
	* @param agent	Which ADragoonCharacter* you wish to remove from the attack circle
	*/
	void RemoveAgentFromCircle( AEnemyAgent* agent );

	/**
	* Returns the location of the slot to which an agent is assigned
	* @param agent	ADragoonCharacter who needs to know their slot's location
	*/
	FVector GetLocationForAgent( AEnemyAgent* agent );

	/**
	* Checks if the agent's requested attack with score is able to be used currently.
	* @param attackScore	The score associated with the attack an agent wishes to make
	* @returns	Will return true if the attack can be made with the current available attack score, or false if it cannot be made
	*/
	UFUNCTION( BlueprintCallable, Category = AttackCircle )
	bool CanAgentPerformAttack( int attackScore );

	/**
	* Returns the score from a finished attack back to the available attack score
	* @param attackScore	The score associated with the attack that has completed
	*/
	UFUNCTION( BlueprintCallable, Category = AttackCircle )
	void AgentAttackFinished( int attackScore );

	/**
	* Sets the player to the supplied pointer
	*/
	UFUNCTION( BlueprintCallable, Category = AttackCircle )
	void SetPlayer( ADragoonCharacter* newPlayer );

	/**
	* Performs initial setup of member variables and TMaps. Requires player to be set before it will run.
	*/
	UFUNCTION( BlueprintCallable, Category = AttackCircle )
	void Initialize();

	/**
	 * Assigns a new slot to an agent already in the attack circle.
	 */
	void GetNewSlotForAgent( AEnemyAgent* agent );

	/**
	* Checks if the circle has space for a requesting agent to join.
	* @param agent	Agent wishing to join circle
	* @returns		Will return true if the agent can join the circle, or false if they are unable
	*/
	UFUNCTION( BlueprintCallable, Category = AttackCircle )
	bool CanAgentJoinCircle( AEnemyAgent* agent );

private:
	/**
	* Compares the distance squared of all the empty attack circle slots and returns the closest one.
	* @param requester	pointer to the enemy requesting to join the attack circle
	* @return	returns the enum value for the closest, empty slot
	*/
	EAttackCircleSlot CheckForClosestAvailableSlot( AEnemyAgent* requester );

	/**
	* Sets circleSlotOccupant value for Key slot to be agent
	* @param agent	pointer to ADragoonCharacter that wants to join circle
	* @param slot	The slot enum which is to be assigned
	*/
	void AssignAgentToSlot( AEnemyAgent* agent, EAttackCircleSlot slot );
};
