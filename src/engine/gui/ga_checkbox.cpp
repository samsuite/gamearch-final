/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_checkbox.h"
#include "ga_label.h"

#include "framework/ga_frame_params.h"

ga_checkbox::ga_checkbox(bool state, const char* text, float x, float y, ga_frame_params* params)
{
	// set boundaries based on the size of the box
	left_x = x;
	upper_y = y - size;
	right_x = x + size;
	lower_y = y;

	// set color based on mouse state
	ga_vec3f box_col = ga_vec3f();
	ga_vec3f x_col = ga_vec3f();
	ga_vec3f interior_col = ga_vec3f();

	if (get_hover(params)) {
		box_col.x = 1;
		box_col.y = 1;
		box_col.z = 1;
	}
	else {
		box_col.x = 0;
		box_col.y = 1;
		box_col.z = 0;
	}

	if (get_held(params)) {
		x_col.x = 1;
		x_col.y = 1;
		x_col.z = 1;
	}
	else {
		x_col.x = 0;
		x_col.y = 1;
		x_col.z = 0;
	}

	interior_col.x = 0;
	interior_col.y = 0.25;
	interior_col.z = 0;


	// draw the box
	ga_dynamic_drawcall box_drawcall;
	box_drawcall._positions.push_back({ left_x, upper_y, 0.0f });
	box_drawcall._positions.push_back({ right_x, upper_y, 0.0f });
	box_drawcall._positions.push_back({ right_x, lower_y, 0.0f });
	box_drawcall._positions.push_back({ left_x, lower_y, 0.0f });

	box_drawcall._indices.push_back(0);
	box_drawcall._indices.push_back(1);
	box_drawcall._indices.push_back(1);
	box_drawcall._indices.push_back(2);
	box_drawcall._indices.push_back(2);
	box_drawcall._indices.push_back(3);
	box_drawcall._indices.push_back(3);
	box_drawcall._indices.push_back(0);

	box_drawcall._color = box_col;
	box_drawcall._draw_mode = GL_LINES;
	box_drawcall._transform.make_identity();
	box_drawcall._material = nullptr;

	while (params->_gui_drawcall_lock.test_and_set(std::memory_order_acquire)) {}
	params->_gui_drawcalls.push_back(box_drawcall);
	params->_gui_drawcall_lock.clear(std::memory_order_release);


	if (state) {
		// draw the X
		ga_dynamic_drawcall x_drawcall;
		x_drawcall._positions.push_back({ left_x, upper_y, 0.0f });
		x_drawcall._positions.push_back({ right_x, upper_y, 0.0f });
		x_drawcall._positions.push_back({ right_x, lower_y, 0.0f });
		x_drawcall._positions.push_back({ left_x, lower_y, 0.0f });

		x_drawcall._indices.push_back(0);
		x_drawcall._indices.push_back(2);
		x_drawcall._indices.push_back(1);
		x_drawcall._indices.push_back(3);

		x_drawcall._color = x_col;
		x_drawcall._draw_mode = GL_LINES;
		x_drawcall._transform.make_identity();
		x_drawcall._material = nullptr;

		while (params->_gui_drawcall_lock.test_and_set(std::memory_order_acquire)) {}
		params->_gui_drawcalls.push_back(x_drawcall);
		params->_gui_drawcall_lock.clear(std::memory_order_release);


		// draw the interior
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
	}

	// draw the label
	ga_label(text, x + size + 5, y, params);
}

ga_checkbox::~ga_checkbox()
{
	// no memory needs to be freed
}

// did the player just release the mouse?
bool ga_checkbox::get_clicked(const ga_frame_params* params) const
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
bool ga_checkbox::get_hover(const ga_frame_params* params) const
{
	bool mouse_in_region =
		params->_mouse_x >= left_x &&
		params->_mouse_y >= upper_y &&
		params->_mouse_x <= right_x &&
		params->_mouse_y <= lower_y;

	return mouse_in_region;
}

// does the player have the mouse held on the box?
bool ga_checkbox::get_held(const ga_frame_params* params) const
{
	bool mouse_in_region =
		params->_mouse_press_mask != 0 &&
		params->_mouse_x >= left_x &&
		params->_mouse_y >= upper_y &&
		params->_mouse_x <= right_x &&
		params->_mouse_y <= lower_y;

	return mouse_in_region;
}
