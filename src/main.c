#include <stdio.h>
#include <stdlib.h>

#include <include/game.h>

int main(void)
{
	stGame *game = game_init();
	int quit = 0;

	while(quit != -1) {
		game_draw(game);

		quit = game_event(game);
		SDL_Delay(20);
		//printf("%d\n", quit);
	}

	game_destroy(game);
	return 0;
}