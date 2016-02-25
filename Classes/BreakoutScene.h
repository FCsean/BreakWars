#ifndef __BREAKOUT_SCENE_H__
#define __BREAKOUT_SCENE_H__

#include "cocos2d.h"
#include <vector>
enum Move{left, right, nomove};
class Breakout : public cocos2d::Layer
{
private:

	const float playerMovedSpace = 5;
	int columns = WIDTH / cocos2d::Sprite::create("bars.png")->getContentSize().width;
	int rows = 5;
	cocos2d::Sprite* player;
	cocos2d::Sprite* ball;
	std::vector<cocos2d::Sprite*> bars;

	cocos2d::Label* startLabel;
	cocos2d::Label* scoreLabel;

	cocos2d::Sprite* livesSprite[3];

	int score;
	int toAdd;

	Move move;

	float x;
	float y;
	
	const float MAX_X = 15;

	int lives = 3;
	bool lose = false;
	bool gameOver = true;

	void playerMove();
	void ballMove();
	void initWorld(); 
	void initPlayerBall();
	void ballCollisionDetection();
	void showLabel();
public:
    static cocos2d::Scene* createScene();
	static const int WIDTH = 960;
	static const int HEIGHT = 640;

	virtual void update(float delta) override;
    virtual bool init();
	
    CREATE_FUNC(Breakout);

};

#endif // __HELLOWORLD_SCENE_H__
