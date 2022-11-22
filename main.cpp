#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>

const char kWindowTitle[] = "LC1A_22_フミモトコウサク_タイトル";

const int kWindowWidth = 1280;
const int kWindowHeight = 720;

//******* パーティクル *******//
const int kParticleMax = 200;
const float gravity = 9.8f;

typedef struct Translte {
	float x;
	float y;
};

//パーティクル物理用タイマー
float particleTimer[kParticleMax];

typedef struct Particle {
	Translte pos;//位置
	int isDo = 0;//作動しているか
	float direction = 0;//飛ぶ向き
	int ViewTime = 60;
};
Particle particle[kParticleMax];
Particle particleDefault;

//角度をラジアンに変換する関数
float DEGtoRAD(float num) {
	float num2;
	num2 = num * (M_PI / 180);
	return num2;
}


#pragma region enum
// enemyの行動パターン
// 中型
enum EnemyMediumPattern {
	NONE_M,
	TAKEOFF_M,
	TACKLE_M,
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
	FLASH_P,
	DUSH_P,
	TELEPORT_P,
	SHOCKWAVE_P,// 攻撃も移動も含んでる
	JUMP_P,
	BACKSTEP_P,
	// 攻撃
	THOUNDER_P,
	NORMAL_P,
	BEHINDATTACK_P,
	LONGRANGE_P,
	RANGEATTACK_P
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
	GUIDE,			// チュートリアル
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
// シェイク
typedef struct Shake {
	Vector2 randPos;
	int randLength;
	int offset;// randの最小値
	int isShake;
};
#pragma region player
// player
typedef struct Player {
	Vector2 translate;
	Vector2 speed;
	Vector2 acceleration;
	float size;
	int frame;
	int hp;
	int isAlive;
	int isDamage;
	int isJump;
	int jumpCount;
	int isLongStun;
	int isShortStun;
	int isMultiHit;
	unsigned int color;
};

typedef struct PlayerAnim {
	int PlayerAnimCount = 0;
	int PlayerAnimSpeed = 6;
	int Attack1AnimCount = 0;
	int Attack1AnimSpeed = 2;
	int Attack1AnimFlag = 0;
	int Attack1Range = 0;
};
PlayerAnim playerAnim;

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
	float endTime;
	float endSpeed;
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
	float theta;
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
	Vector2 size;
	float endTime;
	float endSpeed;
	int count;
	int frame;
	int animCount;
	int isAttack;
	int isMultiHit;
	unsigned int color;
};
// 浮上
typedef struct Takeoff {
	Vector2 translate;
	Vector2 speed;
	float endTime;
	float endSpeed;
	int frame;
	int isTakeoff;
};
// タックル
typedef struct Tackle {
	Vector2 translate;
	Vector2 speed;
	float theta;
	float amplitude;
	float endTime;
	float endSpeed;
	int frame;
	int isTackle;
};
#pragma endregion

#pragma region 人型のenemy
// enemy人型
typedef struct EnemyPerson {
	Vector2 translate;
	Vector2 speed;
	Vector2 acceleration;
	float size;
	float theta;
	int hp;
	int isAlive;
	int isJump;
	int isStun;
	unsigned int color;
};
typedef struct EnemyAnim {
	int WaitAnimCount = 0;
	int WaitAnimSpeed = 6;
	int Attack1AnimCount = 0;
	int Attack1AnimSpeed = 2;
	int AttackAnimFlag = 0;
	int Attack1Range = 0;
	int Attack2AnimCount = 0;
	int Attack2AnimSpeed = 2;
	int Attack2Range = 0;
	int Attack3AnimCount = 0;
	int Attack3AnimSpeed = 2;
	int Attack3Range = 0;
};
EnemyAnim enemyAnim;

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
	Vector2 speed;
	float size;
	float theta;
	float endTime;
	float endSpeed;
	int frame;
	int animCount;
	int isThounder;
	int isMultiHit;
	unsigned int color;
};
// 閃光弾
typedef struct Flash {
	Vector2 translate;
	Vector2 speed;
	float size;
	float theta;
	float endTime;
	float endSpeed;
	int frame;
	int animCount;
	int isFlash;
	int carrentAlpha;
	int color;
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

//雷のアニメーション
int thunderAnimCount = 0;
int thunderAnimSpeed = 1;
int thunderRange = 0;


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

void EaseIn_Out(float& scale, float& scaleSpeed, float& radius, float firstRadius, float endRadius) {
	// 何フレームで最大の大きさになるか
	scale += scaleSpeed;
	float easedT;
	easedT = -(cosf(M_PI * scale) - 1.0f) / 2.0f;
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
		///////
		EaseIn(DushFrame, DushSpeed, enemyX, dushX, playerX - playerSize * 3);
		///////
	}
	if (playerX <= dushX) {
		///////
		EaseIn(DushFrame, DushSpeed, enemyX, dushX, playerX + playerSize * 3);
		///////
	}
	if (DushFrame >= 1.0f) {
		enemyPersonPattern = NORMAL_P;
		DushFrame = 0.0f;
	}
}

float p2e(float& playerX, float& playerY, float& enemyX, float& enemyY) {
	float distance;
	Vector2 p2e;

	p2e.x = enemyX - playerX;
	p2e.y = enemyY - playerY;

	distance = sqrt(p2e.x * p2e.x + p2e.y * p2e.y);
	return distance;
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

	//テクスチャ読み込み
	int PlayerWaitTexture = Novice::LoadTexture("./Resources/Player_taiki.png");
	int PlayerAttack1Texture = Novice::LoadTexture("./Resources/Player_Attack.png");
	int PlayerJumpTexture = Novice::LoadTexture("./Resources/Player_Jump.png");
	int EnemyWaitTexture = Novice::LoadTexture("./Resources/Enemy_taiki.png");
	int EnemyJumpTexture = Novice::LoadTexture("./Resources/Enemy_Jump.png");
	int EnemySlashTexture = Novice::LoadTexture("./Resources/Zangeki.png");
	int EnemyBackstepTexture = Novice::LoadTexture("./Resources/Enemy_BackStep.png");
	int EnemyChageTexture = Novice::LoadTexture("./Resources/Enemy_Tame.png");
	int EnemyAttack1Texture = Novice::LoadTexture("./Resources/Enemy_Attack.png");
	int EnemyAttack2Texture = Novice::LoadTexture("./Resources/Enemy_Attack2.png");
	int EnemyAttack3Texture = Novice::LoadTexture("./Resources/Enemy_Attack3.png");
	int thunderTexture = Novice::LoadTexture("./Resources/thunder.png");

	/**********宣言**********/
#pragma region シェイク
	// シェイク
	Shake shake = {
		{0.0f,0.0f},
		11,
		-5,
		false
	};
	// 乱数
	// 移動パターンの乱数
	unsigned int currentGameTime = time(nullptr);
	srand(currentGameTime);
#pragma endregion

#pragma region player
	// 本体
	Player player = {
		{64 * 2,48}, // 位置
		{7.0f,10.0f}, // 速度
		{0.0f,-0.8f},// 重力加速度
		48.0f,		 // サイズ
		0,
		3,			 // hp
		true,		 // 生き死に
		false,		 // ダメージを受けているか
		false,		 // ジャンプしてるか
		0,			 // ジャンプした数
		false,
		false,
		false,		 // 多段ヒット防止
		WHITE		 // 色
	};

	Vector2 playerPosBox = {
		0.0f, 0.0f
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
		{kWindowWidth - 64.0f * 2,48.0f},
		{6.0f,0.0f},
		{0.0f,-0.8f},
		48.0f,
		0.0f,
		15,
		true,
		false,
		false,
		WHITE
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
				1.0f / 30.0f,
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
				{100.0f,100.0f},
				64.0f,
				0.0f,
				0.0f,
				1.0f / 30.0f,
				0,
				0,
				false,
				false,
				WHITE
		};
	}
	int thounderCount = 0;
	// 閃光弾
	Flash flash = {
		{0.0f,0.0f},
		{6.0f,14.0f},
		16.0f,
		0.0f,
		0.0f,
		1.0f / 50.0f,
		0,
		0,
		false,
		0x00,
		WHITE
	};
	int isMultiHit = false;
	/**************enemyの移動パターン**************/
	// enemyが何もしないとき
	int NoneTime = 60;
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

	// playerとenemyの距離
	float distance;

	// NONE以外は攻撃を食らわない
	int isInvincble = false;
	int enemyCarrentAlpha = 0x00;

	// 形態変化
	int isSecondForm = false;
	int isFadeIn = true;
	int isFadeOut = false;
	int formFrame = 0;
	int formCarrentAlpha = 0x00;

	// 攻撃予測範囲
	// 落雷
	int isThounderPredictionRange[10];
	for (int i = 0; i < 10; i++) {
		isThounderPredictionRange[i] = {
			false
		};
	}
	// 範囲攻撃
	int isRangeAttackPrediction = false;

	// クリア画面に移行するまでに必要な宣言
	Vector2 clearPos = {
		0,0
	};
	float clearRadius = 1;
	float clearEndTime = 0;
	float clearEndSpeed = 1.0f / 60.0f;

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
		///////
		if (!backStep.isBackStep) {
			playerPosBox.x = player.translate.x;
			playerPosBox.y = player.translate.y;
		}
		///////
		// 形態変化
		if (isSecondForm) {
			playerAttack.isAttack = false;
			enemyPersonPattern = NONE_P;
			formFrame++;
			if (formFrame <= 180) {
				isFadeIn = true;
			}
			// だんだん画面が明るくなる
			if (isFadeIn) {
				if (formCarrentAlpha >= -0xff) {
					formCarrentAlpha -= 0x10;
				}
				if (formCarrentAlpha <= -0xff) {
					formCarrentAlpha = -0xff;
				}
			}
			if (formFrame >= 180) {
				// playerとenemyの状態をリセット
				player.translate.x = 64 * 2;
				player.translate.y = 48;
				enemyPerson.translate.x = kWindowWidth - 64;
				enemyPerson.translate.y = 48;
				// フェードアウト開始
				scene = LASTENEMY2;
				isFadeOut = true;
				isFadeIn = false;
			}
			// だんだん画面が暗くなる
			if (isFadeOut) {
				if (formCarrentAlpha <= 0x00) {
					formCarrentAlpha += 0x10;
				}
				if (formCarrentAlpha >= 0x00) {
					formCarrentAlpha = 0x00;
					isSecondForm = false;
				}
			}
		}
		else {
			isFadeOut = false;
			isFadeIn = false;
			formFrame = 0;
			formCarrentAlpha = 0x00;
		}
		// タイトル、ゲームオーバー、クリア画面以外でplayerを動かせる
		if (!isSecondForm) {
			#pragma region player
			// playerの移動キー
			// 左に移動
			if (!player.isLongStun) {
				if (keys[DIK_A] || Novice::IsPressButton(0, kPadButton2)) {
					player.translate.x -= player.speed.x;
					playerDir = LEFT;
				}
				// 右に移動
				if (keys[DIK_D] || Novice::IsPressButton(0, kPadButton3)) {
					player.translate.x += player.speed.x;
					playerDir = RIGHT;
				}
				// ジャンプのキー
				if (!keys[DIK_W] && preKeys[DIK_W] || Novice::IsTriggerButton(0, kPadButton0)) {
					player.jumpCount++;
					// 最初のジャンプ
					if (player.jumpCount == 1) {
						// ジャンプの初速
						//////
						player.speed.y = 17.0f;
						//////
						player.isJump = true;
					}
					// 2段ジャンプ(1段目より高く飛べない)
					if (player.jumpCount == 2) {
						player.speed.y = 14.0f;
						player.isJump = true;
						for (int k = 0; k < 5; k++) {
							for (int i = 0; i < kParticleMax; i++) {
								if (!particle[i].isDo) {
									particle[i].pos.x = player.translate.x;
									particle[i].pos.y = player.translate.y + 224;
									particle[i].direction = DEGtoRAD(180 + (k * 45));
									particle[i].isDo = 1;
									break;
								}
							}
						}
					}
				}
				// 近接攻撃のキー
				if (!keys[DIK_SPACE] && preKeys[DIK_SPACE] && !playerAttack.isAttack) {
					playerAttack.isAttack = true;
				}
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
			// 人型
			if (scene == LASTENEMY1 || scene == LASTENEMY2) {
				if (!player.isMultiHit) {
					// enemy本体とplayerの近接攻撃の当たり判定
					if ((player.translate.x + player.size - 32) > enemyPerson.translate.x - (enemyPerson.size - 32) && player.translate.x - player.size - 32 < (enemyPerson.translate.x + (enemyPerson.size - 32))
						&& (player.translate.y + player.size - 32) > enemyPerson.translate.y - enemyPerson.size - 32 && (player.translate.y - player.size - 32) < enemyPerson.translate.y + enemyPerson.size - 32) {
						player.hp -= 1;
						if (player.frame <= 60) {
							player.isMultiHit = true;
							player.isDamage = true;
						}
					}
				}
			}
			/**********playerとenemyの当たり判定ここまで↑**********/

			// ダメージを食らったら60フレーム無敵+攻撃はできない
			if (player.isDamage) {
				player.frame++;
				if (player.frame >= 60) {
					player.isMultiHit = false;
					player.isDamage = false;
					player.frame = 0;
				}
			}

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
				player.isJump = false;
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
						if (enemyPersonPattern == NONE_P && !isInvincble) {
							enemyPerson.hp -= 1;
							playerAttack.isMultiHit = true;
						}
					}
				}
				// 15以上は攻撃判定を消して初期化
				if (playerAttack.frame >= 10) {
					playerAttack.frame = 0;
					playerAttack.isAttack = false;
					playerAttack.isMultiHit = false;
				}
			}
			/**********playerの近接攻撃処理ここまで↑**********/
