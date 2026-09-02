#include "Enemy.h"
#include "time.h"
#include "Stage.h"
#include "Player.h"
#include <cmath>


namespace
{
	//========================================
	// 敵画像
	//========================================

	const int ENEMY_IMAGE_SIZE = 48;

	//========================================
	// 敵の初期位置
	//========================================

	const Point ENEMY_INIT_POS =
	{
		20 * CHA_SIZE,
		10 * CHA_SIZE
	};

	//========================================
	// 敵の初期方向
	//========================================

	const DIR ENEMY_INIT_DIR = LEFT;

	//========================================
	// 敵の描画サイズ
	//========================================

	const int ENEMY_DRAW_SIZE = CHA_SIZE;

	//========================================
	// アニメーション
	//========================================

	const int animFrame[4] =
	{
		0, 1, 2, 1
	};

	const float ANIM_INTERVAL = 0.2f;

	//========================================
	// 視界
	// 8マス以内
	//========================================

	const float SIGHT_DISTANCE =
		8.0f * CHA_SIZE;

	//========================================
	// 攻撃距離
	// 2マス以内
	//========================================

	const float ATTACK_DISTANCE =
		2.0f * CHA_SIZE;

	//========================================
	// 探索時間
	//========================================

	const float SEARCH_TIME = 5.0f;

	//========================================
	// 移動間隔
	//========================================

	const float MOVE_INTERVAL = 0.2f;
}


//========================================
// コンストラクタ
//========================================

Enemy::Enemy()
	: GameObject()
{
	hImage_ = LoadGraph("Assets/panda_R.png");

	pos_ = ENEMY_INIT_POS;
	dir_ = ENEMY_INIT_DIR;

	// 最初は巡回
	state_ = PATROL;

	searchTimer_ = 0.0f;
	attackTimer_ = 0.0f;

	// 最初のUpdateですぐ動けるようにする
	moveTimer_ = 0.0f;
}


//========================================
// デストラクタ
//========================================

Enemy::~Enemy()
{
}


//========================================
// Update
//========================================

void Enemy::Update()
{
	switch (state_)
	{
	case PATROL:
		Patrol();
		break;

	case CHASE:
		Chase();
		break;

	case ATTACK:
		Attack();
		break;

	case SEARCH:
		Search();
		break;

	default:
		state_ = PATROL;
		break;
	}
}


//========================================
// Patrol
// 巡回
//========================================

void Enemy::Patrol()
{
	//====================================
	// プレイヤーを発見
	//====================================

	if (IsPlayerInSight())
	{
		state_ = CHASE;
		moveTimer_ = 0.0f;
		return;
	}

	//====================================
	// 移動タイマー
	//====================================

	moveTimer_ -= Time::DeltaTime();

	if (moveTimer_ > 0.0f)
	{
		return;
	}

	//====================================
	// 前と右を調べる
	//====================================

	Point frontPos = pos_;
	Point rightPos = pos_;

	switch (dir_)
	{
	case UP:
		frontPos.y -= CHA_SIZE;
		rightPos.x += CHA_SIZE;
		break;

	case RIGHT:
		frontPos.x += CHA_SIZE;
		rightPos.y += CHA_SIZE;
		break;

	case DOWN:
		frontPos.y += CHA_SIZE;
		rightPos.x -= CHA_SIZE;
		break;

	case LEFT:
		frontPos.x -= CHA_SIZE;
		rightPos.y -= CHA_SIZE;
		break;
	}

	//====================================
	// 右側が空いていれば右へ曲がる
	//====================================

	if (IsMovePossible(rightPos))
	{
		switch (dir_)
		{
		case UP:
			dir_ = RIGHT;
			break;

		case RIGHT:
			dir_ = DOWN;
			break;

		case DOWN:
			dir_ = LEFT;
			break;

		case LEFT:
			dir_ = UP;
			break;
		}

		pos_ = rightPos;
	}
	//====================================
	// 前が空いていれば前進
	//====================================

	else if (IsMovePossible(frontPos))
	{
		pos_ = frontPos;
	}
	//====================================
	// 前も右も壁なら左へ方向転換
	//====================================

	else
	{
		switch (dir_)
		{
		case UP:
			dir_ = LEFT;
			break;

		case LEFT:
			dir_ = DOWN;
			break;

		case DOWN:
			dir_ = RIGHT;
			break;

		case RIGHT:
			dir_ = UP;
			break;
		}
	}

	moveTimer_ = MOVE_INTERVAL;
}


//========================================
// Chase
// 追跡
//========================================

