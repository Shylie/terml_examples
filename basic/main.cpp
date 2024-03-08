#include <terml.h>

#include <cstdlib>
#include <cstdio>

struct state
{
	unsigned int x = 0;
	unsigned int y = 0;
	unsigned int tick = 0;
};
static state st;

static void randomize_board()
{
	const unsigned int width = terml_get_width();
	const unsigned int height = terml_get_height();
	for (int i = 0; i < width * height; i++)
	{
		terml_set(i % width, i / width, { ' ', (rand() * rand()) & (rand() * rand()), (rand() * rand()) & (rand() * rand()) });
	}

	terml_set(st.x, st.y, { 0x256C, 0xFFFFFF, 0 });

	terml_flush();
}

static void main_callback()
{
	if (st.tick++ % 10 == 0)
	{
		randomize_board();
	}
}

static void key_callback(char code)
{
	if (code == '\x1B')
	{
		terml_stop();
	}
	else
	{
		bool moved = false;

		if (code == 'a' && st.x > 0)
		{
			terml_set(st.x, st.y, { ' ', (rand() * rand()) & (rand() * rand()), (rand() * rand()) & (rand() * rand()) });
			st.x--;
			moved = true;
		}
			
		if (code == 'd' && st.x < terml_get_width() - 1)
		{
			terml_set(st.x, st.y, { ' ', (rand() * rand()) & (rand() * rand()), (rand() * rand()) & (rand() * rand()) });
			st.x++;
			moved = true;
		}

		if (code == 'w' && st.y > 0)
		{
			terml_set(st.x, st.y, { ' ', (rand() * rand()) & (rand() * rand()), (rand() * rand()) & (rand() * rand()) });
			st.y--;
			moved = true;
		}

		if (code == 's' && st.y < terml_get_height() - 1)
		{
			terml_set(st.x, st.y, { ' ', (rand() * rand()) & (rand() * rand()), (rand() * rand()) & (rand() * rand()) });
			st.y++;
			moved = true;
		}

		if (moved)
		{
			terml_set(st.x, st.y, { 0x256C, 0xFFFFFF, 0 });
			terml_flush();
		}
	}
}

int main(int argc, char** argv)
{
	terml_init();

	terml_set_key_callback(key_callback);
	terml_set_main_callback(main_callback);

	randomize_board();

	terml_start();

	terml_deinit();

	const char* err = terml_get_error();
	if (err)
	{
		printf("%s\n", err);
	}

	return 0;
}