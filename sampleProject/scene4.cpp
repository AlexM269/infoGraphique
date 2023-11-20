#include <Viewer.hpp>
#include <ShaderProgram.hpp>

#include <texturing/TexturedPlaneRenderable.hpp>
#include <texturing/TexturedCubeRenderable.hpp>
#include <texturing/TexturedMeshRenderable.hpp>
#include <texturing/MultiTexturedCubeRenderable.hpp>
#include <texturing/MipMapCubeRenderable.hpp>
#include <texturing/BillBoardPlaneRenderable.hpp>
#include <lighting/DirectionalLightRenderable.hpp>
#include <texturing/TexturedTriangleRenderable.hpp>
#include <texturing/CubeMapRenderable.hpp>
#include <FrameRenderable.hpp>
#include <lighting/SpotLightRenderable.hpp>
#include <lighting/LightedMeshRenderable.hpp>

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <dirent.h>

#include <iostream>
#include <string>

//COFFRE AUX TRESORS//


void initialize_scene( Viewer& viewer )
{
    //Position the camera

    // Lumière directionnelle du jour pour l'ensemble de la scène
    ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/flatVertex.glsl",
                                                                        "../../sfmlGraphicsPipeline/shaders/flatFragment.glsl");
    viewer.addShaderProgram( flatShader );
    glm::vec3 d_direction = glm::normalize(glm::vec3(0.0,-1.0,2.0));
    glm::vec3 d_ambient(1.7,1.5,2.5), d_diffuse(2.0,2.0,2.0), d_specular(1.0,1.5,1.5);
    glm::vec3 lightPosition(0.0,50,0.0);
    DirectionalLightPtr directionalLight = std::make_shared<DirectionalLight>(d_direction, d_ambient, d_diffuse, d_specular);
    DirectionalLightRenderablePtr directionalLightRenderable = std::make_shared<DirectionalLightRenderable>(flatShader, directionalLight);
    directionalLight->setGlobalTransform(getTranslationMatrix(lightPosition) * directionalLight->getGlobalTransform());
    viewer.addDirectionalLight(directionalLight);
    viewer.addRenderable(directionalLightRenderable);

        glm::vec3 cible1 = glm::vec3(-36,4,39);
        float s_innerCutOff=std::cos(glm::radians(7.0f)), s_outerCutOff=std::cos(glm::radians(16.0f));
        auto spot_light = std::make_shared<SpotLight>(glm::vec3(-36,8,40), cible1, glm::vec3(6.0), glm::vec3(18.0), glm::vec3(5.0), 1, 0.4, 0.4, s_innerCutOff, s_outerCutOff);

        spot_light->addGlobalTransformKeyframe(lookAtModel(glm::vec3(-36,8,39), cible1, Light::base_forward), 0);
        spot_light->addGlobalTransformKeyframe(lookAtModel(glm::vec3(-42,8,39), cible1+glm::vec3(-3.0,0.0,1.0), Light::base_forward), 1);
        spot_light->addGlobalTransformKeyframe(lookAtModel(glm::vec3(-46,8,39), cible1+glm::vec3(-8.0,0.0,0.0), Light::base_forward), 2);
        spot_light->addGlobalTransformKeyframe(lookAtModel(glm::vec3(-42,8,39), cible1+glm::vec3(-3.0,0.0,1.0), Light::base_forward), 3);
        spot_light->addGlobalTransformKeyframe(lookAtModel(glm::vec3(-36,8,39), cible1, Light::base_forward), 4);
        viewer.addSpotLight(spot_light);

        glm::vec3 cible2 = glm::vec3(-44,4,42);
        auto spot_light2 = std::make_shared<SpotLight>(glm::vec3(-44,8,42), cible2, glm::vec3(6.0), glm::vec3(18.0), glm::vec3(5.0), 1, 0.4, 0.4, s_innerCutOff, s_outerCutOff);

        spot_light2->addGlobalTransformKeyframe(lookAtModel(glm::vec3(-44,8,42), cible2, Light::base_forward), 0);
        spot_light2->addGlobalTransformKeyframe(lookAtModel(glm::vec3(-41,8,42), cible2+glm::vec3(3.0,0.0,0.0), Light::base_forward), 1);
        spot_light2->addGlobalTransformKeyframe(lookAtModel(glm::vec3(-37,8,42), cible2+glm::vec3(7.0,0.0,-1.0), Light::base_forward), 2);
        spot_light2->addGlobalTransformKeyframe(lookAtModel(glm::vec3(-41,8,42), cible2+glm::vec3(3.0,0.0,0.0), Light::base_forward), 3);
        spot_light2->addGlobalTransformKeyframe(lookAtModel(glm::vec3(-44,8,42), cible2, Light::base_forward), 4);
        viewer.addSpotLight(spot_light2);

        float mini_s_innerCutOff=std::cos(glm::radians(4.0f)), mini_s_outerCutOff=std::cos(glm::radians(9.0f));
        auto mini_spot_light = std::make_shared<SpotLight>(glm::vec3(-36,8,39), cible1, glm::vec3(1.0), glm::vec3(16.0), glm::vec3(6.0), 1, 0.4, 0.4, mini_s_innerCutOff, mini_s_outerCutOff);

        mini_spot_light->addGlobalTransformKeyframe(lookAtModel(glm::vec3(-36,8,39), cible1+glm::vec3(-2.0,0.0,0.0), Light::base_forward), 0);
        mini_spot_light->addGlobalTransformKeyframe(lookAtModel(glm::vec3(-36,8,39), cible1+glm::vec3(0.0,0.0,0.0), Light::base_forward), 1);
        mini_spot_light->addGlobalTransformKeyframe(lookAtModel(glm::vec3(-36,8,39), cible1+glm::vec3(0.0,0.0,3.0), Light::base_forward), 2);
        mini_spot_light->addGlobalTransformKeyframe(lookAtModel(glm::vec3(-36,8,39), cible1+glm::vec3(-2.0,0.0,3.0), Light::base_forward), 3);
        mini_spot_light->addGlobalTransformKeyframe(lookAtModel(glm::vec3(-36,8,39), cible1+glm::vec3(-2.0,0.0,0.0), Light::base_forward), 4);
       viewer.addSpotLight(mini_spot_light);

        auto mini_spot_light2 = std::make_shared<SpotLight>(glm::vec3(-44,8,39), cible2, glm::vec3(1.0), glm::vec3(16.0), glm::vec3(6.0), 1, 0.4, 0.4, mini_s_innerCutOff, mini_s_outerCutOff);

        mini_spot_light2->addGlobalTransformKeyframe(lookAtModel(glm::vec3(-44,8,42), cible2+glm::vec3(0.0,0.0,0.0), Light::base_forward), 0);
        mini_spot_light2->addGlobalTransformKeyframe(lookAtModel(glm::vec3(-44,8,42), cible2+glm::vec3(2.0,0.0,0.0), Light::base_forward), 1);
        mini_spot_light2->addGlobalTransformKeyframe(lookAtModel(glm::vec3(-44,8,42), cible2+glm::vec3(2.0,0.0,-2.0), Light::base_forward), 2);
        mini_spot_light2->addGlobalTransformKeyframe(lookAtModel(glm::vec3(-44,8,42), cible2+glm::vec3(0.0,0.0,-2.0), Light::base_forward), 3);
        mini_spot_light2->addGlobalTransformKeyframe(lookAtModel(glm::vec3(-44,8,42), cible2+glm::vec3(0.0,0.0,0.0), Light::base_forward), 4);
       viewer.addSpotLight(mini_spot_light2);

        glm::vec3 cible3 = glm::vec3(-40.5,4,40);
        auto mini_spot_light3 = std::make_shared<SpotLight>(glm::vec3(-40,8,40), cible3, glm::vec3(1.0), glm::vec3(18.0), glm::vec3(6.0), 1, 0.4, 0.4, std::cos(glm::radians(6.0f)), std::cos(glm::radians(13.0f)));

        mini_spot_light3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(-40,8,40), cible3+glm::vec3(0.0,0.0,0.0), Light::base_forward), 0);
        mini_spot_light3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(-40,8,39), cible3+glm::vec3(0.0,0.0,-2.0), Light::base_forward), 1);
        mini_spot_light3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(-40,8,40), cible3+glm::vec3(0.0,0.0,0.0), Light::base_forward), 2);
        mini_spot_light3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(-40,8,42), cible3+glm::vec3(0.0,0.0,3.0), Light::base_forward), 3);
        mini_spot_light3->addGlobalTransformKeyframe(lookAtModel(glm::vec3(-40,8,40), cible3+glm::vec3(0.0,0.0,0.0), Light::base_forward), 4);
        viewer.addSpotLight(mini_spot_light3);

        viewer.startAnimation();
        // Caméra positionnée à une hauteur de 10 (1er paramètre) et fixant une target à hauteur 10 également (2ème paramètre)
        viewer.getCamera().setViewMatrix( glm::lookAt( glm::vec3(-52, 12, 50), glm::vec3(5, -15, 0), glm::vec3( 1, 1, 0 ) ) );
}

