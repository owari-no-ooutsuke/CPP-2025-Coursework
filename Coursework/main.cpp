#include <SFML/Graphics.hpp>
#include <windows.h>
#define WINDOW_SIZE 800
#define BLOCK_WIDTH 160
#define BLOCK_HEIGHT 20
#define BLOCKS_IN_ROW 5
#define BLOCKS_COUNT 25
#define BALL_RADIUS 12
#define START_SPEED 26
#define SPEED_BOOST 1
#define DELTA 5
#define BONUS_TYPE_COUNT 5
#define PADDLE_SPEED 100
#define BONUS_SPEED 10
#define LOSS_POINTS -10
#define MIN_WIN_POINTS 10

using std::vector;

enum Direction {
    VERTICAL,
    HORIZONTAL
};

enum BonusType {
    NONE = 0,
    CHANGE_SIZE,
    CHANGE_SPEED,
    CHANGE_STICKING,
    BOTTOM,
    EXTRA_BALL
};

class Object {
public:
    float x;
    float y;
    sf::Color color;
};

class Moving : public Object {
public:
    float dirX;
    float dirY;
    float speed;
    void Move(float dTime);
};

void Moving::Move(float dTime) {
    x += speed * dTime * dirX;
    y += speed * dTime * dirY;
}

class Ball : public Moving {
public:
    Ball();
    void Reflect(Direction dir);
    bool isSticky;
    bool isStuck;
};

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

void Ball::Reflect(Direction dir) {
    if (dir == VERTICAL) {
        dirY *= -1;
    }
    else {
        dirX *= -1;
    }
}

class Paddle : public Moving {
public:
    Paddle();
    float size;
};

Paddle::Paddle() {
    x = 0;
    y = WINDOW_SIZE - BLOCK_HEIGHT;
    dirX = 0;
    dirY = 0;
    size = BLOCK_WIDTH;
    speed = PADDLE_SPEED;
    color = sf::Color::Yellow;
}

class Board {
public:
    Board(sf::RenderWindow& wind, sf::Font f);
    vector<Ball> balls;
    Paddle paddle;
    int points;
    sf::RenderWindow& window;
    sf::Font font;
};

Board::Board(sf::RenderWindow& wind, sf::Font f) : window(wind), font(f) {
    points = 0;
    Ball b;
    balls.push_back(b);
}

class Bonus : public Moving {
public:
    Bonus(float startX, float startY, BonusType bonusType);
    BonusType type;
    bool isDropped;
};

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

class Block : public Object {
public:
    Block() { isActive = true; color = sf::Color::Green; };
    bool isActive;
    int health;
    bool isDestructible;
    BonusType bonusType;
    virtual void OnCollision(Board& board, int ballIndex) = 0;
};

class IndestructibleBlock : public Block {
public:
    IndestructibleBlock(float leftX, float topY);
    void OnCollision(Board& board, int ballIndex) override { };
};

IndestructibleBlock::IndestructibleBlock(float leftX, float topY) {
    x = leftX;
    y = topY;
    bonusType = NONE;
    isDestructible = false;
};

class SpeedBoostBlock : public Block {
public:
    SpeedBoostBlock(float leftX, float topY);
    void OnCollision(Board& board, int ballIndex) override;
};

SpeedBoostBlock::SpeedBoostBlock(float leftX, float topY) {
    x = leftX;
    y = topY;
    health = rand() % 3 + 1;
    bonusType = NONE;
    isDestructible = true;
};

void SpeedBoostBlock::OnCollision(Board& board, int ballIndex) {
    board.balls[ballIndex].speed += SPEED_BOOST;
    health--;
    board.points++;
    if (health == 0) {
        isActive = false;
    }
}

class PlainBlock : public Block {
public:
    PlainBlock(float leftX, float topY);
    void OnCollision(Board& board, int ballIndex) override;
};

PlainBlock::PlainBlock(float leftX, float topY) {
    x = leftX;
    y = topY;
    health = rand() % 3 + 1;
    bonusType = (BonusType)(rand() % 6);
    isDestructible = true;
};

void PlainBlock::OnCollision(Board& board, int ballIndex) {
    health--;
    board.points++;
    if (health == 0) {
        isActive = false;
    }
}

class Game {
public:
    Game(Board& b);
    Board& board;
    vector<Block*> blocks;
    vector<Bonus> bonuses;
    bool bottomActivated;
    void DropBonus(float x, float y, BonusType bonusType);
    void ActivateBonus(Bonus bonus);
    void HitAnimation(int blockIndex);
    void SingleBallCollisions(int ballIndex);
    void BallOnBallCollisions();
    void HandleCollisions();
    void Update(float dTime);
    void Display();
    void LossTextDisplay();
    void WinTextDisplay();
    void CheckGameOver();
};

