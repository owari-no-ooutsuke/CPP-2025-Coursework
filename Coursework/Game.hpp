#pragma once
#include "Objects.hpp"
#define DELTA 5
#define BONUS_TYPE_COUNT 5
#define LOSS_POINTS -10
#define MIN_WIN_POINTS 10

//�����, ����������� �����
class Game {
public:
    Game(Board& b);
    Board& board; //������� ����
    vector<Block*> blocks; //������ ������
    vector<Bonus> bonuses; //������ �������
    bool bottomActivated; //������� �� ����������� ���
    void DropBonus(float x, float y, BonusType bonusType);
    void ActivateBonus(Bonus bonus);
    void HitAnimation(int blockIndex);
    void SingleBallCollisions(int ballIndex);
    void BallOnBallCollisions();
    void BonusCollisions(int bonusIndex);
    void HandleCollisions();
    void Update(float dTime);
    void Display();
    void LossTextDisplay();
    void WinTextDisplay();
    void CheckGameOver();
    void Play();
};