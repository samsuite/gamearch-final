/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_lua_component.h"

#include "entity/ga_entity.h"
#include "framework/ga_frame_params.h"

#include <lua.hpp>


// using "z" as the "left" key
static int get_input_left(lua_State *L)
{
	ga_frame_params* params = (ga_frame_params*)lua_touserdata(L, 1);
	
	if ((params->_button_mask & k_button_z))
	{
		lua_pushboolean(L, 1);
	}
	else
	{
		lua_pushboolean(L, 0);
	}

	return 1;
}


// using "x" as the "right" key
static int get_input_right(lua_State *L)
{
	ga_frame_params* params = (ga_frame_params*)lua_touserdata(L, 1);

	if ((params->_button_mask & k_button_x))
	{
		lua_pushboolean(L, 1);
	}
	else
	{
		lua_pushboolean(L, 0);
	}

	return 1;
}

// return a component's entity
static int component_get_entity(lua_State *L)
{
	ga_lua_component* comp = (ga_lua_component*)lua_touserdata(L, 1);

	lua_pushlightuserdata(L, (void*)(comp->_my_entity));
	return 1;
}

// translate an entity
static int entity_translate(lua_State *L)
{
	ga_entity* ent = (ga_entity*)lua_touserdata(L, 1);
	float x = lua_tonumber(L, 2);
	float y = lua_tonumber(L, 3);
	float z = lua_tonumber(L, 4);

	ga_vec3f vec = ga_vec3f();
	vec.x = x;
	vec.y = y;
	vec.z = z;

	ent->translate(vec);

	return 0;
}

ga_lua_component::ga_lua_component(ga_entity* ent, const char* path) : ga_component(ent)
{
	_my_entity = ent;

	lua = luaL_newstate();
	luaL_openlibs(lua);

	extern char g_root_path[256];
	std::string fullpath = g_root_path;
	fullpath += path;

	lua_register(lua, "frame_params_get_input_left", get_input_left);
	lua_register(lua, "frame_params_get_input_right", get_input_right);
	lua_register(lua, "component_get_entity", component_get_entity);
	lua_register(lua, "entity_translate", entity_translate);
	
	luaL_dofile(lua, fullpath.c_str());
	
}

ga_lua_component::~ga_lua_component()
{
	lua_close(lua);
}

void ga_lua_component::update(ga_frame_params* params)
{
	lua_getglobal(lua, "update");
	lua_pushlightuserdata(lua, (void*)this);
	lua_pushlightuserdata(lua, (void*) params);
	lua_call(lua, 2, 0);
}