void createWater(Viewer& viewer){ // Skybox du décor panoramique
    ShaderProgramPtr cubeMapShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/cubeMapVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/cubeMapFragment.glsl");
    viewer.addShaderProgram(cubeMapShader);
    std::string cubemap_dir = "../../sfmlGraphicsPipeline/textures/CubeMap_Water";
    auto cubemap = std::make_shared<CubeMapRenderable>(cubeMapShader, cubemap_dir);
    viewer.addRenderable(cubemap);
}

void createTreasure(Viewer& viewer){
    // Coffre en bois
    ShaderProgramPtr texShader = std::make_shared<ShaderProgram>(   "../../sfmlGraphicsPipeline/shaders/simpleTextureVertex.glsl",
                                                                       "../../sfmlGraphicsPipeline/shaders/simpleTextureFragment.glsl");
    viewer.addShaderProgram( texShader );
    std::string treasure_mesh = "./../../sfmlGraphicsPipeline/meshes/chest.obj";
    std::string treasure_text = "./../../sfmlGraphicsPipeline/textures/chest.jpg";
    auto treasure = std::make_shared<TexturedMeshRenderable>(texShader, treasure_mesh, treasure_text);
    treasure->updateTextureOption(0); // 0 pour mettre à GL_CLAMP_TO_EDGE au lieu de MIRROR_REPEAT
    treasure->setGlobalTransform(getTranslationMatrix(-40,5,40)*getRotationMatrix(M_PI/85,0,0,1.0)*getScaleMatrix(0.05));
    viewer.addRenderable(treasure);
    // Lingots d'or illuminés

    ShaderProgramPtr phongShader = std::make_shared<ShaderProgram>( "../../sfmlGraphicsPipeline/shaders/phongVertex.glsl",
                                                                     "../../sfmlGraphicsPipeline/shaders/phongFragment.glsl");
    viewer.addShaderProgram( phongShader );

    std::string lingot_mesh = "./../../sfmlGraphicsPipeline/meshes/lingo.obj";
    MaterialPtr gold = Material::Gold();
    LightedMeshRenderablePtr lingots1 = std::make_shared<LightedMeshRenderable>(phongShader, lingot_mesh, gold);
    lingots1->setGlobalTransform(getTranslationMatrix(-41,3.2,38.8)*getRotationMatrix(M_PI/85,0,0,1.0)*getRotationMatrix(M_PI/2,0,1.0,0.0)*getScaleMatrix(0.37,0.85,0.9));
    viewer.addRenderable(lingots1);
    LightedMeshRenderablePtr lingots2 = std::make_shared<LightedMeshRenderable>(phongShader, lingot_mesh, gold);
    lingots2->setGlobalTransform(getTranslationMatrix(-41,3.2,41.9)*getRotationMatrix(M_PI/85,0,0,1.0)*getRotationMatrix(M_PI/2,0,1.0,0.0)*getScaleMatrix(0.35,0.95,0.9));
    viewer.addRenderable(lingots2);
}

