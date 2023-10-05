#pragma once
#include <engine.h>

class player_projectile
{
public:
	player_projectile();
	~player_projectile();

	void initialise(engine::ref<engine::game_object> object);
	void shoot(const engine::perspective_camera& camera, float kick);
	void on_update(const engine::timestep& time_step);
	void on_render(const engine::ref<engine::shader>& shader);

	bool ground_collision(float y_val);

	glm::vec3 last_position() const { return m_last_position; }

	engine::ref<engine::game_object> object() const { return m_object; }


private:
	engine::ref<engine::game_object> m_object;

	glm::vec3 m_instantaneous_acceleration{ 0.f };
	glm::vec3 m_instantaneous_angular_acceleration{ 0.f };

	float m_contact_time = 0.0f;

	float m_rotational_inertia = 0.f;

	float m_phi, m_theta;

	glm::vec3 m_last_position{ 0.f };


	// Methods to handle collisions with the ground

};
