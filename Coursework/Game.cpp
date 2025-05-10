#include "Game.hpp"

//создание игры - тип каждого блока задается рандомно
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

//выпадение бонуса 
void Game::DropBonus(float x, float y, BonusType bonusType) {
    Bonus bonus(x, y, bonusType);
    bonuses.push_back(bonus);
}

//активация бонуса
void Game::ActivateBonus(Bonus bonus) {
    switch ((int)bonus.type)
    {
    case 1: //изменение размера каретки
        if (board.paddle.size <= BLOCK_WIDTH) {
            board.paddle.size += BLOCK_WIDTH / 4;
        }
        else {
            board.paddle.size += (float)((rand() % 2) * 2 - 1) * BLOCK_WIDTH / 4;
        }
        break;
    case 2: //изменение скорости мяча
        for (int i = 0; i < board.balls.size(); i++) {
            if (board.balls[i].speed <= START_SPEED) {
                board.balls[i].speed += SPEED_BOOST * 2;
            }
            else {
                board.balls[i].speed += ((rand() % 2) * 2 - 1) * SPEED_BOOST;
            }
        }
        break;
    case 3: //изменение прилипания шарика к каретке
        for (int i = 0; i < board.balls.size(); i++) {
            if (board.balls[i].isSticky) {
                board.balls[i].isSticky = false;
            }
            else {
                board.balls[i].isSticky = true;
            }
        }
        break;
    case 4: //одноразовое дно
        bottomActivated = true;
        break;
    case 5: //второй мяч
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

/* визуальный эффект при столкновении мяча с блоком
* 
* в зависимости от здоровья блока мигает разными цветами
* при столкновении с неразрушаемым блоком анимации нет
*/
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

/* проверка и обработка столкновений для одного мяча
* 
* столкновения со стенками, блоками, кареткой
*/
void Game::SingleBallCollisions(int ballIndex) {

    //с левой и правой границами поля
    if (board.balls[ballIndex].x <= 0 || board.balls[ballIndex].x >= WINDOW_SIZE) {
        board.balls[ballIndex].Reflect(HORIZONTAL);
    }

    //с кареткой
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
                || board.balls[ballIndex].x >= board.paddle.x + board.paddle.size) {
                board.balls[ballIndex].Reflect(HORIZONTAL);
            }
        }
    }

    //с нижней границей поля
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

    //с верхней границей поля
    else if (board.balls[ballIndex].y <= 0) {
        board.balls[ballIndex].Reflect(VERTICAL);
    }

    //с блоками
    else {
        for (int i = BLOCKS_COUNT - 1; i >= 0; i--) {

            if (blocks[i]->isActive) {

                if (board.balls[ballIndex].x >= blocks[i]->x - DELTA
                    && board.balls[ballIndex].x <= blocks[i]->x + BLOCK_WIDTH + DELTA
                    && board.balls[ballIndex].y <= blocks[i]->y + BLOCK_HEIGHT + DELTA
                    && board.balls[ballIndex].y >= blocks[i]->y - DELTA) {

                    if (board.balls[ballIndex].x <= blocks[i]->x
                        || board.balls[ballIndex].x >= blocks[i]->x + BLOCK_WIDTH) {
                        board.balls[ballIndex].Reflect(HORIZONTAL);
                    }

                    if (board.balls[ballIndex].y <= blocks[i]->y
                        || board.balls[ballIndex].y >= blocks[i]->y + BLOCK_HEIGHT) {
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

//проверка и обработка столкновений мячей друг с другом
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

//проверка и обработка столкновений бонусов с кареткой
void Game::BonusCollisions(int bonusIndex) {
    if (bonuses[bonusIndex].isDropped) {
        if (bonuses[bonusIndex].y >= WINDOW_SIZE - BLOCK_HEIGHT - 2 * DELTA
            && bonuses[bonusIndex].x >= board.paddle.x - DELTA
            && bonuses[bonusIndex].x <= board.paddle.x + board.paddle.size + DELTA) {
            bonuses[bonusIndex].isDropped = false;
            ActivateBonus(bonuses[bonusIndex]);
        }
        else if (bonuses[bonusIndex].y >= WINDOW_SIZE) {
            bonuses[bonusIndex].isDropped = false;
        }
    }
}

//проверка и обработка всех столкновений
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
        BonusCollisions(i);
    }
};

/* обновление игры
* 
* движение мячей и проверка столкновений
*/
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

//отображение объектов в окне
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

//отображение текста в случае поражения
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

//отображение текста в случае победы
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

//проверка условий окончания игры (поражения/победы)
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

//обработка нажатия клавиш и обновление игры
void Game::Play() {
    Display();

    float ballSpeed[2], ballDirX[2];

    while (board.window.isOpen()) {
        
        //сохраняем скорости мячей, если они не прилипли
        for (int i = 0; i < board.balls.size(); i++) { 
            if (board.balls[i].isStuck == false) { 
                ballSpeed[i] = board.balls[i].speed;
                ballDirX[i] = board.balls[i].dirX;
            }
        }

        //сдвигаем каретку влево
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            if (board.paddle.x > 0) {
                board.paddle.dirX = -1;
                board.paddle.Move(0.01);
                for (int i = 0; i < board.balls.size(); i++) {
                    if (board.balls[i].isStuck) {
                        board.balls[i].dirX = -1;
                        board.balls[i].Move(0.01);
                    }
                }
            }
        }

        //сдвигаем каретку вправо
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            if (board.paddle.x < WINDOW_SIZE - board.paddle.size) {
                board.paddle.dirX = 1;
                board.paddle.Move(0.01);
                for (int i = 0; i < board.balls.size(); i++) {
                    if (board.balls[i].isStuck) {
                        board.balls[i].dirX = 1;
                        board.balls[i].Move(0.01);
                    }
                }
            }
        }

        //отпускаем прилипшие мячи
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            for (int i = 0; i < board.balls.size(); i++) {
                if (board.balls[i].isStuck) {
                    board.balls[i].isStuck = false;
                    board.balls[i].speed = ballSpeed[i];
                    board.balls[i].dirX = ballDirX[i];
                    board.balls[i].dirY = -1;
                }
            }
            if (board.balls.size() == 2) {
                float dx = board.balls[0].x - board.balls[1].x;
                float dy = board.balls[0].y - board.balls[1].y;
                float dist = sqrt(dx * dx + dy * dy);
                if (dist <= 2 * BALL_RADIUS) {
                    board.balls[0].x += BALL_RADIUS;
                    board.balls[1].x -= BALL_RADIUS;
                }
            }
        }

        //закрытие окна
        sf::Event event = sf::Event();
        while (board.window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                board.window.close();
            }
        }

        //обновление игры
        Update(0.01);
        Display();
        CheckGameOver();
    }
}