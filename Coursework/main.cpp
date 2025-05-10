#include <SFML/Graphics.hpp>
#include <windows.h>
#define WINDOW_SIZE 800
#define BLOCK_WIDTH 160
#define BLOCK_HEIGHT 20
#define BLOCKS_IN_ROW 5
#define BLOCKS_COUNT 25
#define START_SPEED 30
#define SPEED_BOOST 2
#define DELTA 5
#define BONUS_TYPE_COUNT 5
#define PADDLE_SPEED 100
#define BONUS_SPEED 20

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
    CHANGE_TRAJECTORY
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
};

Ball::Ball() {
    speed = START_SPEED;
    x = WINDOW_SIZE / 2;
    y = WINDOW_SIZE / 2;
    dirX = 1;
    dirY = -1;
    color = sf::Color::Red;
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
    Board(sf::RenderWindow& wind, sf::Font f) : window(wind), font(f) { points = 0; };
    Ball ball;
    Paddle paddle;
    int points;
    sf::RenderWindow& window;
    sf::Font font;
};

class Bonus : public Moving {
public:
    Bonus();
    BonusType type;
    bool isDropped;
    void Drop(float startX, float startY, Board& board);
    void Activate(Board& board);
};

Bonus::Bonus() {
    x = 0;
    y = 0;
    dirX = 0;
    dirY = 0;
    speed = BONUS_SPEED;
    type = (BonusType)(rand() % BONUS_TYPE_COUNT);
    isDropped = false;
};

void Bonus::Drop(float startX, float startY, Board& board) {
    x = startX;
    y = startY;
    dirY = 1;
}

void Bonus::Activate(Board& board) {
    isDropped = false;
    switch ((int)type)
    {
    case 0:
        board.paddle.size += ((rand() % 2) * 2 - 1) * BLOCK_WIDTH;
        break;
    case 1:
        board.ball.speed += ((rand() % 2) * 2 - 1) * SPEED_BOOST * 2;
        break;
    case 2:
        break;
    case 3:
        break;
    case 4:
        break;
    }
}


class Block : public Object {
public:
    Block() { isActive = true; color = sf::Color::Green; };
    bool isActive;
    BonusType bonusType;
    virtual void OnCollision(Board& board) = 0;
};

class IndestructibleBlock : public Block {
public:
    IndestructibleBlock(float leftX, float topY) { x = leftX; y = topY; bonusType = NONE; };
    void OnCollision(Board& board) override { };
};

class SpeedBoostBlock : public Block {
public:
    SpeedBoostBlock(float leftX, float topY);
    int health;
    void OnCollision(Board& board) override;
};

SpeedBoostBlock::SpeedBoostBlock(float leftX, float topY) {
    x = leftX;
    y = topY;
    health = rand() % 3 + 1;
    bonusType = NONE;
};

void SpeedBoostBlock::OnCollision(Board& board) {
    board.ball.speed += SPEED_BOOST;
    health--;
    board.points++;
    if (health == 0) {
        isActive = false;
    }
}

class PlainBlock : public Block {
public:
    PlainBlock(float leftX, float topY);
    int health;
    void OnCollision(Board& board) override;
};

PlainBlock::PlainBlock(float leftX, float topY) {
    x = leftX;
    y = topY;
    health = rand() % 3 + 1;
    bonusType = (BonusType)(rand() % 6);
};

void PlainBlock::OnCollision(Board& board) {
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
    void HandleCollisions();
    void Update(float dTime);
    void Display();
};

Game::Game(Board& b) : board(b) {
    int blockType;
    for (int i = 0; i < BLOCKS_COUNT; i++) {
        blockType = rand() % 3;
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

void Game::HandleCollisions() {
    if (board.ball.x <= 0  board.ball.x >= WINDOW_SIZE) {
        board.ball.Reflect(HORIZONTAL);
    }
    if (board.ball.y >= WINDOW_SIZE - BLOCK_HEIGHT - 2 * DELTA
        && board.ball.x >= board.paddle.x - DELTA
        && board.ball.x <= board.paddle.x + board.paddle.size + DELTA) {
        board.ball.Reflect(VERTICAL);
    }
    else if (board.ball.y >= WINDOW_SIZE - DELTA) {
        Sleep(200);
        board.points--;
        board.ball.Reflect(VERTICAL);
    }
    else if (board.ball.y <= 0) {
        board.ball.Reflect(VERTICAL);
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
                if (board.ball.x >= leftX - DELTA
                    && board.ball.x <= leftX + BLOCK_WIDTH + DELTA
                    && board.ball.y <= topY + BLOCK_HEIGHT + DELTA
                    && board.ball.y >= topY - DELTA) {
                    if (board.ball.x <= leftX  board.ball.x >= leftX + BLOCK_WIDTH) {
                        board.ball.Reflect(HORIZONTAL);
                    }
                    if (board.ball.y <= topY || board.ball.y >= topY + BLOCK_HEIGHT) {
                        board.ball.Reflect(VERTICAL);
                    }
                    blocks[i]->OnCollision(board);
                    if (blocks[i]->isActive == false && blocks[i]->bonusType != NONE) {
                        //Drop Bonus
                    }
                    break;
                }
            }
        }
    }
};

void Game::Update(float dTime) {
    board.ball.Move(dTime);
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
    sf::RectangleShape p({ board.paddle.size, BLOCK_HEIGHT });
    p.setPosition({ board.paddle.x, board.paddle.y });
    p.setFillColor(board.paddle.color);
    p.setOutlineColor(sf::Color::Black);
    p.setOutlineThickness(-3.f);
    board.window.draw(p);

    sf::CircleShape c;
    c.setRadius(12);
    c.setPosition(board.ball.x, board.ball.y);
    c.setFillColor(board.ball.color);
    board.window.draw(c);

    sf::Text text("points: " + std::to_string(board.points), board.font, 30);
    text.setPosition({ 650, 750 });
    text.setFillColor(sf::Color::White);
    board.window.draw(text);

    board.window.display();
}


int main() {
    srand(time(NULL));
    sf::RenderWindow window(sf::VideoMode(800, 800), "ARKANOID");

    sf::Font font;
    font.loadFromFile("C:/Windows/Fonts/Arial.ttf");

    Board b(window, font);
    Game g(b);
    g.Display();

    while (window.isOpen()) {

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                if (g.board.paddle.x > 0) {
                    g.board.paddle.dirX = -1;
                    g.board.paddle.Move(0.01);
                }
            }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            if (g.board.paddle.x < WINDOW_SIZE - BLOCK_WIDTH) {
                g.board.paddle.dirX = 1;
                g.board.paddle.Move(0.01);
            }
        }

        sf::Event event = sf::Event();
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        g.Update(0.01);
        g.Display();
    }
    return 0;
}