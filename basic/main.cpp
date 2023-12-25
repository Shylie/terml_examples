#include <terml.h>

#include <cstdlib>
#include <cstdio>

struct state
{
	unsigned int x = 0;
	unsigned int y = 0;
	unsigned int tick = 0;
} st;

void randomize_board()
{
	const unsigned int width = terml_get_width();
	const unsigned int height = terml_get_height();
	for (int i = 0; i < width * height; i++)
	{
		terml_set(i % width, i / width, ' ', (rand() * rand()) & (rand() * rand()), (rand() * rand()) & (rand() * rand()));
	}

	terml_set(st.x, st.y, ' ', 0xFFFFFF, 0xFFFFFF);

	terml_flush();
}

static void main_callback()
{
	if (st.tick++ % 1000 == 0)
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

		terml_set(st.x, st.y, ' ', (rand() * rand()) & (rand() * rand()), (rand() * rand()) & (rand() * rand()));
		if (code == 'a' && st.x > 0)
		{
			st.x--;
			moved = true;
		}
			
		if (code == 'd' && st.x < terml_get_width() - 1)
		{
			st.x++;
			moved = true;
		}

		if (code == 'w' && st.y > 0)
		{
			st.y--;
			moved = true;
		}

		if (code == 's' && st.y < terml_get_height() - 1)
		{
			st.y++;
			moved = true;
		}
		terml_set(st.x, st.y, ' ', 0xFFFFFF, 0xFFFFFF);

		if (moved)
		{
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

	const char* err = terml_get_error();

	terml_deinit();
	
	if (err)
	{
		printf("%s\n", err);
	}

	return 0;
}