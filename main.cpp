#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>

const char kWindowTitle[] = "LC1A_22_フミモトコウサク_タイトル";

const int kWindowWidth = 1280;
const int kWindowHeight = 720;

#pragma region enum
// enemyの行動パターン
// 中型
enum EnemyMediumPattern {
	NONE_M,
	EXPLODE_M,
	CRUSH_M,
	LONGATTACK_M,
	BEAM_M
};
int enemyMediumPattern = NONE_M;

// 人型
enum EnemyPersonPattern {
	// 攻撃するまでに必要な移動
	NONE_P,
	JUMP_P,
	BACKSTEP_P,
	DUSH_P,
	TELEPORT_P,
	SHOCKWAVE_P,// 攻撃も移動も含んでる
	// 攻撃
	NORMAL_P,
	BEHINDATTACK_P,
	LONGRANGE_P,
	RANGEATTACK_P,
	THOUNDER_P
};
int enemyPersonPattern = NONE_P;

// playerの向き
enum PlayerDir {
	RIGHT,
	LEFT
};
int playerDir = RIGHT;
// enemyの向き
typedef enum EnemyDir {
	RIGHT_E,
	LEFT_E
};
int enemyDir = LEFT_E;

// シーン
enum Scene {
	TITLE,
	FIRSTENEMY,		// 中型の敵
	SECONDENEMY,	// 大型の敵
	LASTENEMY1,		// 人型第一形態
	LASTENEMY2,		// 人型第二形態
	GAMEOVER,
	CLEAR
};
int scene = TITLE;
#pragma endregion

#pragma region 構造体
// 座標
typedef struct Vector2 {
	float x;
	float y;
};

#pragma region player
// player
typedef struct Player {
	Vector2 translate;
	Vector2 speed;
	Vector2 acceleration;
	float size;
	int hp;
	int isAlive;
	int isParry;
	int isJump;
	int jumpCount;
	int isMultiHit;
	unsigned int color;
};
// playerの近接攻撃
typedef struct PlayerAttack {
	Vector2 translate;
	float size;
	int count;
	int frame;
	int animCount;
	int isAttack;
	int isMultiHit;
	unsigned int color;
};
#pragma endregion

#pragma region 中型のenemy
// 中型のenemy
typedef struct EnemyMedium {
	Vector2 translate;
	Vector2 speed;
	Vector2 acceleration;
	float size;
	float theta;
	int hp;
	int isAlive;
	int isAttack;
	int isStun;
	unsigned int color;
};
// 中型の爆破攻撃
typedef struct Explode {
	Vector2 translate;
	Vector2 speed;
	float size;
	float endTime;
	float endSpeed;
	int frame;
	int animCount;
	int isAttack;
	int isMultiHit;
	unsigned int color;
};
// 中型の遠距離攻撃
typedef struct LongAttackM {
	Vector2 translate;
	Vector2 speed;
	float size;
	int frame;
	int animCount;
	int isAttack;
	int isMultiHit;
	unsigned int color;
};
// ビーム
typedef struct Beam {
	Vector2 translate;
	float size;
	float endTime;
	float endSpeed;
	int frame;
	int animCount;
	int isAttack;
	int isMultiHit;
	unsigned int color;
};
#pragma endregion

#pragma region 人型のenemy
// enemy人型
typedef struct EnemyPerson {
	Vector2 translate;
	Vector2 speed;
	Vector2 acceleration;
	float size;
	int hp;
	int isAlive;
	int isJump;
	int isStun;
	unsigned int color;
};
#pragma region 攻撃パターン
// enemyの通常攻撃
typedef struct NormalAttack {
	Vector2 translate;
	float size;
	int frame;
	int animCount;
	int isAttack;
	int isMultiHit;
	unsigned int color;
};
// enemyの遠距離攻撃
typedef struct LongAttack {
	Vector2 translate;
	Vector2 speed;
	float size;
	int frame;
	int animCount;
	int isAttack;
	int isMultiHit;
	unsigned int color;
};
// enemyの範囲攻撃(居合切り)
typedef struct RangeAttack {
	Vector2 translate;	 // 座標
	Vector2 drawSwordPos;// 攻撃開始前のenemy本体の位置を記憶
	float size;			 // サイズ
	int frame;			 // フレーム
	int animCount;	     // アニメーション
	int isAttack;	     // 攻撃しているか
	int isMultiHit;		 // 多段ヒット防止
	unsigned int color;  // 色（攻撃してるときは赤）
};
typedef struct ShockWave {
	Vector2 translate;
	Vector2 speed;
	float size;
	float endTime;
	float endSpeed;
	int frame;			 // フレーム
	int animCount;	     // アニメーション
	int isAttack;	     // 攻撃しているか
	int isMultiHit;		 // 多段ヒット防止
	unsigned int color;  // 色（攻撃してるときは赤）
};
// playerの背後から攻撃
typedef struct BehindAttack {
	Vector2 translate;
	Vector2 speed;
	float size;
	int frame;
	int animCount;
	int isBehindAttack;
	int isMultiHit;
	unsigned int color;
};
// 空中攻撃
typedef struct Thounder {
	Vector2 translate;
	float size;
	int frame;
	int animCount;
	int isThounder;
	int isMultiHit;
	unsigned int color;
};
#pragma endregion
#pragma region 移動パターン
// テレポート
typedef struct Teleport {
	Vector2 translate;
	Vector2 behindTranslate;
	int frame;
	int animCount;
	int isTeleport;
};
// enemyのジャンプ
typedef struct Jump {
	Vector2 translate;
	Vector2 speed;
	float endTime;
	float endSpeed;
	int frame;
	int animCount;
	int isJump;
};
// enemyのダッシュ
typedef struct Dash {
	Vector2 translate;
	Vector2 speed;
	float endTime;
	float endSpeed;
	int frame;
	int animCount;
	int isDush;
};
// enemyのバックステップ
typedef struct BackStep {
	Vector2 translate;
	Vector2 speed;
	float endTime;
	float endSpeed;
	int frame;
	int animCount;
	int isBackStep;
};
#pragma endregion
#pragma endregion

