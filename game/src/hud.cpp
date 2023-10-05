#include "pch.h"
#include "hud.h"
#include "quad.h"

hud::hud(const std::string& path, float max_time, float width, float height)
{
	m_max_time = max_time;
	m_texture = engine::texture_2d::create(path, true);

	m_transparency = 0.0f;

	m_quad = quad::create(glm::vec2(width, height));
	s_active = false;
}

hud::~hud()
{}

void hud::on_update(const engine::timestep& time_step)
{
	if (!s_active)
		return;

	m_timer += (float)time_step;

}

void hud::on_render(engine::ref<engine::shader> shader)
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

void hud::activate()
{
	s_active = true;
	m_transparency = 1.0f;
	m_timer = 0.0f;
}

void hud::deactivate()
{
	s_active = false;
}

engine::ref<hud> hud::create(const std::string& path, float max_time, float width, float height)
{
	return std::make_shared<hud>(path, max_time, width, height);
}
