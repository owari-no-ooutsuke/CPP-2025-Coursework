#pragma once
#include "Objects.hpp"
#define DELTA 5
#define BONUS_TYPE_COUNT 5
#define LOSS_POINTS -10
#define MIN_WIN_POINTS 10

//класс, управляющий игрой
class Game {
public:
    Game(Board& b);
    Board& board; //игровое поле
    vector<Block*> blocks; //массив блоков
    vector<Bonus> bonuses; //массив бонусов
    bool bottomActivated; //активно ли одноразовое дно
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