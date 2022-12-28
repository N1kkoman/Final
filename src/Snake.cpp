#include "Snake.hpp"

namespace gSn
{
	// инициализация змейки и установление её положения в середине экрана
	void Snake::Init(sf::Vector2<__int64> field_size)
	{
		cells.clear();
		head_pos.x = field_size.x / 2; // размещаем голову змейки в центре
		head_pos.y = field_size.y / 2;
		direction = Direction::UP; // указываем изначальное направление змейки

		cells.push_back(head_pos);

		texture = new sf::Texture;
		sprite = new sf::Sprite;
	}

	// отрисовка тела и головы змейки, к элементам тела (т.е. списка) обращаемся по итератору
	void Snake::Draw(sf::RenderWindow* window, sf::Image* head, sf::Image* body) const
	{
		texture->loadFromImage(*body);
		sprite->setTexture(*texture);
		for (auto it = cells.cbegin(); it != cells.cend(); ++it)
		{
			if (it != cells.cbegin()) // не отрисовываем голову второй раз
			{
				sprite->setPosition(
					(float)((it->x + 1) * body->getSize().x),
					(float)((it->y + 1) * body->getSize().y)
				);
				window->draw(*sprite);
			}
		}

		texture->loadFromImage(*head); // голову рисуем после тела, чтобы увидеть её положение в конце игры
		sprite->setTexture(*texture);
		sprite->setPosition(
			(float)((head_pos.x + 1) * head->getSize().x),
			(float)((head_pos.y + 1) * head->getSize().y)
		);
		window->draw(*sprite);
	}

	// реализация движения змейки и изменения направления
	void Snake::UpdateStep(bool head_on_apple, sf::Vector2<__int64> field_size)
	{
		if (GameOver(field_size))
			return;

		switch (direction)
		{
		case Snake::Direction::UP:
			--head_pos.y;
			break;
		case Snake::Direction::DOWN:
			++head_pos.y;
			break;
		case Snake::Direction::RIGHT:
			++head_pos.x;
			break;
		case Snake::Direction::LEFT:
			--head_pos.x;
			break;
		}

		auto last_cell = cells.back(); // запоминаем последнюю клетку тела

		for (auto it = cells.rbegin(); it != cells.rend(); ++it) // проходим по телу с конца и сдвигаем клетки тела по направлению головы
		{
			auto jt = it;
			++jt;
			if (jt != cells.rend())
				*it = *jt; // присваеваем предыдущей клетке значение текущей
		}

		cells.pop_front();

		if (head_on_apple) // если съели яблоко, то добавляем в конец last_cell
			cells.push_back(last_cell);
		cells.push_front(head_pos); // добавляем новое положение головы
	}

	// проверка на конец игры: либо голова врезается в тело, либо голова врезается в стенку
	bool Snake::GameOver(sf::Vector2<__int64> field_size) const
	{
		if (head_pos.x < 0 || head_pos.x >= field_size.x || head_pos.y < 0 || head_pos.y >= field_size.y)
			return true;

		for (auto it = cells.cbegin(); it != cells.cend(); ++it)
		{
			if (it != cells.cbegin() && *it == head_pos) // проходимся по всему телу и проверяем, находится ли голова в теле
				return true;
		}
		return false;
	}

	// изменение направления движения змейки: ничего не делает, если змейка состоит только из головы или игрок пытается повернуть змейку "в себя"
	void Snake::UpdateDirection(Direction new_direction)
	{
		if (GetLength() == 1) // в начале игры можно ходить в любом направлении
		{
			direction = new_direction;
			return;
		}

		if (
			direction == Direction::UP && new_direction == Direction::DOWN ||
			direction == Direction::DOWN && new_direction == Direction::UP ||
			direction == Direction::RIGHT && new_direction == Direction::LEFT ||
			direction == Direction::LEFT && new_direction == Direction::RIGHT
			)
			return;

		direction = new_direction;
	}

	// возвращает длину змейки
	__int64 Snake::GetLength() const
	{
		return (__int64)cells.size();
	}

	// возвращает список ячеек
	std::list<sf::Vector2<__int64>> Snake::GetCells() const
	{
		return cells;
	}

	// возвращает позицию головы
	sf::Vector2<__int64> Snake::GetHeadPosition() const
	{
		return head_pos;
	}

	// диструктор
	Snake::~Snake()
	{
		delete texture;
		delete sprite;
	}
}