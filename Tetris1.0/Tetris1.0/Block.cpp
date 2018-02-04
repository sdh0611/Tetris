#include "stdafx.h"
#include "Block.h"

Block::Block() {

	srand(time(NULL));

	Init();

}

Block::~Block() {	}


void Block::Init() {

	numberOfBlock = 0;
	blockRotation = 0;

	PushBlock();
	
}

void Block::TurnBlock() {

	blockRotation = (++blockRotation) % 4;
	
	UpdateBlockPoint();
}

void Block::TurnUndo() {

	(blockRotation == 0) ? (blockRotation = 3):(--blockRotation);
		
	UpdateBlockPoint();
}

Point* Block::PopBlock() {
	
	blockShape = nextBlocks.front();
	nextBlocks.pop_front();
	--numberOfBlock;
	blockRotation = 0;

	PushBlock();
	UpdateBlockPoint();
	return reinterpret_cast<Point*>(block);

}

Point* Block::GetNextBlockInfo() {

	for (int i = 0; i < 4; i++) {
		nextBlock[i].x = Blocks[nextBlocks.front()][0][i].x;
		nextBlock[i].y = Blocks[nextBlocks.front()][0][i].y;
	}

	return reinterpret_cast<Point*>(nextBlock);

}

Point* Block::GetBlockPoint() {

	return reinterpret_cast<Point*>(block);

}

void Block::PushBlock() {
	
	for (; numberOfBlock < MAX; ++numberOfBlock) {
		nextBlocks.push_back(rand() % 7);
	}

}

void Block::UpdateBlockPoint() {
	for (int i = 0; i < 4; i++) {
		block[i].x = Blocks[blockShape][blockRotation][i].x;
		block[i].y = Blocks[blockShape][blockRotation][i].y;
	}

}
