#include "BreakoutScene.h"
#include "ui/CocosGUI.h"
#include "SimpleAudioEngine.h"
#include <vector>
USING_NS_CC;

Scene* Breakout::createScene()
{
	auto scene = Scene::create();

	auto layer = Breakout::create();

	scene->addChild(layer);

	return scene;
}

bool Breakout::init()
{
	if (!Layer::init())
	{
		return false;
	}

	auto audio = CocosDenshion::SimpleAudioEngine::getInstance();
	audio->preloadEffect("deflect.wav");
	audio->preloadEffect("breakbrick.wav");
	audio->playBackgroundMusic("bgm.wav", true);
	auto eventListener = EventListenerKeyboard::create();
	eventListener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* event) {
		switch (keyCode) {
		case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
			move = left;
			break;
		case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
			move = right;
			break;
		case EventKeyboard::KeyCode::KEY_SPACE:
			if (gameOver) {
				gameOver = false;
				lives = 3;
				this->removeAllChildren();
				bars.clear();
				initWorld();
				this->scheduleUpdate();
			}
			else if (lose) {
				lose = false;
				initPlayerBall();
				this->scheduleUpdate();
				this->removeChild(startLabel);
			}
			break;
		}
	};
	eventListener->onKeyReleased = [this](EventKeyboard::KeyCode keyCode, Event* event) {
		switch (keyCode) {
		case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
			if (move == left)
				move = Move::nomove;
			break;
		case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
			if (move == right)
				move = Move::nomove;
			break;
		}
	};
	initWorld();
	showLabel();
	this->_eventDispatcher->addEventListenerWithFixedPriority(eventListener, 1);
	return true;
}

void Breakout::showLabel() {
	startLabel = Label::createWithTTF("Press space to Start", "micross.ttf", 24);
	startLabel->setColor(Color3B::WHITE);
	startLabel->setPosition(Vec2(WIDTH / 2, HEIGHT / 2));
	this->addChild(startLabel);
}

void Breakout::initPlayerBall(){
	player->setPosition(Vec2(WIDTH / 2, 50));
	ball->setPosition(Vec2(WIDTH / 2, 250));
	x = 0;
	y = -5;
	move = nomove;
}

void Breakout::initWorld() {
	for (int i = 0; i < lives; i++) {
		livesSprite[i] = Sprite::create("ball.png");
		auto size = livesSprite[i]->getContentSize();
		livesSprite[i]->setAnchorPoint(Vec2::ZERO);
		livesSprite[i]->setPosition(i*(size.width+5)+size.width/2 + 5, HEIGHT-1);
		this->addChild(livesSprite[i]);
	}
	toAdd = 1;
	score = 0;
	player = Sprite::create("player.png");
	ball = Sprite::create("ball.png");
	initPlayerBall();
	this->addChild(player);
	this->addChild(ball);
	scoreLabel = Label::createWithTTF("0", "micross.ttf", 24);
	scoreLabel->setPosition(Vec2(WIDTH/2-scoreLabel->getContentSize().width/2, HEIGHT+ scoreLabel->getContentSize().height/2));
	scoreLabel->setColor(Color3B::WHITE);
	this->addChild(scoreLabel);
	for (int i = 0; i < columns; i++) {
		for (int j = 0; j < rows; j++) {
			Sprite* bar = Sprite::create("bars.png");
			bar->setAnchorPoint(Vec2::ZERO);
			auto barSize = bar->getContentSize();
			bar->setPosition(Vec2(WIDTH/2-columns*barSize.width/2+i*barSize.width, HEIGHT - 60 - j * barSize.height));
			bars.push_back(bar);
			this->addChild(bar);
		}
	}

}

void Breakout::update(float delta) {
	ballMove();
	playerMove();
	ballCollisionDetection();
}

void Breakout::ballMove() {
	Vec2 position = ball->getPosition();
	ball->setPosition(Vec2(position.x + x, position.y + y));
}
void Breakout::playerMove() {
	Vec2 position = player->getPosition();
	auto playerSize = player->getContentSize();
	switch (move) {
	case left:
		if (position.x - playerMovedSpace > 0 + playerSize.width/2)
			player->setPosition(Vec2(position.x - playerMovedSpace, position.y));
		break;
	case right:
		if (position.x + playerMovedSpace < WIDTH - playerSize.width / 2)
			player->setPosition(Vec2(position.x + playerMovedSpace, position.y));
		break;
	}
}

