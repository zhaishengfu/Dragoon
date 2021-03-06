// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "AttackCircle.h"
#include <deque>	// do not remove this include. this class uses deque containers
#include "EnemyAgent.h"
/**
 * 
 */
class DRAGOON_API DragoonAIBlackboard
{
public:

private:
	// a list of all registered agents who are not currently in the attack circle
	TArray<AEnemyAgent*> agentsNotInCombat;

	// a list of all registered agents who are in the attack circle
	TArray<AEnemyAgent*> agentsInCombat;

	// pointer to an already established instance of an attack circle
	AttackCircle* attackCircle;

	// 3D array that stores ints derived from attacks. Used for predicting attacks from previous patterns.
	int attackNGram[ 27 ][ 27 ][ 27 ];

	// deque to be used to store last x amount of attacks
	std::deque<int> attackHistory;	// deque is used so it can be randomly accessed

	// variables to hold the previous three attacks made indices
	int atk1 = 0, atk2 = 0, atk3 = 0;

	// maximum amount of attacks to store in attack history
	int maxHistorySize = 50;

	// bool to state if attacks need to be removed from the deque when new ones are added
	bool bIsHistoryFull = false;

	// whether the information from history should be added to prediction loop (cannot be true until 3 attacks are in history)
	bool bIsHistoryUsed = false;

	// how much recent history should account for
	int historyWeight = 2;

	// the probability of an unknown or random attack
	float predictionConfidence = 0.8f;

	// what attack is predicted. Uses the ID system of attacks as its value
	int nextAttackPrediction;

public:
	// default c-tor. not to be used.
	DragoonAIBlackboard();

	// C-tor that will set the attack circle to be an already established instance and set agent arrays to be empty.
	DragoonAIBlackboard( AttackCircle* circle );

	// Destructor that sets attack circle pointer to nullptr and empties arrays.
	~DragoonAIBlackboard();

	/**
	 * Adds an entry for an agent to the agents not in combat array.
	 * @param agent	pointer to an agent that is to be added to the blackboard
	 */
	void RegisterAgent( AEnemyAgent* agent );

	/**
	 * Removes a given agent from the blackboard's arrays. Will check for agent in both combat and non-combat arrays. Prints a log message if agent is not in either array.
	 * @param agent	pointer to agent that is to be removed from the blackboard
	 */
	void RemoveAgent( AEnemyAgent* agent );

	/**
	 * Removes agent from non-combat array and adds them to combat array. Calls the agent's JoinCombat function.
	 * @param agent	pointer to agent to join combat
	 */
	void HaveAgentJoinCombat( AEnemyAgent* agent );

	/**
	 * Removes agent from combat array and adds them to non-combat array.
	 * @param agent	pointer to agent to leave combat
	 */
	void HaveAgentFleeCombat( AEnemyAgent* agent );

	/**
	 * Adds the player's most recent attack to the NGram 3D array. Also sends information to AIController of targeted agent to have them respond to attack.
	 * @param atk	Struct containing the direction and type of attack being performed. Also sends target information of attack.
	 */
	void RecordPlayerAttack( FAttack atk );

	/**
	 * Let the AI Controller know that its agent has died
	 * @param agent	the agent who has died
	 */
	void AgentHasDied( AEnemyAgent* agent );

protected:
	/**
	* Algorithm to use attack occurences array and history deque to predict next attack
	*/
	void PredictNextAttack();
};
