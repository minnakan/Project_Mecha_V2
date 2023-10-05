#include"enemy.h"

enemy::enemy(const engine::game_object_properties props) :engine::game_object(props)
{}

enemy::~enemy()
{}

void enemy::init()
{
	m_is_active = true;
	m_damage_done = false;

}
void enemy::update(glm::vec3 c, float dt)
{
	set_position(m_next_move);
	if (glm::distance(c, position()) < 2.0f) {
		m_is_active = false;
		m_damage_done = true;
	}
	
}

void enemy::reset_spider()
{
	m_damage_done = false;
	m_is_active = true;
}

void enemy::set_destination(glm::vec3 destination) {
	dst_xpos = destination.x;
	dst_zpos = destination.z;
}

glm::vec3 enemy::next_move(glm::vec3 src,glm::vec3 dst) {

	//source
	 src_xpos = src.x;
	 src_zpos = src.z;

	//destination
	 dst_xpos = dst.x;
	 dst_zpos = dst.z;

	 //calculating next move in the x direction
	 if (dst_xpos > src_xpos) {
		nxt_xpos = src_xpos +0.03f;
	 }
	 //calculating next move in the z direction
	 if (dst_zpos > src_zpos) {
		 nxt_zpos = src_zpos  +0.03f;
	 }
	 //calculating next move in the x direction
	 if (dst_xpos < src_xpos) {
		 nxt_xpos = src_xpos - 0.03f;
	 }
	 //calculating next move in the z direction
	 if (dst_zpos < src_zpos) {
		 nxt_zpos = nxt_zpos - 0.03f;
	 }
	 m_next_move = { nxt_xpos,0.8,nxt_zpos };
	 return m_next_move;
}

engine::ref<enemy>enemy::create(const engine::game_object_properties& props)
{
	return std::make_shared<enemy>(props);
}