#pragma endregion
// 関数
// だんだん早くなる(EaseIn)
void EaseIn(float& scale, float& scaleSpeed, float& radius, float firstRadius, float endRadius) {
	scale += scaleSpeed;
	float easedT;
	easedT = scale * scale;
	radius = (1.0f - easedT) * firstRadius + easedT * endRadius;
}
// ジャンプ
void JumpPattern(float& enemyx, float& enemyy, float JumpPosX, float JumpPosY, float& speedx, float& speedy, float enemySize, int& isJump, float& t) {
	float enemyAccelerationY = -0.8f;
	isJump = true;
	t += 1.0 / 100.0f;
	if (isJump) {
		if (static_cast<float>(kWindowWidth) / 2 <= JumpPosX) {
			// x軸の移動
			if (t <= 1.0f) {
				enemyx = (1.0f - t) * JumpPosX + t * 100.0f;
			}
		}
		if (static_cast<float>(kWindowWidth) / 2 >= JumpPosX) {
			// x軸の移動
			if (t <= 1.0f) {
				enemyx = (1.0f - t) * JumpPosX + t * 1180.0f;
			}
		}
		// 自由落下
		speedy += enemyAccelerationY;
		enemyy += speedy;
		// 地面についたら初期化
		if (enemyy <= enemySize) {
			enemyy = enemySize;
			speedy = 40.0f;
			t = 0.0f;
			enemyPersonPattern = LONGRANGE_P;
			isJump = false;
		}
	}
}
// 衝撃波
void ShockWavePattern(float& enemyx, float& enemyy, float shockWavePosX, float shockWavePosY, float& speedx, float& speedy, float enemySize, int& isShockWave, float& t) {
	float enemyAccelerationY = -0.8f;
	int isJump = true;
	t += 1.0 / 100.0f;
	if (isJump) {
		// x軸の移動
		if (t <= 1.0f) {
			enemyx = (1.0f - t) * shockWavePosX + t * kWindowWidth / 2;
		}
		// 自由落下
		if (speedy >= 0.0f) {
			speedy += enemyAccelerationY;
			enemyy += speedy;
		}
		// 地面についたら初期化
		if (enemyy <= enemySize) {
			enemyy = enemySize;
			speedy = 40.0f;
			t = 0.0f;
			isShockWave = true;
			isJump = false;
		}
	}
	if (isShockWave) {

	}
}
// ダッシュ攻撃
void DushPattern(float& enemyX, float& enemyY, float dushX, float dushY, float& DushFrame, float DushSpeed, float& playerX, float& playerY, float playerSize) {
	if (playerX >= dushX) {
		EaseIn(DushFrame, DushSpeed, enemyX, dushX, playerX - playerSize * 2);
	}
	if (playerX <= dushX) {
		EaseIn(DushFrame, DushSpeed, enemyX, dushX, playerX + playerSize * 2);
	}
	if (DushFrame >= 1.0f) {
		enemyPersonPattern = NORMAL_P;
		DushFrame = 0.0f;
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	/**********画像の読み込み**********/
	int playerGh = Novice::LoadTexture("white1x1.png");
	int attackGh = Novice::LoadTexture("white1x1.png");

	/**********宣言**********/
#pragma region player
	// 本体
	Player player = {
		{64 * 2,64}, // 位置
		{10.0f,10.0f}, // 速度
		{0.0f,-0.8f},// 重力加速度
		64.0f,		 // サイズ
		3,			 // hp
		true,		 // 生き死に
		false,		 // パリィしているか
		false,		 // ジャンプしてるか
		0,			 // ジャンプした数
		false,		 // 多段ヒット防止
		BLUE		 // 色
	};
	// playerの近接攻撃
	PlayerAttack playerAttack = {
		{0.0f,0.0f},
		32.0f,
		0,
		0,
		0,
		false,
		false,
		WHITE
	};
#pragma endregion

#pragma region 人型のenemy
	// enemy(人型)
	EnemyPerson enemyPerson = {
		{kWindowWidth - 64.0f * 2,64.0f},
		{6.0f,0.0f},
		{0.0f,-0.8f},
		64.0f,
		10,
		true,
		false,
		false,
		BLACK
	};
	/**************enemyの攻撃**************/
	// 近接攻撃
	NormalAttack normalAttack = {
		{0.0f,0.0f},
		32.0f,
		0,
		0,
		false,
		false,
		WHITE
	};
	// playerの背後から攻撃
	BehindAttack behindAttack = {
		{0.0f,0.0f},
		{0.0f,0.0f},
		32.0f,
		0,
		0,
		false,
		false,
		WHITE
	};
	// 遠距離攻撃
	LongAttack longAttack = {
		{0.0f,0.0f},
		{20.0f,10.0f},
		32.0f,
		0,
		0,
		false,
		false,
		WHITE
	};
	// 範囲攻撃
	RangeAttack rangeAttack = {
		{kWindowWidth / 2,64.0f},
		{0.0f,0.0f},
		64.0f,
		0,
		0,
		false,
		false,
		WHITE
	};
	// ステージ中央で衝撃波
	ShockWave shockWave[2]{};
	for (int i = 0; i < 2; i++) {
		shockWave[i] = {
				{0.0f,0.0f},
				{32.0f,0.0f},
				32.0f,
				0.0f,
				1.0f / 100.0f,
				0,
				0,
				false,
				false,
				WHITE
		};
	}
	Vector2 shockWaveJumpPos = {
		0.0f,0.0f
	};
	int count = 0;
	int isShockWaveJump = false;
	// 落雷
	Thounder thounder[10];
	for (int i = 0; i < 10; i++) {
		thounder[i] = {
				{100.0f * i + 1,kWindowHeight / 2},
				64.0f,
				0,
				0,
				false,
				false,
				WHITE
		};
	}
	int thounderCount = 0;
	/**************enemyの移動パターン**************/
	// enemyが何もしないとき
	int NoneTime = 120;
	// 移動パターンの乱数
	unsigned int currentTime = time(nullptr);
	srand(currentTime);
	// テレポート
	Teleport teleport = {
		{0.0f,0.0f},
		{0.0f,0.0f},
		0,
		0,
		false
	};
	// enmeyがジャンプするとき
	Jump jump = {
		{0.0f,0.0f},
		{0.0f,0.0f},
		0.0f,
		1.0f / 100.0f,
		0,
		0,
		false
	};
	// バックステップ
	BackStep backStep = {
		{0.0f,0.0f},
		{0.0f,0.0f},
		0.0f,
		1.0f / 20.0f,
		0,
		0,
		false
	};
	// enemyがダッシュするとき
	Dash dash = {
		{0.0f,0.0f},
		{0.0f,0.0f},
		0.0f,
		1.0f / 15.0f,
		0,
		0,
		false
	};
#pragma endregion

#pragma region 中型のenemy
	// 中型のenemy
	EnemyMedium enemyMedium = {
		{kWindowWidth / 2,kWindowHeight - 64.0f},
		{6.0f,6.0f},
		{0.0f,-0.8f},
		64.0f,
		0.0f,
		10,
		true,
		false,
		false,
		BLACK
	};
	int isTakeOff = false;
	int crushFrame = 0;
	int crushCount = 0;
	// 爆発
	Explode explode = {
		{0.0f,0.0f},
		{4.0f,4.0f},
		96.0f,
		0.0f,
		1.0f / 20.0f,
		0,
		0,
		false,
		false,
		WHITE
	};
	// 遠距離
	LongAttackM longAttackM[5] = {
		{0.0f,0.0f},
		{8.0f,8.0f},
		32.0f,
		0,
		0,
		false,
		false,
		WHITE
	};
	// 範囲攻撃
	Beam beam = {
		{0.0f,0.0f},
		32.0f,
		0.0f,
		1.0f / 30.0f,
		0,
		0,
		false,
		false,
		WHITE
	};
#pragma endregion
	//// playerとenemyの距離
	//Vector2 e2p = {
	//	0.0f,0.0f
	//};


	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///
// タイトル、ゲームオーバー、クリア画面以外でplayerを動かせる
#pragma region player
	// playerの移動キー
	// 左に移動
		if (keys[DIK_A]) {
			player.translate.x -= player.speed.x;
			playerDir = LEFT;
		}
		// 右に移動
		if (keys[DIK_D]) {
			player.translate.x += player.speed.x;
			playerDir = RIGHT;
		}
		// ジャンプのキー
		if (!keys[DIK_W] && preKeys[DIK_W]) {
			player.jumpCount++;
			// 最初のジャンプ
			if (player.jumpCount == 1) {
				// ジャンプの初速
				player.speed.y = 20.0f;
				player.isJump = true;
			}
			// 2段ジャンプ(1段目より高く飛べない)
			if (player.jumpCount == 2) {
				player.speed.y = 17.0f;
				player.isJump = true;
			}
		}
		// 近接攻撃のキー
		if (!keys[DIK_SPACE] && preKeys[DIK_SPACE] && !playerAttack.isAttack) {
			playerAttack.isAttack = true;
		}

		// playerの向きによって攻撃位置を変える
		if (playerDir == LEFT) {
			playerAttack.translate.x = player.translate.x - playerAttack.size * 3;
			playerAttack.translate.y = player.translate.y;
		}
		if (playerDir == RIGHT) {
			playerAttack.translate.x = player.translate.x + playerAttack.size * 3;
			playerAttack.translate.y = player.translate.y;
		}

		/**********playerとenemyの当たり判定ここから↓**********/
		// 中型
		if (scene == FIRSTENEMY) {
			if (!player.isMultiHit) {
				// enemy本体とplayerの近接攻撃の当たり判定
				if ((player.translate.x + player.size) > enemyMedium.translate.x - enemyMedium.size && player.translate.x - player.size < (enemyMedium.translate.x + enemyMedium.size)
					&& (player.translate.y + player.size) > enemyMedium.translate.y - enemyMedium.size && (player.translate.y - player.size) < enemyMedium.translate.y + enemyMedium.size) {
					player.hp -= 1;
					player.isMultiHit = true;
				}
			}
		}
		// 人型
		if (scene == LASTENEMY1) {
			if (!player.isMultiHit) {
				// enemy本体とplayerの近接攻撃の当たり判定
				if ((player.translate.x + player.size) > enemyPerson.translate.x - enemyPerson.size && player.translate.x - player.size < (enemyPerson.translate.x + enemyPerson.size)
					&& (player.translate.y + player.size) > enemyPerson.translate.y - enemyPerson.size && (player.translate.y - player.size) < enemyPerson.translate.y + enemyPerson.size) {
					player.hp -= 1;
					player.isMultiHit = true;
				}
			}
		}

		/**********playerとenemyの当たり判定ここまで↑**********/

		/**********ジャンプ処理ここから↓**********/
		// 自由落下
		if (player.isJump) {
			player.speed.y += player.acceleration.y;
			player.translate.y += player.speed.y;
		}
		// 地面についたら初期化
		if (player.translate.y <= player.size) {
			player.translate.y = player.size;
			player.speed.y = 0.0f;
			player.jumpCount = 0;
		}
		/**********ジャンプ処理ここまで↑**********/


		/**********playerの近接攻撃処理ここから↓**********/
		// 当たり判定の処理
		if (playerAttack.isAttack) {
			playerAttack.frame++;
			playerAttack.color = RED;
			// 攻撃が多段ヒットするのでフラグで止める
			if (!playerAttack.isMultiHit) {
				// 人型enemy本体とplayerの近接攻撃の当たり判定
				if ((playerAttack.translate.x + playerAttack.size) > enemyPerson.translate.x - enemyPerson.size && playerAttack.translate.x - playerAttack.size < (enemyPerson.translate.x + enemyPerson.size)
					&& (playerAttack.translate.y + playerAttack.size) > enemyPerson.translate.y - enemyPerson.size && (playerAttack.translate.y - playerAttack.size) < enemyPerson.translate.y + enemyPerson.size) {
					enemyPerson.hp -= 1;
					playerAttack.isMultiHit = true;
				}
			}
			// 15以上は攻撃判定を消して初期化
			if (playerAttack.frame >= 10) {
				playerAttack.frame = 0;
				playerAttack.isAttack = false;
				playerAttack.isMultiHit = false;
			}

			/**********パリィの処理(enemyの近接攻撃にだけパリィできる)**********/
			// 攻撃始めの2フレームはパリィ判定
			if (playerAttack.frame <= 1) {
				player.isParry = true;
			}
			else if (playerAttack.frame >= 2) {
				player.isParry = false;
			}
			// 当たり判定(現状かなり近距離じゃないと発動しない)
			if ((normalAttack.translate.x + normalAttack.size) > playerAttack.translate.x - playerAttack.size && normalAttack.translate.x - normalAttack.size < (playerAttack.translate.x + playerAttack.size)
				&& (normalAttack.translate.y + normalAttack.size) > playerAttack.translate.y - playerAttack.size && (normalAttack.translate.y - normalAttack.size) < playerAttack.translate.y + playerAttack.size) {
				if (player.isParry) {
					if (normalAttack.isAttack) {
						normalAttack.isAttack = false;
						enemyPerson.isStun = true;
						enemyPersonPattern = NONE_P;
					}
				}
			}
		}
		else {
			playerAttack.color = WHITE;
			player.isParry = false;
		}
		/**********playerの近接攻撃処理ここまで↑**********/

#pragma endregion

		switch (scene) {
		case TITLE:
#pragma region デバッグ用のキー
			if (!keys[DIK_1] && preKeys[DIK_1]) {
				scene = FIRSTENEMY;
			}
			if (!keys[DIK_2] && preKeys[DIK_2]) {
				scene = SECONDENEMY;
			}
			if (!keys[DIK_3] && preKeys[DIK_3]) {
				scene = LASTENEMY1;
			}
#pragma endregion
			break;
		case FIRSTENEMY:
#pragma region デバッグ用のキー
			if (!keys[DIK_1] && preKeys[DIK_1]) {
				enemyMediumPattern = EXPLODE_M;
			}
			if (!keys[DIK_2] && preKeys[DIK_2]) {
				enemyMediumPattern = CRUSH_M;
			}
#pragma endregion

			// playerとenemy間のthetaを取得
			enemyMedium.theta = atan2(player.translate.y - enemyMedium.translate.y, player.translate.x - enemyMedium.translate.x);

			// enemyの向きの処理ここから↓
			if (player.translate.x <= enemyMedium.translate.x) {
				enemyDir = LEFT_E;
			}
			if (player.translate.x >= enemyMedium.translate.x) {
				enemyDir = RIGHT_E;
			}
			// playerに向きによって攻撃位置を変える
			// enemyが左を向いてるとき
			if (enemyDir == LEFT_E) {
				explode.translate.x = enemyMedium.translate.x;
				explode.translate.y = enemyMedium.translate.y;
			}
			// enemyが右を向いてるとき
			if (enemyDir == RIGHT_E) {
				explode.translate.x = enemyMedium.translate.x;
				explode.translate.y = enemyMedium.translate.y;
			}
			// enemyの向きの処理ここまで↑

#pragma region enemyの行動パターン
			// 何もしない
			if (enemyMediumPattern == NONE_M) {
				NoneTime--;
				// enemyがスタンしたとき硬直を長くする
				if (enemyMedium.isStun) {
					enemyMedium.isStun = false;
					NoneTime = 240;
				}
				// 攻撃が終わるごとに2秒硬直
				if (NoneTime <= 0) {
					enemyMediumPattern = rand() % 4 + 1;
					NoneTime = 120;
				}
			}
			// playerの近くに寄って爆発
			if (enemyMediumPattern == EXPLODE_M) {
				explode.frame++;
				explode.isAttack = false;
				//enemyMedium.theta = atan2(player.translate.y - enemyMedium.translate.y, player.translate.x - enemyMedium.translate.x);
				if (!explode.isAttack) {
					enemyMedium.translate.x += (cosf(enemyMedium.theta) * explode.speed.x);
					enemyMedium.translate.y += (sinf(enemyMedium.theta) * explode.speed.y);
				}
				if (explode.frame >= 180 && explode.frame <= 185) {
					explode.isAttack = true;
				}
				if (explode.frame >= 200 && explode.frame <= 205) {
					explode.isAttack = true;
				}
				if (explode.frame >= 220 && explode.frame <= 225) {
					explode.isAttack = true;
				}
				if (explode.frame >= 225) {
					explode.frame = 0;
					explode.isAttack = false;
					enemyMediumPattern = NONE_M;
				}
			}
			// playerの真上で90フレーム追尾して落ちてくる
			if (enemyMediumPattern == CRUSH_M) {
				crushFrame++;
				enemyMedium.isAttack = true;

				if (crushFrame <= 90) {
					enemyMedium.speed.x = 60.0f;
					enemyMedium.translate.x += (cosf(enemyMedium.theta) * enemyMedium.speed.x);
				}
				if (crushFrame >= 90) {
					enemyMedium.speed.y += -0.8f;
					enemyMedium.translate.y += enemyMedium.speed.y;
					if (enemyMedium.speed.y <= 0.0f) {
						enemyMedium.speed.y = -48.0f;
					}
				}
				// 地面についたら初期化
				if (enemyMedium.translate.y <= enemyMedium.size) {
					enemyMedium.translate.y = enemyMedium.size;
					crushFrame = 0;
					enemyMedium.isAttack = false;
					enemyMedium.speed.y = 6.0f;
					enemyMediumPattern = NONE_M;
				}
			}
			// 遠距離攻撃
			if (enemyMediumPattern == LONGATTACK_M) {

			}
			// 範囲攻撃
			if (enemyMediumPattern == BEAM_M) {

			}
#pragma endregion

			break;
		case SECONDENEMY:

			break;
		case LASTENEMY1:
#pragma region デバッグ用のキー
			if (!keys[DIK_1] && preKeys[DIK_1]) {
				NoneTime = 120;
				enemyPersonPattern = JUMP_P;
			}
			if (!keys[DIK_2] && preKeys[DIK_2]) {
				NoneTime = 120;
				enemyPersonPattern = BACKSTEP_P;
			}
			if (!keys[DIK_3] && preKeys[DIK_3]) {
				NoneTime = 120;
				enemyPersonPattern = DUSH_P;
			}
			if (!keys[DIK_4] && preKeys[DIK_4]) {
				NoneTime = 120;
				enemyPersonPattern = TELEPORT_P;
			}
			if (!keys[DIK_5] && preKeys[DIK_5]) {
				NoneTime = 120;
				enemyPersonPattern = SHOCKWAVE_P;
			}
			if (!keys[DIK_6] && preKeys[DIK_6]) {
				NoneTime = 120;
				enemyPersonPattern = THOUNDER_P;
			}
#pragma endregion
			// enemyの向き
			if (player.translate.x <= enemyPerson.translate.x) {
				enemyDir = LEFT_E;
			}
			if (player.translate.x >= enemyPerson.translate.x) {
				enemyDir = RIGHT_E;
			}
			// playerに向きによって攻撃位置を変える
			// enemyが左を向いてるとき
			if (enemyDir == LEFT_E) {
				normalAttack.translate.x = enemyPerson.translate.x - normalAttack.size * 3;
				normalAttack.translate.y = enemyPerson.translate.y;
				behindAttack.translate.x = enemyPerson.translate.x - behindAttack.size * 3;
				behindAttack.translate.y = enemyPerson.translate.y;
			}
			// enemyが右を向いてるとき
			if (enemyDir == RIGHT_E) {
				normalAttack.translate.x = enemyPerson.translate.x + normalAttack.size * 3;
				normalAttack.translate.y = enemyPerson.translate.y;
				behindAttack.translate.x = normalAttack.translate.x;
				behindAttack.translate.y = normalAttack.translate.y;
			}

#pragma region enemyの行動パターン
			// 何もしない
			if (enemyPersonPattern == NONE_P) {
				NoneTime--;
				normalAttack.color = WHITE;
				normalAttack.isAttack = false;
				// enemyがスタンしたとき硬直を長くする
				if (enemyPerson.isStun) {
					enemyPerson.isStun = false;
					NoneTime = 240;
				}
				// 攻撃が終わるごとに2秒硬直
				if (NoneTime <= 0) {
					enemyPersonPattern = rand() % 5 + 1;
					NoneTime = 120;
				}
			}
#pragma region 移動パターン
			// テレポート
			if (enemyPersonPattern == TELEPORT_P) {
				teleport.frame++;
				if (teleport.frame >= 3 && teleport.frame <= 20) {
					teleport.isTeleport = true;
				}
				if (teleport.isTeleport) {
					if (playerDir == RIGHT) {
						enemyPerson.translate.x = player.translate.x - player.size * 2;
					}
					if (playerDir == LEFT) {
						enemyPerson.translate.x = player.translate.x + player.size * 2;
					}
				}
				if (teleport.frame >= 20) {
					teleport.isTeleport = false;
					if (!teleport.isTeleport) {
						enemyPersonPattern = BEHINDATTACK_P;
						teleport.frame = 0;
					}
				}
			}
			// ジャンプ
			if (enemyPersonPattern == JUMP_P) {
				//JumpPattern(typePerson0.translate.x, typePerson0.translate.y, JumpPos.x, JumpPos.y, typePerson0.speed.x, typePerson0.speed.y, typePerson0.size, typePerson0.isJump, jumpEndTime);
				enemyPerson.isJump = true;
				jump.endTime += jump.endSpeed;
				if (static_cast<float>(kWindowWidth) / 2 <= jump.translate.x) {
					// x軸の移動
					if (jump.endTime <= 1.0f) {
						enemyPerson.translate.x = (1.0f - jump.endTime) * jump.translate.x + jump.endTime * 100.0f;
					}
				}
				if (static_cast<float>(kWindowWidth) / 2 >= jump.translate.x) {
					// x軸の移動
					if (jump.endTime <= 1.0f) {
						enemyPerson.translate.x = (1.0f - jump.endTime) * jump.translate.x + jump.endTime * 1180.0f;
					}
				}
				// 自由落下
				enemyPerson.speed.y += enemyPerson.acceleration.y;
				enemyPerson.translate.y += enemyPerson.speed.y;
				// 地面についたら初期化
				if (enemyPerson.translate.y <= enemyPerson.size) {
					enemyPerson.translate.y = enemyPerson.size;
					jump.endTime = 0.0f;
					enemyPerson.isJump = false;
					enemyPersonPattern = LONGRANGE_P;
				}
			}
			else {
				jump.translate.x = enemyPerson.translate.x;
				jump.translate.y = enemyPerson.translate.y;
				enemyPerson.speed.y = 40.0f;
			}
			// バックステップ
			if (enemyPersonPattern == BACKSTEP_P) {
				backStep.isBackStep = true;
				backStep.endTime += backStep.endSpeed;
				// x軸の移動
				if (backStep.endTime <= 1.0f) {
					if (player.translate.x <= backStep.translate.x) {
						enemyPerson.translate.x = (1.0f - backStep.endTime) * backStep.translate.x + backStep.endTime * 1180.0f;
					}
					if (player.translate.x >= backStep.translate.x) {
						enemyPerson.translate.x = (1.0f - backStep.endTime) * backStep.translate.x + backStep.endTime * 100.0f;
					}
				}
				// 自由落下
				backStep.speed.y += enemyPerson.acceleration.y;
				enemyPerson.translate.y += backStep.speed.y;
				// 地面についたら初期化
				if (enemyPerson.translate.y <= enemyPerson.size) {
					enemyPerson.translate.y = enemyPerson.size;
					backStep.speed.y = 10.0f;
					backStep.isBackStep = false;
					enemyPersonPattern = RANGEATTACK_P;
					backStep.endTime = 0.0f;
				}
			}
			else if (!backStep.isBackStep) {
				// ジャンプの初速
				backStep.speed.y = 10.0f;
				backStep.translate.x = enemyPerson.translate.x;
				backStep.translate.y = enemyPerson.translate.y;
			}
			// ダッシュ
			if (enemyPersonPattern == DUSH_P) {
				DushPattern(enemyPerson.translate.x, enemyPerson.translate.y, dash.translate.x, dash.translate.y, dash.endTime, dash.endSpeed, player.translate.x, player.translate.y, player.size);
				/*if (player.translate.x >= DushPos.x) {
					EaseIn(DushFrame, DushSpeed, typePerson0.translate.x, DushPos.x, player.translate.x - player.size * 3);
				}
				if (player.translate.x <= DushPos.x) {
					EaseIn(DushFrame, DushSpeed, typePerson0.translate.x, DushPos.x, player.translate.x + player.size * 3);
				}
				if (DushFrame >= 1.0f) {
					enemyPattern = NORMAL;
					DushFrame = 0.0f;
				}*/
			}
			else {
				dash.translate.x = enemyPerson.translate.x;
				dash.translate.y = enemyPerson.translate.y;
			}
#pragma endregion

#pragma region 攻撃パターン
			// 近接攻撃ここから↓
			if (enemyPersonPattern == NORMAL_P) {
				normalAttack.frame++;
				normalAttack.isAttack = false;
				// 1撃目
				if (normalAttack.frame >= 0 && normalAttack.frame <= 10) {
					normalAttack.isAttack = true;
				}
				// 2撃目
				if (normalAttack.frame >= 30 && normalAttack.frame <= 40) {
					normalAttack.isAttack = true;
				}
				// 3撃目
				if (normalAttack.frame >= 50 && normalAttack.frame <= 60) {
					normalAttack.isAttack = true;
				}
				if (normalAttack.isAttack) {
					normalAttack.color = RED;
					// 攻撃が多段ヒットするのでフラグで止める
					if (!normalAttack.isMultiHit) {
						// 当たり判定
						if ((normalAttack.translate.x + normalAttack.size) > player.translate.x - player.size && normalAttack.translate.x - normalAttack.size < (player.translate.x + player.size)
							&& (normalAttack.translate.y + normalAttack.size) > player.translate.y - player.size && (normalAttack.translate.y - normalAttack.size) < player.translate.y + player.size) {
							player.hp -= 1;
							normalAttack.isMultiHit = true;
						}
					}
				}
				else {
					normalAttack.color = WHITE;
				}
				// 初期化
				if (normalAttack.frame >= 61) {
					normalAttack.isAttack = false;
					normalAttack.isMultiHit = false;
					enemyPersonPattern = NONE_P;
					normalAttack.frame = 0;
				}
			}
			// 近接攻撃ここまで↑

			// 背後から攻撃
			if (enemyPersonPattern == BEHINDATTACK_P) {
				behindAttack.frame++;
				behindAttack.isBehindAttack = false;
				// 1撃目
				if (behindAttack.frame >= 30 && behindAttack.frame <= 40) {
					behindAttack.isBehindAttack = true;
				}
				// 2撃目
				if (behindAttack.frame >= 60 && behindAttack.frame <= 70) {
					behindAttack.isBehindAttack = true;
				}
				// 3撃目
				if (behindAttack.frame >= 80 && behindAttack.frame <= 90) {
					behindAttack.isBehindAttack = true;
				}
				if (behindAttack.isBehindAttack) {
					behindAttack.color = RED;
					// 攻撃が多段ヒットするのでフラグで止める
					if (!behindAttack.isMultiHit) {
						// 当たり判定
						if ((behindAttack.translate.x + behindAttack.size) > player.translate.x - player.size && behindAttack.translate.x - behindAttack.size < (player.translate.x + player.size)
							&& (behindAttack.translate.y + behindAttack.size) > player.translate.y - player.size && (behindAttack.translate.y - behindAttack.size) < player.translate.y + player.size) {
							player.hp -= 1;
							behindAttack.isMultiHit = true;
						}
					}
				}
				else {
					behindAttack.color = WHITE;
				}
				// 初期化
				if (behindAttack.frame >= 90) {
					behindAttack.isBehindAttack = false;
					behindAttack.isMultiHit = false;
					enemyPersonPattern = NONE_P;
					behindAttack.frame = 0;
				}
			}
			// 遠距離攻撃
			if (enemyPersonPattern == LONGRANGE_P) {
				longAttack.frame++;
				longAttack.isAttack = true;
				if (longAttack.frame >= 15) {
					if (static_cast<float>(kWindowWidth) / 2 <= enemyPerson.translate.x) {
						longAttack.translate.x -= longAttack.speed.x;
					}
					if (static_cast<float>(kWindowWidth) / 2 >= enemyPerson.translate.x) {
						longAttack.translate.x += longAttack.speed.x;
					}
				}
				if (longAttack.translate.x <= 0 - longAttack.size || longAttack.translate.x >= kWindowWidth + longAttack.size) {
					longAttack.isAttack = false;
					longAttack.frame = 0;
					enemyPersonPattern = NONE_P;
				}
				// 当たり判定
				if (!longAttack.isMultiHit) {
					if ((longAttack.translate.x + longAttack.size) > player.translate.x - player.size && longAttack.translate.x - longAttack.size < (player.translate.x + player.size)
						&& (longAttack.translate.y + longAttack.size) > player.translate.y - player.size && (longAttack.translate.y - longAttack.size) < player.translate.y + player.size) {
						player.hp -= 1;
						longAttack.isMultiHit = true;
					}
				}
			}
			else {
				longAttack.translate.x = enemyPerson.translate.x;
				longAttack.translate.y = enemyPerson.translate.y;
			}
			// 範囲攻撃
			if (enemyPersonPattern == RANGEATTACK_P) {
				rangeAttack.frame++;
				enemyPerson.speed.x = 100.0f;
				if (rangeAttack.frame >= 8) {
					if (static_cast<float>(kWindowWidth) / 2 >= rangeAttack.drawSwordPos.x) {
						enemyPerson.translate.x += enemyPerson.speed.x;
					}
					if (static_cast<float>(kWindowWidth) / 2 <= rangeAttack.drawSwordPos.x) {
						enemyPerson.translate.x -= enemyPerson.speed.x;
					}
				}
				if (enemyPerson.translate.x >= 1300.0f || enemyPerson.translate.x <= -100.0f) {
					enemyPerson.speed.x = 0.0f;
				}
				if (rangeAttack.frame >= 20) {
					rangeAttack.isAttack = true;
					if (rangeAttack.isAttack) {
						rangeAttack.color = RED;
						// 攻撃が多段ヒットするのでフラグで止めてる
						if (!rangeAttack.isMultiHit) {
							if ((rangeAttack.translate.x + static_cast<float>(kWindowWidth) / 2) > player.translate.x - player.size && rangeAttack.translate.x - static_cast<float>(kWindowWidth) / 2 < (player.translate.x + player.size)
								&& (rangeAttack.translate.y + rangeAttack.size) > player.translate.y - player.size && (rangeAttack.translate.y - rangeAttack.size) < player.translate.y + player.size) {
								player.hp -= 1;
								rangeAttack.isMultiHit = true;
							}
						}
						// 50以上は攻撃判定を消して初期化
						if (rangeAttack.frame >= 50) {
							enemyPersonPattern = JUMP_P;
							rangeAttack.frame = 0;
							rangeAttack.isAttack = false;
							rangeAttack.isMultiHit = false;
						}
					}
				}
			}
			else {
				rangeAttack.drawSwordPos.x = enemyPerson.translate.x;
				rangeAttack.drawSwordPos.y = enemyPerson.translate.y;
			}
			// 衝撃波
			if (enemyPersonPattern == SHOCKWAVE_P) {
				if (count == 0) {
					isShockWaveJump = true;
				}
				if (isShockWaveJump) {
					// x軸の移動
					if (shockWave[0].endTime <= 1.0f) {
						shockWave[0].endTime += shockWave[0].endSpeed;
						enemyPerson.translate.x = (1.0f - shockWave[0].endTime) * shockWaveJumpPos.x + shockWave[0].endTime * kWindowWidth / 2;
					}
					// 自由落下
					shockWave[0].speed.y += -0.8f;
					enemyPerson.translate.y += shockWave[0].speed.y;
					// 地面についたら初期化
					if (enemyPerson.translate.y <= enemyPerson.size) {
						enemyPerson.translate.y = enemyPerson.size;
						if (isShockWaveJump && shockWave[0].endTime >= 0.5f) {
							count++;
						}
						shockWave[0].speed.y = 40.0f;
						shockWave[0].endTime = 0.0f;
						isShockWaveJump = false;
					}
					if (count == 1) {
						shockWave[0].isAttack = true;
						shockWave[1].isAttack = true;
					}
				}
				else {
					shockWave[0].speed.y = 40.0f;
				}

				if (shockWave[0].isAttack) {
					if (shockWave[0].translate.x <= 0 - shockWave[0].size || shockWave[0].translate.x >= kWindowWidth + shockWave[0].size) {
						shockWave[0].frame = 0;
						shockWave[0].isMultiHit = false;
						shockWave[0].isAttack = false;
						enemyPersonPattern = NONE_P;
					}
				}
				if (shockWave[1].isAttack) {
					if (shockWave[1].translate.x <= 0 - shockWave[1].size || shockWave[1].translate.x >= kWindowWidth + shockWave[1].size) {
						shockWave[1].frame = 0;
						shockWave[1].isMultiHit = false;
						shockWave[1].isAttack = false;
					}
				}

				if (shockWave[0].isAttack) {
					shockWave[0].frame++;
					shockWave[0].translate.x += shockWave[0].speed.x;
					// 当たり判定
					if (!shockWave[0].isMultiHit) {
						if ((shockWave[0].translate.x + shockWave[0].size) > player.translate.x - player.size && shockWave[0].translate.x - shockWave[0].size < (player.translate.x + player.size)
							&& (shockWave[0].translate.y + shockWave[0].size) > player.translate.y - player.size && (shockWave[0].translate.y - shockWave[0].size) < player.translate.y + player.size) {
							player.hp -= 1;
							shockWave[0].isMultiHit = true;
						}
					}
				}
				else {
					shockWave[0].translate.x = enemyPerson.translate.x;
					shockWave[0].translate.y = enemyPerson.translate.y;
				}
				if (shockWave[1].isAttack) {
					shockWave[1].frame++;
					shockWave[1].translate.x -= shockWave[1].speed.x;
					// 当たり判定
					if (!shockWave[1].isMultiHit) {
						if ((shockWave[1].translate.x + shockWave[1].size) > player.translate.x - player.size && shockWave[1].translate.x - shockWave[1].size < (player.translate.x + player.size)
							&& (shockWave[1].translate.y + shockWave[1].size) > player.translate.y - player.size && (shockWave[1].translate.y - shockWave[1].size) < player.translate.y + player.size) {
							player.hp -= 1;
							shockWave[1].isMultiHit = true;
						}
					}
				}
				else {
					shockWave[1].translate.x = enemyPerson.translate.x;
					shockWave[1].translate.y = enemyPerson.translate.y;
				}
			}
			else {
				shockWaveJumpPos.x = enemyPerson.translate.x;
				shockWaveJumpPos.y = enemyPerson.translate.y;
				count = 0;
			}
			// 落雷
			if (enemyPersonPattern == THOUNDER_P) {
				thounder[thounderCount].frame++;
				if (thounder[thounderCount].frame >= 30 && thounder[thounderCount].frame <= 35) {
					thounder[thounderCount].isThounder = true;
					if (thounder[thounderCount].isThounder) {
						thounder[thounderCount].color = RED;
						// 攻撃が多段ヒットするのでフラグで止めてる
						if (!thounder[thounderCount].isMultiHit) {
							if ((thounder[thounderCount].translate.x + static_cast<float>(kWindowHeight) / 2) > player.translate.x - player.size && thounder[thounderCount].translate.x - static_cast<float>(kWindowWidth) / 2 < (player.translate.x + player.size)
								&& (thounder[thounderCount].translate.y + thounder[thounderCount].size) > player.translate.y - player.size && (thounder[thounderCount].translate.y - thounder[thounderCount].size) < player.translate.y + player.size) {
								player.hp -= 1;
								thounder[thounderCount].isMultiHit = true;
							}
						}
					}
				}
				if (thounder[thounderCount].frame >= 35) {
					thounder[thounderCount].isThounder = false;
					thounder[thounderCount].isMultiHit = false;
					thounderCount += 1;
					thounder[thounderCount].frame = 0;
				}

				if (thounderCount >= 10) {
					enemyPersonPattern = NONE_P;
					thounderCount = 0;
				}
			}
#pragma endregion

#pragma endregion
			/********各キャラの体力がなくなると死亡********/
					// player
					/*if (player.hp <= 0) {
						player.isAlive = false;
					}*/
					// 人型の敵
			if (enemyPerson.hp <= 0) {
				enemyPerson.isAlive = false;
			}
			break;
		case LASTENEMY2:

			break;
		}


		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		// player
		if (scene != TITLE && scene != GAMEOVER && scene != CLEAR) {
			// 本体
			if (player.isAlive) {
				Novice::DrawQuad(
					player.translate.x - player.size, (player.translate.y - player.size) * -1 + kWindowHeight,
					player.translate.x + player.size, (player.translate.y - player.size) * -1 + kWindowHeight,
					player.translate.x - player.size, (player.translate.y + player.size) * -1 + kWindowHeight,
					player.translate.x + player.size, (player.translate.y + player.size) * -1 + kWindowHeight,
					0, 0,
					128, 128,
					playerGh,
					player.color
				);
			}
			// 攻撃
			if (playerAttack.isAttack) {
				Novice::DrawQuad(
					playerAttack.translate.x - playerAttack.size, (playerAttack.translate.y - playerAttack.size) * -1 + kWindowHeight,
					playerAttack.translate.x + playerAttack.size, (playerAttack.translate.y - playerAttack.size) * -1 + kWindowHeight,
					playerAttack.translate.x - playerAttack.size, (playerAttack.translate.y + playerAttack.size) * -1 + kWindowHeight,
					playerAttack.translate.x + playerAttack.size, (playerAttack.translate.y + playerAttack.size) * -1 + kWindowHeight,
					0, 0,
					128, 128,
					attackGh,
					playerAttack.color
				);
			}
		}
		switch (scene) {
		case TITLE:
			// デバッグ文字
			Novice::ScreenPrintf(0, 10, "Press keys   1:FIRSTENEMY  2:SECONDENEMY  3:LASTENEMY");
			break;
		case FIRSTENEMY:
			// enemy
			if (enemyMedium.isAlive) {
				//if (!teleport.isTeleport) {
				Novice::DrawQuad(
					enemyMedium.translate.x - enemyMedium.size, (enemyMedium.translate.y - enemyMedium.size) * -1 + kWindowHeight,
					enemyMedium.translate.x + enemyMedium.size, (enemyMedium.translate.y - enemyMedium.size) * -1 + kWindowHeight,
					enemyMedium.translate.x - enemyMedium.size, (enemyMedium.translate.y + enemyMedium.size) * -1 + kWindowHeight,
					enemyMedium.translate.x + enemyMedium.size, (enemyMedium.translate.y + enemyMedium.size) * -1 + kWindowHeight,
					0, 0,
					128, 128,
					playerGh,
					enemyMedium.color
				);
				//}
			}
			// 爆発
			if (explode.isAttack) {
				Novice::DrawQuad(
					explode.translate.x - explode.size, (explode.translate.y - explode.size) * -1 + kWindowHeight,
					explode.translate.x + explode.size, (explode.translate.y - explode.size) * -1 + kWindowHeight,
					explode.translate.x - explode.size, (explode.translate.y + explode.size) * -1 + kWindowHeight,
					explode.translate.x + explode.size, (explode.translate.y + explode.size) * -1 + kWindowHeight,
					0, 0,
					128, 128,
					playerGh,
					explode.color
				);
			}
			// デバッグ文字
			Novice::ScreenPrintf(0, 10, "Press keys   1:EXPLODE 2:CRUSH");
			break;
		case SECONDENEMY:
			break;
		case LASTENEMY1:
			// enemy
			if (enemyPerson.isAlive) {
				if (!teleport.isTeleport) {
					Novice::DrawQuad(
						enemyPerson.translate.x - enemyPerson.size, (enemyPerson.translate.y - enemyPerson.size) * -1 + kWindowHeight,
						enemyPerson.translate.x + enemyPerson.size, (enemyPerson.translate.y - enemyPerson.size) * -1 + kWindowHeight,
						enemyPerson.translate.x - enemyPerson.size, (enemyPerson.translate.y + enemyPerson.size) * -1 + kWindowHeight,
						enemyPerson.translate.x + enemyPerson.size, (enemyPerson.translate.y + enemyPerson.size) * -1 + kWindowHeight,
						0, 0,
						128, 128,
						playerGh,
						enemyPerson.color
					);
				}
			}
			// 近接攻撃
			if (normalAttack.isAttack) {
				Novice::DrawQuad(
					normalAttack.translate.x - normalAttack.size, (normalAttack.translate.y - normalAttack.size) * -1 + kWindowHeight,
					normalAttack.translate.x + normalAttack.size, (normalAttack.translate.y - normalAttack.size) * -1 + kWindowHeight,
					normalAttack.translate.x - normalAttack.size, (normalAttack.translate.y + normalAttack.size) * -1 + kWindowHeight,
					normalAttack.translate.x + normalAttack.size, (normalAttack.translate.y + normalAttack.size) * -1 + kWindowHeight,
					0, 0,
					128, 128,
					attackGh,
					normalAttack.color
				);
			}
			// 背後から攻撃
			if (behindAttack.isBehindAttack) {
				Novice::DrawQuad(
					behindAttack.translate.x - behindAttack.size, (behindAttack.translate.y - behindAttack.size) * -1 + kWindowHeight,
					behindAttack.translate.x + behindAttack.size, (behindAttack.translate.y - behindAttack.size) * -1 + kWindowHeight,
					behindAttack.translate.x - behindAttack.size, (behindAttack.translate.y + behindAttack.size) * -1 + kWindowHeight,
					behindAttack.translate.x + behindAttack.size, (behindAttack.translate.y + behindAttack.size) * -1 + kWindowHeight,
					0, 0,
					128, 128,
					attackGh,
					behindAttack.color
				);
			}
			// 範囲攻撃
			if (rangeAttack.isAttack) {
				Novice::DrawQuad(
					rangeAttack.translate.x - static_cast<float>(kWindowWidth) / 2, (rangeAttack.translate.y - rangeAttack.size) * -1 + kWindowHeight,
					rangeAttack.translate.x + static_cast<float>(kWindowWidth) / 2, (rangeAttack.translate.y - rangeAttack.size) * -1 + kWindowHeight,
					rangeAttack.translate.x - static_cast<float>(kWindowWidth) / 2, (rangeAttack.translate.y + rangeAttack.size) * -1 + kWindowHeight,
					rangeAttack.translate.x + static_cast<float>(kWindowWidth) / 2, (rangeAttack.translate.y + rangeAttack.size) * -1 + kWindowHeight,
					0, 0,
					1280, 128,
					attackGh,
					rangeAttack.color
				);
			}
			// 衝撃波
			for (int i = 0; i < 2; i++) {
				if (shockWave[i].isAttack) {
					Novice::DrawQuad(
						shockWave[i].translate.x - shockWave[i].size, (shockWave[i].translate.y - shockWave[i].size * 2) * -1 + kWindowHeight,
						shockWave[i].translate.x + shockWave[i].size, (shockWave[i].translate.y - shockWave[i].size * 2) * -1 + kWindowHeight,
						shockWave[i].translate.x - shockWave[i].size, (shockWave[i].translate.y + shockWave[i].size * 2) * -1 + kWindowHeight,
						shockWave[i].translate.x + shockWave[i].size, (shockWave[i].translate.y + shockWave[i].size * 2) * -1 + kWindowHeight,
						0, 0,
						64, 128,
						attackGh,
						RED
					);
				}
			}
			// 遠距離攻撃
			if (longAttack.isAttack) {
				Novice::DrawQuad(
					longAttack.translate.x - longAttack.size, (longAttack.translate.y - longAttack.size) * -1 + kWindowHeight,
					longAttack.translate.x + longAttack.size, (longAttack.translate.y - longAttack.size) * -1 + kWindowHeight,
					longAttack.translate.x - longAttack.size, (longAttack.translate.y + longAttack.size) * -1 + kWindowHeight,
					longAttack.translate.x + longAttack.size, (longAttack.translate.y + longAttack.size) * -1 + kWindowHeight,
					0, 0,
					64, 64,
					attackGh,
					RED
				);
			}
			// 落雷
			for (int i = 0; i < 10; i++) {
				if (thounder[i].isThounder) {
					Novice::DrawQuad(
						thounder[i].translate.x - thounder[i].size, (thounder[i].translate.y - thounder[i].size * 6) * -1 + kWindowHeight,
						thounder[i].translate.x + thounder[i].size, (thounder[i].translate.y - thounder[i].size * 6) * -1 + kWindowHeight,
						thounder[i].translate.x - thounder[i].size, (thounder[i].translate.y + thounder[i].size * 6) * -1 + kWindowHeight,
						thounder[i].translate.x + thounder[i].size, (thounder[i].translate.y + thounder[i].size * 6) * -1 + kWindowHeight,
						0, 0,
						64, 720,
						attackGh,
						RED
					);
				}
			}
			// デバッグ文字
			Novice::ScreenPrintf(0, 10, "Press keys    1:JUMP 2:BACKSTEP 3:DUSH 4:TELEPORT 5:SHOCKWAVE 6:THOUNDER");
			break;
		case LASTENEMY2:

			break;
		}
		///
		/// ↑描画処理ここまで
		///

	// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}


	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
