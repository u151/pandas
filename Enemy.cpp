#include "Enemy.h"
#include "time.h"
#include "Stage.h"
#include "Player.h"
#include <cmath>

namespace
{
	const int ENEMY_IMAGE_SIZE = 48;
	const Point ENEMY_INIT_POS = { 20 * CHA_SIZE, 10 * CHA_SIZE };
	const DIR ENEMY_INIT_DIR = LEFT;
	const int ENEMY_DRAW_SIZE = CHA_SIZE;
	const int animFrame[4] = { 0, 1, 2, 1 };
	const float ANIM_INTERVAL = 0.2f;
	const float SIGHT_DISTANCE = 8.0f * CHA_SIZE;
	const float ATTACK_DISTANCE = 2.0f * CHA_SIZE;
	const float SEARCH_TIME = 5.0f;
	const float MOVE_INTERVAL = 0.3f;
}

Enemy::Enemy()
	: GameObject()
{
	hImage_ = LoadGraph("Assets/panda_R.png");
	pos_ = ENEMY_INIT_POS;
	dir_ = ENEMY_INIT_DIR;
	state_ = PATROL;
	searchTimer_ = 0.0f;
	attackTimer_ = 0.0f;
	moveTimer_ = 0.0f;
}

Enemy::~Enemy()
{
}

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

void Enemy::Patrol()
{
	if (IsPlayerInSight())
	{
		state_ = CHASE;
		moveTimer_ = 0.0f;
		return;
	}
	moveTimer_ -= Time::DeltaTime();
	if (moveTimer_ > 0.0f)
	{
		return;
	}
	Point frontPos = pos_;
	Point rightPos = pos_;
	Point leftPos = pos_;
	switch (dir_)
	{
	case UP:
		frontPos.y -= CHA_SIZE;
		rightPos.x += CHA_SIZE;
		leftPos.x -= CHA_SIZE;
		break;
	case RIGHT:
		frontPos.x += CHA_SIZE;
		rightPos.y += CHA_SIZE;
		leftPos.y -= CHA_SIZE;
		break;
	case DOWN:
		frontPos.y += CHA_SIZE;
		rightPos.x -= CHA_SIZE;
		leftPos.x += CHA_SIZE;
		break;
	case LEFT:
		frontPos.x -= CHA_SIZE;
		rightPos.y -= CHA_SIZE;
		leftPos.y += CHA_SIZE;
		break;
	}
	if (IsMovePossible(frontPos))
	{
		pos_ = frontPos;
	}
	else
	{
		bool canRight = IsMovePossible(rightPos);
		bool canLeft = IsMovePossible(leftPos);
		if (canRight && !canLeft)
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
		else if (!canRight && canLeft)
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
			pos_ = leftPos;
		}
		else if (canRight && canLeft)
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
		else
		{
			switch (dir_)
			{
			case UP:
				dir_ = DOWN;
				break;
			case DOWN:
				dir_ = UP;
				break;
			case LEFT:
				dir_ = RIGHT;
				break;
			case RIGHT:
				dir_ = LEFT;
				break;
			}
		}
	}
	moveTimer_ = MOVE_INTERVAL;
}

void Enemy::Chase()
{
	Player* player = FindGameObject<Player>();
	if (player == nullptr)
	{
		state_ = PATROL;
		return;
	}
	if (!IsPlayerInSight())
	{
		state_ = SEARCH;
		searchTimer_ = 0.0f;
		moveTimer_ = 0.0f;
		return;
	}
	if (IsAttackRange())
	{
		state_ = ATTACK;
		attackTimer_ = 0.0f;
		return;
	}
	moveTimer_ -= Time::DeltaTime();
	if (moveTimer_ > 0.0f)
	{
		return;
	}
	Point playerPos = player->GetPosition();
	int dx = playerPos.x - pos_.x;
	int dy = playerPos.y - pos_.y;
	Point nextPos = pos_;
	if (std::abs(dx) > std::abs(dy))
	{
		if (dx > 0)
		{
			nextPos.x += CHA_SIZE;
			dir_ = RIGHT;
		}
		else
		{
			nextPos.x -= CHA_SIZE;
			dir_ = LEFT;
		}
	}
	else
	{
		if (dy > 0)
		{
			nextPos.y += CHA_SIZE;
			dir_ = DOWN;
		}
		else
		{
			nextPos.y -= CHA_SIZE;
			dir_ = UP;
		}
	}
	if (IsMovePossible(nextPos))
	{
		pos_ = nextPos;
	}
	moveTimer_ = MOVE_INTERVAL;
}

void Enemy::Attack()
{
	if (!IsAttackRange())
	{
		state_ = SEARCH;
		searchTimer_ = 0.0f;
		moveTimer_ = 0.0f;
		return;
	}
	attackTimer_ -= Time::DeltaTime();
	if (attackTimer_ <= 0.0f)
	{
		attackTimer_ = 1.0f;
	}
}

void Enemy::Search()
{
	if (IsPlayerInSight())
	{
		state_ = CHASE;
		searchTimer_ = 0.0f;
		moveTimer_ = 0.0f;
		return;
	}
	searchTimer_ += Time::DeltaTime();
	if (searchTimer_ >= SEARCH_TIME)
	{
		state_ = PATROL;
		searchTimer_ = 0.0f;
		moveTimer_ = 0.0f;
		return;
	}
	moveTimer_ -= Time::DeltaTime();
	if (moveTimer_ > 0.0f)
	{
		return;
	}
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
	if (IsMovePossible(newPos))
	{
		pos_ = newPos;
	}
	moveTimer_ = MOVE_INTERVAL;
}
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

bool Enemy::IsPlayerInSight()
{
	return GetPlayerDistance() <= SIGHT_DISTANCE;
}

bool Enemy::IsAttackRange()
{
	return GetPlayerDistance() <= ATTACK_DISTANCE;
}

bool Enemy::IsMovePossible(Point pos)
{
	int mapX = pos.x / CHA_SIZE;
	int mapY = pos.y / CHA_SIZE;
	if (mapX < 0 ||
		mapX >= STAGE_WIDTH ||
		mapY < 0 ||
		mapY >= STAGE_HEIGHT)
	{
		return false;
	}

	Stage* stage = FindGameObject<Stage>();
	if (stage == nullptr)
	{
		return false;
	}

	int mapValue = stage->GetMap(mapX, mapY);
	if (mapValue == 1)
	{
		return false;
	}
	return true;
}

void Enemy::Draw()
{
	static float animTimer = ANIM_INTERVAL;
	static int frame = 0;
	int nowFrame = animFrame[frame];
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
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	DrawBox(pos_.x, pos_.y, pos_.x + ENEMY_DRAW_SIZE, pos_.y + ENEMY_DRAW_SIZE, GetColor(255, 255, 0), FALSE, 2);

	//====================================
	// “G•`‰æ
	//====================================

	DrawRectExtendGraph(pos_.x, pos_.y, pos_.x + ENEMY_DRAW_SIZE, pos_.y + ENEMY_DRAW_SIZE, iRect[dir_].x, iRect[dir_].y, iRect[dir_].w, iRect[dir_].h, hImage_, TRUE);

	animTimer -= Time::DeltaTime();
	if (animTimer <= 0.0f)
	{
		frame = (frame + 1) % 4;
		animTimer += ANIM_INTERVAL;
	}
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}