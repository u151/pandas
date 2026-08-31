#pragma once

#include "Library\GameObject.h"
#include "global.h"

enum EnemyState
{
	PATROL,		// 巡回
	CHASE,		// 追跡
	ATTACK,		// 攻撃
	SEARCH		// 探索
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
	// 各ステートの処理
	void Patrol();
	void Chase();
	void Attack();
	void Search();

	// プレイヤーとの距離
	float GetPlayerDistance();

	// プレイヤーが視界にいるか
	bool IsPlayerInSight();

	// 攻撃距離にいるか
	bool IsAttackRange();

	// マップが通れるか
	bool IsMovePossible(Point pos);

private:
	int hImage_;			// 画像ID
	Point pos_;				// 敵の座標
	DIR dir_;				// 敵の方向

	EnemyState state_;		// 現在のステート

	float searchTimer_;		// 探索タイマー
	float attackTimer_;		// 攻撃タイマー
	float moveTimer_;		// 移動タイマー
};