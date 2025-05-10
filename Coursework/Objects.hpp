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

//����������� ������� ����
enum Direction {
    VERTICAL,
    HORIZONTAL
};

//��� ������
enum BonusType {
    NONE = 0,
    CHANGE_SIZE, //��������� ������� �������
    CHANGE_SPEED, //��������� �������� ����
    CHANGE_STICKING, //��������� ���������� ������ � �������
    BOTTOM, //����������� ���
    EXTRA_BALL //������ ���
};

//������ - ����� ���������� � ����
class Object {
public:
    float x;
    float y;
    sf::Color color;
};

//���������� ������
class Moving : public Object {
public:
    float dirX;
    float dirY;
    float speed;
    void Move(float dTime);
};

//���
class Ball : public Moving {
public:
    Ball();
    void Reflect(Direction dir);
    bool isSticky; //��������� �� ��� � �������
    bool isStuck; //��������� �� ��� ������ � ��������� ���������
};

//�������
class Paddle : public Moving {
public:
    Paddle();
    float size; //������ �� �����������
};

//������� ����: ����, ���(�), �������
class Board {
public:
    Board(sf::RenderWindow& wind, sf::Font f);
    vector<Ball> balls;
    Paddle paddle;
    int points; //������� �����
    sf::RenderWindow& window;
    sf::Font font;
};

//�����
class Bonus : public Moving {
public:
    Bonus(float startX, float startY, BonusType bonusType);
    BonusType type;
    bool isDropped; //������� �� ����� (���������� �� ���)
};

//���� - ����������� �����
class Block : public Object {
public:
    Block() { isActive = true; color = sf::Color::Green; };
    bool isActive; //������� ��
    int health; //��������
    bool isDestructible; //����� �� ����������
    BonusType bonusType; //��� ������ ��� �����������
    virtual void OnCollision(Board& board, int ballIndex) = 0;
};

//������������� ����
class IndestructibleBlock : public Block {
public:
    IndestructibleBlock(float leftX, float topY);
    void OnCollision(Board& board, int ballIndex) override { };
};

//����, ������������� �������� ���� ��� ������������
class SpeedBoostBlock : public Block {
public:
    SpeedBoostBlock(float leftX, float topY);
    void OnCollision(Board& board, int ballIndex) override;
};

//������� ���� - ����� ���� � ������� ��� ���
class PlainBlock : public Block {
public:
    PlainBlock(float leftX, float topY);
    void OnCollision(Board& board, int ballIndex) override;
};

