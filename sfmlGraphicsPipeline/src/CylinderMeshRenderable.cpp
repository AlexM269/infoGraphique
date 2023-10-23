#include "./../include/CylinderMeshRenderable.hpp"
#include "./../include/gl_helper.hpp"
#include "./../include/log.hpp"
#include "./../include/Utils.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>

CylinderMeshRenderable::~CylinderMeshRenderable()
{}

CylinderMeshRenderable::CylinderMeshRenderable(ShaderProgramPtr shaderProgram, bool indexed, unsigned int slices, bool vertex_normals) :
    MeshRenderable(shaderProgram, indexed)
{
    if (indexed){
        std::vector<glm::uvec3> indices;
        getUnitIndexedCylinder(m_positions, m_normals, indices, 20);
        // getUniIndexedCube fills a std::vector<glm::uvec3> of length n,
        // but m_indices is a std::vector<unsigned int> of length 3n.
        // We need to unpack the values.
        unpack(indices, m_indices);

        // Set random colors per vertex
        m_colors.resize(m_positions.size(), glm::vec4(0));
        for (size_t i=0; i<m_colors.size(); ++i)
            m_colors[i] = randomColor();
        
    }else{
        // Go to Utils.cpp and fill this function
        getUnitCylinder(m_positions, m_normals, m_tcoords, slices, vertex_normals);
        // Set random colors per triangle
        m_colors.resize(m_positions.size(), glm::vec4(0));


        glm::vec4 top_col = glm::vec4(1,0,1,1);
        glm::vec4 lat_col = glm::vec4(0,0,1,1);
        glm::vec4 bot_col = glm::vec4(0,1,0,1);


        for (size_t i=0; i<m_colors.size()/12 ; ++i){
        size_t voffset = 12 * i; // 4 x 3 = 12 vertices per slice


        // top triangle
        m_colors[ voffset + 0 ] = top_col;
        m_colors[ voffset + 1 ] = top_col;
        m_colors[ voffset + 2 ] = top_col;


        // side triangles
        m_colors[ voffset + 3 ] = lat_col;
        m_colors[ voffset + 4 ] = lat_col;
        m_colors[ voffset + 5 ] = lat_col;
        m_colors[ voffset + 6 ] = lat_col;
        m_colors[ voffset + 7 ] = lat_col;
        m_colors[ voffset + 8 ] = lat_col;


        // bottom triangle
        m_colors[ voffset + 9 ] = bot_col;
        m_colors[ voffset + 10 ] = bot_col;
        m_colors[ voffset + 11 ] = bot_col;
        }

        /*for (size_t i=0; i<m_colors.size() / 3; ++i){
            glm::vec4 color = randomColor();
            m_colors[ 3 * i + 0 ] = color;
            m_colors[ 3 * i + 1 ] = color;
            m_colors[ 3 * i + 2 ] = color;
        }*/
    }

    // See MeshRenderable::update_all_buffers
    update_all_buffers();
}