Game::Game(Board& b) : board(b) {
    bottomActivated = false;
    int blockType;
    for (int i = 0; i < BLOCKS_COUNT; i++) {
        blockType = rand() % 5;
        if (blockType == 0) {
            IndestructibleBlock* b = new IndestructibleBlock((i % BLOCKS_IN_ROW) * BLOCK_WIDTH,
                (i / BLOCKS_IN_ROW) * BLOCK_HEIGHT);
            blocks.push_back(b);
        }
        else if (blockType == 1) {
            SpeedBoostBlock* b = new SpeedBoostBlock((i % BLOCKS_IN_ROW) * BLOCK_WIDTH,
                (i / BLOCKS_IN_ROW) * BLOCK_HEIGHT);
            blocks.push_back(b);
        }
        else {
            PlainBlock* b = new PlainBlock((i % BLOCKS_IN_ROW) * BLOCK_WIDTH,
                (i / BLOCKS_IN_ROW) * BLOCK_HEIGHT);
            blocks.push_back(b);
        }
    }
}

void Game::DropBonus(float x, float y, BonusType bonusType) {
    Bonus bonus(x, y, bonusType);
    bonuses.push_back(bonus);
}

void Game::ActivateBonus(Bonus bonus) {
    switch ((int)bonus.type)
    {
    case 1:
        if (board.paddle.size <= BLOCK_WIDTH) {
            board.paddle.size += BLOCK_WIDTH / 4;
        }
        else {
            board.paddle.size += (float)((rand() % 2) * 2 - 1) * BLOCK_WIDTH / 4;
        }
        break;
    case 2:
        for (int i = 0; i < board.balls.size(); i++) {
            if (board.balls[i].speed <= START_SPEED) {
                board.balls[i].speed += SPEED_BOOST * 2;
            }
            else {
                board.balls[i].speed += ((rand() % 2) * 2 - 1) * SPEED_BOOST;
            }
        }
        break;
    case 3:
        for (int i = 0; i < board.balls.size(); i++) {
            if (board.balls[i].isSticky) {
                board.balls[i].isSticky = false;
            }
            else {
                board.balls[i].isSticky = true;
            }
            break;
        }
    case 4:
        bottomActivated = true;
        break;
    case 5:
        if (board.balls.size() == 1) {
            Ball b;
            board.balls.push_back(b);
        }
        else {
            board.balls.pop_back();
        }
        break;
    }
}

void Game::HitAnimation(int blockIndex) {
    if (blocks[blockIndex]->isDestructible) {
        sf::Color color;
        if (blocks[blockIndex]->health == 3) {
            color = sf::Color::White;
        }
        else if (blocks[blockIndex]->health == 2) {
            color = sf::Color::Yellow;
        }
        else {
            color = sf::Color::Red;
        }

        for (int i = 0; i < 2; i++) {
            blocks[blockIndex]->color = color;
            Display();
            Sleep(30);
            blocks[blockIndex]->color = sf::Color::Green;
            Display();
            Sleep(30);
        }
    }
}

