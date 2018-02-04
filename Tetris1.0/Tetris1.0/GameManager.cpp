#include "stdafx.h"
#include "GameManager.h"

GameManager::GameManager() : block() {	}

GameManager::~GameManager() {	}

void GameManager::Exec() {

	while (run) {
		//게임 초기화
		Init();
		PrintUi();
		PrintNextBlock();
		
		while (!dead) {
			//게임 진행
			Update();
		}
		
		//게임을 다시 시작할건지 물어본다.
		DrawBoard();
		IsGameExit();

	}

}

//커서 숨김
void GameManager::HideCursor(){

	CONSOLE_CURSOR_INFO info;

	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
	info.bVisible = false;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);

}

//게임 시작시 속성 초기화
void GameManager::Init() {

	system("cls");
	
	highScore = 0;
	totalScore = 0;
	scoreWeight = 10;
	gameSpeed = 10;
	drawFlag.x = WIDTH / 2;
	drawFlag.y = 0;
	run = true;
	dead = false;
	blockGenerate = true;
	spaceOn = false;

	HideCursor();
	InitBoard();	
	
	blockPoint = block.PopBlock();
	nextBlock = block.GetNextBlockInfo();
	blockGenerate = false;
	UpdateBlock(BLOCK);

}

void GameManager::Update() {
	//Block좌표 받아옴
	if (IsBlockGenerate()) {
		//블록 생성좌표 초기화
		drawFlag.x = WIDTH / 2;
		drawFlag.y = 0;
		//다음 블록의 정보를 가져옴.
		blockPoint = block.PopBlock();
		EraseNextBlock();
		//다음에 생성될 예정인 블록의 정보를 가져옴.
		nextBlock = block.GetNextBlockInfo();
		PrintUi();
		PrintNextBlock();
		//블록이 생성되었음을 알림.
		blockGenerate = false;
		UpdateBlock(BLOCK);
	}

	for (int i = 0; i< gameSpeed; i++) {
		//보드 그림
		DrawBoard();
		//키입력 처리
		UpdateBlock(EMPTY);
		PlayerControl();
		UpdateBlock(BLOCK);

		if (spaceOn) {
			spaceOn = false;
			break;
		}

	}

	//블록을 게임속도에 맞춰 내림
	UpdateBlock(EMPTY);
	++drawFlag.y;

	//바닥에 닿은 경우
	if (!IsMove()) {
		--drawFlag.y;
		//바닥의 BLOCK들을 INACTIVE BLOCK으로 변경
		UpdateBlock(INACTIVE);
		//라인 클리어 여부 확인
		ClearLine();
		//새 블럭을 생성해야함을 알림.
		blockGenerate = true;
	}
	else {
		UpdateBlock(BLOCK);
	}

	if (IsDead()) {
		dead = true;
		EraseNextBlock();
	}

}

//시작시 보드 초기화
void GameManager::InitBoard() {

	for (int i = 0; i < HEIGHT + 1; i++) {
		for (int j = 0; j < WIDTH + 2; j++)
			board[i][j] = EMPTY;
	}

	for (int i = 0; i < HEIGHT + 1; i++) 
		board[i][0] = board[i][WIDTH + 1] = WALL;
	
	for (int i = 1; i < WIDTH + 2; i++) 
		board[HEIGHT][i] = WALL;
	

}

//보드 Render
void GameManager::DrawBoard() {
		
	for(int i=0; i<HEIGHT+1; i++)
		for (int j = 0; j < WIDTH + 2; j++) {
			if (board[i][j] == WALL) {
				GotoXy(j+GAP, i+GAP);
				puts("▩");
			}
			else if (board[i][j] == BLOCK) {
				GotoXy(j+GAP, i+GAP);
				puts("■");
			}
			else if (board[i][j] == INACTIVE) {
				GotoXy(j + GAP, i + GAP);
				puts("□");
			}
			else {
				GotoXy(j+GAP, i+GAP);
				puts(" .");
			}
		}		

}

//board에 각각의 속성들을 업데이트함.
void GameManager::UpdateBlock(int mode) {
	
	if(mode == BLOCK)
		for (int i = 0; i < 4; i++) 
			board[drawFlag.y + blockPoint[i].y][drawFlag.x + blockPoint[i].x] = BLOCK;
	else if(mode == EMPTY)
		for (int i = 0; i < 4; i++)
			board[drawFlag.y + blockPoint[i].y][drawFlag.x + blockPoint[i].x] = EMPTY;
	else if(mode == INACTIVE)
		for (int i = 0; i < 4; i++)
			board[drawFlag.y + blockPoint[i].y][drawFlag.x + blockPoint[i].x] = INACTIVE;

}


//Key Processing 수행
void GameManager::PlayerControl() {

	
	if (KEY_DOWN(VK_DOWN)) {
		++drawFlag.y;
		if (!IsMove())
			--drawFlag.y;
	}
	else if (KEY_DOWN(VK_UP)) {
		block.TurnBlock();
		blockPoint = block.GetBlockPoint();
		if (!IsMove()) {
			block.TurnUndo();
			blockPoint = block.GetBlockPoint();
		}
	}
	else if (KEY_DOWN(VK_RIGHT)) {
		++drawFlag.x;
		if (!IsMove())
			--drawFlag.x;
	}
	else if (KEY_DOWN(VK_LEFT)) {
		--drawFlag.x;
		if (!IsMove())
			++drawFlag.x;
	}
	
	if (KEY_DOWN(VK_SPACE)) {
		while (true){		
			++drawFlag.y;
			if (!IsMove()) {
				--drawFlag.y;
				break;
			}
		}
		spaceOn = true;		
	}

	_sleep(50);

}

