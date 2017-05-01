/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "framework/ga_camera.h"
#include "framework/ga_compiler_defines.h"
#include "framework/ga_input.h"
#include "framework/ga_sim.h"
#include "framework/ga_output.h"
#include "jobs/ga_job.h"

#include "entity/ga_entity.h"
#include "entity/ga_lua_component.h"

#include "graphics/ga_cube_component.h"
#include "graphics/ga_trail_component.h"
#include "graphics/ga_wobbler_component.h"
#include "graphics/ga_program.h"

#include "gui/ga_font.h"

#include "gui/ga_button.h"
#include "gui/ga_checkbox.h"
#include "gui/ga_label.h"

#include <iomanip>
#include <string>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#if defined(GA_MINGW)
#include <unistd.h>
#endif

ga_font* g_font = nullptr;
static void trail_gui(ga_frame_params* params, trail_data* ui_data);
static void set_root_path(const char* exepath);


int main(int argc, const char** argv)
{
	set_root_path(argv[0]);

	ga_job::startup(0xffff, 256, 256);

	// Create objects for three phases of the frame: input, sim and output.
	ga_input* input = new ga_input();
	ga_sim* sim = new ga_sim();
	ga_output* output = new ga_output(input->get_window());

	// Create camera.
	ga_camera* camera = new ga_camera({ 0.0f, 7.0f, 20.0f });
	ga_quatf rotation;
	rotation.make_axis_angle(ga_vec3f::y_vector(), ga_degrees_to_radians(180.0f));
	camera->rotate(rotation);
	rotation.make_axis_angle(ga_vec3f::x_vector(), ga_degrees_to_radians(15.0f));
	camera->rotate(rotation);


	trail_data * ui_data = new trail_data;
	trail_data new_data;
	new_data.num_breaks = 25;
	new_data.update_rate = 1;
	new_data.start_width = 2;
	new_data.end_width = 0;
	*ui_data = new_data;


	// Create an entity whose movement is driven by Lua script.
	ga_entity lua;
	lua.translate({ 0.0f, 2.0f, 1.0f });
	ga_lua_component lua_move(&lua, "data/scripts/move.lua");
	ga_cube_component lua_model(&lua, "data/textures/rpi.png");
	ga_trail_component lua_trail(&lua, "data/textures/rpi.png", ui_data);
	ga_wobbler_component lua_wobbler(&lua);
	sim->add_entity(&lua);

	g_font = new ga_font("VeraMono.ttf", 16.0f, 512, 512);


	// Main loop:
	while (true)
	{
		// We pass frame state through the 3 phases using a params object.
		ga_frame_params params;

		// Gather user input and current time.
		if (!input->update(&params))
		{
			break;
		}

		// Update the camera.
		camera->update(&params);

		// Run gameplay.
		sim->update(&params);

		// Perform the late update.
		sim->late_update(&params);

		// Update gui.
		trail_gui(&params, ui_data);

		// Draw to screen.
		output->update(&params);
	}

	delete output;
	delete sim;
	delete input;
	delete camera;

	ga_job::shutdown();

	return 0;
}

static void trail_gui(ga_frame_params* params, trail_data* ui_data) {

	int num_breaks = ui_data->num_breaks;
	int update_rate = ui_data->update_rate;
	float start_width = ui_data->start_width;
	float end_width = ui_data->end_width;

	ga_label("LENGTH", 15.0f, 40.0f, params);
	if (num_breaks > 2) {
		if (ga_button("-", 120.0f, 40.0f, params).get_clicked(params)) {
			num_breaks--;
		}
	}
	if (ga_button("+", 142.0f, 40.0f, params).get_clicked(params)) {
		num_breaks++;
	}
	ga_label(std::to_string(num_breaks).c_str(), 170.0f, 40.0f, params);



	ga_label("UPDATE DELAY", 15.0f, 60.0f, params);
	if (update_rate > 1) {
		if (ga_button("-", 120.0f, 60.0f, params).get_clicked(params)) {
			update_rate--;
		}
	}
	if (ga_button("+", 142.0f, 60.0f, params).get_clicked(params)) {
		update_rate++;
	}
	ga_label(std::to_string(update_rate).c_str(), 170.0f, 60.0f, params);



	ga_label("START WIDTH", 15.0f, 80.0f, params);
	if (start_width >= 0.09) {
		if (ga_button("-", 120.0f, 80.0f, params).get_clicked(params)) {
			start_width -= 0.1;
		}
	}
	if (ga_button("+", 142.0f, 80.0f, params).get_clicked(params)) {
		start_width += 0.1;
	}
	std::stringstream ss1;
	ss1 << std::fixed << std::setprecision(1) << start_width;
	ga_label(ss1.str().c_str(), 170.0f, 80.0f, params);



	ga_label("END WIDTH", 15.0f, 100.0f, params);
	if (end_width >= 0.09) {
		if (ga_button("-", 120.0f, 100.0f, params).get_clicked(params)) {
			end_width -= 0.1;
		}
	}
	if (ga_button("+", 142.0f, 100.0f, params).get_clicked(params)) {
		end_width += 0.1;
	}
	std::stringstream ss2;
	ss2 << std::fixed << std::setprecision(1) << end_width;
	ga_label(ss2.str().c_str(), 170.0f, 100.0f, params);
	

	trail_data new_data;
	new_data.num_breaks = num_breaks;
	new_data.update_rate = update_rate;
	new_data.start_width = start_width;
	new_data.end_width = end_width;

	*ui_data = new_data;
}

char g_root_path[256];
static void set_root_path(const char* exepath)
{
#if defined(GA_MSVC)
	strcpy_s(g_root_path, sizeof(g_root_path), exepath);

	// Strip the executable file name off the end of the path:
	char* slash = strrchr(g_root_path, '\\');
	if (!slash)
	{
		slash = strrchr(g_root_path, '/');
	}
	if (slash)
	{
		slash[1] = '\0';
	}
#elif defined(GA_MINGW)
	char* cwd;
	char buf[PATH_MAX + 1];
	cwd = getcwd(buf, PATH_MAX + 1);
	strcpy_s(g_root_path, sizeof(g_root_path), cwd);

	g_root_path[strlen(cwd)] = '/';
	g_root_path[strlen(cwd) + 1] = '\0';
#endif
}