void Game::SingleBallCollisions(int ballIndex) {
    if (board.balls[ballIndex].x <= 0   board.balls[ballIndex].x >= WINDOW_SIZE) {
        board.balls[ballIndex].Reflect(HORIZONTAL);
    }
    if (board.balls[ballIndex].y >= WINDOW_SIZE - BLOCK_HEIGHT - 2 * DELTA
        && board.balls[ballIndex].x >= board.paddle.x - DELTA
        && board.balls[ballIndex].x <= board.paddle.x + board.paddle.size + DELTA) {
        if (board.balls[ballIndex].isSticky) {
            board.balls[ballIndex].isStuck = true;
            board.balls[ballIndex].speed = board.paddle.speed;
            board.balls[ballIndex].dirY = 0;
        }
        else {
            if (board.balls[ballIndex].y <= WINDOW_SIZE - BLOCK_HEIGHT) {
                board.balls[ballIndex].Reflect(VERTICAL);
            }
            if (board.balls[ballIndex].x <= board.paddle.x
                board.balls[ballIndex].x >= board.paddle.x + board.paddle.size) {
                board.balls[ballIndex].Reflect(HORIZONTAL);
            }
        }
    }
    else if (board.balls[ballIndex].y >= WINDOW_SIZE - DELTA) {
        if (bottomActivated) {
            board.balls[ballIndex].Reflect(VERTICAL);
            bottomActivated = false;
        }
        else {
            board.balls[ballIndex].color = sf::Color::Black;
            Display();
            Sleep(400);

            board.balls[ballIndex].color = sf::Color::Red;
            board.balls[ballIndex].x = WINDOW_SIZE / 2;
            board.balls[ballIndex].y = WINDOW_SIZE / 2;
            board.balls[ballIndex].dirX = 1;
            board.balls[ballIndex].dirY = -1;
            board.points--;
            Display();
            Sleep(400);
        }
    }
    else if (board.balls[ballIndex].y <= 0) {
        board.balls[ballIndex].Reflect(VERTICAL);
    }
    else {
        int row, col;
        float leftX, topY;
        for (int i = BLOCKS_COUNT - 1; i >= 0; i--) {
            if (blocks[i]->isActive) {
                row = i / BLOCKS_IN_ROW;
                col = i % BLOCKS_IN_ROW;
                leftX = col * BLOCK_WIDTH;
                topY = row * BLOCK_HEIGHT;
                if (board.balls[ballIndex].x >= leftX - DELTA
                    && board.balls[ballIndex].x <= leftX + BLOCK_WIDTH + DELTA
                    && board.balls[ballIndex].y <= topY + BLOCK_HEIGHT + DELTA
                    && board.balls[ballIndex].y >= topY - DELTA) {
                    if (board.balls[ballIndex].x <= leftX
                        board.balls[ballIndex].x >= leftX + BLOCK_WIDTH) {
                        board.balls[ballIndex].Reflect(HORIZONTAL);
                    }
                    if (board.balls[ballIndex].y <= topY
                        board.balls[ballIndex].y >= topY + BLOCK_HEIGHT) {
                        board.balls[ballIndex].Reflect(VERTICAL);
                    }
                    HitAnimation(i);
                    blocks[i]->OnCollision(board, ballIndex);
                    if (blocks[i]->isActive == false && blocks[i]->bonusType != NONE) {
                        DropBonus(blocks[i]->x + BLOCK_WIDTH / 2, blocks[i]->y + BLOCK_HEIGHT,
                            blocks[i]->bonusType);
                    }
                    break;
                }
            }
        }
    }
};

void Game::BallOnBallCollisions() {
    float dx = board.balls[0].x - board.balls[1].x;
    float dy = board.balls[0].y - board.balls[1].y;
    float dist = sqrt(dx * dx + dy * dy);
    if (dist <= 2 * BALL_RADIUS) {
        std::swap(board.balls[0].dirX, board.balls[1].dirX);
        std::swap(board.balls[0].dirY, board.balls[1].dirY);
        std::swap(board.balls[0].speed, board.balls[1].speed);
        for (int i = 0; i < 2; i++) {
            board.balls[i].Move(0.01);
        }
    }
}

void Game::HandleCollisions() {
    for (int i = 0; i < board.balls.size(); i++) {
        SingleBallCollisions(i);
    }
    if (board.balls.size() == 2) {
        if (board.balls[0].isStuck == false && board.balls[1].isStuck == false) {
            BallOnBallCollisions();
        }
    }
    for (int i = 0; i < bonuses.size(); i++) {
        if (bonuses[i].isDropped) {
            if (bonuses[i].y >= WINDOW_SIZE - BLOCK_HEIGHT - 2 * DELTA
                && bonuses[i].x >= board.paddle.x - DELTA
                && bonuses[i].x <= board.paddle.x + board.paddle.size + DELTA) {
                bonuses[i].isDropped = false;
                ActivateBonus(bonuses[i]);
            }
            else if (bonuses[i].y >= WINDOW_SIZE) {
                bonuses[i].isDropped = false;
            }
        }
    }
};

void Game::Update(float dTime) {
    for (int i = 0; i < board.balls.size(); i++) {
        if (board.balls[i].isStuck == false) {
            board.balls[i].Move(dTime);
        }
    }
    for (int i = 0; i < bonuses.size(); i++) {
        if (bonuses[i].isDropped) {
            bonuses[i].Move(dTime);
        }
    }
    HandleCollisions();
}

