#pragma once
namespace engine
{
	class mesh;

	///\brief Class creating a tetrahedron object with a mesh of a specified size

	class pyramid {
	public:
		///\brief Constructor
		pyramid(std::vector<glm::vec3>vertices);

		///\briefDestructor
		~pyramid();

		///\briefGettermethods
		std::vector<glm::vec3>vertices()const { return m_vertices; }
		ref<engine::mesh>mesh()const { return m_mesh; }

		static ref<pyramid>create(std::vector<glm::vec3>vertices);
	private:
		///\briefFields//verticesofthetetrahedronspecifiedbyavectorofpositions.
		std::vector<glm::vec3>m_vertices;
		ref<engine::mesh>m_mesh;
		glm::vec3 m_half_extents;
	};
}

