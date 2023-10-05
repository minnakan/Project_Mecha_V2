#pragma once
#include<engine.h>
#include"glm/gtx/rotate_vector.hpp"
class player
{
public:
	player();
	~player();

	void initialise(engine::ref<engine::game_object>object);
	void on_update(glm::vec3 pos ,const engine::timestep & time_step);
	void on_damage(float h);

	engine::ref<engine::game_object>object()const { return m_object; }

private:
	float m_speed{0.f};
	float m_timer;
	glm::vec3 player_pos;
	engine::ref<engine::game_object>m_object;
};
