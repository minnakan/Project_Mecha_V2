#include "pch.h"
#include "intro_screen.h"
#include "quad.h"

intro_screen::intro_screen(const std::string& path, float max_time, float width, float height)
{
	m_max_time = max_time;
	m_texture = engine::texture_2d::create(path, true);

	m_transparency = 0.0f;

	m_quad = quad::create(glm::vec2(width, height));
	s_active = false;
}

intro_screen::~intro_screen()
{}

void intro_screen::on_update(const engine::timestep& time_step)
{
	if (!s_active)
		return;

	m_timer += (float)time_step;

}

void intro_screen::on_render(engine::ref<engine::shader> shader)
{
	if (!s_active)
		return;

	glm::mat4 transform(1.0f);
	transform = glm::translate(transform, glm::vec3(0.f, 0.f, 0.1f));

	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", m_transparency);
	m_texture->bind();
	engine::renderer::submit(shader, m_quad->mesh(), transform);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", 1.0f);
}

void intro_screen::activate()
{
	s_active = true;
	m_transparency = 1.0f;
	m_timer = 0.0f;
}

void intro_screen::deactivate()
{
	s_active = false;
}

engine::ref<intro_screen> intro_screen::create(const std::string& path, float max_time, float width, float height)
{
	return std::make_shared<intro_screen>(path, max_time, width, height);
}
