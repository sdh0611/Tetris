#include "stdafx.h"
#include "GameManager.h"

GameManager::GameManager() : block() {	}

GameManager::~GameManager() {	}

void GameManager::Exec() {

	while (run) {
		//���� �ʱ�ȭ
		Init();
		PrintUi();
		PrintNextBlock();
		
		while (!dead) {
			//���� ����
			Update();
		}
		
		//������ �ٽ� �����Ұ��� �����.
		DrawBoard();
		IsGameExit();

	}

}

//Ŀ�� ����
void GameManager::HideCursor(){

	CONSOLE_CURSOR_INFO info;

	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
	info.bVisible = false;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);

}

//���� ���۽� �Ӽ� �ʱ�ȭ
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
	//Block��ǥ �޾ƿ�
	if (IsBlockGenerate()) {
		//��� ������ǥ �ʱ�ȭ
		drawFlag.x = WIDTH / 2;
		drawFlag.y = 0;
		//���� ����� ������ ������.
		blockPoint = block.PopBlock();
		EraseNextBlock();
		//������ ������ ������ ����� ������ ������.
		nextBlock = block.GetNextBlockInfo();
		PrintUi();
		PrintNextBlock();
		//����� �����Ǿ����� �˸�.
		blockGenerate = false;
		UpdateBlock(BLOCK);
	}

	for (int i = 0; i< gameSpeed; i++) {
		//���� �׸�
		DrawBoard();
		//Ű�Է� ó��
		UpdateBlock(EMPTY);
		PlayerControl();
		UpdateBlock(BLOCK);

		if (spaceOn) {
			spaceOn = false;
			break;
		}

	}

	//����� ���Ӽӵ��� ���� ����
	UpdateBlock(EMPTY);
	++drawFlag.y;

	//�ٴڿ� ���� ���
	if (!IsMove()) {
		--drawFlag.y;
		//�ٴ��� BLOCK���� INACTIVE BLOCK���� ����
		UpdateBlock(INACTIVE);
		//���� Ŭ���� ���� Ȯ��
		ClearLine();
		//�� ���� �����ؾ����� �˸�.
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

//���۽� ���� �ʱ�ȭ
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

//���� Render
void GameManager::DrawBoard() {
		
	for(int i=0; i<HEIGHT+1; i++)
		for (int j = 0; j < WIDTH + 2; j++) {
			if (board[i][j] == WALL) {
				GotoXy(j+GAP, i+GAP);
				puts("��");
			}
			else if (board[i][j] == BLOCK) {
				GotoXy(j+GAP, i+GAP);
				puts("��");
			}
			else if (board[i][j] == INACTIVE) {
				GotoXy(j + GAP, i + GAP);
				puts("��");
			}
			else {
				GotoXy(j+GAP, i+GAP);
				puts(" .");
			}
		}		

}

//board�� ������ �Ӽ����� ������Ʈ��.
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


//Key Processing ����
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

//�̵��� ����� ��ġ�� ���̳� INACTIVE BLCOK ��ǥ�� ��ġ���� �˻�
bool GameManager::IsMove() {

	for (int i = 0; i < 4; i++) {
		if (board[drawFlag.y + blockPoint[i].y][drawFlag.x + blockPoint[i].x] == WALL 
			|| board[drawFlag.y + blockPoint[i].y][drawFlag.x + blockPoint[i].x] == INACTIVE)
			return false;
	}

	return true;
}

//���� �ϳ��� INACTIVE BLOCK���� ä�������� �˻�
bool GameManager::IsLineClear(int row) {

	for (int i = 1; i < WIDTH+1; i++) {
		if (board[row][i] != INACTIVE)
			return false;
	}

	return true;
}

//������ ������.
void GameManager::ClearLine() {
	
	//�� �ؿ������� �˻�
	for (int i = HEIGHT - 1; i > 0;) { 
		//������ ������� �� ä������ ���� ���
		//���� �������� Ž�������� �ѱ�.
		if (!IsLineClear(i)) {
			--i;
		}
		else{
			//������ ������� ä���� ���
			//�ش� ������ ����� ���� board�� ������ ��ĭ�� �Ʒ��� ����.
			for (int j = i; j > 0; j--) {
				for (int k = 1; k < WIDTH + 1; k++) {
					board[j][k] = board[j - 1][k];
				}				
			}
			//���� ����.
			totalScore += scoreWeight;
			//������ ���� ���ӳ��̵� ����
			GameSpeedManager();

		}
		
	}
}

//�Ӱ輱�� ����� ��Ҵ��� �˻�. ���� ��� ����
bool GameManager::IsDead(){

	for (int i = 1; i < WIDTH + 1; i++) {
		if (board[2][i] ==INACTIVE)
			return true;
	}
	
	return false;
}

//������ ���� ����� Render
void GameManager::PrintNextBlock() {

	for (int i = 0; i < 4; i++) {
		GotoXy(GAP+WIDTH+2+1+nextBlock[i].x, GAP+HEIGHT/3 - 5 +nextBlock[i].y);
		puts("��");
	}
}

//������ ���� ����� ����ϴ� �κ��� ����.
void GameManager::EraseNextBlock() {

	for (int i = 0; i < 4; i++) {
		GotoXy(GAP + WIDTH + 2 + 1 + nextBlock[i].x, GAP + HEIGHT / 3 - 5 + nextBlock[i].y);
		puts("  ");
	}

}

//������ ���� ����� ����� �κ��� �ٹ�.
void GameManager::PrintNextBlockUi() {

	for (int i = WIDTH + 2; i < WIDTH + 2 + 5; i++) {
		GotoXy(GAP+i, GAP+HEIGHT/3-1);
		puts("��");
	}

	for (int i = HEIGHT / 3-1; i > -1; i--) {
		GotoXy(GAP+WIDTH+1+6, GAP+i);
		puts("��");
	}

}


void GameManager::PrintUi() {

	GotoXy(GAP + WIDTH + 2 + 1, GAP + HEIGHT / 2-2);	printf("HIghScore : %d", highScore);
	GotoXy(GAP + WIDTH + 2 + 3, GAP + HEIGHT / 2);	printf("Score : %d", totalScore);


	GotoXy(GAP + WIDTH + 2 + 1, GAP + HEIGHT / 2 + 2); puts("------Ű����-------");
	GotoXy(GAP + WIDTH + 2 + 1, GAP + HEIGHT / 2 + 4);	puts("    ��");
	GotoXy(GAP + WIDTH + 2 + 1, GAP + HEIGHT / 2 + 5);	puts("  ��  ��");
	GotoXy(GAP + WIDTH + 2 + 1, GAP + HEIGHT / 2 + 6);	puts("    ��");
	GotoXy(GAP + WIDTH + 2 + 1, GAP + HEIGHT / 2 + 8); puts(" Block Down : SpaceBar ");

	PrintNextBlockUi();
	
}

//������ �ٽ� �������� ������.
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

//���� ���̵� �� ���� ����ġ ����
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



