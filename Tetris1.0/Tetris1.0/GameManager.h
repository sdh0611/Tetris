#ifndef GAMEMANGER_H
#define GAMEMANGER_H

#include "stdafx.h"
#include "Block.h"

#define GAP 3

//게임의 흐름을 담당하는 클래스
class GameManager {

public:
	GameManager();
	~GameManager();
	void Exec();
	bool IsGameExit();

private:
	void Init();
	void Update();

private:
	void HideCursor();
	void InitBoard();
	void DrawBoard();
	void UpdateBlock(int mode);
	void PlayerControl();
	bool IsBlockGenerate() { return blockGenerate; }
	bool IsMove();
	bool IsLineClear(int row);
	void ClearLine();
	bool IsDead();
	void PrintNextBlock();
	void EraseNextBlock();
	void PrintNextBlockUi();
	void PrintUi();
	void GameSpeedManager();

private:
	int highScore;
	int totalScore;
	int scoreWeight;
	int gameSpeed;
	bool run;
	bool dead;
	bool blockGenerate;
	bool spaceOn;
	Block block;
	Point drawFlag;
	Point* blockPoint;
	Point* nextBlock;
	char board[HEIGHT+1][WIDTH+2];



};

#endif