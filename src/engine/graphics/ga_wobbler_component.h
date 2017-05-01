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

/*
** Renderable basic textured cubed.
*/
class ga_wobbler_component : public ga_component
{
public:
	ga_wobbler_component(class ga_entity* ent);
	virtual ~ga_wobbler_component();

	virtual void update(struct ga_frame_params* params) override;
};
