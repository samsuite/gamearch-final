#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_component.h"
#include "framework/ga_frame_params.h"
#include <lua.hpp>

/*
** A component whose logic is implemented in LUA.
*/
class ga_lua_component : public ga_component
{
public:
	ga_lua_component(class ga_entity* ent, const char* path);
	virtual ~ga_lua_component();
	lua_State* lua;
	ga_entity* _my_entity;

	virtual void update(struct ga_frame_params* params) override;
};
