/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_button.h"
#include "ga_label.h"
#include "math/ga_vec2f.h"

#include "framework/ga_frame_params.h"

ga_button::ga_button(const char* text, float x, float y, ga_frame_params* params)
{
	// start by writing the text, so we know how big it is
	ga_label my_label = ga_label(text, x + buffer, y - buffer, params);

	width = 20;
	height = 20;

	left_x = x;
	upper_y = y - height;
	right_x = x + width;
	lower_y = y;


	// recolor the rectangle based on the mouse state
	ga_vec3f col = ga_vec3f();
	ga_vec3f interior_col = ga_vec3f();

	if (get_held(params)) {
		col.x = 0.5;
		col.y = 0.25;
		col.z = 0;
	}
	else if (get_hover(params)) {
		col.x = 1;
		col.y = 1;
		col.z = 1;
	}
	else {
		col.x = 1;
		col.y = 0.5;
		col.z = 0;
	}

	interior_col.x = 0.25;
	interior_col.y = 0;
	interior_col.z = 0;


	// draw the rectangle
	ga_dynamic_drawcall drawcall;

	drawcall._positions.push_back({ left_x, upper_y, 0.0f });
	drawcall._positions.push_back({ right_x, upper_y, 0.0f });
	drawcall._positions.push_back({ right_x, lower_y, 0.0f });
	drawcall._positions.push_back({ left_x, lower_y, 0.0f });

	drawcall._indices.push_back(0);
	drawcall._indices.push_back(1);
	drawcall._indices.push_back(1);
	drawcall._indices.push_back(2);
	drawcall._indices.push_back(2);
	drawcall._indices.push_back(3);
	drawcall._indices.push_back(3);
	drawcall._indices.push_back(0);

	drawcall._color = col;
	drawcall._draw_mode = GL_LINES;
	drawcall._transform.make_identity();
	drawcall._material = nullptr;

	while (params->_gui_drawcall_lock.test_and_set(std::memory_order_acquire)) {}
	params->_gui_drawcalls.push_back(drawcall);
	params->_gui_drawcall_lock.clear(std::memory_order_release);


	// draw the interior rectangle
	if (get_hover(params)) {
		ga_dynamic_drawcall interior_drawcall;

		interior_drawcall._positions.push_back({ left_x, upper_y, 0.0f });
		interior_drawcall._positions.push_back({ right_x, upper_y, 0.0f });
		interior_drawcall._positions.push_back({ right_x, lower_y, 0.0f });
		interior_drawcall._positions.push_back({ left_x, lower_y, 0.0f });

		interior_drawcall._indices.push_back(2);
		interior_drawcall._indices.push_back(1);
		interior_drawcall._indices.push_back(0);

		interior_drawcall._indices.push_back(0);
		interior_drawcall._indices.push_back(3);
		interior_drawcall._indices.push_back(2);


		interior_drawcall._color = interior_col;
		interior_drawcall._draw_mode = GL_TRIANGLES;
		interior_drawcall._transform.make_identity();
		interior_drawcall._material = nullptr;

		while (params->_gui_drawcall_lock.test_and_set(std::memory_order_acquire)) {}
		params->_gui_drawcalls.push_back(interior_drawcall);
		params->_gui_drawcall_lock.clear(std::memory_order_release);


		// now we have to draw the text again, since we painted over it. it'd be more efficient to just draw the interior rectangle first,
		// but we need the text first so we know the dimensions of the button.
		
		ga_label(text, x + buffer, y - buffer, params);
	}
}

ga_button::~ga_button()
{
	// no memory needs to be freed
}

// did the player just release the mouse?
bool ga_button::get_clicked(const ga_frame_params* params) const
{
	bool click_in_region =
		params->_mouse_click_mask != 0 &&
		params->_mouse_x >= left_x &&
		params->_mouse_y >= upper_y &&
		params->_mouse_x <= right_x &&
		params->_mouse_y <= lower_y;

	return click_in_region;
}

// is the player hovering over the box?
bool ga_button::get_hover(const ga_frame_params* params) const
{
	bool mouse_in_region =
		params->_mouse_x >= left_x &&
		params->_mouse_y >= upper_y &&
		params->_mouse_x <= right_x &&
		params->_mouse_y <= lower_y;

	return mouse_in_region;
}

// does the player have the mouse held on the box?
bool ga_button::get_held(const ga_frame_params* params) const
{
	bool mouse_in_region =
		params->_mouse_press_mask != 0 &&
		params->_mouse_x >= left_x &&
		params->_mouse_y >= upper_y &&
		params->_mouse_x <= right_x &&
		params->_mouse_y <= lower_y;

	return mouse_in_region;
}