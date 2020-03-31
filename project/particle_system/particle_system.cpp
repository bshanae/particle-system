#include "particle_system.h"

					particle_system::particle_system(engine::core &engine, computer::core &computer) :
					engine(engine),
					computer(computer)
{
	initialize_engine();
	initialize_computer();
	
	arguments.position.acquire();
	kernels.reset.run();
	kernels.update.run();
	arguments.position.release();
}

void				particle_system::initialize_engine()
{
	engine.attach_renderer(particle_renderer);

	engine.generate_timer(1.f / 60.f, &particle_system::timer, this);

	engine.generate_callback(engine::event::type::key_press, &particle_system::callback, this);
	engine.generate_callback(engine::event::type::key_hold, &particle_system::callback, this);
}

void				particle_system::initialize_computer()
{
	kernels.reset = computer.generate_kernel();
	kernels.reset.add_source("project/resources/OpenCL/vector.txt");
	kernels.reset.add_source("project/resources/OpenCL/reset.txt");
	kernels.reset.build("reset", number_of_particles);

	kernels.update = computer.generate_kernel();
	kernels.update.add_source("project/resources/OpenCL/vector.txt");
	kernels.update.add_source("project/resources/OpenCL/update.txt");
	kernels.update.build("update", number_of_particles);

	kernels.physics = computer.generate_kernel();
	kernels.physics.add_source("project/resources/OpenCL/macros.txt");
	kernels.physics.add_source("project/resources/OpenCL/vector.txt");
	kernels.physics.add_source("project/resources/OpenCL/physics.txt");
	kernels.physics.build("physics", number_of_particles);

	arguments.position = kernels.reset.generate_argument(particle_renderer.buffer.receive_attribute(0));
	arguments.velocity = kernels.reset.generate_argument<float>(number_of_particles * 3);
	arguments.acceleration = kernels.reset.generate_argument<float>(number_of_particles * 3);

	kernels.reset.link_argument(arguments.position);
	kernels.reset.link_argument(arguments.velocity);
	kernels.reset.link_argument(arguments.acceleration);

	kernels.update.link_argument(arguments.position);
	kernels.update.link_argument(arguments.velocity);
	kernels.update.link_argument(arguments.acceleration);

	kernels.physics.link_argument(arguments.position);
	kernels.physics.link_argument(arguments.velocity);
	kernels.physics.link_argument(arguments.acceleration);
}

void 				particle_system::timer()
{
	arguments.position.acquire();
	kernels.physics.run();
	kernels.update.run();
	arguments.position.release();
	engine::core::should_render = true;
}

void 				particle_system::callback()
{
	engine::event	&event = engine.receive_event();

	if (event.read_key() == GLFW_KEY_ESCAPE)
		engine.finish();
	else if (event.read_key() == GLFW_KEY_A)
		particle_renderer.camera.move(engine::axis::x, engine::sign::negative);
	else if (event.read_key() == GLFW_KEY_D)
		particle_renderer.camera.move(engine::axis::x, engine::sign::positive);
	else if (event.read_key() == GLFW_KEY_W)
		particle_renderer.camera.move(engine::axis::z, engine::sign::negative);
	else if (event.read_key() == GLFW_KEY_S)
		particle_renderer.camera.move(engine::axis::z, engine::sign::positive);
	else if (event.read_key() == GLFW_KEY_Q)
		particle_renderer.camera.move(engine::axis::y, engine::sign::positive);
	else if (event.read_key() == GLFW_KEY_E)
		particle_renderer.camera.move(engine::axis::y, engine::sign::negative);
	else if (event.read_key() == GLFW_KEY_LEFT)
		particle_renderer.camera.rotate(engine::axis::y, engine::sign::positive);
	else if (event.read_key() == GLFW_KEY_RIGHT)
		particle_renderer.camera.rotate(engine::axis::y, engine::sign::negative);
	else if (event.read_key() == GLFW_KEY_UP)
		particle_renderer.camera.rotate(engine::axis::x, engine::sign::positive);
	else if (event.read_key() == GLFW_KEY_DOWN)
		particle_renderer.camera.rotate(engine::axis::x, engine::sign::negative);
	else
		return ;
	engine::core::should_render = true;
}