void Enemy::Chase()
{
	Player* player = FindGameObject<Player>();

	if (player == nullptr)
	{
		state_ = PATROL;
		return;
	}

	//====================================
	// 攻撃距離に入った
	//====================================

	if (IsAttackRange())
	{
		state_ = ATTACK;
		attackTimer_ = 0.0f;
		return;
	}

	//====================================
	// プレイヤーを見失った
	//====================================

	if (!IsPlayerInSight())
	{
		state_ = SEARCH;
		searchTimer_ = 0.0f;
		moveTimer_ = 0.0f;
		return;
	}

	//====================================
	// 移動タイマー
	//====================================

	moveTimer_ -= Time::DeltaTime();

	if (moveTimer_ > 0.0f)
	{
		return;
	}

	Point playerPos = player->GetPosition();

	int dx = playerPos.x - pos_.x;
	int dy = playerPos.y - pos_.y;

	//====================================
	// X方向・Y方向の移動候補
	//====================================

	Point xPos = pos_;
	Point yPos = pos_;

	if (dx > 0)
	{
		xPos.x += CHA_SIZE;
	}
	else if (dx < 0)
	{
		xPos.x -= CHA_SIZE;
	}

	if (dy > 0)
	{
		yPos.y += CHA_SIZE;
	}
	else if (dy < 0)
	{
		yPos.y -= CHA_SIZE;
	}

	bool moved = false;

	//====================================
	// 距離が大きい方向を優先
	//====================================

	if (std::abs(dx) >= std::abs(dy))
	{
		// X方向
		if (dx != 0 && IsMovePossible(xPos))
		{
			pos_ = xPos;
			moved = true;

			if (dx > 0)
			{
				dir_ = RIGHT;
			}
			else
			{
				dir_ = LEFT;
			}
		}
		// Xが壁ならY方向
		else if (dy != 0 && IsMovePossible(yPos))
		{
			pos_ = yPos;
			moved = true;

			if (dy > 0)
			{
				dir_ = DOWN;
			}
			else
			{
				dir_ = UP;
			}
		}
	}
	else
	{
		// Y方向
		if (dy != 0 && IsMovePossible(yPos))
		{
			pos_ = yPos;
			moved = true;

			if (dy > 0)
			{
				dir_ = DOWN;
			}
			else
			{
				dir_ = UP;
			}
		}
		// Yが壁ならX方向
		else if (dx != 0 && IsMovePossible(xPos))
		{
			pos_ = xPos;
			moved = true;

			if (dx > 0)
			{
				dir_ = RIGHT;
			}
			else
			{
				dir_ = LEFT;
			}
		}
	}

	//====================================
	// どちらにも進めなかった場合
	//====================================

	if (!moved)
	{
		// 現在の方向を変えて次回再挑戦
		switch (dir_)
		{
		case UP:
			dir_ = RIGHT;
			break;

		case RIGHT:
			dir_ = DOWN;
			break;

		case DOWN:
			dir_ = LEFT;
			break;

		case LEFT:
			dir_ = UP;
			break;
		}
	}

	moveTimer_ = MOVE_INTERVAL;
}


//========================================
// Attack
// 攻撃
//========================================

void Enemy::Attack()
{
	//====================================
	// プレイヤーが攻撃距離から離れた
	//====================================

	if (!IsAttackRange())
	{
		// まだ視界内なら追跡
		if (IsPlayerInSight())
		{
			state_ = CHASE;
		}
		// 視界からも消えたら探索
		else
		{
			state_ = SEARCH;
			searchTimer_ = 0.0f;
		}

		return;
	}

	//====================================
	// 攻撃タイマー
	//====================================

	attackTimer_ -= Time::DeltaTime();

	if (attackTimer_ <= 0.0f)
	{
		//================================
		// ここに攻撃処理を追加する
		//================================

		attackTimer_ = 1.0f;
	}
}


//========================================
// Search
// 探索
//========================================

void Enemy::Search()
{
	//====================================
	// プレイヤーを再発見
	//====================================

	if (IsPlayerInSight())
	{
		// ★重要
		// 再発見しただけでは攻撃しない
		// 攻撃距離ならChaseからAttackへ進む
		state_ = CHASE;

		searchTimer_ = 0.0f;
		moveTimer_ = 0.0f;

		return;
	}

	//====================================
	// 探索時間
	//====================================

	searchTimer_ += Time::DeltaTime();

	if (searchTimer_ >= SEARCH_TIME)
	{
		// 5秒見つからなければ巡回へ
		state_ = PATROL;

		searchTimer_ = 0.0f;
		moveTimer_ = 0.0f;

		return;
	}

	//====================================
	// 探索移動
	//====================================

	moveTimer_ -= Time::DeltaTime();

	if (moveTimer_ > 0.0f)
	{
		return;
	}

	//====================================
	// 方向を変更
	//====================================

	switch (dir_)
	{
	case UP:
		dir_ = RIGHT;
		break;

	case RIGHT:
		dir_ = DOWN;
		break;

	case DOWN:
		dir_ = LEFT;
		break;

	case LEFT:
		dir_ = UP;
		break;
	}

	//====================================
	// 新しい移動先
	//====================================

	Point newPos = pos_;

	switch (dir_)
	{
	case UP:
		newPos.y -= CHA_SIZE;
		break;

	case DOWN:
		newPos.y += CHA_SIZE;
		break;

	case LEFT:
		newPos.x -= CHA_SIZE;
		break;

	case RIGHT:
		newPos.x += CHA_SIZE;
		break;
	}

	//====================================
	// 移動できる場合だけ移動
	//====================================

	if (IsMovePossible(newPos))
	{
		pos_ = newPos;
	}

	moveTimer_ = MOVE_INTERVAL;
}