void createEpave(Viewer& viewer){
    ShaderProgramPtr texShader = std::make_shared<ShaderProgram>(   "../../sfmlGraphicsPipeline/shaders/simpleTextureVertex.glsl",
                                                                       "../../sfmlGraphicsPipeline/shaders/simpleTextureFragment.glsl");
    viewer.addShaderProgram( texShader );
	std::string ship_mesh_path = "./../../sfmlGraphicsPipeline/meshes/ship_ep.obj";
    std::string ship_texture_path = "./../../sfmlGraphicsPipeline/textures/ship_epave.png";
    auto ship = std::make_shared<TexturedMeshRenderable>(texShader, ship_mesh_path, ship_texture_path);
    ship->updateTextureOption(0); // 0 pour mettre à GL_CLAMP_TO_EDGE au lieu de MIRROR_REPEAT
    ship->setGlobalTransform(getTranslationMatrix(0,-30,0)*getRotationMatrix(3*M_PI/4,0,1,0)*getRotationMatrix(-M_PI/8,0,0,1.0)*getScaleMatrix(60));
    viewer.addRenderable(ship);
}

void initialize_seal(Viewer& viewer){ // Creation du phoque qui regarde le coffre

    //Instanciation du shader pour les objets non rigides
    ShaderProgramPtr NonRigidSealShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/NonRigidSealVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/NonRigidSealFragment.glsl");

    //Ajout du fixedNonRigidSealShader au viewer 
    viewer.addShaderProgram(NonRigidSealShader);
    
    //Initialisation du mesh et de la texture des phoques endormis
    std::string seal_final_mesh_path = "../../sfmlGraphicsPipeline/meshes/seal_final.obj";
    std::string seal_final_texture_path = "../../sfmlGraphicsPipeline/textures/seal/seal_texture_4.png";

    //Initialisation du phoque principal et de sa postion
    auto seal_final = std::make_shared<TexturedMeshRenderable>(NonRigidSealShader, seal_final_mesh_path, seal_final_texture_path);
    //A l'origine (-41,6,41) et PI/2(0,1,0)
    //seal_final->setGlobalTransform(getTranslationMatrix(-25,4.5,58)*getRotationMatrix(-3*M_PI/4,0,1,0)*getScaleMatrix(20,20,20));

    seal_final->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(200,7,-100),
    glm::angleAxis(float(-M_PI/6),glm::vec3(0,1,0)), glm::vec3(14,14,14)),0);

    //Etape pour rotation
    seal_final->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(9,7,34),
    glm::angleAxis(float(-M_PI/4),glm::vec3(0,1,0)), glm::vec3(14,14,14)),8.5);   

    //Arrivée devant le coffre
    seal_final->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-25,4.5,50),
    glm::angleAxis(float(-3*M_PI/4),glm::vec3(0,1,0)), glm::vec3(14,14,14)),10.5);

    //Arrêt
    seal_final->addGlobalTransformKeyframe(GeometricTransformation(glm::vec3(-25,4.5,50),
    glm::angleAxis(float(-3*M_PI/4),glm::vec3(0,1,0)), glm::vec3(14,14,14)),18);


    viewer.addRenderable(seal_final);
    viewer.startAnimation();
}

int main()
{
	Viewer viewer(1280,720, glm::vec4(0.8, 0.8, 0.8, 1.0));
	initialize_scene(viewer);
	createWater(viewer);
	createTreasure(viewer);
	createEpave(viewer);
    initialize_seal(viewer);

	while( viewer.isRunning() )
	{
		viewer.handleEvent();
		viewer.animate();
		viewer.draw();
		viewer.display();
	}

	return EXIT_SUCCESS;
}
