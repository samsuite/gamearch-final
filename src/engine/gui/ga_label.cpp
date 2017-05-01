/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_label.h"
#include "ga_font.h"

#include "framework/ga_frame_params.h"

ga_label::ga_label(const char* text, float x, float y, ga_frame_params* params)
{
	extern ga_font* g_font;
	ga_vec3f col = ga_vec3f();
	col.x = 1;
	col.y = 1;
	col.z = 1;

	top_left = new ga_vec2f();
	bot_right = new ga_vec2f();

	g_font->print(params, text, x, y, col, top_left, bot_right);
}

ga_label::~ga_label()
{
	delete top_left;
	delete bot_right;
}
