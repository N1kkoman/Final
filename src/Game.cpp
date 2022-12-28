#include "Game.hpp"
#include "Random.hpp"
#include <SFML\Audio.hpp>
#include <iostream>

namespace gSn
{
	void Game::Init()
	{
		// задаем количество ячеек
		{
			__int64 x = 20;
			__int64 y = 15;
			field_size.x = x;
			field_size.y = y;
		}

		//загрузка всех данных
		{
			cell = new sf::Image;
			if (!cell->loadFromFile("data/cell.png"))
				exit(0);;
			border = new sf::Image;
			if (!border->loadFromFile("data/border.png"))
				exit(0);

			texture = new sf::Texture;
			sprite = new sf::Sprite;
		}
		{
			head = new sf::Image;
			if (!head->loadFromFile("data/head.png"))
				exit(0);

			body = new sf::Image;
			if (!body->loadFromFile("data/body.png"))
				exit(0);

			apple = new sf::Image;
			if (!apple->loadFromFile("data/apple.png"))
				exit(0);

			musicGame = new sf::Music;
			if (!musicGame->openFromFile("data/musicGame.ogg"))
				exit(0);

			musicEnding = new sf::Music;
			if (!musicEnding->openFromFile("data/musicEnding.ogg"))
				exit(0);

		}

		//создание окна игры 
		{
			window = new sf::RenderWindow(
				sf::VideoMode((unsigned)(cell->getSize().x * (field_size.x + 2)), (unsigned)(cell->getSize().y * (field_size.y + 2 + 1))),
				L"Snake"
			);
		}

		//создание змейки и её инициализация
		{
			snake = new gSn::Snake;
			snake->Init(field_size);
		}

		{
			direction_updated = false;
		}

		//добавление яблока
		{
			AddApple();
			last_step_on_apple = false;
		}

		//подгрузка шрифта и создание счётчика очков
		{
			font = new sf::Font;
			if (!font->loadFromFile("data/font.ttf"))
				exit(0);
			text = new sf::Text;
			text->setFont(*font);
			text->setCharacterSize(cell->getSize().y);
			text->setPosition(0.f, (float)(cell->getSize().y * (field_size.y + 2)));

			score = 0;
		}

		//подгрузка изображения о конце игры
		{
			game_over = new sf::Image;
			if (!game_over->loadFromFile("data/game_over.png"))
				exit(0);
			game_over_t = new sf::Texture;
			game_over_s = new sf::Sprite;

			game_over_t->loadFromImage(*game_over);
			game_over_s->setTexture(*game_over_t);
		}
	}

	// отрисовка всех объектов
	void Game::Draw()
	{
		window->clear();
		DrawField();
		snake->Draw(window, head, body);
		DrawApple();
		DrawScore();

		DrawGameOver();
		window->display();
	}

	// отрисовка поля игры, по краям окна находятся стенки
	void Game::DrawField()
	{
		for (__int64 x = 0; x < field_size.x + 2; ++x)
			for (__int64 y = 0; y < field_size.y + 2; ++y)
			{
				sf::Image* image = nullptr;
				if (x == 0 || x == field_size.x + 1 || y == 0 || y == field_size.y + 1)
					image = border;
				else
					image = cell;

				texture->loadFromImage(*image);
				sprite->setTexture(*texture);
				sprite->setPosition((float)(x * cell->getSize().x), (float)(y * cell->getSize().y));
				window->draw(*sprite);
			}
	}

	// отрисовка яблока
	void Game::DrawApple()
	{
		texture->loadFromImage(*apple);
		sprite->setTexture(*texture);
		sprite->setPosition(
			(float)(apple->getSize().x * (apple_pos.x + 1)),
			(float)(apple->getSize().y * (apple_pos.y + 1))
		);
		window->draw(*sprite);
	}

	// отрисовка счёта игры
	void Game::DrawScore()
	{
		char s[100];
		sprintf_s(s, "Score %lld", score);
		text->setString(s);
		window->draw(*text);
	}

