#include <terml.h>

#include <cstdlib>

class TileBehaviour
{
public:
	virtual ~TileBehaviour() = default;
	virtual bool blocks_vision() const = 0;
	virtual int pathing_cost() const = 0;
	virtual char representation() const = 0;
};

class FloorBehaviour : public TileBehaviour
{
public:
	virtual bool blocks_vision() const override { return false; }
	virtual int pathing_cost() const override { return 0; }
	virtual char representation() const override { return ' '; }
};

class WallBehaviour : public TileBehaviour
{
public:
	virtual bool blocks_vision() const override { return true; }
	virtual int pathing_cost() const override { return -1; }
	virtual char representation() const override { return '#'; }
};

class Tile
{
public:
	Tile(const TileBehaviour* behaviour) :
		m_behaviour(behaviour)
	{ }

	virtual ~Tile() = default;

	bool blocks_vision() const { return m_behaviour->blocks_vision(); }
	int pathing_cost() const { return m_behaviour->pathing_cost(); }
	void draw(unsigned int x, unsigned int y) const
	{
		terml_set(x, y, m_behaviour->representation(), 0, 0xFFFFFF);
	}

private:
	const TileBehaviour* const m_behaviour;
};

class Map
{
public:
	Map(unsigned int width, unsigned int height) :
		m_width(width),
		m_height(height),
		m_tiles(new Tile* [width * height] {})
	{
		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				if (rand() % 10 == 0)
				{
					set_tile(x, y, new Tile(get_wall()));
				}
				else
				{
					set_tile(x, y, new Tile(get_floor()));
				}
			}
		}
	}

	Map(const Map&) = delete;
	Map(Map&&) = delete;
	Map& operator=(const Map&) = delete;
	Map& operator=(Map&&) = delete;

	virtual ~Map()
	{
		for (int i = 0; i < m_width * m_height; i++)
		{
			delete m_tiles[i];
		}

		delete[] m_tiles;
	}

	const Tile* get_tile(unsigned int x, unsigned int y) const
	{
		if (x < m_width && y < m_height)
		{
			return m_tiles[x + y * m_width];
		}
		else
		{
			return nullptr;
		}
	}

	Tile* get_tile(unsigned int x, unsigned int y)
	{
		if (x < m_width && y < m_height)
		{
			return m_tiles[x + y * m_width];
		}
		else
		{
			return nullptr;
		}
	}

	void set_tile(unsigned int x, unsigned int y, Tile* tile)
	{
		if (x < m_width && y < m_height)
		{
			const unsigned int idx = x + y * m_width;
			if (m_tiles[idx])
			{
				delete m_tiles[idx];
			}
			m_tiles[idx] = tile;
		}
	}

	void draw(int ox, int oy) const
	{
		const unsigned int tw = terml_get_width();
		const unsigned int th = terml_get_height();

		for (unsigned int x = 0; x < tw; x++)
		{
			for (unsigned int y = 0; y < th; y++)
			{
				const Tile* tile = get_tile(x + ox, y + oy);
				if (tile)
				{
					tile->draw(x, y);
				}
				else
				{
					terml_set(x, y, ' ', 0, 0);
				}
			}
		}
	}

protected:
	const FloorBehaviour* get_floor() const { return &f_floor; }
	const WallBehaviour* get_wall() const { return &f_wall; }

private:
	unsigned int m_width;
	unsigned int m_height;
	Tile** m_tiles;

	FloorBehaviour f_floor;
	WallBehaviour f_wall;
};

static int px = 0;
static int py = 0;
static Map map(120, 70);

static void draw()
{
	map.draw(px - terml_get_width() / 2, py - terml_get_height() / 2);
	terml_set(terml_get_width() / 2, terml_get_height() / 2, '@', 0, 0xFFFFFF);

	terml_flush();
}

static void resize_cb(unsigned int, unsigned int, unsigned int, unsigned int)
{
	draw();
}

static void key_cb(char c)
{
	const Tile* tile;
	switch (c)
	{
	case '\x1B':
		terml_stop();
		break;

	case 'w':
		if ((tile = map.get_tile(px, py - 1)) && tile->pathing_cost() >= 0)
		{
			py -= 1;
		}
		break;

	case 'a':
		if ((tile = map.get_tile(px - 1, py)) && tile->pathing_cost() >= 0)
		{
			px -= 1;
		}
		break;

	case 's':
		if ((tile = map.get_tile(px, py + 1)) && tile->pathing_cost() >= 0)
		{
			py += 1;
		}
		break;

	case 'd':
		if ((tile = map.get_tile(px + 1, py)) && tile->pathing_cost() >= 0)
		{
			px += 1;
		}
		break;
	}

	draw();
}

int main(int argc, char** argv)
{
	terml_init();
	terml_set_key_callback(key_cb);
	terml_set_resize_callback(resize_cb);

	draw();
	terml_start();

	terml_deinit();
}