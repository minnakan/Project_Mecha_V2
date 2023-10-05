#pragma once
namespace engine
{
	class mesh;

	///\brief Class creating a tetrahedron object with a mesh of a specified size

	class octahedron {
	public:
		///\brief Constructor
		octahedron(std::vector<glm::vec3>vertices);

		///\briefDestructor
		~octahedron();

		///\briefGettermethods
		std::vector<glm::vec3>vertices()const { return m_vertices; }
		ref<engine::mesh>mesh()const { return m_mesh; }

		static ref<octahedron>create(std::vector<glm::vec3>vertices);
	private:
		std::vector<glm::vec3>m_vertices;

		ref<engine::mesh>m_mesh;
	};
}
