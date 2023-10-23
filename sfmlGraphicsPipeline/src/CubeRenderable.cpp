/*#include "./../include/CubeRenderable.hpp"
#include "./../include/gl_helper.hpp"
#include "./../include/log.hpp"
#include "./../include/Utils.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>


CubeRenderable::CubeRenderable(ShaderProgramPtr shaderProgram)
  : Renderable(shaderProgram), m_vBuffer(0), m_list_colorsBuffer(0)
{
	// Build the geometry : just a simple triangle for now
	m_positions.push_back( glm::vec3 (0 ,0 ,0) );
	m_positions.push_back( glm::vec3 (0 ,0 ,1) );
	m_positions.push_back( glm::vec3 (-1 ,0 ,0) );

	m_positions2.push_back( glm::vec3 (-1 ,0 ,0) );
	m_positions2.push_back( glm::vec3 (1 ,0 ,0) );
	m_positions2.push_back( glm::vec3 (0 ,1 ,0) );


	m_positions.push_back( glm::vec3 (-1 ,0 ,0) );
	m_positions.push_back( glm::vec3 (1 ,0 ,0) );
	m_positions.push_back( glm::vec3 (0 ,1 ,0) );
	m_positions.push_back( glm::vec3 (-1 ,0 ,0) );
	m_positions.push_back( glm::vec3 (1 ,0 ,0) );
	m_positions.push_back( glm::vec3 (0 ,1 ,0) );
	m_positions.push_back( glm::vec3 (-1 ,0 ,0) );
	m_positions.push_back( glm::vec3 (1 ,0 ,0) );

	// Build the color
	m_list_colors.push_back (glm::vec4(1,0,0,1));
	m_list_colors.push_back (glm::vec4(0,1,0,1));
	m_list_colors.push_back (glm::vec4(0,0,1,1));

	// Set the model matrix to identity
	m_model = glm::mat4(1.0);

	//Create buffers
	glGenBuffers(1, &m_vBuffer); //vertices

	//Activate buffer and send data to the graphics card
	glBindBuffer(GL_ARRAY_BUFFER, m_vBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_positions.size()*sizeof(glm::vec3), m_positions.data(), GL_STATIC_DRAW);

	//Same thing for the colors
	glGenBuffers(1, &m_list_colorsBuffer); //vertices

	glBindBuffer(GL_ARRAY_BUFFER, m_list_colorsBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_list_colors.size()*sizeof(glm::vec4), m_list_colors.data(), GL_STATIC_DRAW);
}

void CubeRenderable::do_draw()
{
	// Get the identifier ( location ) of the uniform modelMat in the shader program
	int modelLocation = m_shaderProgram->getUniformLocation("modelMat");
	// Send the data corresponding to this identifier on the GPU
	glUniformMatrix4fv( modelLocation , 1, GL_FALSE , glm::value_ptr( m_model ));

	// Get the identifier of the attribute vPosition in the shader program
	int positionLocation = m_shaderProgram->getAttributeLocation("vPosition");
	// Activate the attribute array at this location
	glEnableVertexAttribArray( positionLocation );
	// Bind the position buffer on the GL_ARRAY_BUFFER target
	glBindBuffer( GL_ARRAY_BUFFER , m_vBuffer );
	// Specify the location and the format of the vertex position attribute
	glVertexAttribPointer( positionLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);



	// Draw the triangles
	glDrawArrays( GL_TRIANGLES, 0, m_positions.size());

	// Release the vertex attribute array
	glDisableVertexAttribArray( positionLocation );


	
//Same thing for colors

	int colorLocation = m_shaderProgram->getAttributeLocation("vertexColor");
	glEnableVertexAttribArray( colorLocation );
	glBindBuffer( GL_ARRAY_BUFFER , m_list_colorsBuffer );
	glVertexAttribPointer( colorLocation, 4	, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glDisableVertexAttribArray( colorLocation );
}

CubeRenderable::~CubeRenderable()
{
    glcheck(glDeleteBuffers(1, &m_vBuffer));
    glcheck(glDeleteBuffers(1, &m_list_colorsBuffer));

}*/

#include "./../include/CubeRenderable.hpp"
#include "./../include/gl_helper.hpp"
#include "./../include/log.hpp"
#include "./../include/Utils.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>


