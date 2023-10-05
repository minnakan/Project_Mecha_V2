#pragma once
#include <engine.h>

class pickup : public engine::game_object {
public:
	pickup(const engine::game_object_properties props);
	~pickup();
	void init();
	void update(glm::vec3 c, float dt);
	bool active() { return m_is_active; }
	bool playerhealed() { return m_player_healed; }
	void reset();

	static engine::ref<pickup> create(const engine::game_object_properties& props);
private:
	bool m_is_active;
	bool m_player_healed;
};

