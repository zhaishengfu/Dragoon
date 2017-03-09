// Fill out your copyright notice in the Description page of Project Settings.

#include "Dragoon.h"
#include "DragoonAIController.h"
#include "DragoonAIBlackboard.h"

DragoonAIBlackboard::DragoonAIBlackboard()
{
	// initialize arrays to be empty
	agentsInCombat.Empty();
	agentsNotInCombat.Empty();
}

DragoonAIBlackboard::DragoonAIBlackboard( AttackCircle* circle ) {
	// initialize arrays to be empty
	agentsInCombat.Empty();
	agentsNotInCombat.Empty();

	// set attack circle reference
	attackCircle = circle;
}

DragoonAIBlackboard::~DragoonAIBlackboard()
{
	// remove pointer reference
	attackCircle = nullptr;

	// empty arrays of references
	agentsInCombat.Empty();
	agentsNotInCombat.Empty();
}

void DragoonAIBlackboard::RegisterAgent( AEnemyAgent* agent ) {
	// check to make sure there aren't multiple references to the same agent in either array
	if ( agentsNotInCombat.Contains( agent ) || agentsInCombat.Contains( agent ) )
		return;
	else
		agentsNotInCombat.Add( agent );	// put agent in non-combat array
}

void DragoonAIBlackboard::RemoveAgent( AEnemyAgent* agent ) {
	// check through arrays for agent and remove from containing array
	if ( agentsInCombat.Find( agent ) )
		agentsInCombat.Remove( agent );
	else if ( agentsNotInCombat.Find( agent ) )
		agentsNotInCombat.Remove( agent );
	else
		UE_LOG( LogTemp, Error, TEXT( "Agent %s requesting removal from AI Blackboard is not registered in the blackboard!" ), *agent->GetName() );	// print error to log if an unregistered agent makes the request
}

void DragoonAIBlackboard::HaveAgentJoinCombat( AEnemyAgent* agent ) {
	// checks for agent to have valid state
	if ( agent->GetIsDead() || agent->GetIsInCombat() || agentsInCombat.Contains( agent ) || !agentsNotInCombat.Contains( agent ) )
		return;
	else {
		// change the agents reference to be in combat array
		RemoveAgent( agent );
		agentsInCombat.Add( agent );
		agent->JoinCombat();	// call agents function to perform any agent specific behavior needed for joining combat
	}
}

void DragoonAIBlackboard::HaveAgentFleeCombat( AEnemyAgent* agent ) {
	// checks for agent to have valid state
	if ( agent->GetIsDead() || !agent->GetIsInCombat() || !agentsInCombat.Contains( agent ) || agentsNotInCombat.Contains( agent ) )
		return;
	else {
		// swap agents reference to be in non-combat array
		RemoveAgent( agent );
		RegisterAgent( agent );
		agent->LeaveCombat();	// call agents function to perform any agent specific behavior for leaving combat
	}
}

void DragoonAIBlackboard::RecordPlayerAttack( FAttack atk ) {
	// make sure an active agent is in combat
	if ( agentsInCombat.Num() == 0 )
		return;

	// cycle attacks forward one slot. put most recent attack in atk3. The order is important as contents are being overwritten
	atk1 = atk2;
	atk2 = atk3;
	atk3 = atk.id;

	// debug logging for testing
	UE_LOG( LogTemp, Warning, TEXT( "Player is attacking %s" ), *atk.target->GetName() );

	// update the n gram array to have another reference to attack sequence
	attackNGram[ atk1 ][ atk2 ][ atk3 ]++;

	// update history deque with new attack
	if ( !bIsHistoryFull ) {
		attackHistory.push_back( atk3 );
		// check if history is now at our max size
		if ( attackHistory.size() == maxHistorySize )
			bIsHistoryFull = true;
	}
	else {
		// remove oldest entry (will be the first entry as we add to the back), and add new attack to back of history
		attackHistory.pop_front();
		attackHistory.push_back( atk3 );
	}

	// call agent behavior to respond to attack if it is in combat
	if ( agentsInCombat.Contains( atk.target ) ) {
		// make target react to incoming attack
		ADragoonAIController* AIController = (ADragoonAIController*)atk.target->GetController();
		// testing access to ai controller. needs to be updated with actual logic for reacting to attacks
	}
}

void DragoonAIBlackboard::AgentHasDied( AEnemyAgent* agent ) {
	// notify controller that agent has died
	ADragoonAIController* AIController = ( ADragoonAIController* )agent->GetController();
	AIController->AgentHasDied();
}

void DragoonAIBlackboard::PredictNextAttack() {
	if ( !bIsHistoryUsed ) {
		if ( attackHistory.size() >= 3 )
			bIsHistoryUsed = true;
	}
	else {
		// map to contain attack IDs and the amount of times they appear in recent history and in total
		TMap<int, int> historyOccurences;

		// check for matching patterns to last two attacks
		for ( int i = 0; i < attackHistory.size() - 2; i++ ) {
			if ( attackHistory[ i ] == atk2 ) {
				if ( attackHistory[ i + 1 ] == atk3 ) {
					// add an occurence for the next value in history to the map
					if ( historyOccurences.Contains( attackHistory[ i + 2 ] ) )
						historyOccurences[ attackHistory[ i + 2 ] ]++;
					else
						historyOccurences.Add( attackHistory[ i + 2 ], 1 );
				}
			}
		}

		// map to contain attack occurences from the entire playtime
		TMap<int, int> cumulativeAttackOccurences;
		for ( int i = 0; i < 27; i++ ) {
			// add any attack that has more than 0 occurences to map
			if ( attackNGram[ atk2 ][ atk3 ][ i ] > 0 )
				cumulativeAttackOccurences.Add( i, attackNGram[ atk2 ][ atk3 ][ i ] );
		}


	}
}