	// отрисовка изображения об конце игры
	void Game::DrawGameOver()
	{

		if (!snake->GameOver(field_size))
			return;
		musicGame->stop();

		game_over_s->setPosition(
			(float)(__int64)((field_size.x + 2) / 2.f * cell->getSize().x - game_over->getSize().x / 2.f),
			(float)(__int64)((field_size.y + 2) / 2.f * cell->getSize().y - game_over->getSize().y / 2.f)
		);

		window->draw(*game_over_s);

	}

	// функция обновления шага (отлична от функции змейки), добавляет очко к текущему счёту и добавляет новое яблоко
	void Game::UpdateStep()
	{
		snake->UpdateStep(last_step_on_apple, field_size);
		if (last_step_on_apple = apple_pos == snake->GetHeadPosition())
		{
			AddApple();
			++score;
		}
	}

	// функция изменения направления змейки с помощью нажатия WASD или стрелочек
	void Game::UpdateKeyboard()
	{
		if (!window->hasFocus())
			return;
		if (direction_updated)
			return;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
		{
			snake->UpdateDirection(gSn::Snake::Direction::LEFT);
			direction_updated = true;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
		{
			snake->UpdateDirection(gSn::Snake::Direction::DOWN);
			direction_updated = true;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
		{
			snake->UpdateDirection(gSn::Snake::Direction::RIGHT);
			direction_updated = true;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
		{
			snake->UpdateDirection(gSn::Snake::Direction::UP);
			direction_updated = true;
		}
	}

	// функция добаления яблока, ищем свободные ячейки и случайно добавляем яблоко
	void Game::AddApple()
	{
		// проверка на то, занимает ли змейка всё поле
		if (snake->GetLength() == field_size.x * field_size.y)
		{
			std::cout << "There are no free cells. Game over!";
			exit(0);
		}


		std::vector<sf::Vector2<__int64>> free_cells; // вектор свободных ячеек
		auto busy_cells = snake->GetCells();

		// поиск свободных ячеек
		for (__int64 x = 0; x < field_size.x; ++x)
			for (__int64 y = 0; y < field_size.y; ++y)
			{
				bool is_busy = false;

				for (auto it = busy_cells.cbegin(); !is_busy && it != busy_cells.cend(); ++it)
					is_busy = *it == sf::Vector2<__int64>(x, y);

				if (!is_busy)
					free_cells.push_back(sf::Vector2<__int64>(x, y));
			}

		if (free_cells.empty())
		{
			std::cout << "There are no free cells. Game over!";
			exit(0);
		}

		apple_pos = free_cells[GetRandomNumber() % free_cells.size()];
	}

	// функция запуска игры
	void Game::Run()
	{
		Init();          // инициализация
		musicGame->play();
		musicGame->setLoop(true);// зацикливание музыки

		sf::Clock clk;                  // запуск таймера для создания движения
		float timer = 0, delay = 0.1;

		while (window->isOpen())
		{

			float time = clk.getElapsedTime().asSeconds();
			clk.restart();
			timer += time;
			sf::Event event;
			while (window->pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
				{
					musicEnding->stop();
					window->close();
				}

			}

			// выход из игры по нажатию Escape
			if (window->hasFocus() && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
				window->close();

			if (!snake->GameOver(field_size))
			{
				UpdateKeyboard();

				if (timer >= delay)
				{
					timer = 0;
					UpdateStep();
					direction_updated = false;
				}
				if (snake->GameOver(field_size))
				{
					musicEnding->play();
				}
			}

			Draw();


		}
	}

	// диструктор
	Game::~Game()
	{
		delete cell;
		delete border;
		delete texture;
		delete sprite;
		delete window;
		delete head;
		delete body;

		delete apple;
		delete font;
		delete text;

		delete game_over;
		delete game_over_t;
		delete game_over_s;

		delete musicGame;
		delete musicEnding;
	}
}