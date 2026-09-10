#pragma once

#include "Library\GameObject.h"
#include "global.h"

enum EnemyState
{
	PATROL,	
	CHASE,		
	ATTACK,		
	SEARCH		
};

class Enemy :
	public GameObject
{
public:
	Enemy();
	~Enemy();
	void Update() override;
	void Draw() override;
private:
	void Patrol();
	void Chase();
	void Attack();
	void Search();
	float GetPlayerDistance();
	bool IsPlayerInSight();
	bool IsAttackRange();
	bool IsMovePossible(Point pos);
private:
	int hImage_;			
	Point pos_;			
	DIR dir_;			
	EnemyState state_;		
	float searchTimer_;	
	float attackTimer_;	
	float moveTimer_;	
};