#include <SDL.h>

#include "clparser/CommandLineParser.hpp"
#include "clparser/CommandLineOptions.hpp"
#include "clparser/Required.hpp"
#include "clparser/Optional.hpp"

class Opt : public clparser::CommandLineOptions
{
public:
	Opt() :
		req(*this, "f", "file", "mydesc"),
		opt(*this, "n", "number", "mydesc2", 10) {}

private:
	clparser::Required<std::string> req;
	clparser::Optional<int> opt;
};

int main(int argc, char* argv[])
{
	clparser::CommandLineParser parser(argc, argv);
	Opt opt = parser.parse<Opt>();

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = SDL_CreateWindow(
		"SDL2Test",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		640,
		480,
		0
	);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	SDL_SetRenderDrawColor(renderer, 200, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

	SDL_Delay(3000);

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