//이동한 블록의 위치가 벽이나 INACTIVE BLCOK 좌표와 겹치는지 검사
bool GameManager::IsMove() {

	for (int i = 0; i < 4; i++) {
		if (board[drawFlag.y + blockPoint[i].y][drawFlag.x + blockPoint[i].x] == WALL 
			|| board[drawFlag.y + blockPoint[i].y][drawFlag.x + blockPoint[i].x] == INACTIVE)
			return false;
	}

	return true;
}

//라인 하나가 INACTIVE BLOCK으로 채워졌는지 검사
bool GameManager::IsLineClear(int row) {

	for (int i = 1; i < WIDTH+1; i++) {
		if (board[row][i] != INACTIVE)
			return false;
	}

	return true;
}

//라인을 정리함.
void GameManager::ClearLine() {
	
	//맨 밑에서부터 검사
	for (int i = HEIGHT - 1; i > 0;) { 
		//라인이 블록으로 다 채워지지 않은 경우
		//다음 라인으로 탐색범위를 넘김.
		if (!IsLineClear(i)) {
			--i;
		}
		else{
			//라인이 블록으로 채워진 경우
			//해당 라인을 지우고 윗쪽 board의 정보를 한칸씩 아래로 내림.
			for (int j = i; j > 0; j--) {
				for (int k = 1; k < WIDTH + 1; k++) {
					board[j][k] = board[j - 1][k];
				}				
			}
			//점수 누적.
			totalScore += scoreWeight;
			//점수에 따른 게임난이도 조절
			GameSpeedManager();

		}
		
	}
}

//임계선에 블록이 닿았는지 검사. 닿은 경우 죽음
bool GameManager::IsDead(){

	for (int i = 1; i < WIDTH + 1; i++) {
		if (board[2][i] ==INACTIVE)
			return true;
	}
	
	return false;
}

//다음에 나올 블록을 Render
void GameManager::PrintNextBlock() {

	for (int i = 0; i < 4; i++) {
		GotoXy(GAP+WIDTH+2+1+nextBlock[i].x, GAP+HEIGHT/3 - 5 +nextBlock[i].y);
		puts("■");
	}
}

//다음에 나올 블록을 출력하던 부분을 지움.
void GameManager::EraseNextBlock() {

	for (int i = 0; i < 4; i++) {
		GotoXy(GAP + WIDTH + 2 + 1 + nextBlock[i].x, GAP + HEIGHT / 3 - 5 + nextBlock[i].y);
		puts("  ");
	}

}

//다음에 나올 블록을 출력할 부분을 꾸밈.
void GameManager::PrintNextBlockUi() {

	for (int i = WIDTH + 2; i < WIDTH + 2 + 5; i++) {
		GotoXy(GAP+i, GAP+HEIGHT/3-1);
		puts("▩");
	}

	for (int i = HEIGHT / 3-1; i > -1; i--) {
		GotoXy(GAP+WIDTH+1+6, GAP+i);
		puts("▩");
	}

}


void GameManager::PrintUi() {

	GotoXy(GAP + WIDTH + 2 + 1, GAP + HEIGHT / 2-2);	printf("HIghScore : %d", highScore);
	GotoXy(GAP + WIDTH + 2 + 3, GAP + HEIGHT / 2);	printf("Score : %d", totalScore);


	GotoXy(GAP + WIDTH + 2 + 1, GAP + HEIGHT / 2 + 2); puts("------키조작-------");
	GotoXy(GAP + WIDTH + 2 + 1, GAP + HEIGHT / 2 + 4);	puts("    △");
	GotoXy(GAP + WIDTH + 2 + 1, GAP + HEIGHT / 2 + 5);	puts("  ◁  ▷");
	GotoXy(GAP + WIDTH + 2 + 1, GAP + HEIGHT / 2 + 6);	puts("    ▽");
	GotoXy(GAP + WIDTH + 2 + 1, GAP + HEIGHT / 2 + 8); puts(" Block Down : SpaceBar ");

	PrintNextBlockUi();
	
}

//게임을 다시 시작할지 결정함.
bool GameManager::IsGameExit() {

	if(highScore < totalScore)
		highScore = totalScore;

	GotoXy((WIDTH + 2) / 2, (HEIGHT + 1) / 2); puts("Game Over...");
	GotoXy((WIDTH + 2) / 2, (HEIGHT + 1) / 2 + 1); puts("<If you want restart, press enter>");

	while (true) {
		if (KEY_DOWN(VK_RETURN)) {
			break;
		}
		else if (KEY_DOWN(VK_ESCAPE)) {
			run = false;
			break;
		}
		
	}

	return false;

}

//게임 난이도 및 점수 가중치 조절
void GameManager::GameSpeedManager() {

	if (totalScore == 70) {
		gameSpeed -= 2;
		scoreWeight += 10;	
	}
	else if (totalScore == 270) {
		gameSpeed -= 2;
		scoreWeight += 15;
	}
	else if (totalScore == 620) {
		gameSpeed -= 2;
		scoreWeight += 20;	
	}
	else if (totalScore == 1170) {
		gameSpeed -= 2;
		scoreWeight += 30;		
	}
	else if (totalScore == 2020) {
		gameSpeed -= 1;
		scoreWeight += 40;
	}

}



