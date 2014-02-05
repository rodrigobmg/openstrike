/*
 * Copyright (C) 2013-2014 Dmitry Marakasov
 *
 * This file is part of openstrike.
 *
 * openstrike is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * openstrike is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with openstrike.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <limits>

#include <SDL2/SDL.h>

#include <SDL2pp/SDL2pp.hh>

#include <dat/datfile.hh>
#include <dat/datgraphics.hh>
#include <graphics/spritemanager.hh>

#include <graphics/renderer.hh>
#include <game/game.hh>
#include <gameobjects/heli.hh>

void usage(const char* progname) {
	std::cerr << "Usage: " << progname << " <filename.dat>" << std::endl;
}

int realmain(int argc, char** argv) {
	if (argc != 2) {
		usage(argv[0]);
		return 1;
	}

	// Data file
	DatFile datfile(argv[1]);

	// SDL stuff
	SDL2pp::SDL sdl(SDL_INIT_VIDEO);
	SDL2pp::Window window("OpenStrike", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_RESIZABLE);
	SDL2pp::Renderer renderer(window, -1, SDL_RENDERER_ACCELERATED);

	renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
	renderer.SetLogicalSize(640, 480);

	// Game stuff
	SpriteManager spriteman(renderer);

	Game game;
	Renderer game_renderer(renderer, datfile, spriteman);

	Heli* heli = game.Spawn<Heli>();

	unsigned int delta_ms, prev_ms, this_ms = SDL_GetTicks();
	while (1) {
		// Process events
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				return 0;
			} else if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_q)
					return 0;
				if (event.key.keysym.sym == SDLK_LEFT)
					heli->Left();
				if (event.key.keysym.sym == SDLK_RIGHT)
					heli->Right();
			}
		}

		// Update
		prev_ms = this_ms;
		this_ms = SDL_GetTicks();
		delta_ms = (prev_ms <= this_ms) ? (this_ms - prev_ms) : (std::numeric_limits<unsigned int>::max() - prev_ms + this_ms);

		game.Update(delta_ms);

		// Render
		renderer.SetDrawColor(0, 0, 0);
		renderer.Clear();

		renderer.SetDrawColor(32, 64, 0);
		renderer.FillRect(SDL2pp::Rect(0, 0, 640, 480));

		game.Accept(game_renderer);

		renderer.Present();

		// Frame limiter
		SDL_Delay(1);
	}

	return 0;
}

int main(int argc, char** argv) {
	try {
		return realmain(argc, argv);
	} catch (SDL2pp::Exception& e) {
		std::cerr << "Error: " << e.what() << " (" << e.GetSDLError() << ")" << std::endl;
	} catch (std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}

	return 1;
}
