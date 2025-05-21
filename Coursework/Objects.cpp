#include "Objects.hpp"

//движение
void Moving::Move(float dTime) {
    x += speed * dTime * dirX;
    y += speed * dTime * dirY;
}

//создание мяча
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

//отскок мяча
void Ball::Reflect(Direction dir) {
    if (dir == VERTICAL) {
        dirY *= -1;
    }
    else {
        dirX *= -1;
    }
}

//создание каретки
Paddle::Paddle() {
    x = 0;
    y = WINDOW_SIZE - BLOCK_HEIGHT;
    dirX = 0;
    dirY = 0;
    size = BLOCK_WIDTH;
    speed = PADDLE_SPEED;
    color = sf::Color::Yellow;
}

//создание игрового поля
Board::Board(sf::RenderWindow& wind, sf::Font f) : window(wind), font(f) {
    points = 0;
    Ball b;
    balls.push_back(b);
    bottomActivated = false;
}

//создание неразрушаемого блока
IndestructibleBlock::IndestructibleBlock(float leftX, float topY) {
    x = leftX;
    y = topY;
    hasBonus = false;
    isDestructible = false;
};

//создание блока, увеличивающего скорость шарика при столкновении - здоровье от 1 до 3
SpeedBoostBlock::SpeedBoostBlock(float leftX, float topY) {
    x = leftX;
    y = topY;
    health = rand() % 3 + 1;
    hasBonus = false;
    isDestructible = true;
};

//при столкновении скорость мяча увеличивается, здоровье блока уменьшается
void SpeedBoostBlock::OnCollision(Board& board, int ballIndex) {
    board.balls[ballIndex].speed += SPEED_BOOST;
    health--;
    board.points++;
    if (health == 0) {
        isActive = false;
    }
}

//создание обычного блока - здоровье от 1 до 3, рандомный бонус (или без бонуса)
PlainBlock::PlainBlock(float leftX, float topY) {
    x = leftX;
    y = topY;
    health = rand() % 3 + 1;
    int bonusNum = (rand() % 6);
    if (bonusNum == 0) {
        hasBonus = false;
    }
    else {
        hasBonus = true;
    }
    isDestructible = true;
};

//при столкновении здоровье блока уменьшается
void PlainBlock::OnCollision(Board& board, int ballIndex) {
    health--;
    board.points++;
    if (health == 0) {
        isActive = false;
    }
}

//создание бонуса по координатам и типу
Bonus::Bonus(float startX, float startY) {
    x = startX;
    y = startY;
    dirX = 0;
    dirY = 1;
    speed = BONUS_SPEED;
    isDropped = true;
};

ChangeSizeBonus::ChangeSizeBonus(float startX, float startY): Bonus(startX, startY) {
    color = sf::Color::Yellow;
}

void ChangeSizeBonus::Activate(Board& board) {
    if (board.paddle.size <= BLOCK_WIDTH) {
        board.paddle.size += BLOCK_WIDTH / 4;
    }
    else {
        board.paddle.size += (float)((rand() % 2) * 2 - 1) * BLOCK_WIDTH / 4;
    }
}

ChangeSpeedBonus::ChangeSpeedBonus(float startX, float startY) : Bonus(startX, startY) {
    color = sf::Color::Red;
}

void ChangeSpeedBonus::Activate(Board& board) {
    for (int i = 0; i < board.balls.size(); i++) {
        if (board.balls[i].speed <= START_SPEED) {
            board.balls[i].speed += SPEED_BOOST * 2;
        }
        else {
            board.balls[i].speed += ((rand() % 2) * 2 - 1) * SPEED_BOOST;
        }
    }
}

ChangeStickingBonus::ChangeStickingBonus(float startX, float startY) : Bonus(startX, startY) {
    color = sf::Color::Blue;
}

void ChangeStickingBonus::Activate(Board& board) {
    for (int i = 0; i < board.balls.size(); i++) {
        if (board.balls[i].isSticky) {
            board.balls[i].isSticky = false;
        }
        else {
            board.balls[i].isSticky = true;
        }
    }
}

BottomBonus::BottomBonus(float startX, float startY) : Bonus(startX, startY) {
    color = sf::Color::Cyan;
}

void BottomBonus::Activate(Board& board) {
    board.bottomActivated = true;
}

ExtraBallBonus::ExtraBallBonus(float startX, float startY) : Bonus(startX, startY) {
    color = sf::Color::Magenta;
}

void ExtraBallBonus::Activate(Board& board) {
    if (board.balls.size() == 1) {
        Ball b;
        board.balls.push_back(b);
    }
    else {
        board.balls.pop_back();
    }
}