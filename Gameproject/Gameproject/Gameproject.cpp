#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <bangtal.h>
#include <stdlib.h>
#include <time.h>

SceneID scene1;
ObjectID start;

struct POS {
	int x, y;
};
const POS positions[16] = {
	{329, 511}, {479, 511}, {629, 511}, {779, 511},
	{329, 361}, {479, 361}, {629, 361}, {779, 361},
	{329, 211}, {479, 211}, {629, 211}, {779, 211},
	{329, 61}, {479, 61}, {629, 61}, {779, 61},
};

ObjectID initObjects[16];
ObjectID gameObjects[16];
int blank = 15;

TimerID timerMixing;
const int mixCount = 100;
const Second animationTime = 0.01f;
int mixing = 0;

bool game = false;


void game_init()
{
	scene1 = createScene("알라딘", "background.jpg");

	char image[20];
	for (int i = 0; i < 16; i++) {
		sprintf(image, "%d.png", i + 1);
		initObjects[i] = createObject(image);
		gameObjects[i] = initObjects[i];

		locateObject(gameObjects[i], scene1, positions[i].x, positions[i].y);
		showObject(gameObjects[i]);
	}

	start = createObject("start.png");
	locateObject(start, scene1, 540, 100);
	scaleObject(start, 2);
	showObject(start);

	blank = 15;
	game = false;

	srand((unsigned int)time(NULL));
	timerMixing = createTimer();
}

int game_index(ObjectID oid) {
	for (int i = 0; i < 16; i++) {
		if (gameObjects[i] == oid) return i;
	}

	return -1;
}

bool game_move(int index)
{
	if (index < 0) return false;
	if (index > 15) return false;

	if (index == (blank - 4) || index == (blank + 4) ||
		(index == (blank - 1) && (blank % 4) != 0) ||
		(index == (blank + 1) && (blank % 4) != 3)) {

		locateObject(gameObjects[index], scene1, positions[blank].x, positions[blank].y);
		locateObject(gameObjects[blank], scene1, positions[index].x, positions[index].y);

		ObjectID o = gameObjects[blank];
		gameObjects[blank] = gameObjects[index];
		gameObjects[index] = o;

		blank = index;

		return true;
	}

	return false;
}

void game_random_move()
{
	bool possible = false;
	int next = -1;
	while (!possible) {
		switch (rand() % 4) {
		case 0:	next = blank - 4;
			break;
		case 1:	next = blank + 4;
			break;
		case 2: next = (blank % 4 != 0) ? (blank - 1) : -1;
			break;
		case 3:	next = (blank % 4 != 3) ? (blank + 1) : -1;
			break;
		}

		possible = game_move(next);
	}
}

void game_prepare()
{
	hideObject(gameObjects[blank]);

	mixing = mixCount;

	setTimer(timerMixing, animationTime);
	startTimer(timerMixing);
}

bool game_check()
{
	for (int i = 0; i < 16; i++) {
		if (gameObjects[i] != initObjects[i]) return false;
	}

	return true;
}

void game_end()
{
	game = false;

	setObjectImage(start, "restart.png");
	showObject(gameObjects[blank]);
	showObject(start);
}

void mouseCallback(ObjectID object, int x, int y, MouseAction action)
{
	if (game) {
		game_move(game_index(object));

		if (game_check()) {
			game_end();
		}
	}
	else if (mixing == 0 && object == start) {
		game_prepare();
	}
}

void timerCallback(TimerID timer)
{
	game_random_move();

	if (mixing > 0) {
		--mixing;

		setTimer(timerMixing, animationTime);
		startTimer(timerMixing);
	}
	else {
		hideObject(start);
		game = true;
	}
}

int main()
{
	setMouseCallback(mouseCallback);
	setTimerCallback(timerCallback);

	game_init();

	showMessage("퍼즐을 맞추시오!!");

	startGame(scene1);
}