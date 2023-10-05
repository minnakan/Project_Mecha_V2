#pragma once
#include <engine.h>
#include "player.h"
#include "intro_screen.h"
#include "hud.h"
#include "player_projectile.h"
#include "explosion.h"
#include "cross_fade.h"
#include "bounding_box_bullet.h"

class pickup;
class enemy;

class example_layer : public engine::layer
{
public:
    example_layer();
	~example_layer();

    void on_update(const engine::timestep& time_step) override;
    void on_render() override; 
    void on_event(engine::event& event) override;

private:
	void check_bounce();
	void hexagon_shape();

	engine::ref<engine::skybox>			m_skybox{};
	engine::ref<engine::game_object>	m_terrain{};

	engine::ref<engine::game_object>	m_cuboid{};
	player								m_player{};
	engine::ref<engine::game_object>	m_player_object{};
	float								player_health{};
	float								player_score{};

	engine::ref<pickup>					m_pickup{};
	std::vector<glm::vec3>				m_pickup_positions;
	int									m_pickup_counter;

	engine::ref<enemy>					m_enemy{};
	engine::ref<cross_fade>				m_cross_fade{};
	glm::vec3							m_next_move{};
	bool								reset_enemy_pos{};

	engine::ref<enemy>					m_enemy2{};
	glm::vec3							m_next_move_2{};
	bool								reset_enemy2_pos{};

	engine::ref<engine::game_object>	m_missile{};
	player_projectile					m_player_projectile{};
	engine::ref<explosion>				m_explosion{};
	bool								explosion_done;//used to render explosion only once due to missile bounce
	int									p_ammo;
	bool								render_missile;// stops rendering missile on collisions
	
	engine::ref<engine::game_object>	m_satellite{};
	float								satellite_health{};

	engine::ref<engine::game_object>	m_pyramid{};
	std::vector<bounding_box>			m_pyramid_boxes;
	engine::ref<engine::material>		m_material{};
	engine::ref<engine::material>		m_tetrahedron_material{};

	engine::DirectionalLight            m_directionalLight;

	std::vector<engine::ref<engine::game_object>>     m_game_objects{};

	engine::ref<engine::bullet_manager> m_physics_manager{};
	engine::ref<engine::audio_manager>  m_audio_manager{};
	float								m_prev_sphere_y_vel = 0.f;
	engine::ref<engine::text_manager>	m_text_manager{};

	engine::ref<intro_screen>			m_intro_screen{};
	engine::ref<hud>					m_hud{};
	engine::ref<intro_screen>			m_end_screen{};

	engine::SpotLight					m_spotLight;
	uint32_t							num_spot_lights = 1;
	engine::ref<engine::material>		m_lightsource_material{};
	engine::ref<engine::game_object>	m_ball{};

	engine::PointLight					m_pointLight;
	uint32_t							num_point_lights = 1;
	float								lighton;

	bounding_box						m_player_box;
	bounding_box						m_enemy_box;
	bounding_box						m_enemy2_box;
	bounding_box						m_satellite_box;
	bounding_box						m_missile_box;
	bounding_box						m_pyramid_box;

    engine::orthographic_camera       m_2d_camera; 
    engine::perspective_camera        m_3d_camera;
	bool game_start = false;
	bool game_end = false;
	
};