//========================================
// プレイヤーとの距離
//========================================

float Enemy::GetPlayerDistance()
{
	Player* player = FindGameObject<Player>();

	if (player == nullptr)
	{
		return 99999.0f;
	}

	Point playerPos = player->GetPosition();

	float dx =
		static_cast<float>(playerPos.x) -
		static_cast<float>(pos_.x);

	float dy =
		static_cast<float>(playerPos.y) -
		static_cast<float>(pos_.y);

	return std::sqrt(dx * dx + dy * dy);
}


//========================================
// 視界判定
//========================================

bool Enemy::IsPlayerInSight()
{
	return GetPlayerDistance() <= SIGHT_DISTANCE;
}


//========================================
// 攻撃距離判定
//========================================

bool Enemy::IsAttackRange()
{
	return GetPlayerDistance() <= ATTACK_DISTANCE;
}


//========================================
// 移動可能判定
//========================================

bool Enemy::IsMovePossible(Point pos)
{
	//====================================
	// ステージの範囲外
	//====================================

	int mapX = pos.x / CHA_SIZE;
	int mapY = pos.y / CHA_SIZE;

	if (mapX < 0 ||
		mapX >= STAGE_WIDTH ||
		mapY < 0 ||
		mapY >= STAGE_HEIGHT)
	{
		return false;
	}

	//====================================
	// Stage取得
	//====================================

	Stage* stage = FindGameObject<Stage>();

	if (stage == nullptr)
	{
		return false;
	}

	//====================================
	// マップ取得
	//
	// 0 = 通路
	// 1 = 壁
	//====================================

	int mapValue = stage->GetMap(mapX, mapY);

	if (mapValue == 1)
	{
		return false;
	}

	return true;
}


//========================================
// Draw
//========================================

void Enemy::Draw()
{
	static float animTimer = ANIM_INTERVAL;
	static int frame = 0;

	int nowFrame = animFrame[frame];

	//====================================
	// 画像の切り出し位置
	//====================================

	Rect iRect[4] =
	{
		{
			nowFrame * ENEMY_IMAGE_SIZE,
			3 * ENEMY_IMAGE_SIZE,
			ENEMY_IMAGE_SIZE,
			ENEMY_IMAGE_SIZE
		},

		{
			nowFrame * ENEMY_IMAGE_SIZE,
			0 * ENEMY_IMAGE_SIZE,
			ENEMY_IMAGE_SIZE,
			ENEMY_IMAGE_SIZE
		},

		{
			nowFrame * ENEMY_IMAGE_SIZE,
			1 * ENEMY_IMAGE_SIZE,
			ENEMY_IMAGE_SIZE,
			ENEMY_IMAGE_SIZE
		},

		{
			nowFrame * ENEMY_IMAGE_SIZE,
			2 * ENEMY_IMAGE_SIZE,
			ENEMY_IMAGE_SIZE,
			ENEMY_IMAGE_SIZE
		}
	};

	//====================================
	// 当たり判定確認用の枠
	//====================================

	SetDrawBlendMode(
		DX_BLENDMODE_ALPHA,
		128);

	DrawBox(
		pos_.x,
		pos_.y,
		pos_.x + ENEMY_DRAW_SIZE,
		pos_.y + ENEMY_DRAW_SIZE,
		GetColor(255, 255, 0),
		FALSE,
		2);

	//====================================
	// 敵描画
	//====================================

	DrawRectExtendGraph(
		pos_.x,
		pos_.y,
		pos_.x + ENEMY_DRAW_SIZE,
		pos_.y + ENEMY_DRAW_SIZE,
		iRect[dir_].x,
		iRect[dir_].y,
		iRect[dir_].w,
		iRect[dir_].h,
		hImage_,
		TRUE);

	//====================================
	// アニメーション
	//====================================

	animTimer -= Time::DeltaTime();

	if (animTimer <= 0.0f)
	{
		frame = (frame + 1) % 4;

		animTimer += ANIM_INTERVAL;
	}

	SetDrawBlendMode(
		DX_BLENDMODE_NOBLEND,
		0);
}