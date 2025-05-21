#pragma once
#include <SFML/Graphics.hpp>
#include <windows.h>
#define WINDOW_SIZE 800 
#define BLOCK_WIDTH 160 
#define BLOCK_HEIGHT 20 
#define BLOCKS_IN_ROW 5 
#define BLOCKS_COUNT 25
#define BALL_RADIUS 12
#define START_SPEED 26
#define SPEED_BOOST 2
#define PADDLE_SPEED 100
#define BONUS_SPEED 10

using std::vector;

//направления отскока мяча
enum Direction {
    VERTICAL,
    HORIZONTAL
};

//объект - имеет координаты и цвет
class Object {
public:
    float x;
    float y;
    sf::Color color;
};

//движущийся объект
class Moving : public Object {
public:
    float dirX;
    float dirY;
    float speed;
    void Move(float dTime);
};

//мяч
class Ball : public Moving {
public:
    Ball();
    void Reflect(Direction dir);
    bool isSticky; //прилипает ли мяч к каретке
    bool isStuck; //находится ли мяч сейчас в прилипшем состоянии
};

//каретка
class Paddle : public Moving {
public:
    Paddle();
    float size; //размер по горизонтали
};

//игровое поле: окно, мяч(и), каретка
class Board {
public:
    Board(sf::RenderWindow& wind, sf::Font f);
    vector<Ball> balls;
    Paddle paddle;
    int points; //счетчик очков
    bool bottomActivated; //активно ли одноразовое дно
    sf::RenderWindow& window;
    sf::Font font;
};

//блок - абстрактный класс
class Block : public Object {
public:
    Block() { isActive = true; color = sf::Color::Green; };
    bool isActive; //активен ли
    int health; //здоровье
    bool isDestructible; //можно ли уничтожить
    bool hasBonus; //есть ли бонус
    virtual void OnCollision(Board& board, int ballIndex) = 0;
};

//неразрушаемый блок
class IndestructibleBlock : public Block {
public:
    IndestructibleBlock(float leftX, float topY);
    void OnCollision(Board& board, int ballIndex) override { };
};

//блок, увеличивающий скорость мяча при столкновении
class SpeedBoostBlock : public Block {
public:
    SpeedBoostBlock(float leftX, float topY);
    void OnCollision(Board& board, int ballIndex) override;
};

//обычный блок - может быть с бонусом или без
class PlainBlock : public Block {
public:
    PlainBlock(float leftX, float topY);
    void OnCollision(Board& board, int ballIndex) override;
};

//бонус - абстрактный класс
class Bonus : public Moving {
public:
    Bonus(float startX, float startY);
    bool isDropped; //выпущен ли бонус (отображать ли его)
    virtual void Activate(Board& board) = 0;
};

//бонус - изменение размера каретки
class ChangeSizeBonus : public Bonus {
public:
    ChangeSizeBonus(float startX, float startY);
    void Activate(Board& board) override;
};

//бонус - изменение скорости мяча
class ChangeSpeedBonus : public Bonus {
public:
    ChangeSpeedBonus(float startX, float startY);
    void Activate(Board& board) override;
};

//бонус - изменение прилипания мяча к каретке
class ChangeStickingBonus : public Bonus {
public:
    ChangeStickingBonus(float startX, float startY);
    void Activate(Board& board) override;
};

//бонус - одноразовое дно
class BottomBonus : public Bonus {
public:
    BottomBonus(float startX, float startY);
    void Activate(Board& board) override;
};

//бонус - второй мяч
class ExtraBallBonus : public Bonus {
public:
    ExtraBallBonus(float startX, float startY);
    void Activate(Board& board) override;
};



