/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_wobbler_component.h"
#include "ga_material.h"

#include "entity/ga_entity.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <vector>
#include <queue>
#include <math.h>

ga_wobbler_component::ga_wobbler_component(ga_entity* ent) : ga_component(ent)
{
}

ga_wobbler_component::~ga_wobbler_component()
{
}

void ga_wobbler_component::update(ga_frame_params* params)
{
	float time = std::chrono::duration_cast<std::chrono::duration<float>>(params->_current_time.time_since_epoch()).count();

	ga_vec3f movement = ga_vec3f();
	movement.axes[0] = sin(time * 10) / 2.0;
	movement.axes[1] = sin(time * 2) / 4.63;
	movement.axes[2] = cos(time * 10) / 2.0;

	//movement.axes[0] = sin(time * 10) / 2.0;
	//movement.axes[1] = cos(time * 10) / 2.0;
	//movement.axes[2] = 0;

	get_entity()->translate(movement);
}
