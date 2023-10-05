#include "example_layer.h"
#include "platform/opengl/gl_shader.h"

#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>
#include "engine/events/key_event.h"
#include "engine/utils/track.h"
#include "pickup.h"
#include "enemy.h"

example_layer::example_layer() 
    :m_2d_camera(-1.6f, 1.6f, -0.9f, 0.9f), 
    m_3d_camera((float)engine::application::window().width(), (float)engine::application::window().height())


{

    // Hide the mouse and lock it inside the window
    //engine::input::anchor_mouse(true);
    engine::application::window().hide_mouse_cursor();

	// Initialise audio and play background music
	m_audio_manager = engine::audio_manager::instance();
	m_audio_manager->init();
	m_audio_manager->load_sound("assets/audio/damage.mp3", engine::sound_type::event, "damage"); // Royalty free sound from freesound.org
	m_audio_manager->load_sound("assets/audio/shoot.mp3", engine::sound_type::event, "shoot"); 
	m_audio_manager->load_sound("assets/audio/menu.mp3", engine::sound_type::track, "menu");
	m_audio_manager->load_sound("assets/audio/back.mp3", engine::sound_type::track, "background");
	m_audio_manager->play("menu");

	// Initialise the shaders, materials and lights
	auto mesh__material_shader = engine::renderer::shaders_library()->get("mesh_material");
	auto mesh_lighting_shader = engine::renderer::shaders_library()->get("mesh_lighting");
	auto text_shader = engine::renderer::shaders_library()->get("text_2D");
	auto animated_mesh_shader = engine::renderer::shaders_library()->get("animated_mesh");

	m_directionalLight.Color = glm::vec3(1.0f, 1.0f, 1.f);
	m_directionalLight.AmbientIntensity = 0.10f;
	m_directionalLight.DiffuseIntensity = 0.2f;
	m_directionalLight.Direction = glm::normalize(glm::vec3(1.0f, -1.0f, 0.0f));

	m_pointLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
	m_pointLight.AmbientIntensity = 0.f;
	m_pointLight.DiffuseIntensity = 0.f;
	m_pointLight.Position = m_3d_camera.position();
	lighton = false;

	// set color texture unit
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->set_uniform("gColorMap", 0);
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->set_uniform("lighting_on", true);
	m_directionalLight.submit(animated_mesh_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->set_uniform("gMatSpecularIntensity", 0.5f);
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->set_uniform("gSpecularPower", 5.f);
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->set_uniform("transparency", 1.0f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh__material_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(mesh__material_shader)->set_uniform("lighting_on", true);
	//m_directionalLight.submit(mesh__material_shader);

	std::dynamic_pointer_cast<engine::gl_shader>(mesh__material_shader)->set_uniform("gMatSpecularIntensity", 1.f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh__material_shader)->set_uniform("gSpecularPower", 10.f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("gColorMap", 0);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("lighting_on", true);
	m_directionalLight.submit(mesh_lighting_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("gMatSpecularIntensity", 0.5f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("gSpecularPower", 5.f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("transparency", 1.0f);
	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->set_uniform("projection",
		glm::ortho(0.f, (float)engine::application::window().width(), 0.f,
		(float)engine::application::window().height()));
	m_material = engine::material::create(1.0f, glm::vec3(1.0f, 0.1f, 0.07f),
		glm::vec3(1.0f, 0.1f, 0.07f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);

	m_lightsource_material = engine::material::create(1.f, m_pointLight.Color, m_pointLight.Color, m_pointLight.Color, 1.0f);

	std::vector<engine::ref<engine::texture_2d>> pyramid_textures = { engine::texture_2d::create("assets/textures/stone.jpg",false) };


	// Skybox texture from https://opengameart.org/content/interstellar-skybox-png
	m_skybox = engine::skybox::create(50.f,
		{ engine::texture_2d::create("assets/textures/skybox/SkyboxFront.bmp", true),
		  engine::texture_2d::create("assets/textures/skybox/SkyboxRight.bmp", true),
		  engine::texture_2d::create("assets/textures/skybox/SkyboxBack.bmp", true),
		  engine::texture_2d::create("assets/textures/skybox/SkyboxLeft.bmp", true),
		  engine::texture_2d::create("assets/textures/skybox/SkyboxTop.bmp", true),
		  engine::texture_2d::create("assets/textures/skybox/SkyboxBottom.bmp", true)
		});


	// Load the terrain texture and create a terrain mesh. Create a terrain object. Set its properties
	std::vector<engine::ref<engine::texture_2d>> terrain_textures = { engine::texture_2d::create("assets/textures/terrain.jpg", false) };
	engine::ref<engine::terrain> terrain_shape = engine::terrain::create(100.f, 0.5f, 100.f);
	engine::game_object_properties terrain_props;
	terrain_props.meshes = { terrain_shape->mesh() };
	terrain_props.textures = terrain_textures;
	terrain_props.is_static = true;
	terrain_props.type = 0;
	terrain_props.bounding_shape = glm::vec3(100.f, 0.5f, 100.f);
	terrain_props.restitution = 0.92f;
	m_terrain = engine::game_object::create(terrain_props);


	//walls
	engine::ref<engine::cuboid>cuboid_shape = engine::cuboid::create(glm::vec3(2.f),false);
	engine::ref<engine::texture_2d> wall_texture = engine::texture_2d::create("assets/textures/Red.bmp", true);
	engine::game_object_properties cuboid_props;
	cuboid_props.position = { 4.f, 0.5f, -5.f };
	cuboid_props.meshes = { cuboid_shape->mesh() };
	cuboid_props.textures = { wall_texture };
	m_cuboid = pickup::create(cuboid_props);

	//spider enemy #1
	engine::ref <engine::model> spider_model = engine::model::create("assets/models/static/spider.obj");
	engine::game_object_properties spider_props;
	spider_props.meshes = spider_model->meshes();
	spider_props.textures = spider_model->textures();
	float spider_scale = 1.f / glm::max(spider_model->size().x, glm::max(spider_model->size().y, spider_model->size().z));
	spider_props.position = { 25.f, 0.8f, -25.f };
	spider_props.scale = glm::vec3(spider_scale);
	spider_props.bounding_shape = spider_model->size() * 2.f;
	m_enemy = enemy::create(spider_props);
	m_enemy->init();
	m_enemy->set_destination( glm::vec3(0.f, 1.f, 20.f));

	m_enemy->set_offset(spider_model->offset());
	m_enemy_box.set_box(spider_props.bounding_shape.x * spider_scale, spider_props.bounding_shape.y * spider_scale, spider_props.bounding_shape.z * spider_scale, spider_props.position);

	reset_enemy_pos = false;

	//spider enemy #2
	engine::game_object_properties spider_props_2;
	spider_props_2.meshes = spider_model->meshes();
	spider_props_2.textures = spider_model->textures();
	float spider2_scale = 1.f / glm::max(spider_model->size().x, glm::max(spider_model->size().y, spider_model->size().z));
	spider_props_2.position = { -25.f, 0.8f, -25.f };
	spider_props_2.scale = glm::vec3(spider_scale*2.f);
	spider_props_2.bounding_shape = spider_model->size() * 4.f;
	m_enemy2 = enemy::create(spider_props_2);
	m_enemy2->init();
	m_enemy->set_destination(glm::vec3(0.f, 1.f, 20.f));

	m_enemy2->set_offset(spider_model->offset());
	m_enemy2_box.set_box(spider_props_2.bounding_shape.x* spider2_scale, spider_props_2.bounding_shape.y* spider2_scale, spider_props_2.bounding_shape.z* spider2_scale, spider_props_2.position);

	reset_enemy2_pos = false;

	//loading the satellite model 
	engine::ref<engine::model>satellite_model = engine::model::create("assets/models/static/com_sat.obj");
	engine::game_object_properties sat_props;
	sat_props.meshes = satellite_model->meshes();
	sat_props.textures = satellite_model->textures();
	float sat_scale = 1.f / glm::max(satellite_model->size().x, glm::max(satellite_model->size().y, satellite_model->size().z));
	sat_props.position = { 0.f, 1.f, 20.f };
	sat_props.scale = glm::vec3(sat_scale);
	sat_props.bounding_shape = satellite_model->size() * 10.f;
	m_satellite = engine::game_object::create(sat_props);

	m_satellite->set_offset(spider_model->offset());
	m_satellite_box.set_box(spider_props.bounding_shape.x* sat_scale *0.10, sat_props.bounding_shape.y* sat_scale*0.5f, sat_props.bounding_shape.z* sat_scale*0.5f, sat_props.position);

	//pyramid
	std::vector <glm::vec3> pyramid_vertices;
	pyramid_vertices.push_back(glm::vec3(0.f, 10.f, -5.f));	//0
	pyramid_vertices.push_back(glm::vec3(10.f, 0.f, 0.f));	//1
	pyramid_vertices.push_back(glm::vec3(-10.f, 0.f, 0.f));	//2
	pyramid_vertices.push_back(glm::vec3(-10.f, 0.f, -10.f));//3
	pyramid_vertices.push_back(glm::vec3(10.f, 0.f, -10.f));//4

	engine::ref<engine::pyramid> pyramid_shape = engine::pyramid::create(pyramid_vertices);
	engine::game_object_properties pyramid_props;
	pyramid_props.position = { 0.f, 0.5f, -20.f };
	pyramid_props.meshes = { pyramid_shape->mesh() };
	pyramid_props.textures = pyramid_textures;

	m_pyramid = engine::game_object::create(pyramid_props);

	//player
	m_player.initialise(m_player_object);
	player_health = 100.f;
	player_score = 0.f;
	p_ammo = 200;
	satellite_health = 100;

	//player projectile
	//engine::ref<engine::sphere> sphere_shape = engine::sphere::create(10, 20, 0.1f);
	engine::ref <engine::model> missile_model = engine::model::create("assets/models/static/missile.obj");
	engine::game_object_properties missile_props;
	missile_props.position = { 100.f, 5.f, -5.f };
	missile_props.meshes = missile_model->meshes();
	missile_props.textures = missile_model->textures();
	float missile_scale = 1.f / glm::max(missile_model->size().x, glm::max(missile_model->size().y, missile_model->size().z));
	missile_props.type = 1;
	missile_props.bounding_shape = missile_model->size() / 2.0f;
	missile_props.restitution = 0.92f;
	missile_props.mass = 0.45f;
	m_missile = engine::game_object::create(missile_props);
	m_player_projectile.initialise(m_missile);
	render_missile = true;

	m_missile->set_offset(missile_model->offset());
	m_missile_box.set_box(missile_props.bounding_shape.x* missile_scale *0.25f, missile_props.bounding_shape.y* missile_scale , missile_props.bounding_shape.z* missile_scale , missile_props.position);

	m_explosion = explosion::create("assets/textures/Explosion.tga", 4, 5, 16);
	explosion_done = false;

	

	engine::ref<engine::sphere> sphere_shape = engine::sphere::create(20, 40, 0.5f);
	engine::game_object_properties sphere_props;
	sphere_props.position = { -2.5f, 0.0f, -2.5f };
	sphere_props.meshes = { sphere_shape->mesh() };
	sphere_props.type = 1;
	sphere_props.bounding_shape = glm::vec3(0.5f);
	sphere_props.restitution = 0.92f;
	sphere_props.mass = 0.000001f;
	m_ball = engine::game_object::create(sphere_props);
	

	m_game_objects.push_back(m_terrain);
	m_game_objects.push_back(m_missile);
	m_game_objects.push_back(m_cuboid);
	m_game_objects.push_back(m_pyramid);
	m_physics_manager = engine::bullet_manager::create(m_game_objects);

	m_text_manager = engine::text_manager::create();

	m_pickup_positions.push_back({ 5.f, 2.f, 5.f });
	m_pickup_positions.push_back({ -5.f, 2.f, 5.f });
	m_pickup_positions.push_back({ 5.f, 2.f, -5.f });
	m_pickup_positions.push_back({ -5.f, 2.f, -5.f });
	m_pickup_positions.push_back({ -10.f, -10.f, -10.f });
	m_pickup_counter = 0;

	//Pickup - Shield
	engine::ref<engine::sphere> pickup_shape = engine::sphere::create(10,20,0.5f);
	engine::ref<engine::texture_2d> pickup_texture = engine::texture_2d::create("assets/textures/medkit.jpg",true);
	engine::game_object_properties pickup_props;
	pickup_props.position = m_pickup_positions.at(m_pickup_counter);
	pickup_props.meshes = { pickup_shape->mesh() };
	pickup_props.textures = { pickup_texture };
	m_pickup = pickup::create(pickup_props);
	m_pickup->init();
	

	m_intro_screen = intro_screen::create("assets/textures/menu.png", 2.0f, 1.6f, 0.9f);
	m_end_screen = intro_screen::create("assets/textures/end.png", 2.0f, 1.6f, 0.9f);
	m_cross_fade = cross_fade::create("assets/textures/damage.png", 1.5f, 1.6f, 0.9f);

	m_hud = hud::create("assets/textures/hud.png", 2.0f, 1.6f, 0.9f);

	m_player_box.set_box(0.05f, 0.05f, 0.05f, m_3d_camera.position());

}

example_layer::~example_layer() {}

void example_layer::on_update(const engine::timestep& time_step) 
{

	m_intro_screen->activate();

	if (game_start) {
		glm::vec3 pos = m_3d_camera.position();

		m_intro_screen->deactivate();
		m_end_screen->deactivate();
		m_hud->activate();

		m_3d_camera.get_player_health(player_health);
		m_3d_camera.on_update(time_step);
		m_player.on_update(pos, time_step);

		//updating bounding boxes
		m_player_box.on_update(m_3d_camera.position()-glm::vec3(0,0.05,0),0.f,glm::vec3(0.f,1.f,0.f));
		m_enemy_box.on_update(m_enemy->position() - glm::vec3(0.f, m_enemy->offset().y, 0.f) * m_enemy->scale(), m_enemy->rotation_amount(), m_enemy->rotation_axis());//#1
		m_enemy2_box.on_update(m_enemy2->position() - glm::vec3(0.f, m_enemy2->offset().y, 0.f) * m_enemy2->scale(), m_enemy2->rotation_amount(), m_enemy2->rotation_axis());//#2
		m_satellite_box.on_update(m_satellite->position() - glm::vec3(0.f, m_satellite->offset().y, 0.f) * m_satellite->scale(), m_satellite->rotation_amount(), m_satellite->rotation_axis());
		m_missile_box.on_update(m_missile->position() - glm::vec3(0.f, m_missile->offset().y, 0.f) * m_missile->scale(), m_missile->rotation_amount(), m_missile->rotation_axis());

		

		//Collisions
		//enemy #1 collisions
		if (m_missile_box.collision(m_enemy_box)) {
			m_enemy->reset_spider();
			reset_enemy_pos = true;//resets enemy location in on_render
			player_score = player_score + 10.f;
		}
		if (m_satellite_box.collision(m_enemy_box)) {
			m_enemy->reset_spider();
			reset_enemy_pos = true;
			satellite_health = satellite_health - 10;
		}
		//enemy #2 collisions
		if (m_missile_box.collision(m_enemy2_box)) {
			m_enemy2->reset_spider();
			reset_enemy2_pos = true;
			player_score = player_score + 15.f;
		}
		if (m_satellite_box.collision(m_enemy2_box)) {
			m_enemy2->reset_spider();
			reset_enemy2_pos = true;
			satellite_health = satellite_health - 20;
		}

		if (m_player_projectile.ground_collision(0.75f)) {
			render_missile = false;
			glm::vec3 missile_pos = m_missile_box.get_position();
			if(!explosion_done)
			m_explosion->activate(glm::vec3(missile_pos.x, 0.5f, missile_pos.z), 4.f, 4.f);
			explosion_done = true;
		}

		if (player_health <= 0 || satellite_health <= 0) {
			game_end = true;
		}
		
		m_player_projectile.on_update(time_step);
		m_explosion->on_update(time_step);
		m_cross_fade->on_update(time_step);
		m_pointLight.Position = m_3d_camera.position();

		glm::vec3 spider_pos = m_enemy_box.get_position();
		glm::vec3 spider2_pos = m_enemy2_box.get_position();
		glm::vec3 destination_pos = m_satellite_box.get_position();

		m_enemy->next_move(spider_pos, destination_pos);
		m_enemy2->next_move(spider2_pos, destination_pos);

		m_physics_manager->dynamics_world_update(m_game_objects, double(time_step));

		m_enemy->update(m_3d_camera.position(), time_step);
		m_enemy2->update(m_3d_camera.position(), time_step);
		m_pickup->update(m_3d_camera.position(), time_step);


	}
	if (game_end) { //game over display- game over screen
		game_start = false;
		m_end_screen->activate();
		m_audio_manager->stop_all();
	}

} 

void example_layer::on_render() 
{
    engine::render_command::clear_color({0.2f, 0.3f, 0.3f, 1.0f}); 
    engine::render_command::clear();


	const auto textured_lighting_shader = engine::renderer::shaders_library()->get("mesh_lighting");
	engine::renderer::begin_scene(m_3d_camera, textured_lighting_shader);

	// Set up some of the scene's parameters in the shader
	std::dynamic_pointer_cast<engine::gl_shader>(textured_lighting_shader)->set_uniform("gEyeWorldPos", m_3d_camera.position());

	std::dynamic_pointer_cast<engine::gl_shader>(textured_lighting_shader)->set_uniform("gNumPointLights", (int)num_point_lights); m_pointLight.submit(textured_lighting_shader, 0);


	// Position the skybox centred on the player and render it
	glm::mat4 skybox_tranform(1.0f);
	skybox_tranform = glm::translate(skybox_tranform, m_3d_camera.position());
	for (const auto& texture : m_skybox->textures())
	{
		texture->bind();
	}
	engine::renderer::submit(textured_lighting_shader, m_skybox, skybox_tranform);

	engine::renderer::submit(textured_lighting_shader, m_terrain);

	//rendering the bounding boxes
	/*m_player_box.on_render(2.5f, 0.f, 0.f, textured_lighting_shader);
	m_enemy_box.on_render(2.5f, 0.f, 0.f, textured_lighting_shader);
	m_enemy2_box.on_render(2.5f, 0.f, 0.f, textured_lighting_shader);
	m_satellite_box.on_render(2.5f, 0.f, 0.f, textured_lighting_shader);
	m_missile_box.on_render(2.5f, 0.f, 0.f, textured_lighting_shader);*/


	//multiple pyramids with different transformations
	float pyr_scale = -0.10f;
	int box_counter = 0;
	for (float i = -25.f; i < 50.f; i=i+25.f) {
		m_pyramid->set_position(glm::vec3(i, 0.5, -20.f));
		m_pyramid->set_scale(glm::vec3(1.f+pyr_scale, 1.f+pyr_scale , 1.f ));
		engine::renderer::submit(textured_lighting_shader, m_pyramid);
		pyr_scale *= -2.f;
	}
		 
	//rendering the pickup
	if (m_pickup->active()) {
		m_pickup->textures().at(0)->bind();
		glm::mat4 pickup_transform(1.0f);
		pickup_transform = glm::translate(pickup_transform, m_pickup->position());
		pickup_transform = glm::rotate(pickup_transform, m_pickup->rotation_amount(), m_pickup->rotation_axis());
		engine::renderer::submit(textured_lighting_shader, m_pickup->meshes().at(0), pickup_transform);

	}

	if (m_pickup->playerhealed() && m_pickup_counter<=4) {
		player_health = player_health + 10.f;
		m_pickup->set_position(m_pickup_positions.at(m_pickup_counter));
		m_pickup_counter = m_pickup_counter + 1;
		m_pickup->reset();
	}

	
	//rendering the enemy model #1
	if (m_enemy->active()) {
		glm::mat4 enemy_transform(1.0f);
		enemy_transform = glm::translate(enemy_transform, m_enemy->position() - glm::vec3(m_enemy->offset().x, 0.8f, m_enemy->offset().z) * m_enemy->scale().x);
		enemy_transform = glm::rotate(enemy_transform, m_enemy->rotation_amount(), m_enemy->rotation_axis());
		enemy_transform = glm::scale(enemy_transform, m_enemy->scale() * 2.f);
		engine::renderer::submit(textured_lighting_shader, enemy_transform, m_enemy);
	}
	//checking for collision and displaying cross fade
	if (m_enemy->damage_done()) {
		player_health = player_health - 10.f;
		m_cross_fade->activate();
		m_audio_manager->play("damage");
		m_enemy->set_position(glm::vec3(25.f, 0.5f, -25.f));
		m_enemy->reset_spider();
	}
	if (reset_enemy_pos)
	{
		m_enemy->set_position(glm::vec3(25.f, 0.5f, -25.f));
		reset_enemy_pos = false;
	}
	//rendering the enemy model #2
	if (m_enemy2->active()) {
		glm::mat4 enemy_transform(1.0f);
		enemy_transform = glm::translate(enemy_transform, m_enemy2->position() - glm::vec3(m_enemy2->offset().x, 0.f, m_enemy2->offset().z) * m_enemy2->scale().x);
		enemy_transform = glm::rotate(enemy_transform, m_enemy2->rotation_amount(), m_enemy2->rotation_axis());
		enemy_transform = glm::scale(enemy_transform, m_enemy2->scale() * 2.f);
		engine::renderer::submit(textured_lighting_shader, enemy_transform, m_enemy2);
	}
	//checking for collision and displaying cross fade
	if (m_enemy2->damage_done()) {
		player_health = player_health - 20.f;
		m_cross_fade->activate();
		m_audio_manager->play("damage");
		m_enemy2->set_position(glm::vec3(-25.f, 0.5f, -25.f));
		m_enemy2->reset_spider();
	}
	if (reset_enemy2_pos) {
		m_enemy2->set_position(glm::vec3(-25.f, 0.5f, -25.f));
		reset_enemy2_pos = false; 
	}

	// stop rendering the missile after it collides
	if (render_missile) {
		m_player_projectile.on_render(textured_lighting_shader);
	}
	m_explosion->on_render(m_3d_camera, textured_lighting_shader);

	//rendering the satellite
	glm::mat4 satellite_transform(1.0f);
	satellite_transform = glm::translate(satellite_transform, m_satellite->position()-glm::vec3(m_satellite->offset().x, 0.f, m_satellite->offset().z) * m_satellite->scale().x);
	satellite_transform = glm::rotate(satellite_transform, m_satellite->rotation_amount(), m_satellite->rotation_axis());
	satellite_transform = glm::scale(satellite_transform, m_satellite->scale()*10.f);
	engine::renderer::submit(textured_lighting_shader, satellite_transform, m_satellite);

	//wall left
	glm::mat4 cuboid_transform(1.0f);
	for (float i = -29.f; i <= 45.f; i = i + 4.f) {
		m_cuboid->set_position(glm::vec3(-40.f, 0.5f, i));
		engine::renderer::submit(textured_lighting_shader, m_cuboid);
	}

	//wall right
	for (float i = -29.f; i <= 45.f; i = i + 2.f) {
		m_cuboid->set_position(glm::vec3(40.f, 0.5f, i));
		engine::renderer::submit(textured_lighting_shader, m_cuboid);
	}

	//wall back
	for (float i = -37.f; i <= 37.f; i = i + 2.f) {
		m_cuboid->set_position(glm::vec3(i, 0.5f,42.f));
		engine::renderer::submit(textured_lighting_shader, m_cuboid);
	}
	//wall front
	for (float i = -37.f; i <= 37.f; i = i + 2.f) {
		m_cuboid->set_position(glm::vec3(i, 0.5f, -31.f));
		engine::renderer::submit(textured_lighting_shader, m_cuboid);
	}

	//text based hud for displaying info
	const auto text_shader = engine::renderer::shaders_library()->get("text_2D");
	int ph = player_health;
	m_text_manager->render_text(text_shader, "Health: " + std::to_string(ph), 10.f, (float)engine::application::window().height() - 25.f, 0.5f, glm::vec4(0.f, 1.f, 0.f, 1.f));
	int ps = player_score;
	m_text_manager->render_text(text_shader, "Score: " + std::to_string(ps), 1150.f, (float)engine::application::window().height() - 25.f, 0.5f, glm::vec4(0.f, 1.f, 0.f, 1.f));

	m_text_manager->render_text(text_shader, "Ammo " + std::to_string(p_ammo), 200.f, (float)engine::application::window().height() - 25.f, 0.5f, glm::vec4(0.f, 1.f, 0.f, 1.f));
	int sh = satellite_health;
	m_text_manager->render_text(text_shader, "Satellite Integrity: " + std::to_string(sh), 800.f, (float)engine::application::window().height() - 25.f, 0.5f, glm::vec4(0.f, 1.f, 0.f, 1.f));

	if(player_health>50.f)
		m_text_manager->render_text(text_shader, "Flight:ON " , 400.f, (float)engine::application::window().height() - 25.f, 0.5f, glm::vec4(0.f, 1.f, 0.f, 1.f));
	else
		m_text_manager->render_text(text_shader, "Flight:OFF ", 400.f, (float)engine::application::window().height() - 25.f, 0.5f, glm::vec4(0.f, 1.f, 0.f, 1.f));
	if(lighton)
		m_text_manager->render_text(text_shader, "Flash:ON ", 600.f, (float)engine::application::window().height() - 25.f, 0.5f, glm::vec4(0.f, 1.f, 0.f, 1.f));
	else
		m_text_manager->render_text(text_shader, "Flash:OFF ", 600.f, (float)engine::application::window().height() - 25.f, 0.5f, glm::vec4(0.f, 1.f, 0.f, 1.f));

	//Flashlight feature binds to f
	std::dynamic_pointer_cast<engine::gl_shader>(textured_lighting_shader)->set_uniform("lighting_on", false);
	m_lightsource_material->submit(textured_lighting_shader);
	engine::renderer::submit(textured_lighting_shader, m_ball->meshes().at(0), glm::translate(glm::mat4(1.f), m_pointLight.Position));
	std::dynamic_pointer_cast<engine::gl_shader>(textured_lighting_shader)->set_uniform("lighting_on", true);

    engine::renderer::end_scene();


	// Set up material shader. (does not render textures, renders materials instead)
	const auto material_shader = engine::renderer::shaders_library()->get("mesh_material");
	engine::renderer::begin_scene(m_3d_camera, material_shader);

	m_material->submit(material_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(material_shader)->set_uniform("gEyeWorldPos", m_3d_camera.position());


	engine::renderer::end_scene();

	const auto animated_mesh_shader = engine::renderer::shaders_library()->get("animated_mesh");
	engine::renderer::begin_scene(m_3d_camera, animated_mesh_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->set_uniform("gEyeWorldPos", m_3d_camera.position());
	engine::renderer::end_scene();
	//glm::mat4 aniTransform = glm::mat4(1.0f);


	engine::renderer::begin_scene(m_2d_camera, textured_lighting_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(textured_lighting_shader)->set_uniform("lighting_on", false);
	m_end_screen->on_render(textured_lighting_shader);

	engine::renderer::begin_scene(m_2d_camera, textured_lighting_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(textured_lighting_shader)->set_uniform("lighting_on", false);
	m_intro_screen->on_render(textured_lighting_shader);

	std::dynamic_pointer_cast<engine::gl_shader>(textured_lighting_shader)->set_uniform("lighting_on", true);
	engine::renderer::begin_scene(m_2d_camera, textured_lighting_shader);
	m_cross_fade->on_render(textured_lighting_shader);

	//lighting off to render with transperency
	std::dynamic_pointer_cast<engine::gl_shader>(textured_lighting_shader)->set_uniform("lighting_on", false);
	m_hud->on_render(textured_lighting_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(textured_lighting_shader)->set_uniform("lighting_on", true);

} 

void example_layer::on_event(engine::event& event) 
{ 
    if(event.event_type() == engine::event_type_e::key_pressed) 
    { 
        auto& e = dynamic_cast<engine::key_pressed_event&>(event); 
        if(e.key_code() == engine::key_codes::KEY_TAB) 
        { 
            engine::render_command::toggle_wireframe();
        }

		if (e.key_code() == engine::key_codes::KEY_ENTER)
		{
			game_start = true;//triggers game sequence in on_update() method
			m_audio_manager->stop_all();
			m_audio_manager->play("background");
			
		}

		if (e.key_code() == engine::key_codes::KEY_SPACE)
		{
			if (p_ammo > 0) {
				render_missile = true;
				explosion_done = false;
				m_player_projectile.shoot(m_3d_camera, 50.0f);
				
				m_audio_manager->play("shoot");
				p_ammo = p_ammo - 1;
			}
		}

		if (e.key_code() == engine::key_codes::KEY_F)
		{
			if (!lighton) {
				m_pointLight.AmbientIntensity = 0.25f;
				m_pointLight.DiffuseIntensity = 0.6f;
				lighton = true;
			}
			else {
				m_pointLight.AmbientIntensity = 0.f;
				m_pointLight.DiffuseIntensity = 0.f;
				lighton = false;
			}
		}
		if (event.event_type() == engine::event_type_e::mouse_button_pressed) {
			m_player_projectile.shoot(m_3d_camera, 50.0f);
		}
    } 
}
