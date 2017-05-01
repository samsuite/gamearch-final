#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "entity/ga_component.h"

#include <deque>
#include <cstdint>

struct trail_data {
	int num_breaks;
	int update_rate;
	float start_width;
	float end_width;
};

class ga_trail_component : public ga_component
{
public:
	ga_trail_component(class ga_entity* ent, const char* texture_file, trail_data* data);
	virtual ~ga_trail_component();

	virtual void update(struct ga_frame_params* params) override;

private:
	class ga_material* _material;
	uint32_t _vao;
	uint32_t _vbos[4];
	uint32_t _index_count;

	std::deque<ga_vec3f> latest_positions;
	int update_counter;

	int num_breaks;		// the number of breaks between and on either side of the quads (this is 1 greater than the number of quads)
	int update_rate;	// how regularly do we update the positions list?
	float start_width;	// trail width at the entity
	float end_width;	// trail width at the end of the trail

	trail_data* ui_data;
};
