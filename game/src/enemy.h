#pragma once
#include <engine.h>


class enemy : public engine::game_object {
public:
	enemy(const engine::game_object_properties props);
	~enemy();
	void init();
	void update(glm::vec3 c, float dt);
	bool active() { return m_is_active; }
	void set_destination(glm::vec3 destination);
	bool damage_done() { return m_damage_done; }
	void reset_spider();
	glm::vec3 next_move(glm::vec3 src, glm::vec3 ds);

	static engine::ref<enemy> create(const engine::game_object_properties& props);
private:
	bool m_is_active;
	bool m_damage_done;
	glm::vec3 m_next_move;
	glm::vec3 start_position;
	float src_xpos;
	float src_zpos;

	float dst_xpos;
	float dst_zpos;

	float nxt_xpos;
	float nxt_zpos;
};