void Game::Display() {
    board.window.clear();
    for (int i = 0; i < blocks.size(); i++) {
        if (blocks[i]->isActive) {
            sf::RectangleShape r({ BLOCK_WIDTH, BLOCK_HEIGHT });
            r.setPosition({ blocks[i]->x, blocks[i]->y });
            r.setFillColor(blocks[i]->color);
            r.setOutlineColor(sf::Color::Black);
            r.setOutlineThickness(-3.f);
            board.window.draw(r);
        }
    }

    if (bottomActivated) {
        sf::RectangleShape bottom({ WINDOW_SIZE, DELTA });
        bottom.setPosition({ 0, WINDOW_SIZE - DELTA });
        bottom.setFillColor(sf::Color::White);
        board.window.draw(bottom);
    }

    sf::RectangleShape p({ board.paddle.size, BLOCK_HEIGHT });
    p.setPosition({ board.paddle.x, board.paddle.y });
    p.setFillColor(board.paddle.color);
    p.setOutlineColor(sf::Color::Black);
    p.setOutlineThickness(-3.f);
    board.window.draw(p);

    for (int i = 0; i < board.balls.size(); i++) {
        sf::CircleShape c;
        c.setRadius(BALL_RADIUS);
        c.setPosition({ board.balls[i].x, board.balls[i].y });
        c.setFillColor(board.balls[i].color);
        board.window.draw(c);
    }

    sf::Text text("points: " + std::to_string(board.points), board.font, 30);
    text.setPosition({ 650, 750 });
    text.setFillColor(sf::Color::White);
    board.window.draw(text);

    for (int i = 0; i < bonuses.size(); i++) {
        if (bonuses[i].isDropped) {
            sf::CircleShape bon(15, 4);
            bon.setPosition({ bonuses[i].x, bonuses[i].y });
            bon.setFillColor(bonuses[i].color);
            board.window.draw(bon);
        }
    }

    board.window.display();
}

void Game::LossTextDisplay() {
    sf::Text text("Game Over!\n You Lost!\n Points: " + std::to_string(board.points),
        board.font, 50);
    text.setPosition({ 260, 300 });
    text.setFillColor(sf::Color::White);
    board.window.clear();
    board.window.draw(text);
    board.window.display();
    Sleep(5000);
}

void Game::WinTextDisplay() {
    sf::Text text("Game Over!\n  You Won!\n  Points: " + std::to_string(board.points),
        board.font, 50);
    text.setPosition({ 260, 300 });
    text.setFillColor(sf::Color::White);
    board.window.clear();
    board.window.draw(text);
    board.window.display();
    Sleep(5000);
}

void Game::CheckGameOver() {
    if (board.points < LOSS_POINTS) {
        LossTextDisplay();
        board.window.close();
        return;
    }
    for (int i = 0; i < BLOCKS_COUNT; i++) {
        if (blocks[i]->isDestructible && blocks[i]->isActive) {
            return;
        }
    }
    if (board.points >= MIN_WIN_POINTS) {
        WinTextDisplay();
        board.window.close();
    }
    else {
        LossTextDisplay();
        board.window.close();
    }
};


int main() {
    srand(time(NULL));
    sf::RenderWindow window(sf::VideoMode(800, 800), "ARKANOID");
    sf::Font font;
    font.loadFromFile("C:/Windows/Fonts/Arial.ttf");

    Board b(window, font);
    Game game(b);
    game.Display();

    float ballSpeed[2], ballDirX[2];

    while (window.isOpen()) {

        for (int i = 0; i < game.board.balls.size(); i++) {
            if (game.board.balls[i].isStuck == false) {
                ballSpeed[i] = game.board.balls[i].speed;
                ballDirX[i] = game.board.balls[i].dirX;
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            if (game.board.paddle.x > 0) {
                game.board.paddle.dirX = -1;
                game.board.paddle.Move(0.01);
                for (int i = 0; i < game.board.balls.size(); i++) {
                    if (game.board.balls[i].isStuck) {
                        game.board.balls[i].dirX = -1;
                        game.board.balls[i].Move(0.01);
                    }
                }
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            if (game.board.paddle.x < WINDOW_SIZE - game.board.paddle.size) {
                game.board.paddle.dirX = 1;
                game.board.paddle.Move(0.01);
                for (int i = 0; i < game.board.balls.size(); i++) {
                    if (game.board.balls[i].isStuck) {
                        game.board.balls[i].dirX = 1;
                        game.board.balls[i].Move(0.01);
                    }
                }
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            for (int i = 0; i < game.board.balls.size(); i++) {
                if (game.board.balls[i].isStuck) {
                    game.board.balls[i].isStuck = false;
                    game.board.balls[i].speed = ballSpeed[i];
                    game.board.balls[i].dirX = ballDirX[i];
                    game.board.balls[i].dirY = -1;
                }
            }
        }

        sf::Event event = sf::Event();
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        game.Update(0.01);
        game.Display();
        game.CheckGameOver();
    }
    return 0;
}