#include "Objects.hpp"

//��������
void Moving::Move(float dTime) {
    x += speed * dTime * dirX;
    y += speed * dTime * dirY;
}

//�������� ����
Ball::Ball() {
    speed = START_SPEED;
    x = WINDOW_SIZE / 2;
    y = WINDOW_SIZE / 2;
    dirX = 1;
    dirY = -1;
    color = sf::Color::Red;
    isSticky = false;
    isStuck = false;
}

//������ ����
void Ball::Reflect(Direction dir) {
    if (dir == VERTICAL) {
        dirY *= -1;
    }
    else {
        dirX *= -1;
    }
}

//�������� �������
Paddle::Paddle() {
    x = 0;
    y = WINDOW_SIZE - BLOCK_HEIGHT;
    dirX = 0;
    dirY = 0;
    size = BLOCK_WIDTH;
    speed = PADDLE_SPEED;
    color = sf::Color::Yellow;
}

//�������� �������� ����
Board::Board(sf::RenderWindow& wind, sf::Font f) : window(wind), font(f) {
    points = 0;
    Ball b;
    balls.push_back(b);
}

//�������� ������ �� ����������� � ����
Bonus::Bonus(float startX, float startY, BonusType bonusType) {
    x = startX;
    y = startY;
    dirX = 0;
    dirY = 1;
    speed = BONUS_SPEED;
    isDropped = true;
    type = bonusType;
    switch ((int)type)
    {
    case 1:
        color = sf::Color::Yellow;
        break;
    case 2:
        color = sf::Color::Red;
        break;
    case 3:
        color = sf::Color::Blue;
        break;
    case 4:
        color = sf::Color::Cyan;
        break;
    case 5:
        color = sf::Color::Magenta;
        break;
    }
};

//�������� �������������� �����
IndestructibleBlock::IndestructibleBlock(float leftX, float topY) {
    x = leftX;
    y = topY;
    bonusType = NONE;
    isDestructible = false;
};

//�������� �����, �������������� �������� ������ ��� ������������ - �������� �� 1 �� 3
SpeedBoostBlock::SpeedBoostBlock(float leftX, float topY) {
    x = leftX;
    y = topY;
    health = rand() % 3 + 1;
    bonusType = NONE;
    isDestructible = true;
};

//��� ������������ �������� ���� �������������, �������� ����� �����������
void SpeedBoostBlock::OnCollision(Board& board, int ballIndex) {
    board.balls[ballIndex].speed += SPEED_BOOST;
    health--;
    board.points++;
    if (health == 0) {
        isActive = false;
    }
}

//�������� �������� ����� - �������� �� 1 �� 3, ��������� ����� (��� ��� ������)
PlainBlock::PlainBlock(float leftX, float topY) {
    x = leftX;
    y = topY;
    health = rand() % 3 + 1;
    bonusType = (BonusType)(rand() % 6);
    isDestructible = true;
};

//��� ������������ �������� ����� �����������
void PlainBlock::OnCollision(Board& board, int ballIndex) {
    health--;
    board.points++;
    if (health == 0) {
        isActive = false;
    }
}