#pragma endregion
		}
		// playerとenemyの距離
		distance = p2e(player.translate.x, player.translate.y, enemyPerson.translate.x, enemyPerson.translate.y);

		// シェイクの処理
		if (shake.isShake) {
			shake.randPos.x = rand() % shake.randLength - shake.offset;
			shake.randPos.y = rand() % shake.randLength - shake.offset;
		}
		else {
			shake.randPos.x = 0.0f;
			shake.randPos.y = 0.0f;
		}
		// 攻撃が通らないとき
		// enemyを半透明にする
		if (isInvincble) {
			enemyCarrentAlpha = 0x80;
		}
		else {
			enemyCarrentAlpha = 0x00;
		}
		
		switch (scene) {
		case TITLE:
#pragma region デバッグ用のキー
			enemyPersonPattern = NONE_P;
			if (!keys[DIK_1] && preKeys[DIK_1]) {
				scene = GUIDE;
			}
			if (!keys[DIK_2] && preKeys[DIK_2]) {
				scene = LASTENEMY1;
			}
			if (!keys[DIK_3] && preKeys[DIK_3]) {
				scene = LASTENEMY2;
			}
#pragma endregion
			break;
		case GUIDE:

			break;
		case LASTENEMY1:
#pragma region デバッグ用のキー
			if (!keys[DIK_1] && preKeys[DIK_1]) {
				NoneTime = 60;
				enemyPersonPattern = JUMP_P;
			}
			if (!keys[DIK_2] && preKeys[DIK_2]) {
				NoneTime = 60;
				enemyPersonPattern = BACKSTEP_P;
			}
			if (!keys[DIK_3] && preKeys[DIK_3]) {
				NoneTime = 60;
				enemyPersonPattern = DUSH_P;
			}
			if (!keys[DIK_4] && preKeys[DIK_4]) {
				NoneTime = 60;
				enemyPersonPattern = TELEPORT_P;
			}
			if (!keys[DIK_5] && preKeys[DIK_5]) {
				NoneTime = 60;
				enemyPersonPattern = SHOCKWAVE_P;
			}
			if (!keys[DIK_6] && preKeys[DIK_6]) {
				NoneTime = 60;
				enemyPersonPattern = THOUNDER_P;
			}
#pragma endregion
			
			if (!isSecondForm) {
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

#pragma region 行動パターン
				
				// 何もしない
				if (enemyPersonPattern == NONE_P) {
					isInvincble = false;
					enemyPerson.speed.y = -10.0f;
					enemyPerson.speed.y += enemyPerson.acceleration.y;
					enemyPerson.translate.y += enemyPerson.speed.y;
					if (enemyPerson.translate.y <= enemyPerson.size) {
						enemyPerson.translate.y = enemyPerson.size;
						NoneTime--;
					}
					normalAttack.color = WHITE;
					normalAttack.isAttack = false;
					// 攻撃が終わるごとに2秒硬直
					if (NoneTime <= 0) {
						enemyPerson.speed.y = 38.0f;
						NoneTime = 60;
						if (distance >= 0 && distance <= 200) {
							enemyPersonPattern = rand() % 6 + 3;
						}
						if (distance >= 201) {
							enemyPersonPattern = rand() % 6 + 2;
						}
					}
				}
				else {
					// NONE_P以外は攻撃を食らわない
					isInvincble = true;
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
							///////
							enemyPerson.translate.x = player.translate.x - player.size * 3;
						}
						if (playerDir == LEFT) {
							enemyPerson.translate.x = player.translate.x + player.size * 3;
						}
						///////
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
						///////
						if (playerPosBox.x <= backStep.translate.x) {
							///////
							enemyPerson.translate.x = (1.0f - backStep.endTime) * backStep.translate.x + backStep.endTime * 1180.0f;
						}
						///////
						if (playerPosBox.x >= backStep.translate.x) {
							///////
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

#pragma region 近接攻撃
				// 近接攻撃ここから↓
				if (enemyPersonPattern == NORMAL_P) {
					normalAttack.frame++;
					normalAttack.isAttack = false;
					///////
					// 攻撃前はenemyが揺れる
					if (normalAttack.frame <= 30) {
						shake.isShake = true;
					}
					else {
						shake.isShake = false;
					}
					///////

					// 1撃目
					if (normalAttack.frame >= 30 && normalAttack.frame <= 40) {
						normalAttack.isAttack = true;
						enemyAnim.AttackAnimFlag = 1;
						if (enemyDir == RIGHT_E) {
							enemyPerson.translate.x += 0.5f;
						}
						if (enemyDir == LEFT_E) {
							enemyPerson.translate.x -= 0.5f;
						}
					}
					// 2撃目
					if (normalAttack.frame >= 60 && normalAttack.frame <= 70) {
						normalAttack.isAttack = true;
						enemyAnim.AttackAnimFlag = 2;
						if (enemyDir == RIGHT_E) {
							enemyPerson.translate.x += 0.5f;
						}
						if (enemyDir == LEFT_E) {
							enemyPerson.translate.x -= 0.5f;
						}
					}
					// 3撃目
					if (normalAttack.frame >= 80 && normalAttack.frame <= 90) {
						normalAttack.isAttack = true;
						enemyAnim.AttackAnimFlag = 3;
						if (enemyDir == RIGHT_E) {
							enemyPerson.translate.x += 10.0f;
						}
						if (enemyDir == LEFT_E) {
							enemyPerson.translate.x -= 10.0f;
						}
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
					if (normalAttack.frame >= 91) {
						normalAttack.isAttack = false;
						normalAttack.isMultiHit = false;
						enemyPersonPattern = NONE_P;
						normalAttack.frame = 0;
					}
				}
				// 近接攻撃ここまで↑

#pragma endregion

#pragma region 背後から攻撃
			// 背後から攻撃
				if (enemyPersonPattern == BEHINDATTACK_P) {
					behindAttack.frame++;
					behindAttack.isBehindAttack = false;
					// 攻撃前はenemyが揺れる
					if (behindAttack.frame <= 30) {
						shake.isShake = true;
					}
					else {
						shake.isShake = false;
					}

					// 1撃目
					if (behindAttack.frame >= 30 && behindAttack.frame <= 40) {
						behindAttack.isBehindAttack = true;
						enemyAnim.AttackAnimFlag = 1;
						if (enemyDir == RIGHT_E) {
							enemyPerson.translate.x += 0.5f;
						}
						if (enemyDir == LEFT_E) {
							enemyPerson.translate.x -= 0.5f;
						}
					}
					// 2撃目
					if (behindAttack.frame >= 60 && behindAttack.frame <= 70) {
						behindAttack.isBehindAttack = true;
						enemyAnim.AttackAnimFlag = 2;
						if (enemyDir == RIGHT_E) {
							enemyPerson.translate.x += 0.5f;
						}
						if (enemyDir == LEFT_E) {
							enemyPerson.translate.x -= 0.5f;
						}
					}
					// 3撃目
					if (behindAttack.frame >= 80 && behindAttack.frame <= 90) {
						behindAttack.isBehindAttack = true;
						enemyAnim.AttackAnimFlag = 3;
						if (enemyDir == RIGHT_E) {
							enemyPerson.translate.x += 10.0f;
						}
						if (enemyDir == LEFT_E) {
							enemyPerson.translate.x -= 10.0f;
						}
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
						enemyAnim.AttackAnimFlag = 0;
						behindAttack.isMultiHit = false;
						enemyPersonPattern = NONE_P;
						behindAttack.frame = 0;
					}
				}

#pragma endregion

#pragma region 遠距離攻撃
				// 遠距離攻撃
				if (enemyPersonPattern == LONGRANGE_P) {
					longAttack.frame++;
					// 攻撃前はenemyが揺れる
					if (longAttack.frame <= 15) {
						shake.isShake = true;
					}
					else {
						shake.isShake = false;
					}

					if (longAttack.frame >= 15) {
						longAttack.isAttack = true;
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
#pragma endregion

#pragma region 範囲攻撃
				// 範囲攻撃
				if (enemyPersonPattern == RANGEATTACK_P) {
					rangeAttack.frame++;
					enemyPerson.speed.x = 100.0f;
					// 攻撃前はenemyが揺れる
					// 攻撃の予測範囲が出る
					if (rangeAttack.frame <= 25) {
						shake.isShake = true;
						isRangeAttackPrediction = true;
					}
					else {
						shake.isShake = false;
						isRangeAttackPrediction = false;
					}

					if (rangeAttack.frame >= 25) {
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
					if (rangeAttack.frame >= 32) {
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
#pragma endregion

#pragma region 衝撃波
				// 衝撃波
				if (enemyPersonPattern == SHOCKWAVE_P) {
					/// ジャンプの処理ここから↓
					if (count == 0) {
						isShockWaveJump = true;
						for (int i = 0; i < 2; i++) {
							shockWave[0].endSpeed = 1.0f / 30.0f;
						}
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
							for (int i = 0; i < 2; i++) {
								shockWave[0].endSpeed = 1.0f / 40.0f;
							}
						}
					}
					else {
						shockWave[0].speed.y = 40.0f;
					}
					/// ジャンプの処理ここまで↑

					// 画面外に出たら弾を消す
					for (int i = 0; i < 2; i++) {
						if (shockWave[i].isAttack) {
							if (shockWave[i].translate.x <= 0 - shockWave[i].size || shockWave[i].translate.x >= kWindowWidth + shockWave[i].size) {
								shockWave[i].frame = 0;
								shockWave[i].isMultiHit = false;
								shockWave[i].isAttack = false;
							}
						}
					}
					// 両方の弾が消えたらNONEに戻す
					if (count == 1) {
						shockWave[0].endSpeed = 1.0f / 40.0f;
						shockWave[1].endSpeed = 1.0f / 40.0f;
						if (!shockWave[0].isAttack && !shockWave[1].isAttack) {
							enemyPersonPattern = NONE_P;
						}
					}

					/// 当たり判定の処理ここから↓
					// 右に行く衝撃波
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
					// 左に行く衝撃波
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
					/// 当たり判定の処理ここまで↑

				}
				else {
					shockWave[0].translate.x = enemyPerson.translate.x;
					shockWave[0].translate.y = enemyPerson.translate.y;
					shockWave[1].translate.x = enemyPerson.translate.x;
					shockWave[1].translate.y = enemyPerson.translate.y;
					shockWaveJumpPos.x = enemyPerson.translate.x;
					shockWaveJumpPos.y = enemyPerson.translate.y;
					count = 0;
				}
#pragma endregion

#pragma region 落雷
				// 落雷
				// playerとenemy間のthetaを取得
				thounder[thounderCount].theta = atan2(player.translate.y - thounder[thounderCount].translate.y, player.translate.x - thounder[thounderCount].translate.x);
				if (enemyPersonPattern == THOUNDER_P) {
					if (!teleport.isTeleport && thounder[thounderCount].frame == 0) {
						teleport.frame = 1;
					}
					if (thounder[0].frame == 0) {
						teleport.frame++;
					}
					if (teleport.frame >= 1 && teleport.frame <= 20) {
						teleport.isTeleport = true;
					}
					if (teleport.isTeleport) {
						enemyPerson.translate.x = kWindowWidth / 2;
						enemyPerson.translate.y = 600.0f;
					}
					if (teleport.frame >= 21) {
						teleport.isTeleport = false;
						teleport.frame = 0;
					}
					if (!teleport.isTeleport) {
						thounder[thounderCount].frame++;
					}
					// 攻撃判定が出るまでplayerを追尾
					if (!thounder[thounderCount].isThounder) {
						// 攻撃の判定が出る20フレームまえに追尾をやめる
						if (thounder[thounderCount].frame <= 20) {
							// 追尾の処理
							thounder[thounderCount].translate.x += (cosf(thounder[thounderCount].theta) * thounder[thounderCount].speed.x);
						}
						// 落雷の予測位置が出る
						else if (thounder[thounderCount].frame >= 21 && thounder[thounderCount].frame <= 39) {
							isThounderPredictionRange[thounderCount] = true;
						}
						else if (thounder[thounderCount].frame >= 40) {
							isThounderPredictionRange[thounderCount] = false;
						}
					}

					if (thounder[thounderCount].frame >= 40 && thounder[thounderCount].frame <= 43) {
						thounder[thounderCount].isThounder = true;
					}
					// 初期化
					if (thounder[thounderCount].frame >= 43) {
						thounder[thounderCount].isThounder = false;
						thounder[thounderCount].isMultiHit = false;
						thounderCount += 1;
						thounder[thounderCount].frame = 1;
					}
					// 全部打ち終えたらパターンNONEに戻る
					if (thounderCount >= 10 && thounder[thounderCount].frame <= 80) {
						for (int i = 0; i < 10; i++) {
							thounder[i].frame = 0;
						}
						enemyPersonPattern = NONE_P;
						thounderCount = 0;
					}
					if (thounder[thounderCount].isThounder) {
						// 当たり判定がある時だけ赤くなる
						thounder[thounderCount].color = RED;

						// 攻撃が多段ヒットするのでフラグで止めてる
						if (!thounder[thounderCount].isMultiHit) {
							// playerとの当たり判定
							if ((thounder[thounderCount].translate.x + thounder[thounderCount].size) > player.translate.x - player.size && thounder[thounderCount].translate.x - thounder[thounderCount].size < (player.translate.x + player.size)
								&& (thounder[thounderCount].translate.y + static_cast<float>(kWindowHeight) / 2) > player.translate.y - player.size && (thounder[thounderCount].translate.y - static_cast<float>(kWindowHeight) / 2) < player.translate.y + player.size) {
								player.hp -= 1;
								thounder[thounderCount].isMultiHit = true;
							}
						}
					}
				}
#pragma endregion
				// 攻撃パターンのendregion
#pragma endregion
// 行動パターンのendregion
#pragma endregion
			}
			// 死ぬ処理
			// player
			if (player.hp <= 0) {
				player.isAlive = false;
				scene = GAMEOVER;
			}

			// 形態変化
			// 人型の敵
			if (enemyPerson.hp <= 0) {
				isSecondForm = true;
				enemyPerson.hp = 15;
			}
			break;
		case LASTENEMY2:
#pragma region デバッグ用のキー
			if (!keys[DIK_1] && preKeys[DIK_1]) {
				NoneTime = 60;
				enemyPersonPattern = JUMP_P;
			}
			if (!keys[DIK_2] && preKeys[DIK_2]) {
				NoneTime = 60;
				enemyPersonPattern = BACKSTEP_P;
			}
			if (!keys[DIK_3] && preKeys[DIK_3]) {
				NoneTime = 60;
				enemyPersonPattern = DUSH_P;
			}
			if (!keys[DIK_4] && preKeys[DIK_4]) {
				NoneTime = 60;
				enemyPersonPattern = TELEPORT_P;
			}
			if (!keys[DIK_5] && preKeys[DIK_5]) {
				NoneTime = 60;
				enemyPersonPattern = SHOCKWAVE_P;
			}
			if (!keys[DIK_6] && preKeys[DIK_6]) {
				NoneTime = 60;
				enemyPersonPattern = THOUNDER_P;
			}
			if (!keys[DIK_7] && preKeys[DIK_7]) {
				NoneTime = 60;
				enemyPersonPattern = FLASH_P;
			}
#pragma endregion

			if (!isSecondForm) {
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
#pragma region 行動パターン
				// 何もしない
				if (enemyPersonPattern == NONE_P) {
					isInvincble = false;
					// 地面まで引きずり落とす
					enemyPerson.speed.y = -10.0f;
					enemyPerson.speed.y += enemyPerson.acceleration.y;
					enemyPerson.translate.y += enemyPerson.speed.y;
					if (enemyPerson.translate.y <= enemyPerson.size) {
						enemyPerson.translate.y = enemyPerson.size;
						NoneTime--;
					}
					normalAttack.color = WHITE;
					normalAttack.isAttack = false;
					// 攻撃が終わるごとに2秒硬直
					if (NoneTime <= 0) {
						enemyPerson.speed.y = 38.0f;
						NoneTime = 60;
						if (distance >= 0 && distance <= 200) {
							enemyPersonPattern = rand() % 6 + 3;
						}
						if (distance >= 201 && distance <= 400) {
							enemyPersonPattern = rand() % 6 + 2;
						}
						if (distance >= 401) {
							enemyPersonPattern = rand() % 7 + 1;
						}
					}
				}
				else {
					// NONE_P以外は攻撃を食らわない
					isInvincble = true;
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
							enemyPerson.translate.x = player.translate.x - player.size * 3;
						}
						if (playerDir == LEFT) {
							enemyPerson.translate.x = player.translate.x + player.size * 3;
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
					enemyPerson.speed.y = 38.0f;
				}
				// バックステップ
				if (enemyPersonPattern == BACKSTEP_P) {
					backStep.isBackStep = true;
					backStep.endTime += backStep.endSpeed;
					// x軸の移動
					if (backStep.endTime <= 1.0f) {
						///////
						if (playerPosBox.x <= backStep.translate.x) {
							enemyPerson.translate.x = (1.0f - backStep.endTime) * backStep.translate.x + backStep.endTime * 1180.0f;
						}
						if (playerPosBox.x >= backStep.translate.x) {
							enemyPerson.translate.x = (1.0f - backStep.endTime) * backStep.translate.x + backStep.endTime * 100.0f;
						}
						///////
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
					//DushPattern(enemyPerson.translate.x, enemyPerson.translate.y, dash.translate.x, dash.translate.y, dash.endTime, dash.endSpeed, player.translate.x, player.translate.y, player.size);
					if (player.translate.x >= dash.translate.x) {
						///////
						EaseIn(dash.endTime, dash.endSpeed, enemyPerson.translate.x, dash.translate.x, player.translate.x - player.size * 3);
						///////
					}
					if (player.translate.x <= dash.translate.x) {
						///////
						EaseIn(dash.endTime, dash.endSpeed, enemyPerson.translate.x, dash.translate.x, player.translate.x + player.size * 3);
						///////
					}
					if (dash.endTime >= 1.0f) {
						enemyPersonPattern = rand() % 6 + 3;
						dash.endTime = 0.0f;
					}
				}
				else {
					dash.translate.x = enemyPerson.translate.x;
					dash.translate.y = enemyPerson.translate.y;
				}
#pragma endregion

#pragma region 攻撃パターン

#pragma region 近接攻撃
				if (enemyPersonPattern == NORMAL_P) {
					normalAttack.frame++;
					normalAttack.isAttack = false;
					if (normalAttack.frame <= 30) {
						shake.isShake = true;
					}
					else {
						shake.isShake = false;
					}
					// 1撃目
					if (normalAttack.frame >= 30 && normalAttack.frame <= 40) {
						normalAttack.isAttack = true;
						if (enemyDir == RIGHT_E) {
							enemyPerson.translate.x += 0.5f;
						}
						if (enemyDir == LEFT_E) {
							enemyPerson.translate.x -= 0.5f;
						}
					}
					// 2撃目
					if (normalAttack.frame >= 60 && normalAttack.frame <= 70) {
						normalAttack.isAttack = true;
						if (enemyDir == RIGHT_E) {
							enemyPerson.translate.x += 0.5f;
						}
						if (enemyDir == LEFT_E) {
							enemyPerson.translate.x -= 0.5f;
						}
					}
					// 3撃目
					if (normalAttack.frame >= 80 && normalAttack.frame <= 90) {
						normalAttack.isAttack = true;
						if (enemyDir == RIGHT_E) {
							enemyPerson.translate.x += 10.0f;
						}
						if (enemyDir == LEFT_E) {
							enemyPerson.translate.x -= 10.0f;
						}
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
					if (normalAttack.frame >= 91) {
						normalAttack.isAttack = false;
						normalAttack.isMultiHit = false;
						enemyPersonPattern = NONE_P;
						normalAttack.frame = 0;
					}
				}
#pragma endregion

#pragma region 背後から攻撃
				// 背後から攻撃
				if (enemyPersonPattern == BEHINDATTACK_P) {
					behindAttack.frame++;
					behindAttack.isBehindAttack = false;
					if (behindAttack.frame <= 30) {
						shake.isShake = true;
					}
					else {
						shake.isShake = false;
					}
					// 1撃目
					if (behindAttack.frame >= 30 && behindAttack.frame <= 40) {
						behindAttack.isBehindAttack = true;
						if (enemyDir == RIGHT_E) {
							enemyPerson.translate.x += 0.5f;
						}
						if (enemyDir == LEFT_E) {
							enemyPerson.translate.x -= 0.5f;
						}
					}
					// 2撃目
					if (behindAttack.frame >= 60 && behindAttack.frame <= 70) {
						behindAttack.isBehindAttack = true;
						if (enemyDir == RIGHT_E) {
							enemyPerson.translate.x += 0.5f;
						}
						if (enemyDir == LEFT_E) {
							enemyPerson.translate.x -= 0.5f;
						}
					}
					// 3撃目
					if (behindAttack.frame >= 80 && behindAttack.frame <= 90) {
						behindAttack.isBehindAttack = true;
						if (enemyDir == RIGHT_E) {
							enemyPerson.translate.x += 10.0f;
						}
						if (enemyDir == LEFT_E) {
							enemyPerson.translate.x -= 10.0f;
						}
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
#pragma endregion

#pragma region 遠距離攻撃
				// 遠距離攻撃
				if (enemyPersonPattern == LONGRANGE_P) {
					longAttack.frame++;
					longAttack.speed.x = 30.0f;
					// 攻撃前はenemyが揺れる
					if (longAttack.frame <= 40) {
						shake.isShake = true;
					}
					else {
						shake.isShake = false;
					}

					if (longAttack.frame >= 40) {
						longAttack.isAttack = true;
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
					longAttack.speed.x = 20.0f;
				}
#pragma endregion

#pragma region 範囲攻撃
				// 範囲攻撃
				if (enemyPersonPattern == RANGEATTACK_P) {
					rangeAttack.frame++;
					enemyPerson.speed.x = 100.0f;
					// 攻撃前はenemyが揺れる
					// 攻撃の予測範囲が出る
					if (rangeAttack.frame <= 25) {
						shake.isShake = true;
						isRangeAttackPrediction = true;
					}
					else {
						shake.isShake = false;
						isRangeAttackPrediction = false;
					}

					if (rangeAttack.frame >= 25) {
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

					if (rangeAttack.frame >= 32) {
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
								enemyPersonPattern = SHOCKWAVE_P;
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
#pragma endregion 

#pragma region 衝撃波
				// 衝撃波
				if (enemyPersonPattern == SHOCKWAVE_P) {
					if (count == 0) {
						isShockWaveJump = true;
						for (int i = 0; i < 2; i++) {
							shockWave[i].endSpeed = 1.0f / 30.0f;
						}
					}
					if (isShockWaveJump) {
						// x軸の移動
						if (shockWave[0].endTime <= 1.0f) {
							shockWave[0].endSpeed = 1.0f / 30.0f;
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
						shockWave[0].frame++;
						if (shockWave[0].endTime <= 2.0f) {
							EaseIn_Out(shockWave[0].endTime, shockWave[0].endSpeed, shockWave[0].translate.x, enemyPerson.translate.x, enemyPerson.translate.x + kWindowWidth);
						}
						else {
							shockWave[0].frame = 0;
							shockWave[0].endTime = 0.0f;
							shockWave[0].isMultiHit = false;
							shockWave[0].isAttack = false;
						}
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
						if (shockWave[1].endTime <= 2.0f) {
							EaseIn_Out(shockWave[1].endTime, shockWave[1].endSpeed, shockWave[1].translate.x, enemyPerson.translate.x, enemyPerson.translate.x - kWindowWidth);
						}
						else {
							shockWave[1].frame = 0;
							shockWave[1].endTime = 0.0f;
							shockWave[1].isMultiHit = false;
							shockWave[1].isAttack = false;
						}
						// 当たり判定
						if (!shockWave[1].isMultiHit) {
							if ((shockWave[1].translate.x + shockWave[1].size) > player.translate.x - player.size && shockWave[1].translate.x - shockWave[1].size < (player.translate.x + player.size)
								&& (shockWave[1].translate.y + shockWave[1].size) > player.translate.y - player.size && (shockWave[1].translate.y - shockWave[1].size) < player.translate.y + player.size) {
								player.hp -= 1;
								shockWave[1].isMultiHit = true;
							}
						}
						if (count == 1) {
							for (int i = 0; i < 2; i++) {
								shockWave[0].endSpeed = 1.0f / 40.0f;
								shockWave[1].endSpeed = 1.0f / 40.0f;
								if (!shockWave[0].isAttack && !shockWave[1].isAttack) {
									enemyPersonPattern = NONE_P;
								}
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
#pragma endregion

#pragma region 落雷
				// 落雷
				// playerとenemy間のthetaを取得
				thounder[thounderCount].theta = atan2(player.translate.y - thounder[thounderCount].translate.y, player.translate.x - thounder[thounderCount].translate.x);
				if (enemyPersonPattern == THOUNDER_P) {
#pragma region 攻撃前のテレポート
					if (!teleport.isTeleport && thounder[thounderCount].frame == 0) {
						teleport.frame = 1;
					}
					if (thounder[0].frame == 0) {
						teleport.frame++;
					}
					if (teleport.frame >= 1 && teleport.frame <= 20) {
						teleport.isTeleport = true;
					}
					if (teleport.isTeleport) {
						enemyPerson.translate.x = kWindowWidth / 2;
						enemyPerson.translate.y = 600.0f;
					}
					if (teleport.frame >= 21) {
						teleport.isTeleport = false;
						teleport.frame = 0;
					}
					if (!teleport.isTeleport) {
						thounder[thounderCount].frame++;
					}
#pragma endregion

					// 攻撃判定が出るまでplayerを追尾
					if (!thounder[thounderCount].isThounder) {
						// 攻撃の判定が出る20フレームまえに追尾をやめる
						if (thounder[thounderCount].frame <= 20) {
							// 追尾の処理
							thounder[thounderCount].translate.x += (cosf(thounder[thounderCount].theta) * thounder[thounderCount].speed.x);
						}
						// 落雷の予測位置が出る
						else if (thounder[thounderCount].frame >= 21 && thounder[thounderCount].frame <= 39) {
							isThounderPredictionRange[thounderCount] = true;
						}
						else if (thounder[thounderCount].frame >= 40) {
							isThounderPredictionRange[thounderCount] = false;
						}
					}

					if (thounder[thounderCount].frame >= 40 && thounder[thounderCount].frame <= 43) {
						thounder[thounderCount].isThounder = true;
					}
					// 初期化
					if (thounder[thounderCount].frame >= 43) {
						thounder[thounderCount].isThounder = false;
						thounder[thounderCount].isMultiHit = false;
						thounderCount += 1;
						thounder[thounderCount].frame = 1;
					}
					// 全部打ち終えたらパターンNONEに戻る
					if (thounderCount >= 10 && thounder[thounderCount].frame <= 80) {
						for (int i = 0; i < 10; i++) {
							thounder[i].frame = 0;
						}
						enemyPersonPattern = NONE_P;
						thounderCount = 0;
					}
					if (thounder[thounderCount].isThounder) {
						// 当たり判定がある時だけ赤くなる
						thounder[thounderCount].color = RED;

						// 攻撃が多段ヒットするのでフラグで止めてる
						if (!thounder[thounderCount].isMultiHit) {
							// playerとの当たり判定
							if ((thounder[thounderCount].translate.x + thounder[thounderCount].size) > player.translate.x - player.size && thounder[thounderCount].translate.x - thounder[thounderCount].size < (player.translate.x + player.size)
								&& (thounder[thounderCount].translate.y + static_cast<float>(kWindowHeight) / 2) > player.translate.y - player.size && (thounder[thounderCount].translate.y - static_cast<float>(kWindowHeight) / 2) < player.translate.y + player.size) {
								player.hp -= 1;
								thounder[thounderCount].isMultiHit = true;
							}
						}
					}
				}
#pragma endregion

#pragma region 閃光弾
				// 閃光弾
				if (enemyPersonPattern == FLASH_P) {
					flash.frame++;
					if (flash.frame <= 60) {
						flash.isFlash = true;
					}
					else {
						flash.isFlash = false;
					}
					if (flash.isFlash) {
						flash.speed.y += -0.3f;
						flash.translate.y += flash.speed.y;
						if (enemyDir == LEFT_E) {
							EaseIn(flash.endTime, flash.endSpeed, flash.translate.x, enemyPerson.translate.x, enemyPerson.translate.x - 150.0f);
						}
						if (enemyDir == RIGHT_E) {
							EaseIn(flash.endTime, flash.endSpeed, flash.translate.x, enemyPerson.translate.x, enemyPerson.translate.x + 150.0f);
						}
					}
					else {
						flash.endTime = 0.0f;
					}
					if (flash.frame >= 60 && flash.frame <= 61) {
						if (playerDir == LEFT && player.translate.x >= flash.translate.x || playerDir == RIGHT && player.translate.x <= flash.translate.x) {
							player.isLongStun = true;
						}
						else {
							player.isShortStun = true;
						}
					}
					if (flash.frame >= 61) {
						flash.frame = 0;
						enemyPersonPattern = NONE_P;
					}
				}
				else {
					flash.translate.x = enemyPerson.translate.x;
					flash.translate.y = enemyPerson.translate.y;
					flash.speed.y = 14.0f;
				}
				// 長いスタン
				if (player.isLongStun) {
					flash.carrentAlpha += 0x02;
				}
				// スタンなし
				if (player.isShortStun) {
					flash.carrentAlpha += 0x10;
				}
				if (flash.carrentAlpha >= 0xff) {
					flash.carrentAlpha = 0x00;
					player.isLongStun = false;
					player.isShortStun = false;
				}
#pragma endregion

// 攻撃パターンのendregion
#pragma endregion 
// enemyの行動パターンのendregion
#pragma endregion
			}
			// player
			if (player.hp <= 0) {
				player.isAlive = false;
				scene = GAMEOVER;
			}
			// 人型の敵
			if (enemyPerson.hp <= 0) {
				enemyPerson.isAlive = false;
			}

			// クリア画面への移行
			if (!enemyPerson.isAlive) {
				clearPos.x = enemyPerson.translate.x;
				clearPos.y = enemyPerson.translate.y;

				clearEndTime += clearEndSpeed;
				EaseIn(clearEndTime, clearEndSpeed, clearRadius, 0.0f, kWindowWidth);

				if (clearEndTime >= 3.0f) {
					scene = CLEAR;
				}
			}
			break;
		case CLEAR:
#pragma region SPACEキーで初期化
			enemyPersonPattern = NONE_P;
			enemyPerson.isAlive = true;
			enemyPerson.hp = 15;
			enemyPerson.translate.x = kWindowWidth - 64 * 2;
			enemyPerson.translate.y = enemyPerson.size;
			shake.isShake = false;
			shake.randPos.x = 0;
			shake.randPos.y = 0;
			NoneTime = 60;

			teleport.isTeleport = false;
			teleport.frame = 0;

			jump.isJump = false;
			jump.frame = 0;
			jump.endTime = 0;

			backStep.isBackStep = false;
			backStep.frame = 0;
			backStep.endTime = 0;

			dash.isDush = false;
			dash.frame = 0;
			dash.endTime = 0;

			normalAttack.isAttack = false;
			normalAttack.frame = 0;

			behindAttack.isBehindAttack = false;
			behindAttack.frame = 0;

			longAttack.isAttack = false;
			longAttack.frame = 0;

			rangeAttack.isAttack = false;
			rangeAttack.frame = 0;
			rangeAttack.drawSwordPos.x = enemyPerson.translate.x;
			rangeAttack.drawSwordPos.y = enemyPerson.translate.y;

			for (int i = 0; i < 2; i++) {
				shockWave[i].isAttack = false;
				shockWave[i].frame = 0;
			}

			for (int i = 0; i < 10; i++) {
				thounder[i].isThounder = false;
				thounder[i].frame = 0;
				thounder[i].endTime = 0;
			
				isThounderPredictionRange[i] = {
					false
				};
			}
			isRangeAttackPrediction = false;

			flash.isFlash = false;
			flash.frame = 0;
			flash.endTime = 0;
			flash.carrentAlpha = 0x00;

			clearEndTime = 0;
			clearRadius = 1;

			player.translate.x = 64 * 2;
			player.translate.y = player.size;
			player.hp = 3;
			player.isAlive = true;
			if (!keys[DIK_SPACE] && preKeys[DIK_SPACE]) {
				scene = TITLE;
			}
#pragma endregion

			break;
		case GAMEOVER:
#pragma region SPACEキーで初期化
			enemyPersonPattern = NONE_P;
			enemyPerson.isAlive = true;
			enemyPerson.hp = 15;
			enemyPerson.translate.x = kWindowWidth - 64 * 2;
			enemyPerson.translate.y = enemyPerson.size;
			shake.isShake = false;
			shake.randPos.x = 0;
			shake.randPos.y = 0;

			NoneTime = 60;

			teleport.isTeleport = false;
			teleport.frame = 0;

			jump.isJump = false;
			jump.frame = 0;
			jump.endTime = 0;

			backStep.isBackStep = false;
			backStep.frame = 0;
			backStep.endTime = 0;

			dash.isDush = false;
			dash.frame = 0;
			dash.endTime = 0;

			normalAttack.isAttack = false;
			normalAttack.frame = 0;

			behindAttack.isBehindAttack = false;
			behindAttack.frame = 0;

			longAttack.isAttack = false;
			longAttack.frame = 0;

			rangeAttack.isAttack = false;
			rangeAttack.frame = 0;
			rangeAttack.drawSwordPos.x = enemyPerson.translate.x;
			rangeAttack.drawSwordPos.y = enemyPerson.translate.y;

			for (int i = 0; i < 2; i++) {
				shockWave[i].isAttack = false;
				shockWave[i].frame = 0;
			}

			for (int i = 0; i < 10; i++) {
				thounder[i].isThounder = false;
				thounder[i].frame = 0;
				thounder[i].endTime = 0;

				isThounderPredictionRange[i] = {
					false
				};
			}
			isRangeAttackPrediction = false;

			flash.isFlash = false;
			flash.frame = 0;
			flash.endTime = 0;
			flash.carrentAlpha = 0x00;

			clearEndTime = 0;
			clearRadius = 1;

			player.translate.x = 64 * 2;
			player.translate.y = player.size;
			player.hp = 3;
			player.isAlive = true;
			if (!keys[DIK_SPACE] && preKeys[DIK_SPACE]) {
				scene = TITLE;
			}
#pragma endregion

			break;
		}

		//アニメーションの処理
		//プレイヤー待機
		playerAnim.PlayerAnimCount++;
		if (playerAnim.PlayerAnimCount >= 8 * playerAnim.PlayerAnimSpeed) {
			playerAnim.PlayerAnimCount = 0;
		}

		if (playerAnim.Attack1AnimFlag) {
			playerAnim.Attack1AnimCount++;
			if (playerAnim.Attack1AnimCount >= 6 * playerAnim.Attack1AnimSpeed) {
				playerAnim.Attack1Range = 128 * 5;
			}
			else {
				playerAnim.Attack1Range = (playerAnim.Attack1AnimCount / playerAnim.Attack1AnimSpeed) * 128;
			}
			if (playerAnim.Attack1AnimCount >= 20) {
				playerAnim.Attack1AnimCount = 0;
				playerAnim.Attack1AnimFlag = 0;
			}
		}

		//エネミー待機
		enemyAnim.WaitAnimCount++;
		if (enemyAnim.WaitAnimCount >= 8 * enemyAnim.WaitAnimSpeed) {
			enemyAnim.WaitAnimCount = 0;
		}

		thunderAnimCount++;
		if (thunderAnimCount >= 4 * thunderAnimSpeed) {
			thunderAnimCount = 0;
		}

		if (behindAttack.isBehindAttack) {
			if (enemyAnim.AttackAnimFlag == 1) {
				enemyAnim.Attack1AnimCount++;
				if (enemyAnim.Attack1AnimCount >= 6 * enemyAnim.Attack1AnimSpeed) {
					enemyAnim.Attack1Range = 128 * 5;
				}
				else {
					enemyAnim.Attack1Range = (enemyAnim.Attack1AnimCount / enemyAnim.Attack1AnimSpeed) * 128;
				}
				if (enemyAnim.Attack1AnimCount >= 20) {
					enemyAnim.Attack1AnimCount = 0;
					enemyAnim.AttackAnimFlag = 0;
				}
			}
			else if (enemyAnim.AttackAnimFlag == 2) {
				enemyAnim.Attack2AnimCount++;
				if (enemyAnim.Attack2AnimCount >= 7 * enemyAnim.Attack2AnimSpeed) {
					enemyAnim.Attack2Range = 128 * 5;
				}
				else {
					enemyAnim.Attack2Range = (enemyAnim.Attack2AnimCount / enemyAnim.Attack2AnimSpeed) * 128;
				}
				if (enemyAnim.Attack2AnimCount >= 30) {
					enemyAnim.Attack2AnimCount = 0;
					enemyAnim.AttackAnimFlag = 0;
				}
			}
			else if (enemyAnim.AttackAnimFlag == 3) {
				enemyAnim.Attack3AnimCount++;
				if (enemyAnim.Attack3AnimCount >= 8 * enemyAnim.Attack3AnimSpeed) {
					enemyAnim.Attack3Range = 128 * 5;
				}
				else {
					enemyAnim.Attack3Range = (enemyAnim.Attack3AnimCount / enemyAnim.Attack3AnimSpeed) * 128;
				}
				if (enemyAnim.Attack3AnimCount >= 20) {
					enemyAnim.Attack3AnimCount = 0;
					enemyAnim.AttackAnimFlag = 0;
				}
			}
		}
		else {
			if (enemyAnim.AttackAnimFlag == 1) {
				enemyAnim.Attack1AnimCount++;
				if (enemyAnim.Attack1AnimCount >= 6 * enemyAnim.Attack1AnimSpeed) {
					enemyAnim.Attack1Range = 128 * 5;
				}
				else {
					enemyAnim.Attack1Range = (enemyAnim.Attack1AnimCount / enemyAnim.Attack1AnimSpeed) * 128;
				}
				if (enemyAnim.Attack1AnimCount >= 10) {
					enemyAnim.Attack1AnimCount = 0;
					enemyAnim.AttackAnimFlag = 0;
				}
			}
			else if (enemyAnim.AttackAnimFlag == 2) {
				enemyAnim.Attack2AnimCount++;
				if (enemyAnim.Attack2AnimCount >= 7 * enemyAnim.Attack2AnimSpeed) {
					enemyAnim.Attack2Range = 128 * 5;
				}
				else {
					enemyAnim.Attack2Range = (enemyAnim.Attack2AnimCount / enemyAnim.Attack2AnimSpeed) * 128;
				}
				if (enemyAnim.Attack2AnimCount >= 20) {
					enemyAnim.Attack2AnimCount = 0;
					enemyAnim.AttackAnimFlag = 0;
				}
			}
			else if (enemyAnim.AttackAnimFlag == 3) {
				enemyAnim.Attack3AnimCount++;
				if (enemyAnim.Attack3AnimCount >= 8 * enemyAnim.Attack3AnimSpeed) {
					enemyAnim.Attack3Range = 128 * 5;
				}
				else {
					enemyAnim.Attack3Range = (enemyAnim.Attack3AnimCount / enemyAnim.Attack3AnimSpeed) * 128;
				}
				if (enemyAnim.Attack3AnimCount >= 20) {
					enemyAnim.Attack3AnimCount = 0;
					enemyAnim.AttackAnimFlag = 0;
				}
			}
		}


		//パーティクルの移動
		for (int i = 0; i < kParticleMax; i++) {
			if (particle[i].isDo) {
				particleTimer[i]++;
				particle[i].pos.x += 5 * cosf(particle[i].direction);
				particle[i].pos.y += 2 * sinf(particle[i].direction) + (int)(gravity * ((float)particleTimer[i] / 70.0f));
				particle[i].ViewTime--;
			}
		}

		//パーティクルの初期化
		for (int i = 0; i < kParticleMax; i++) {
			if (particle[i].ViewTime <= 0) {
				particle[i] = particleDefault;
				particleTimer[i] = 0.0f;
			}
		}
		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		//パーティクル
		for (int i = 0; i < kParticleMax; i++) {
			if (particle[i].isDo) {
				Novice::DrawBox(particle[i].pos.x, particle[i].pos.y, 10, 10, 0, RED, kFillModeSolid);
			}
		}

		// player
		if (scene != TITLE && scene != GAMEOVER && scene != CLEAR) {
			// 本体
			if (player.isAlive) {
				if (player.isJump) {
					if (playerDir) {
						Novice::DrawQuad(
							player.translate.x + player.size, (player.translate.y + player.size) * -1 + kWindowHeight,
							player.translate.x - player.size, (player.translate.y + player.size) * -1 + kWindowHeight,
							player.translate.x + player.size, (player.translate.y - player.size) * -1 + kWindowHeight,
							player.translate.x - player.size, (player.translate.y - player.size) * -1 + kWindowHeight,
							0, 0,
							128, 128,
							PlayerJumpTexture,
							player.color
						);
					}
					else {
						Novice::DrawQuad(
							player.translate.x - player.size, (player.translate.y + player.size) * -1 + kWindowHeight,
							player.translate.x + player.size, (player.translate.y + player.size) * -1 + kWindowHeight,
							player.translate.x - player.size, (player.translate.y - player.size) * -1 + kWindowHeight,
							player.translate.x + player.size, (player.translate.y - player.size) * -1 + kWindowHeight,
							0, 0,
							128, 128,
							PlayerJumpTexture,
							player.color
						);
					}
				}
				else if (playerAnim.Attack1AnimFlag) {
					if (playerDir) {
						Novice::DrawQuad(
							player.translate.x + player.size, (player.translate.y + player.size) * -1 + kWindowHeight,
							player.translate.x - player.size, (player.translate.y + player.size) * -1 + kWindowHeight,
							player.translate.x + player.size, (player.translate.y - player.size) * -1 + kWindowHeight,
							player.translate.x - player.size, (player.translate.y - player.size) * -1 + kWindowHeight,
							playerAnim.Attack1Range, 0,
							128, 128,
							PlayerAttack1Texture,
							player.color
						);
					}
					else {
						Novice::DrawQuad(
							player.translate.x - player.size, (player.translate.y + player.size) * -1 + kWindowHeight,
							player.translate.x + player.size, (player.translate.y + player.size) * -1 + kWindowHeight,
							player.translate.x - player.size, (player.translate.y - player.size) * -1 + kWindowHeight,
							player.translate.x + player.size, (player.translate.y - player.size) * -1 + kWindowHeight,
							playerAnim.Attack1Range, 0,
							128, 128,
							PlayerAttack1Texture,
							player.color
						);
					}
				}
				else {
					/*Novice::DrawQuad(
						player.translate.x - player.size, (player.translate.y - player.size) * -1 + kWindowHeight,
						player.translate.x + player.size, (player.translate.y - player.size) * -1 + kWindowHeight,
						player.translate.x - player.size, (player.translate.y + player.size) * -1 + kWindowHeight,
						player.translate.x + player.size, (player.translate.y + player.size) * -1 + kWindowHeight,
						0, 0,
						128, 128,
						playerGh,
						0x0000FF4D
					);*/
					if (playerDir) {
						Novice::DrawQuad(
							player.translate.x + player.size, (player.translate.y + player.size) * -1 + kWindowHeight,
							player.translate.x - player.size, (player.translate.y + player.size) * -1 + kWindowHeight,
							player.translate.x + player.size, (player.translate.y - player.size) * -1 + kWindowHeight,
							player.translate.x - player.size, (player.translate.y - player.size) * -1 + kWindowHeight,
							(playerAnim.PlayerAnimCount / playerAnim.PlayerAnimSpeed) * 128, 0,
							128, 128,
							PlayerWaitTexture,
							player.color
						);
					}
					else {
						Novice::DrawQuad(
							player.translate.x - player.size, (player.translate.y + player.size) * -1 + kWindowHeight,
							player.translate.x + player.size, (player.translate.y + player.size) * -1 + kWindowHeight,
							player.translate.x - player.size, (player.translate.y - player.size) * -1 + kWindowHeight,
							player.translate.x + player.size, (player.translate.y - player.size) * -1 + kWindowHeight,
							(playerAnim.PlayerAnimCount / playerAnim.PlayerAnimSpeed) * 128, 0,
							128, 128,
							PlayerWaitTexture,
							player.color
						);
					}
				}
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
			Novice::ScreenPrintf(0, 10, "Press keys   1:FIRSTENEMY  2:SECONDENEMY  3:LASTENEMY   4:LASTENEMY2");
			break;
		case GUIDE:

			break;
		case LASTENEMY1:
			// enemy

			if (enemyPerson.isAlive) {
				if (!teleport.isTeleport) {
					// 近接攻撃
					if ((enemyAnim.AttackAnimFlag == 1)) {
						if (enemyDir == LEFT_E) {
							Novice::DrawQuad(
								enemyPerson.translate.x + enemyPerson.size, (enemyPerson.translate.y + enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x - enemyPerson.size, (enemyPerson.translate.y + enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x + enemyPerson.size, (enemyPerson.translate.y - enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x - enemyPerson.size, (enemyPerson.translate.y - enemyPerson.size) * -1 + kWindowHeight,
								enemyAnim.Attack1Range, 0,
								128, 128,
								EnemyAttack1Texture,
								enemyPerson.color - enemyCarrentAlpha
							);
						}
						else {
							Novice::DrawQuad(
								enemyPerson.translate.x - enemyPerson.size, (enemyPerson.translate.y + enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x + enemyPerson.size, (enemyPerson.translate.y + enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x - enemyPerson.size, (enemyPerson.translate.y - enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x + enemyPerson.size, (enemyPerson.translate.y - enemyPerson.size) * -1 + kWindowHeight,
								enemyAnim.Attack1Range, 0,
								128, 128,
								EnemyAttack1Texture,
								enemyPerson.color - enemyCarrentAlpha
							);
						}
					}
					else if ((enemyAnim.AttackAnimFlag == 2)) {
						if (enemyDir == LEFT_E) {
							Novice::DrawQuad(
								enemyPerson.translate.x + enemyPerson.size, (enemyPerson.translate.y + enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x - enemyPerson.size, (enemyPerson.translate.y + enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x + enemyPerson.size, (enemyPerson.translate.y - enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x - enemyPerson.size, (enemyPerson.translate.y - enemyPerson.size) * -1 + kWindowHeight,
								enemyAnim.Attack2Range, 0,
								128, 128,
								EnemyAttack2Texture,
								enemyPerson.color - enemyCarrentAlpha
							);
						}
						else {
							Novice::DrawQuad(
								enemyPerson.translate.x - enemyPerson.size, (enemyPerson.translate.y + enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x + enemyPerson.size, (enemyPerson.translate.y + enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x - enemyPerson.size, (enemyPerson.translate.y - enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x + enemyPerson.size, (enemyPerson.translate.y - enemyPerson.size) * -1 + kWindowHeight,
								enemyAnim.Attack2Range, 0,
								128, 128,
								EnemyAttack2Texture,
								enemyPerson.color - enemyCarrentAlpha
							);
						}
					}
					else if ((enemyAnim.AttackAnimFlag == 3)) {
						if (enemyDir == LEFT_E) {
							Novice::DrawQuad(
								enemyPerson.translate.x + enemyPerson.size, (enemyPerson.translate.y + enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x - enemyPerson.size, (enemyPerson.translate.y + enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x + enemyPerson.size, (enemyPerson.translate.y - enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x - enemyPerson.size, (enemyPerson.translate.y - enemyPerson.size) * -1 + kWindowHeight,
								enemyAnim.Attack3Range, 0,
								128, 128,
								EnemyAttack3Texture,
								enemyPerson.color - enemyCarrentAlpha
							);
						}
						else {
							Novice::DrawQuad(
								enemyPerson.translate.x - enemyPerson.size, (enemyPerson.translate.y + enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x + enemyPerson.size, (enemyPerson.translate.y + enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x - enemyPerson.size, (enemyPerson.translate.y - enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x + enemyPerson.size, (enemyPerson.translate.y - enemyPerson.size) * -1 + kWindowHeight,
								enemyAnim.Attack3Range, 0,
								128, 128,
								EnemyAttack3Texture,
								enemyPerson.color - enemyCarrentAlpha
							);
						}

					}
					else if (longAttack.frame >= 15) {
						if (enemyDir == LEFT_E) {
							Novice::DrawQuad(
								enemyPerson.translate.x + enemyPerson.size, (enemyPerson.translate.y + enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x - enemyPerson.size, (enemyPerson.translate.y + enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x + enemyPerson.size, (enemyPerson.translate.y - enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x - enemyPerson.size, (enemyPerson.translate.y - enemyPerson.size) * -1 + kWindowHeight,
								768, 0,
								128, 128,
								EnemyAttack2Texture,
								enemyPerson.color - enemyCarrentAlpha
							);
						}
						else {
							Novice::DrawQuad(
								enemyPerson.translate.x - enemyPerson.size, (enemyPerson.translate.y + enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x + enemyPerson.size, (enemyPerson.translate.y + enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x - enemyPerson.size, (enemyPerson.translate.y - enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x + enemyPerson.size, (enemyPerson.translate.y - enemyPerson.size) * -1 + kWindowHeight,
								768, 0,
								128, 128,
								EnemyAttack2Texture,
								enemyPerson.color - enemyCarrentAlpha
							);
						}
					}
					else if (longAttack.frame < 15 && longAttack.frame >= 1) {
						if (enemyDir == LEFT_E) {
							Novice::DrawQuad(
								enemyPerson.translate.x + enemyPerson.size, (enemyPerson.translate.y + enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x - enemyPerson.size, (enemyPerson.translate.y + enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x + enemyPerson.size, (enemyPerson.translate.y - enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x - enemyPerson.size, (enemyPerson.translate.y - enemyPerson.size) * -1 + kWindowHeight,
								0, 0,
								128, 128,
								EnemyAttack2Texture,
								enemyPerson.color - enemyCarrentAlpha
							);
						}
						else {
							Novice::DrawQuad(
								enemyPerson.translate.x - enemyPerson.size, (enemyPerson.translate.y + enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x + enemyPerson.size, (enemyPerson.translate.y + enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x - enemyPerson.size, (enemyPerson.translate.y - enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x + enemyPerson.size, (enemyPerson.translate.y - enemyPerson.size) * -1 + kWindowHeight,
								0, 0,
								128, 128,
								EnemyAttack2Texture,
								enemyPerson.color - enemyCarrentAlpha
							);
						}
					}
					else if (enemyPerson.isJump) {
						if (enemyDir == LEFT_E) {
							Novice::DrawQuad(
								enemyPerson.translate.x + enemyPerson.size, (enemyPerson.translate.y + enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x - enemyPerson.size, (enemyPerson.translate.y + enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x + enemyPerson.size, (enemyPerson.translate.y - enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x - enemyPerson.size, (enemyPerson.translate.y - enemyPerson.size) * -1 + kWindowHeight,
								0, 0,
								128, 128,
								EnemyJumpTexture,
								enemyPerson.color - enemyCarrentAlpha
							);
						}
						else {
							Novice::DrawQuad(
								enemyPerson.translate.x - enemyPerson.size, (enemyPerson.translate.y + enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x + enemyPerson.size, (enemyPerson.translate.y + enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x - enemyPerson.size, (enemyPerson.translate.y - enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x + enemyPerson.size, (enemyPerson.translate.y - enemyPerson.size) * -1 + kWindowHeight,
								0, 0,
								128, 128,
								EnemyJumpTexture,
								enemyPerson.color - enemyCarrentAlpha
							);
						}
					}
					else if (backStep.isBackStep) {
						if (enemyDir == LEFT_E) {
							Novice::DrawQuad(
								enemyPerson.translate.x + enemyPerson.size, (enemyPerson.translate.y + enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x - enemyPerson.size, (enemyPerson.translate.y + enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x + enemyPerson.size, (enemyPerson.translate.y - enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x - enemyPerson.size, (enemyPerson.translate.y - enemyPerson.size) * -1 + kWindowHeight,
								0, 0,
								128, 128,
								EnemyBackstepTexture,
								enemyPerson.color - enemyCarrentAlpha
							);
						}
						else {
							Novice::DrawQuad(
								enemyPerson.translate.x - enemyPerson.size, (enemyPerson.translate.y + enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x + enemyPerson.size, (enemyPerson.translate.y + enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x - enemyPerson.size, (enemyPerson.translate.y - enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x + enemyPerson.size, (enemyPerson.translate.y - enemyPerson.size) * -1 + kWindowHeight,
								0, 0,
								128, 128,
								EnemyBackstepTexture,
								enemyPerson.color - enemyCarrentAlpha
							);
						}
					}
					else {
						if (enemyDir == LEFT_E) {
							Novice::DrawQuad(
								enemyPerson.translate.x + enemyPerson.size + shake.randPos.x, (enemyPerson.translate.y + enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x - enemyPerson.size + shake.randPos.x, (enemyPerson.translate.y + enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x + enemyPerson.size + shake.randPos.x, (enemyPerson.translate.y - enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x - enemyPerson.size + shake.randPos.x, (enemyPerson.translate.y - enemyPerson.size) * -1 + kWindowHeight,
								(enemyAnim.WaitAnimCount / enemyAnim.WaitAnimSpeed) * 128, 0,
								128, 128,
								EnemyWaitTexture,
								enemyPerson.color - enemyCarrentAlpha
							);
						}
						else {
							Novice::DrawQuad(
								enemyPerson.translate.x - enemyPerson.size + shake.randPos.x, (enemyPerson.translate.y + enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x + enemyPerson.size + shake.randPos.x, (enemyPerson.translate.y + enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x - enemyPerson.size + shake.randPos.x, (enemyPerson.translate.y - enemyPerson.size) * -1 + kWindowHeight,
								enemyPerson.translate.x + enemyPerson.size + shake.randPos.x, (enemyPerson.translate.y - enemyPerson.size) * -1 + kWindowHeight,
								(enemyAnim.WaitAnimCount / enemyAnim.WaitAnimSpeed) * 128, 0,
								128, 128,
								EnemyWaitTexture,
								enemyPerson.color - enemyCarrentAlpha
							);
						}
					}
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
				if (enemyDir == LEFT_E) {
					Novice::DrawQuad(
						longAttack.translate.x + longAttack.size, (longAttack.translate.y + longAttack.size) * -1 + kWindowHeight,
						longAttack.translate.x - longAttack.size, (longAttack.translate.y + longAttack.size) * -1 + kWindowHeight,
						longAttack.translate.x + longAttack.size, (longAttack.translate.y - longAttack.size) * -1 + kWindowHeight,
						longAttack.translate.x - longAttack.size, (longAttack.translate.y - longAttack.size) * -1 + kWindowHeight,
						0, 0,
						64, 64,
						EnemySlashTexture,
						WHITE
					);
				}
				else {
					Novice::DrawQuad(
						longAttack.translate.x - longAttack.size, (longAttack.translate.y + longAttack.size) * -1 + kWindowHeight,
						longAttack.translate.x + longAttack.size, (longAttack.translate.y + longAttack.size) * -1 + kWindowHeight,
						longAttack.translate.x - longAttack.size, (longAttack.translate.y - longAttack.size) * -1 + kWindowHeight,
						longAttack.translate.x + longAttack.size, (longAttack.translate.y - longAttack.size) * -1 + kWindowHeight,
						0, 0,
						64, 64,
						EnemySlashTexture,
						WHITE
					);
				}

			}
			// 落雷
			for (int i = 0; i < 10; i++) {
				if (thounder[i].isThounder) {
					Novice::DrawQuad(
						thounder[i].translate.x - thounder[i].size - 64, (thounder[i].translate.y - thounder[i].size * 6) * -1 + kWindowHeight,
						thounder[i].translate.x + thounder[i].size + 64, (thounder[i].translate.y - thounder[i].size * 6) * -1 + kWindowHeight,
						thounder[i].translate.x - thounder[i].size - 64, (thounder[i].translate.y + thounder[i].size * 6) * -1 + kWindowHeight,
						thounder[i].translate.x + thounder[i].size + 64, (thounder[i].translate.y + thounder[i].size * 6) * -1 + kWindowHeight,
						(thunderAnimCount / thunderAnimSpeed) * 128, 0,
						128, 720,
						thunderTexture,
						WHITE
					);
				}
			}
			// デバッグ文字
			Novice::ScreenPrintf(0, 10, "Press keys    1:JUMP 2:BACKSTEP 3:DUSH 4:TELEPORT 5:SHOCKWAVE 6:THOUNDER");
			break;
		case LASTENEMY2:
			// enemy
			if (shake.isShake) {
				if (enemyPerson.isAlive) {
					if (!teleport.isTeleport) {
						Novice::DrawQuad(
							enemyPerson.translate.x - enemyPerson.size + shake.randPos.x, (enemyPerson.translate.y - enemyPerson.size + shake.randPos.y) * -1 + kWindowHeight,
							enemyPerson.translate.x + enemyPerson.size + shake.randPos.x, (enemyPerson.translate.y - enemyPerson.size + shake.randPos.y) * -1 + kWindowHeight,
							enemyPerson.translate.x - enemyPerson.size + shake.randPos.x, (enemyPerson.translate.y + enemyPerson.size + shake.randPos.y) * -1 + kWindowHeight,
							enemyPerson.translate.x + enemyPerson.size + shake.randPos.x, (enemyPerson.translate.y + enemyPerson.size + shake.randPos.y) * -1 + kWindowHeight,
							0, 0,
							128, 128,
							playerGh,
							enemyPerson.color - enemyCarrentAlpha
						);
					}
				}
			}
			else {
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
							enemyPerson.color - enemyCarrentAlpha
						);
					}
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
			// 閃光弾
			if (flash.isFlash) {
				Novice::DrawQuad(
					flash.translate.x - flash.size, (flash.translate.y - flash.size) * -1 + kWindowHeight,
					flash.translate.x + flash.size, (flash.translate.y - flash.size) * -1 + kWindowHeight,
					flash.translate.x - flash.size, (flash.translate.y + flash.size) * -1 + kWindowHeight,
					flash.translate.x + flash.size, (flash.translate.y + flash.size) * -1 + kWindowHeight,
					0, 0,
					32, 32,
					attackGh,
					RED
				);
			}
			// フラッシュ状態
			if (player.isShortStun || player.isLongStun) {
				Novice::DrawBox(0, 0, kWindowWidth, kWindowHeight, 0.0f, WHITE - flash.carrentAlpha, kFillModeSolid);
			}

			Novice::DrawEllipse(clearPos.x, clearPos.y * -1 + kWindowHeight, clearRadius, clearRadius, 0.0f, WHITE, kFillModeSolid);

			// デバッグ文字
			Novice::ScreenPrintf(0, 10, "Press keys    1:JUMP 2:BACKSTEP 3:DUSH 4:TELEPORT 5:SHOCKWAVE 6:THOUNDER 7:FLASH");
			Novice::ScreenPrintf(0, 30, "%d", player.hp);
			Novice::ScreenPrintf(0, 50, "%d", enemyPerson.hp);
			Novice::ScreenPrintf(0, 70, "%f", distance);
			break;
		case CLEAR:

			break;
		case GAMEOVER:

			break;
		}
		// 範囲攻撃の予測位置
		if (isRangeAttackPrediction) {
			Novice::DrawQuad(
				rangeAttack.translate.x - static_cast<float>(kWindowWidth) / 2, (rangeAttack.translate.y - rangeAttack.size) * -1 + kWindowHeight,
				rangeAttack.translate.x + static_cast<float>(kWindowWidth) / 2, (rangeAttack.translate.y - rangeAttack.size) * -1 + kWindowHeight,
				rangeAttack.translate.x - static_cast<float>(kWindowWidth) / 2, (rangeAttack.translate.y + rangeAttack.size) * -1 + kWindowHeight,
				rangeAttack.translate.x + static_cast<float>(kWindowWidth) / 2, (rangeAttack.translate.y + rangeAttack.size) * -1 + kWindowHeight,
				0, 0,
				1280, 128,
				attackGh,
				WHITE - 0xf1
			);
		}
		// 落雷の落ちる地点
		for (int i = 0; i < 10; i++) {
			if (isThounderPredictionRange[i]) {
				Novice::DrawQuad(
					thounder[i].translate.x - thounder[i].size, (thounder[i].translate.y - thounder[i].size * 6) * -1 + kWindowHeight,
					thounder[i].translate.x + thounder[i].size, (thounder[i].translate.y - thounder[i].size * 6) * -1 + kWindowHeight,
					thounder[i].translate.x - thounder[i].size, (thounder[i].translate.y + thounder[i].size * 6) * -1 + kWindowHeight,
					thounder[i].translate.x + thounder[i].size, (thounder[i].translate.y + thounder[i].size * 6) * -1 + kWindowHeight,
					0, 0,
					64, 720,
					attackGh,
					WHITE - 0xf1
				);
			}
		}
		// 第二形態
		if (isSecondForm) {
			Novice::DrawBox(0, 0, kWindowWidth, kWindowHeight, 0.0f, BLACK - formCarrentAlpha, kFillModeSolid);
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