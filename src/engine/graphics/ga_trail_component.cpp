/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_trail_component.h"
#include "ga_material.h"

#include "entity/ga_entity.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <vector>
#include <queue>

ga_trail_component::ga_trail_component(ga_entity* ent, const char* texture_file, trail_data* data) : ga_component(ent)
{
	_material = new ga_unlit_texture_material(texture_file);
	_material->init();


	update_counter = 0;
	ui_data = data;

	num_breaks = ui_data->num_breaks;
	update_rate = ui_data->update_rate;
	start_width = ui_data->start_width;
	end_width = ui_data->end_width;
}

ga_trail_component::~ga_trail_component()
{
	delete _material;
}

void ga_trail_component::update(ga_frame_params* params)
{

	num_breaks = ui_data->num_breaks;
	update_rate = ui_data->update_rate;
	start_width = ui_data->start_width;
	end_width = ui_data->end_width;

	update_counter++;

	ga_mat4f current_entity_transform = get_entity()->get_transform();
	ga_vec3f current_entity_position = current_entity_transform.get_translation();
	
	
	if (update_counter >= update_rate) {
		update_counter = 0;

		// update the list of past positions the entity has occupied
		latest_positions.push_front(current_entity_position);

		while (latest_positions.size() > num_breaks) {
			latest_positions.pop_back();
		}
	}

	std::vector<ga_vec3f> positions;
	std::vector<ga_vec2f> texcoords;
	std::vector<uint16_t> indices;


	// do we have anything to draw at all?
	if (latest_positions.size() > 1)
	{
		float current_width = start_width;
		ga_vec3f last_center_point = latest_positions.at(0) - current_entity_position;

		// add the first two points so we can get the strip going
		ga_vec3f pos1 = ga_vec3f();
		pos1.axes[0] = 0;
		pos1.axes[1] = 0;
		pos1.axes[2] = current_width/2;
		positions.push_back(pos1);

		ga_vec3f pos2 = ga_vec3f();
		pos2.axes[0] = 0;
		pos2.axes[1] = 0;
		pos2.axes[2] = current_width/2;
		positions.push_back(pos2);


		ga_vec2f tex1 = ga_vec2f();
		tex1.axes[0] = -1.0;
		tex1.axes[1] = 0.0;
		texcoords.push_back(tex1);
		ga_vec2f tex2 = ga_vec2f();
		tex2.axes[0] = 1.0;
		tex2.axes[1] = 0.0;
		texcoords.push_back(tex2);


		indices.push_back(1);
		indices.push_back(0);


		for (int i = 1; i < latest_positions.size(); i++)
		{
			current_width = (((i*1.0) / latest_positions.size()) * end_width) + ( (1.0 - ((i*1.0) / latest_positions.size())) * start_width);

			ga_vec3f center_point = latest_positions.at(i) - current_entity_position;

			ga_vec3f up = ga_vec3f();
			up.axes[0] = 0;
			up.axes[1] = 1;
			up.axes[2] = 0;

			ga_vec3f tangent_vector = center_point - last_center_point;
			ga_vec3f normal_vector = ga_vec3f_cross(center_point, up);
			normal_vector.normalize();


			last_center_point = center_point;

			ga_vec3f temp_pos1 = ga_vec3f();
			temp_pos1 = center_point + (normal_vector.scale_result(current_width / 2.0));
			positions.push_back(temp_pos1);
			ga_vec3f temp_pos2 = ga_vec3f();
			temp_pos2 = center_point - (normal_vector.scale_result(current_width / 2.0));
			positions.push_back(temp_pos2);

			ga_vec2f temp_tex1 = ga_vec2f();
			temp_tex1.axes[0] = 1.0;
			temp_tex1.axes[1] = 1.0;
			texcoords.push_back(temp_tex1);
			ga_vec2f temp_tex2 = ga_vec2f();
			temp_tex2.axes[0] = 0.0;
			temp_tex2.axes[1] = 1.0;
			texcoords.push_back(temp_tex2);

			indices.push_back(i * 2);
			indices.push_back(i * 2 + 1);
		}


		ga_dynamic_drawcall draw;
		draw._name = "ga_trail_component";
		draw._positions = positions;
		draw._indices = indices;
		draw._texcoords = texcoords;
		draw._transform = get_entity()->get_transform();
		draw._draw_mode = GL_TRIANGLE_STRIP;
		draw._material = _material;


		while (params->_dynamic_drawcall_lock.test_and_set(std::memory_order_acquire)) {}
		params->_dynamic_drawcalls.push_back(draw);
		params->_dynamic_drawcall_lock.clear(std::memory_order_release);

	}
}