void Breakout::ballCollisionDetection() {
	auto ballSize = ball->getContentSize();
	auto ballPosition = ball->getPosition();
	Rect ballRect = Rect::Rect(ballPosition.x - ballSize.width / 2, ballPosition.y - ballSize.height / 2, ballSize.width, ballSize.height);

	if (ballPosition.x < 0 + ballSize.width/2 && x < 0) {
		x *= -1;
	}

	if (ballPosition.x > WIDTH - ballSize.width/2 && x > 0) {
		x *= -1;
	}
	if (ballPosition.y > HEIGHT - ballSize.height/2) {
		y *= -1;
	}

	if (ballPosition.y < 0 - ballSize.height / 2) {
		this->unscheduleUpdate();
		lives--;
		this->removeChild(livesSprite[lives]);
		if (lives > 0){
			lose = true;
			showLabel();
		}
		else {
			gameOver = true;
			startLabel = Label::createWithTTF("You Lose!\nPress space to Restart", "micross.ttf", 24);
			startLabel->setColor(Color3B::WHITE);
			startLabel->setPosition(Vec2(WIDTH / 2, HEIGHT / 2));
			this->addChild(startLabel);
		}
	}

	auto playerSize = player->getContentSize();
	auto playerPosition = player->getPosition();
	Rect playerRect = Rect::Rect(playerPosition.x - playerSize.width / 2, playerPosition.y - playerSize.height / 2, playerSize.width, playerSize.height);
	if (ballRect.intersectsRect(playerRect)) {
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("deflect.wav");
		if (ballPosition.y >= playerPosition.y + (playerSize.height / 2)) {
			if (playerPosition.x > ballPosition.x) {
				float diff = playerPosition.x - ballPosition.x;
				if (x > 0) {
					x *= -1;
				}
				x = -diff / 30;
			}			
			else {
				float diff = ballPosition.x - playerPosition.x;
				if (x < 0) {
					x *= -1;
				}
				x = diff / 30;
				
			}
			if (y < 0)
				y *= -1;
		}
		else if (ballPosition.x < playerPosition.x || ballPosition.x > playerPosition.x){
			x *= -1;
			if (move == left && ballPosition.x < playerPosition.x) {
				x -= playerMovedSpace + 5;
			}
			else if (move == right && ballPosition.x > playerPosition.x) {
				x += playerMovedSpace + 5;
			}
		}
		if (x < MAX_X * -1) {
			x = MAX_X * -1;
		}
		else if (x > MAX_X) {
			x = MAX_X;
		}
	}
	bool reversed = false;
	for (unsigned i = 0; i < bars.size(); i++) {
		auto barSize = bars[i]->getContentSize();
		auto barPosition = bars[i]->getPosition();
		Rect barRect = Rect::Rect(barPosition.x, barPosition.y, barSize.width, barSize.height);
		if (ballRect.intersectsRect(barRect)) {
			if (!reversed) {
				if (ballPosition.y <= barPosition.y || ballPosition.y >= barPosition.y + (playerSize.height)) {
					y *= -1;
				}
				else if (ballPosition.x < barPosition.x + barSize.width/2 || ballPosition.x > barPosition.x + barSize.width / 2) {
					x *= -1;
				}
				CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("breakbrick.wav");
			}
			reversed = true;
			this->removeChild(bars[i]); 
			bars.erase(bars.begin() + i);
			i--;
			score += toAdd;
			toAdd++;
			scoreLabel->setString(std::to_string(score));
		}
	}
	if (bars.size() <= 0) {
		this->unscheduleUpdate();
		gameOver = true;
		startLabel = Label::createWithTTF("You Win!\nPress space to Restart", "micross.ttf", 24);
		startLabel->setColor(Color3B::WHITE);
		startLabel->setPosition(Vec2(WIDTH / 2, HEIGHT / 2));
		this->addChild(startLabel);
	}

}