CubeRenderable::CubeRenderable(ShaderProgramPtr shaderProgram)
  : Renderable(shaderProgram),
    m_pBuffer(0), m_cBuffer(0), m_nBuffer(0)
{
    std::vector<glm::vec2> tmp_tex;
    getUnitCube(m_positions, m_normals, tmp_tex);

    //Assign one color to each face
    m_colors.push_back(glm::vec4(1,0,0,1));
    m_colors.push_back(glm::vec4(1,0,0,1));
    m_colors.push_back(glm::vec4(1,0,0,1));

    m_colors.push_back(glm::vec4(0,1,0,1));
    m_colors.push_back(glm::vec4(0,1,0,1));
    m_colors.push_back(glm::vec4(0,1,0,1));
	
    m_colors.push_back(glm::vec4(0,0,1,1));
    m_colors.push_back(glm::vec4(0,0,1,1));
    m_colors.push_back(glm::vec4(0,0,1,1));

    m_colors.push_back(glm::vec4(0,1,1,1));
    m_colors.push_back(glm::vec4(0,1,1,1));
    m_colors.push_back(glm::vec4(0,1,1,1));

    m_colors.push_back(glm::vec4(1,0,1,1));
    m_colors.push_back(glm::vec4(1,0,1,1));
    m_colors.push_back(glm::vec4(1,0,1,1));

    m_colors.push_back(glm::vec4(1,1,0,1));
    m_colors.push_back(glm::vec4(1,1,0,1));
    m_colors.push_back(glm::vec4(1,1,0,1));

    m_colors.push_back(glm::vec4(0,0.5,0.5,1));
    m_colors.push_back(glm::vec4(0,0.5,0.5,1));
    m_colors.push_back(glm::vec4(0,0.5,0.5,1));

    m_colors.push_back(glm::vec4(0.5,0,0.5,1));
    m_colors.push_back(glm::vec4(0.5,0,0.5,1));
    m_colors.push_back(glm::vec4(0.5,0,0.5,1));

    m_colors.push_back(glm::vec4(0.5,0.5,0,1));
    m_colors.push_back(glm::vec4(0.5,0.5,0,1));
    m_colors.push_back(glm::vec4(0.5,0.5,0,1));

    m_colors.push_back(glm::vec4(0.8,0.2,0,1));
    m_colors.push_back(glm::vec4(0.8,0.2,0,1));
    m_colors.push_back(glm::vec4(0.8,0.2,0,1));

    m_colors.push_back(glm::vec4(0.2,0,0.8,1));
    m_colors.push_back(glm::vec4(0.2,0,0.8,1));
    m_colors.push_back(glm::vec4(0.2,0,0.8,1));

    m_colors.push_back(glm::vec4(0,0.8,0.2,1));
    m_colors.push_back(glm::vec4(0,0.8,0.2,1));
    m_colors.push_back(glm::vec4(0,0.8,0.2,1));

    //Create buffers
    glGenBuffers(1, &m_pBuffer); //vertices
    glGenBuffers(1, &m_cBuffer); //colors
    glGenBuffers(1, &m_nBuffer); //normals

    //Activate buffer and send data to the graphics card
    glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_pBuffer));
    glcheck(glBufferData(GL_ARRAY_BUFFER, m_positions.size()*sizeof(glm::vec3), m_positions.data(), GL_STATIC_DRAW));
    glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_cBuffer));
    glcheck(glBufferData(GL_ARRAY_BUFFER, m_colors.size()*sizeof(glm::vec4), m_colors.data(), GL_STATIC_DRAW));
    glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_nBuffer));
    glcheck(glBufferData(GL_ARRAY_BUFFER, m_normals.size()*sizeof(glm::vec3), m_normals.data(), GL_STATIC_DRAW));
}

void CubeRenderable::do_draw()
{
    //Location
    int positionLocation = m_shaderProgram->getAttributeLocation("vPosition");
    int colorLocation = m_shaderProgram->getAttributeLocation("vColor");
    int normalLocation = m_shaderProgram->getAttributeLocation("vNormal");
    int modelLocation = m_shaderProgram->getUniformLocation("modelMat");

    //Send data to GPU
    if(modelLocation != ShaderProgram::null_location)
    {
        glcheck(glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(getModelMatrix())));
    }

    if(positionLocation != ShaderProgram::null_location)
    {
        //Activate location
        glcheck(glEnableVertexAttribArray(positionLocation));
        //Bind buffer
        glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_pBuffer));
        //Specify internal format
        glcheck(glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));
    }

    if(colorLocation != ShaderProgram::null_location)
    {
        glcheck(glEnableVertexAttribArray(colorLocation));
        glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_cBuffer));
        glcheck(glVertexAttribPointer(colorLocation, 4, GL_FLOAT, GL_FALSE, 0, (void*)0));
    }

    if(normalLocation != ShaderProgram::null_location)
    {
        glcheck(glEnableVertexAttribArray(normalLocation));
        glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_nBuffer));
        glcheck(glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));
    }

    //Draw triangles elements
    glcheck(glDrawArrays(GL_TRIANGLES,0, m_positions.size()));

    if(positionLocation != ShaderProgram::null_location)
    {
        glcheck(glDisableVertexAttribArray(positionLocation));
    }
    if(colorLocation != ShaderProgram::null_location)
    {
        glcheck(glDisableVertexAttribArray(colorLocation));
    }
    if(normalLocation != ShaderProgram::null_location)
    {
        glcheck(glDisableVertexAttribArray(normalLocation));
    }
}

CubeRenderable::~CubeRenderable()
{
    glcheck(glDeleteBuffers(1, &m_pBuffer));
    glcheck(glDeleteBuffers(1, &m_cBuffer));
    glcheck(glDeleteBuffers(1, &m_nBuffer));
}