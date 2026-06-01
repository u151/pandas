#include "Enemy.h"
#include "time.h"
#include "Stage.h"

namespace
{
	const int ENEMY_SIZE = 48; //敵のサイズ 32*32
	const Point ENEMY_START_POS = { 20 * ENEMY_SIZE, 10 * ENEMY_SIZE }; //敵の初期位置
	const DIR INIT_ENEMY_DIR = { LEFT };
	const int ENEMY_DRAW_SIZE = 32; //敵の描画サイズ
	const int animFrame[4]{ 0, 1, 2, 1 };
	const float ANIM_INTERVAL = 0.2f;
}


Enemy::Enemy()
	: GameObject()
{
	hImage_ = LoadGraph("Assets/panda_R.png");
	pos_ = ENEMY_START_POS; //32はブロックの位置pos_
	dir_ = INIT_ENEMY_DIR;
}

Enemy::~Enemy()
{
}

void Enemy::Update()
{
	//GetRand(数値)
	static float dir_timer = 3.0f;
	static float prog_timer = 0.5f;
	float dt = Time::DeltaTime();
	dir_timer = dir_timer - dt;
	prog_timer = prog_timer - dt;
	Point newPos = pos_;

	//int mapValue = FindGameObject<Stage>()->GetMap(newPos.x / CHA_SIZE, newPos.y / CHA_SIZE);
	//if (mapValue == 1) {
	//}
	//if (dir_timer < 0.0f)
	//{	
	//dir_ = (DIR)(GetRand(3));
	//dir_timer = 3.0f + dir_timer;
	//}
	if (prog_timer < 0.0f)
	{
		switch (dir_)
		{
		case UP:
			newPos.y -= ENEMY_DRAW_SIZE;
			break;
		case DOWN:
			newPos.y += ENEMY_DRAW_SIZE;
			break;
		case LEFT:
			newPos.x -= ENEMY_DRAW_SIZE;
			break;
		case RIGHT:
			newPos.x += ENEMY_DRAW_SIZE;
			break;
		default:
			break;
		}
		int mapValue = FindGameObject<Stage>()->GetMap(newPos.x / CHA_SIZE, newPos.y / CHA_SIZE);
		//Stage*stage=FindGameObject<Stage>
		// 
		//
		//移動先がステージの外に出ないようにする
		//if (!(newPos.x < 1 || newPos.x >(STAGE_WIDTH - 2) * ENEMY_DRAW_SIZE
		//	|| newPos.y < 1 || newPos.y >(STAGE_HEIGHT - 2) * ENEMY_DRAW_SIZE))
		//{
		//	pos_ = newPos;
		//}
		if (mapValue == 1)
		{
			//newPos = pos_;
			dir_ = static_cast<DIR>((dir_ + 1) % 2);
		}
		else
		{
			pos_ = newPos;
		}
		prog_timer = 0.1f + prog_timer;
	}
	// 透明度を50%（128/255）に設定SetDrawBlendMode( DX_BLENDMODE_ALPHA, 128 ); // 赤色の半透明矩形を描画（引数: 左上X, 左上Y, 右下X, 右下Y, 色, 塗りつぶし）DrawBox( 100, 100, 300, 300, GetColor( 255, 0, 0 ), TRUE ); // 描画が終わったら通常のブレンドモードに戻すSetDrawBlendMode( DX_BLENDMODE_NOBLEND, 0 );
	/*if (prog_timer < 0.0f)
	{
		Stage* stage = FindGameObject<Stage>();
		if (stage != nullptr)
		{
			// 1. 現在の方向を基準に「前」「左」「右」の次の座標（仮）を計算する
			Point frontPos = pos_;
			Point leftPos = pos_;
			Point rightPos = pos_;

			// 現在の向きに応じて、前・左・右の座標をマッピング
			switch (dir_)
			{
			case UP:
				frontPos.y -= ENEMY_DRAW_SIZE; // 前
				leftPos.x -= ENEMY_DRAW_SIZE; // 左
				rightPos.x += ENEMY_DRAW_SIZE; // 右
				break;
			case DOWN:
				frontPos.y += ENEMY_DRAW_SIZE; // 前
				leftPos.x += ENEMY_DRAW_SIZE; // 左（下を向いているので、左側はXプラス方向）
				rightPos.x -= ENEMY_DRAW_SIZE; // 右
				break;
			case LEFT:
				frontPos.x -= ENEMY_DRAW_SIZE; // 前
				leftPos.y += ENEMY_DRAW_SIZE; // 左
				rightPos.y -= ENEMY_DRAW_SIZE; // 右
				break;
			case RIGHT:
				frontPos.x += ENEMY_DRAW_SIZE; // 前
				leftPos.y -= ENEMY_DRAW_SIZE; // 左
				rightPos.y += ENEMY_DRAW_SIZE; // 右
				break;
			default:
				break;
			}

			// 2. それぞれの場所のマップチップ（壁かどうか）を取得
			int frontMap = stage->GetMap(frontPos.x / CHA_SIZE, frontPos.y / CHA_SIZE);
			int leftMap = stage->GetMap(leftPos.x / CHA_SIZE, leftPos.y / CHA_SIZE);

			// 3. 【条件】前が壁(1) かつ 左が壁(1) なら、右に曲がって進む
			if (frontMap == 1 && leftMap == 1)
			{
				// 向きを右に変更する
				switch (dir_)
				{
				case UP:    dir_ = RIGHT; break;
				case DOWN:  dir_ = LEFT;  break;
				case LEFT:  dir_ = UP;    break;
				case RIGHT: dir_ = DOWN;  break;
				}
				// 右に進めるなら進む（右も壁ならその場にとどまる）
				int rightMap = stage->GetMap(rightPos.x / CHA_SIZE, rightPos.y / CHA_SIZE);
				if (rightMap != 1)
				{
					pos_ = rightPos;
				}
			}
			// 4. 前と左が両方1ではない場合（通常の移動処理）
			else if (frontMap != 1)
			{
				// 前が進めるならそのまま前進
				pos_ = frontPos;
			}
			else
			{
				// 前が壁だけど左が開いている時などの処理（必要に応じてここに反転などを入れる）
				// 例: とりあえず左に曲がってみるなど
				switch (dir_)
				{
				case UP:    dir_ = LEFT;  break;
				case DOWN:  dir_ = RIGHT; break;
				case LEFT:  dir_ = DOWN;  break;
				case RIGHT: dir_ = UP;    break;
				}
			}
		}

		prog_timer = 0.1f; // タイマーリセット
	}*/
}

void Enemy::Draw()
{
	static float animTimer = ANIM_INTERVAL;
	static int frame = 0;
	int nowFrame = animFrame[frame];

	Rect iRect[4] = {
		{  nowFrame * ENEMY_SIZE, 3 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE},
		{  nowFrame * ENEMY_SIZE, 0 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE},
		{  nowFrame * ENEMY_SIZE, 1 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE},
		{  nowFrame * ENEMY_SIZE, 2 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE}
	};
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	//DrawBox(100, 100, 300, 300, GetColor(255, 0, 0), TRUE);
	DrawBox(pos_.x, pos_.y, pos_.x + ENEMY_DRAW_SIZE, pos_.y + ENEMY_DRAW_SIZE,
		GetColor(255, 255, 0), FALSE, 2);
	DrawRectExtendGraph(pos_.x, pos_.y, pos_.x + ENEMY_DRAW_SIZE, pos_.y + ENEMY_DRAW_SIZE,
		iRect[dir_].x, iRect[dir_].y, iRect[dir_].w, iRect[dir_].h, hImage_, TRUE);
	if (animTimer < 0) {
		frame = (++frame) % 4;
		animTimer = ANIM_INTERVAL + animTimer;
	}
	animTimer = animTimer - Time::DeltaTime();
}
