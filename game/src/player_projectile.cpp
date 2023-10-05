#include "pch.h"
#include "player_projectile.h"

player_projectile::player_projectile()
{}

player_projectile::~player_projectile()
{}

void player_projectile::initialise(engine::ref<engine::game_object> object)
{
	m_object = object;
	m_rotational_inertia = (2.f / 3.f) * object->mass() * (m_object->bounding_shape().y / 2.f) * (m_object->bounding_shape().y / 2.f);
	m_contact_time = 0.05f;

}

void player_projectile::shoot(const engine::perspective_camera& camera, float shoot)
{
	m_object->set_velocity(glm::vec3(0.f));
	m_object->set_acceleration(glm::vec3(0.f, 0.f, 0.f));
	m_object->set_torque(glm::vec3(0.f));
	m_object->set_rotation_amount(0.f);
	//m_object->set_rotation_axis(glm::vec3(0.f, 0.f, 0.f));
	m_object->set_angular_velocity(glm::vec3(0.f));
	m_contact_time = 0.0f;

	// Set the missile to the current camera position
	m_object->set_position(glm::vec3(camera.position().x,camera.position().y,camera.position().z));
	m_object->set_velocity(15.0f * camera.front_vector());
	glm::vec3 force = camera.front_vector() * shoot;
	m_instantaneous_acceleration = force / m_object->mass();

	glm::vec3 torque = glm::vec3(0, 0, 0);
	torque *= shoot / 20.0f;
	m_instantaneous_angular_acceleration = torque / m_rotational_inertia;

	// Determine rotation angles of camera (from Lab 4)
	m_theta = engine::PI / 2.f - acos(camera.front_vector().y);
	m_phi = atan2(camera.front_vector().x, camera.front_vector().z);
}

void player_projectile::on_update(const engine::timestep& time_step)
{
	// Update physical quanitities
	m_last_position = m_object->position();

	m_object->set_velocity(m_object->velocity() + (m_object->acceleration() + m_instantaneous_acceleration) * (float)time_step);
	m_object->set_position(m_object->position() + m_object->velocity() * (float)time_step);


}

void player_projectile::on_render(const engine::ref<engine::shader>& shader)
{
	glm::mat4 transform(1.0f);
	transform = glm::translate(transform, m_object->position());
	transform = glm::rotate(transform, -m_theta, glm::vec3(1.f, 0.f, 0.f));
	transform = glm::rotate(transform, m_phi, glm::vec3(0.f, 1.f, 0.f));
	transform = glm::rotate(transform, m_object->rotation_amount(), m_object->rotation_axis());
	engine::renderer::submit(shader, transform, m_object);

}

bool player_projectile::ground_collision(float y_val) {
	if (m_object->position().y - m_object->bounding_shape().y < y_val && m_object->velocity().y < 0) {
		return true;
	}
	return false;
}



