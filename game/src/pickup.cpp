#include"pickup.h"

pickup::pickup(const engine::game_object_properties props) :engine::game_object(props)
{}

pickup::~pickup()
{}

void pickup::init()
{
	m_is_active = true;
	m_player_healed = false;
	set_rotation_axis(glm::vec3(0.f, 1.f, 0.f));
	set_rotation_amount(0.0f);
}
void pickup::update(glm::vec3 c, float dt)
{
	set_rotation_amount(rotation_amount() + dt * 1.5f);

	if (glm::distance(c, position()) < 1.0f) {
		m_is_active = false;
		m_player_healed = true;
	}
}

void pickup::reset() {
	m_is_active = true;
	m_player_healed = false;
}

engine::ref<pickup>pickup::create(const engine::game_object_properties& props)
{
	return std::make_shared<pickup>(props);
}


