#include <terml.h>

#include <cstdlib>

class TileBehaviour
{
public:
	virtual ~TileBehaviour() = default;
	virtual bool blocks_vision() const = 0;
	virtual int pathing_cost() const = 0;
	virtual unsigned int representation() const = 0;
};

class FloorBehaviour : public TileBehaviour
{
public:
	virtual bool blocks_vision() const override { return false; }
	virtual int pathing_cost() const override { return 0; }
	virtual unsigned int representation() const override { return ' '; }
};
static FloorBehaviour floor_behaviour;

class WallBehaviour : public TileBehaviour
{
public:
	virtual bool blocks_vision() const override { return true; }
	virtual int pathing_cost() const override { return -1; }
	virtual unsigned int representation() const override { return '#'; }
};
static WallBehaviour wall_behaviour;

class Tile
{
public:
	Tile(const TileBehaviour& behaviour) :
		m_behaviour(behaviour)
	{ }

	virtual ~Tile() = default;

	bool blocks_vision() const { return m_behaviour.blocks_vision(); }
	int pathing_cost() const { return m_behaviour.pathing_cost(); }
	void draw(unsigned int x, unsigned int y) const
	{
		terml_set(x, y, { m_behaviour.representation(), 0, 0xFFFFFF });
	}

private:
	const TileBehaviour& m_behaviour;
};

class Map;
class MapGenerator
{
public:
	virtual ~MapGenerator() = default;

	void generate(Map* map)
	{
		m_current_map = map;
		generate_impl();
	}

protected:
	virtual void generate_impl() = 0;

	Map* current_map() const
	{
		return m_current_map;
	}

private:
	Map* m_current_map;
};

class Map
{
public:
	Map(unsigned int width, unsigned int height, MapGenerator& generator) :
		m_width(width),
		m_height(height),
		m_tiles(new Tile*[width * height]{})
	{
		generator.generate(this);
	}

	Map(const Map&) = delete;
	Map(Map&&) = delete;
	Map& operator=(const Map&) = delete;
	Map& operator=(Map&&) = delete;

	~Map()
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
					terml_set(x, y, { ' ', 0, 0 });
				}
			}
		}
	}

	unsigned int get_width() const { return m_width; }
	unsigned int get_height() const { return m_height; }

private:
	unsigned int m_width;
	unsigned int m_height;
	Tile** m_tiles;
};

template <unsigned int ROOM_COUNT>
class BasicMapGenerator : public MapGenerator
{
protected:
	struct room
	{
		unsigned int x, y, w, h;
	} rooms[ROOM_COUNT];

	virtual void generate_impl() override
	{
		for (int i = 0; i < current_map()->get_width() * current_map()->get_height(); i++)
		{
			current_map()->set_tile(i % current_map()->get_width(), i / current_map()->get_width(), new Tile(floor_behaviour));
		}

		for (int i = 0; i < ROOM_COUNT; i++)
		{
			rooms[i] = generate_room();
		}
	}

	room generate_room() const
	{
		const unsigned int x = rand() % current_map()->get_width();
		const unsigned int y = rand() % current_map()->get_height();
		const int w = rand() % (2 * current_map()->get_width() / ROOM_COUNT) + 1;
		const int h = rand() % (2 * current_map()->get_height() / ROOM_COUNT) + 1;

		for (int ox = -w; ox <= w; ox++)
		{
			for (int oy = -h; oy <= h; oy++)
			{
				current_map()->set_tile(x + ox, y + oy, new Tile(wall_behaviour));
			}
		}

		return { x, y, static_cast<unsigned int>(w), static_cast<unsigned int>(h) };
	}
};

static constexpr unsigned int MAP_WIDTH = 100;
static constexpr unsigned int MAP_HEIGHT = 100;
static constexpr unsigned int TILES_PER_ROOM = 200;
static constexpr unsigned int NUM_ROOMS = (MAP_WIDTH * MAP_HEIGHT) / TILES_PER_ROOM;

static BasicMapGenerator<NUM_ROOMS> room_generator;

static int px = 0;
static int py = 0;
static Map map(MAP_WIDTH, MAP_HEIGHT, room_generator);

static void draw()
{
	map.draw(px - terml_get_width() / 2, py - terml_get_height() / 2);
	terml_set(terml_get_width() / 2, terml_get_height() / 2, { '@', 0, 0xFFFFFF });

	terml_flush();
}

static void resize_cb(unsigned int, unsigned int)
{
	draw();
}

static void key_cb(unsigned int c)
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