#include "system.h"

using namespace		gui;

					system::renderer::renderer(const spaces_type &spaces) :
					spaces(spaces)
{}

void 				system::renderer::render() const
{
	for (const auto &space : spaces)
		space->render();
}