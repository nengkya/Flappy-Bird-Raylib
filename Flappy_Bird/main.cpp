#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#include <iostream>
#include <ctime>
#include <string>
#include <sstream>
#include <fstream>
#include "raylib.h"
using namespace std;

struct Bird {
	float x = 200; float y = GetScreenHeight() / 2 - 30;
	
	void Reset(float& speed) {
		x = 200; y = GetScreenHeight() / 2 - 30;
		speed = 1;
	}
};

struct Pipe {
	float x, y, defaultX, defaultY;

	Pipe(float defX, float defY) {
		x = defX; defaultX = defX; y = defY; defaultY = defY;
	}

	int Random() {
		SetRandomSeed(time(0)); return GetRandomValue(-35, 35);
	}

	void Reset(bool xTrue, bool yTrue) {
		if (xTrue)
			x = defaultX;
		if (yTrue)
			y = defaultY;
	}
};

int main() {
	InitWindow(800, 600, "Flappy Bird"); SetWindowState(FLAG_VSYNC_HINT); const char * windowIcon = "./assets/images/Flappy_Bird_icon.png";
	SetWindowIcon(LoadImage(windowIcon)); InitAudioDevice(); Bird bird; Rectangle birdRect = Rectangle{bird.x, bird.y, 60, 61};

	Pipe pipe1TOP = Pipe(100, -30);	   Rectangle pipe1TOPRect = Rectangle{ pipe1TOP.x, pipe1TOP.y, 88, 266 };
	Pipe pipe1BOTTOM = Pipe(100, 380); Rectangle pipe1BOTTOMRect = Rectangle{ pipe1BOTTOM.x, pipe1BOTTOM.y, 88, 266 };
	Pipe pipe2TOP = Pipe(350, -10);	   Rectangle pipe2TOPRect = Rectangle{ pipe2TOP.x, pipe2TOP.y, 88, 266 };
	Pipe pipe2BOTTOM = Pipe(350, 400); Rectangle pipe2BOTTOMRect = Rectangle{ pipe2BOTTOM.x, pipe2BOTTOM.y, 88, 266 };
	Pipe pipe3TOP = Pipe(600, -50);    Rectangle pipe3TOPRect = Rectangle{ pipe3TOP.x, pipe3TOP.y, 88, 266 };
	Pipe pipe3BOTTOM = Pipe(600, 360); Rectangle pipe3BOTTOMRect = Rectangle{ pipe3BOTTOM.x, pipe3BOTTOM.y, 88, 266 };

	float speed = 1, splashScreenShakeY = 0;
	int offset = 0, playSound = 0;
	bool alive = false, splashScreen = true, splashScreenUpCycle = true;
	Texture2D splashScreenTexture = LoadTexture("./assets/images/flappybirdsplash.png"); Texture2D birdTexture = LoadTexture("./assets/images/bird.png");
	Texture2D pipeTexture = LoadTexture("./assets/images/pipe.png"); Texture2D pipeTexture180 = LoadTexture("./assets/images/pipe180.png");
	Sound hit = LoadSound("./assets/sounds/sfx_hit.wav"); Sound die = LoadSound("./assets/sounds/sfx_die.wav");
	Sound restart = LoadSound("./assets/sounds/sfx_point.wav"); Sound start = LoadSound("./assets/sounds/sfx_swooshing.wav");
	Sound theme = LoadSound("./assets/sounds/theme.wav"); PlaySound(theme);

	while (!WindowShouldClose())
	{
		// consistencely play the theme
		if (!IsSoundPlaying(theme)) {
			PlaySound(theme);
		}

		if (splashScreen) {
			if ((splashScreenShakeY == 0 || splashScreenShakeY < 5) && splashScreenUpCycle)
				splashScreenShakeY += 5 * GetFrameTime();
			else {
				splashScreenUpCycle = false;
				if (!splashScreenUpCycle) {
					splashScreenShakeY -= 5 * GetFrameTime();
					if (splashScreenShakeY < -5)
						splashScreenUpCycle = true;
				}
			}
			if (IsKeyDown(KEY_SPACE)) {
				splashScreen = false; alive = true;
			}
		}

		if (!splashScreen) {
			playSound++;
			if (playSound == 1) {
				PlaySound(die); playSound++;
			}
			if (IsKeyDown(KEY_SPACE)) {
				bird.Reset(speed); pipe1TOP.Reset(true, true); pipe1BOTTOM.Reset(true, true); pipe2TOP.Reset(true, true); pipe2BOTTOM.Reset(true, true);
				pipe2TOP.Reset(true, true); pipe2BOTTOM.Reset(true, true); pipe3TOP.Reset(true, true); pipe3BOTTOM.Reset(true, true);
				offset = 0; alive = true; playSound = 0; PlaySound(start);
			}
		}

		if ((IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_UP) || IsKeyDown(KEY_W) || IsMouseButtonDown(MOUSE_BUTTON_LEFT)) && alive)
			bird.y -= 240 * GetFrameTime();
		else if (alive)
			bird.y += 200 * GetFrameTime();
		else
			bird.y += 400 * GetFrameTime();
		if ((bird.y < 0 || bird.y > GetScreenHeight() + 30) && alive)
			bird.Reset(speed);
		if (pipe1TOP.x - 88 < 0) {
			pipe1TOP.x = pipe3TOP.x + 250; pipe1BOTTOM.x = pipe3TOP.x + 250; offset = pipe1TOP.Random(); pipe1TOP.y += offset; pipe1BOTTOM.y += offset;
		}
		if (pipe2TOP.x - 88 < 0) {
			pipe2TOP.x = pipe1TOP.x + 250; pipe2BOTTOM.x = pipe1TOP.x + 250; offset = pipe2TOP.Random(); pipe2TOP.y += offset; pipe2BOTTOM.y += offset;
		}

		if (pipe3TOP.x - 88 < 0) {
			pipe3TOP.x = pipe2TOP.x + 250; pipe3BOTTOM.x = pipe2TOP.x + 250; offset = pipe3TOP.Random(); pipe3TOP.y += offset; pipe3BOTTOM.y += offset;
		}

		if (pipe1TOP.y < -80 || pipe1TOP.y > 60) {
			pipe1TOP.Reset(false, true); pipe1BOTTOM.Reset(false, true);
		}

		if (pipe2TOP.y < -80 || pipe2TOP.y > 60) {
			pipe2TOP.Reset(false, true); pipe2BOTTOM.Reset(false, true);
		}

		if (pipe3TOP.y < -80 || pipe3TOP.y > 60) {
			pipe3TOP.Reset(false, true); pipe3BOTTOM.Reset(false, true);
		}

		birdRect = Rectangle{ bird.x, bird.y, 60, 61 };
		pipe1TOPRect = Rectangle{ pipe1TOP.x, pipe1TOP.y, 88, 266 }; pipe1BOTTOMRect = Rectangle{ pipe1BOTTOM.x, pipe1BOTTOM.y, 88, 266 };
		pipe2TOPRect = Rectangle{ pipe2TOP.x, pipe2TOP.y, 88, 266 }; pipe2BOTTOMRect = Rectangle{ pipe2BOTTOM.x, pipe2BOTTOM.y, 88, 266 };
		pipe3TOPRect = Rectangle{ pipe3TOP.x, pipe3TOP.y, 88, 266 }; pipe3BOTTOMRect = Rectangle{ pipe3BOTTOM.x, pipe3BOTTOM.y, 88, 266 };

		if (CheckCollisionRecs(birdRect, pipe1TOPRect) && alive) {
			PlaySound(hit); alive = false;
		} else if (CheckCollisionRecs(birdRect, pipe1BOTTOMRect) && alive) {
			PlaySound(hit); alive = false;
		} else if (CheckCollisionRecs(birdRect, pipe2TOPRect) && alive)
		{
			PlaySound(hit); alive = false;
		} else if (CheckCollisionRecs(birdRect, pipe2BOTTOMRect) && alive) {
			PlaySound(hit); alive = false;
		} else if (CheckCollisionRecs(birdRect, pipe3TOPRect) && alive) {
			PlaySound(hit); alive = false;
		} else if (CheckCollisionRecs(birdRect, pipe3BOTTOMRect) && alive) {
			PlaySound(hit); alive = false;
		}

		BeginDrawing(); ClearBackground(GREEN);
		if (splashScreen)
			DrawTexture(splashScreenTexture, 25, 150 + splashScreenShakeY, WHITE);
		else {
			DrawTexture(pipeTexture180, pipe1TOP.x, pipe1TOP.y, WHITE); DrawTexture(pipeTexture, pipe1BOTTOM.x, pipe1BOTTOM.y, WHITE);
			DrawTexture(pipeTexture180, pipe2TOP.x, pipe2TOP.y, WHITE); DrawTexture(pipeTexture, pipe2BOTTOM.x, pipe2BOTTOM.y, WHITE);
			DrawTexture(pipeTexture180, pipe3TOP.x, pipe3TOP.y, WHITE); DrawTexture(pipeTexture, pipe3BOTTOM.x, pipe3BOTTOM.y, WHITE);
			DrawTexture(birdTexture, bird.x, bird.y, WHITE);
			if (!alive)
				DrawText("Press Space To Start!", GetScreenWidth() / 2 - MeasureText("Press Space To Start!", 35) / 2, 50, 35, RED);
			DrawText("Made By: meta_legend!", GetScreenWidth() / 2 - MeasureText("Credit: Pranab Shukla!", 50) / 2, 525, 50, DARKBLUE);
		}
		EndDrawing();

		if (alive) {
			pipe1TOP.x -= speed; pipe1BOTTOM.x -= speed; pipe2TOP.x -= speed; pipe2BOTTOM.x -= speed; pipe3TOP.x -= speed; pipe3BOTTOM.x -= speed;
		}

		if (speed < 4)
			speed += 0.1 * GetFrameTime();
	}

}
