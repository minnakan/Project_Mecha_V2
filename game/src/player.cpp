#include "pch.h"
#include "player.h"
#include "engine/core/input.h"
#include "engine/key_codes.h"


player::player() : m_timer(0.0f), m_speed(1.5f)
{
}

player::~player()
{}

void player::initialise(engine::ref<engine::game_object> object)
{
	m_object = object;

}

void player::on_update(glm::vec3 pos, const engine::timestep& time_step)
{
	player_pos = pos;
}




