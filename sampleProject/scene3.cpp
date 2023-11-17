
#include <Viewer.hpp>
#include <ShaderProgram.hpp>
#include <MeshRenderable.hpp>
#include <texturing/FlagRenderable.hpp>
#include <dynamics/ConstantForceField.hpp>
#include <dynamics/DynamicSystemRenderable.hpp>
#include <dynamics/EulerExplicitSolver.hpp>
#include <dynamics/DampingForceField.hpp>
#include <texturing/TexturedPlaneRenderable.hpp>
#include <texturing/MipMapCubeRenderable.hpp>
#include <texturing/CubeMapRenderable.hpp>
#include <lighting/LightedMeshRenderable.hpp>
#include <lighting/PointLightRenderable.hpp>
#include <SFML/System/Clock.hpp>





void createSardine(Viewer& viewer, const std::string sardine_path,glm::mat4 positionInit,glm::mat4 positionEnd);
void createWater(Viewer& viewer);
void createTreasure(Viewer& viewer);
void createEpave(Viewer& viewer, DynamicSystemPtr& system, DynamicSystemRenderablePtr &systemRenderable);
void movingCamera(Viewer& viewer, float time);


void initialize_scene( Viewer& viewer,DynamicSystemPtr system )
{

    //Initialize a dynamic system (Solver, Time step, Restitution coefficient)
   // DynamicSystemPtr system = std::make_shared<DynamicSystem>();
    EulerExplicitSolverPtr solver = std::make_shared<EulerExplicitSolver>();
    system->setSolver(solver);
    system->setDt(0.001);

    //Create a renderable associated to the dynamic system
    DynamicSystemRenderablePtr systemRenderable = std::make_shared<DynamicSystemRenderable>(system);
    viewer.addRenderable(systemRenderable);


    createEpave(viewer,system,systemRenderable);
    createSardine(viewer, "../../sfmlGraphicsPipeline/meshes/sardine.obj",getTranslationMatrix(-10,5,10),getTranslationMatrix(-10,5,-50));
    viewer.startAnimation();
    system->setDt(5e-4);


     // Lumière directionnelle illuminer tous les lingots
    ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/flatVertex.glsl",
                                                                        "../../sfmlGraphicsPipeline/shaders/flatFragment.glsl");
    viewer.addShaderProgram( flatShader );

    glm::vec3 d_direction = glm::normalize(glm::vec3(0.0,-1.0,2.0));
    glm::vec3 d_ambient(2,2,2.5), d_diffuse(2,2,2.5), d_specular(2,2,3);
    glm::vec3 lightPosition(0.0,50,0.0);
    DirectionalLightPtr directionalLight = std::make_shared<DirectionalLight>(d_direction, d_ambient, d_diffuse, d_specular);
    directionalLight->setGlobalTransform(getTranslationMatrix(lightPosition) * directionalLight->getGlobalTransform());
    viewer.addDirectionalLight(directionalLight);

    //viewer.getCamera().setViewMatrix(glm::lookAt(glm::vec3(-5,18,0),glm::vec3(-40,18,5), glm::vec3(0, 1, 0)));

    
}

int main() 
{
    DynamicSystemPtr system = std::make_shared<DynamicSystem>();
    glm::vec4 background_color(0.8,0.8,0.8,1);
	Viewer viewer(1280,720, background_color);
	initialize_scene(viewer,system);
    createWater(viewer);
	createTreasure(viewer);
    float animationTime = 0.0f;

	while( viewer.isRunning() )
	{
		viewer.handleEvent();
        movingCamera(viewer, animationTime);
		viewer.animate();
		viewer.draw();
		viewer.display();
        animationTime += 0.0017f; // Ajustez le pas de temps en conséquence
	}	

	return EXIT_SUCCESS;
}

void movingCamera(Viewer& viewer, float time)
{

    static float angle = 0.0f; // Angle de rotation initial
    glm::vec3 cameraPosition(30, 30, 30); // Position initiale de la caméra
    glm::vec3 lookAtPoint(-0.8,2.5,2); // Point autour duquel faire tourner la caméra
    float rotationSpeed = glm::radians(360.0f / 3.0f);
    
     if (time < 5.0f) {
    // tourner la caméra autour du point (0,0,0)
    angle += rotationSpeed * time*10e-5;

    if (angle >= glm::radians(360.0f)) {
        // Arrêt de la rotation après un tour complet (360 degrés)
        angle = glm::radians(360.0f);
    }

    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0));
    cameraPosition = glm::vec3(rotationMatrix * glm::vec4(cameraPosition - lookAtPoint, 1.0)) + lookAtPoint;

  // Accélération vers le point (0, 5, 5)
    glm::vec3 targetPosition(0, 5, 5);
    float acceleration = 1.0f; // Vous pouvez ajuster cette valeur pour contrôler l'accélération

    // Calcul de la direction et de la distance à la cible
    glm::vec3 direction = glm::normalize(targetPosition - cameraPosition);
    float distance = glm::length(targetPosition - cameraPosition);

    // Calcul de la vitesse en fonction de l'accélération
    float speed = sqrt(2 * acceleration * distance);

    // Mettre à jour la position de la caméra en fonction de la vitesse
    cameraPosition += direction * speed * time;

    viewer.getCamera().setViewMatrix(glm::lookAt(cameraPosition, lookAtPoint, glm::vec3(0, 1, 0)));

     }
    else
    {
        // Transition de la caméra vers le nouveau point de vue
        float transitionDuration = 2.0f;
        float transitionStartTime = 7.0f;
        float transitionEndTime = transitionStartTime + transitionDuration;

        if (time >= transitionStartTime && time < transitionEndTime)
        {
            float t = (time - transitionStartTime) / transitionDuration;
            glm::vec3 startViewPosition = cameraPosition;
            glm::vec3 startViewTarget = lookAtPoint;
            glm::vec3 endViewPosition(-5, 18, 0);
            glm::vec3 endViewTarget(-40, 18, 5);

            cameraPosition = glm::mix(startViewPosition, endViewPosition, t);
            lookAtPoint = glm::mix(startViewTarget, endViewTarget, t);

            viewer.getCamera().setViewMatrix(glm::lookAt(cameraPosition, lookAtPoint, glm::vec3(0, 1, 0)));
        }
    }
    

}







void createSardine(Viewer& viewer, const std::string sardine_path,glm::mat4 positionInit,glm::mat4 positionEnd){

    // Create a shader program
	ShaderProgramPtr fishShader = std::make_shared<ShaderProgram>(
        "../../sfmlGraphicsPipeline/shaders/fishVertex.glsl",
        "../../sfmlGraphicsPipeline/shaders/fishFragment.glsl");

  // Add the shader program to the viewer
    viewer.addShaderProgram( fishShader );
    std::vector<MeshRenderablePtr> tableauDeSardines;

    MeshRenderablePtr sardine1 = std::make_shared<MeshRenderable>(fishShader, sardine_path);
    tableauDeSardines.push_back(sardine1);
    MeshRenderablePtr sardine2 = std::make_shared<MeshRenderable>(fishShader, sardine_path);
    tableauDeSardines.push_back(sardine2);
    MeshRenderablePtr sardine3 = std::make_shared<MeshRenderable>(fishShader, sardine_path);
    tableauDeSardines.push_back(sardine3);
    MeshRenderablePtr sardine4 = std::make_shared<MeshRenderable>(fishShader, sardine_path);
    tableauDeSardines.push_back(sardine4);
    MeshRenderablePtr sardine5 = std::make_shared<MeshRenderable>(fishShader, sardine_path);
    tableauDeSardines.push_back(sardine5);
    MeshRenderablePtr sardine6 = std::make_shared<MeshRenderable>(fishShader, sardine_path);
    tableauDeSardines.push_back(sardine6);
    MeshRenderablePtr sardine7 = std::make_shared<MeshRenderable>(fishShader, sardine_path);
    tableauDeSardines.push_back(sardine7);
    MeshRenderablePtr sardine8 = std::make_shared<MeshRenderable>(fishShader, sardine_path);
    tableauDeSardines.push_back(sardine8);
    MeshRenderablePtr sardine9 = std::make_shared<MeshRenderable>(fishShader, sardine_path);
    tableauDeSardines.push_back(sardine9);
    MeshRenderablePtr sardine10 = std::make_shared<MeshRenderable>(fishShader, sardine_path);
    tableauDeSardines.push_back(sardine10);
    MeshRenderablePtr sardine11 = std::make_shared<MeshRenderable>(fishShader, sardine_path);
    tableauDeSardines.push_back(sardine11);
    MeshRenderablePtr sardine12 = std::make_shared<MeshRenderable>(fishShader, sardine_path);
    tableauDeSardines.push_back(sardine12);
    MeshRenderablePtr sardine13 = std::make_shared<MeshRenderable>(fishShader, sardine_path);
    tableauDeSardines.push_back(sardine13);
    MeshRenderablePtr sardine14 = std::make_shared<MeshRenderable>(fishShader, sardine_path);
    tableauDeSardines.push_back(sardine14);
    MeshRenderablePtr sardine15 = std::make_shared<MeshRenderable>(fishShader, sardine_path);
    tableauDeSardines.push_back(sardine15);
    MeshRenderablePtr sardine16 = std::make_shared<MeshRenderable>(fishShader, sardine_path);
    tableauDeSardines.push_back(sardine16);
    MeshRenderablePtr sardine17 = std::make_shared<MeshRenderable>(fishShader, sardine_path);
    tableauDeSardines.push_back(sardine17);
    MeshRenderablePtr sardine18 = std::make_shared<MeshRenderable>(fishShader, sardine_path);
    tableauDeSardines.push_back(sardine18);
    MeshRenderablePtr sardine19 = std::make_shared<MeshRenderable>(fishShader, sardine_path);
    tableauDeSardines.push_back(sardine19);
    MeshRenderablePtr sardine20 = std::make_shared<MeshRenderable>(fishShader, sardine_path);
    tableauDeSardines.push_back(sardine20);
    MeshRenderablePtr sardine21 = std::make_shared<MeshRenderable>(fishShader, sardine_path);
    tableauDeSardines.push_back(sardine21);
    MeshRenderablePtr sardine22 = std::make_shared<MeshRenderable>(fishShader, sardine_path);
    tableauDeSardines.push_back(sardine22);
    MeshRenderablePtr sardine23 = std::make_shared<MeshRenderable>(fishShader, sardine_path);
    tableauDeSardines.push_back(sardine23);
    MeshRenderablePtr sardine24 = std::make_shared<MeshRenderable>(fishShader, sardine_path);
    tableauDeSardines.push_back(sardine24);
    MeshRenderablePtr sardine25 = std::make_shared<MeshRenderable>(fishShader, sardine_path);
    tableauDeSardines.push_back(sardine25);
    MeshRenderablePtr sardine26 = std::make_shared<MeshRenderable>(fishShader, sardine_path);
    tableauDeSardines.push_back(sardine26);
    MeshRenderablePtr sardine27 = std::make_shared<MeshRenderable>(fishShader, sardine_path);
    tableauDeSardines.push_back(sardine27);
    MeshRenderablePtr sardine28 = std::make_shared<MeshRenderable>(fishShader, sardine_path);
    tableauDeSardines.push_back(sardine28);
    MeshRenderablePtr sardine29 = std::make_shared<MeshRenderable>(fishShader, sardine_path);
    tableauDeSardines.push_back(sardine29);

	//sardines
    //sardine1-> setGlobalTransform(positionInit);
	sardine2 -> setGlobalTransform(getTranslationMatrix(750,0,0));
    sardine3 -> setGlobalTransform(getTranslationMatrix(-500,0,0));
    sardine4 -> setGlobalTransform(getTranslationMatrix(500,650,0));
    sardine5 -> setGlobalTransform(getTranslationMatrix(-550,-800,0));
    sardine6 -> setGlobalTransform(getTranslationMatrix(0,0,650));
	sardine7 -> setGlobalTransform(getTranslationMatrix(-250,50,-625));
    sardine8 -> setGlobalTransform(getTranslationMatrix(-550,50,600));
    sardine9 -> setGlobalTransform(getTranslationMatrix(525,750,-650));
    sardine10 -> setGlobalTransform(getTranslationMatrix(-550,-550,725));
    sardine11 -> setGlobalTransform(getTranslationMatrix(250,0,600));
	sardine12 -> setGlobalTransform(getTranslationMatrix(700,300,100));
    sardine13 -> setGlobalTransform(getTranslationMatrix(-500,250,0));
    sardine14 -> setGlobalTransform(getTranslationMatrix(500,600,350));
    sardine15 -> setGlobalTransform(getTranslationMatrix(-400,-600,110));
    sardine16 -> setGlobalTransform(getTranslationMatrix(-300,200,600));
	sardine17 -> setGlobalTransform(getTranslationMatrix(0,450,-625));
    sardine18 -> setGlobalTransform(getTranslationMatrix(-600,0,-600));
    sardine19 -> setGlobalTransform(getTranslationMatrix(400,600,-300));
    sardine20 -> setGlobalTransform(getTranslationMatrix(-500,-250,250));
    sardine21 -> setGlobalTransform(getTranslationMatrix(400,-600,-600));
    sardine22 -> setGlobalTransform(getTranslationMatrix(-250,300,700));
    sardine23 -> setGlobalTransform(getTranslationMatrix(750,-110,-750));
    sardine24 -> setGlobalTransform(getTranslationMatrix(800,200,-700));
    sardine25 -> setGlobalTransform(getTranslationMatrix(750,-300,350));


    sardine1->addGlobalTransformKeyframe(positionInit*getScaleMatrix(0.005), 0);
    sardine1->addGlobalTransformKeyframe((positionInit+positionEnd)*getScaleMatrix(0.005), 30);
    sardine1->addGlobalTransformKeyframe((positionInit+positionEnd)*getScaleMatrix(0.005), 50);
    

     for(int i =0; i<28;i=i+2){
        tableauDeSardines[i]->addLocalTransformKeyframe(getRotationMatrix(0, 0,0,1), 0);
        for(float t = 0.5;t<=30;t++){
            tableauDeSardines[i]->addLocalTransformKeyframe(getRotationMatrix(M_PI/10,0,1,0), t);
            tableauDeSardines[i]->addLocalTransformKeyframe(getRotationMatrix(M_PI/10,0,-1,0), t+0.5);
            }
    }
    for(int i =1; i<29;i=i+2){
        tableauDeSardines[i]->addLocalTransformKeyframe(getRotationMatrix(0, 0,0,1), 0);
        for(float t = 0.25;t<=30;t++){
            tableauDeSardines[i]->addLocalTransformKeyframe(getRotationMatrix(M_PI/10,0,1,0), t);
            tableauDeSardines[i]->addLocalTransformKeyframe(getRotationMatrix(M_PI/10,0,-1,0), t+0.5);
            }
    }
    

    for(int i =0; i<28;i++){
        HierarchicalRenderable::addChild(sardine1 , tableauDeSardines[i+1]);
    }

    viewer.addRenderable(sardine1);  
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
    treasure->setGlobalTransform(getTranslationMatrix(-0.8,2.5,2)*getRotationMatrix(M_PI/85,0,0,1.0)*getScaleMatrix(0.005)*getRotationMatrix(M_PI/8,0,0,1.0)*getRotationMatrix(M_PI/3,0,1.0,0));
    viewer.addRenderable(treasure);
    // Lingots d'or illuminés

    ShaderProgramPtr phongShader = std::make_shared<ShaderProgram>( "../../sfmlGraphicsPipeline/shaders/phongVertex.glsl",
                                                                     "../../sfmlGraphicsPipeline/shaders/phongFragment.glsl");
    viewer.addShaderProgram( phongShader );

    std::string lingot_mesh = "./../../sfmlGraphicsPipeline/meshes/lingo.obj";
    MaterialPtr gold = Material::Gold();
    LightedMeshRenderablePtr lingots1 = std::make_shared<LightedMeshRenderable>(phongShader, lingot_mesh, gold);
    //lingots1->setGlobalTransform(getTranslationMatrix(-41,3.2,38.8)*getRotationMatrix(M_PI/85,0,0,1.0)*getRotationMatrix(M_PI/2,0,1.0,0.0)*getScaleMatrix(0.37,0.85,0.9));
    lingots1->setGlobalTransform(getTranslationMatrix(-0.8,2.4,1.8)*getRotationMatrix(M_PI/8,0,0,1.0)*getRotationMatrix(M_PI/6.5,0,-1,0)*getScaleMatrix(0.036,0.084,0.08));
    viewer.addRenderable(lingots1);
    LightedMeshRenderablePtr lingots2 = std::make_shared<LightedMeshRenderable>(phongShader, lingot_mesh, gold);
    lingots2->setGlobalTransform(getTranslationMatrix(-0.6,2.45,1.9)*getRotationMatrix(M_PI/8,0,0,1.0)*getRotationMatrix(M_PI/6.5,0,-1,0)*getScaleMatrix(0.036,0.084,0.08));
    viewer.addRenderable(lingots2);
    
}

void createEpave(Viewer& viewer, DynamicSystemPtr& system, DynamicSystemRenderablePtr &systemRenderable){
    ShaderProgramPtr texShader = std::make_shared<ShaderProgram>(   "../../sfmlGraphicsPipeline/shaders/simpleTextureVertex.glsl",
                                                                       "../../sfmlGraphicsPipeline/shaders/simpleTextureFragment.glsl");
    viewer.addShaderProgram( texShader );
	std::string ship_mesh_path = "./../../sfmlGraphicsPipeline/meshes/ship_ep.obj";
    std::string ship_texture_path = "./../../sfmlGraphicsPipeline/textures/ship_epave.png";
    auto ship = std::make_shared<TexturedMeshRenderable>(texShader, ship_mesh_path, ship_texture_path);
    ship->updateTextureOption(0); // 0 pour mettre à GL_CLAMP_TO_EDGE au lieu de MIRROR_REPEAT


    ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/flatVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/flatFragment.glsl");
    ShaderProgramPtr instancedShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/instancedVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/instancedFragment.glsl");
    viewer.addShaderProgram( flatShader );
    viewer.addShaderProgram( instancedShader );

    std::string texture_path = "../../sfmlGraphicsPipeline/textures/drapeau_epave.png";
    FlagRenderablePtr flag = std::make_shared<FlagRenderable>(texShader,
    0.39f, 0.39, 15, 10, 100e03, 10, texture_path);


    for (const ParticlePtr & particle : flag->getParticles())
        system->addParticle(particle);
    for (const ForceFieldPtr & force_field : flag->getSprings())
        system->addForceField(force_field);


    ConstantForceFieldPtr courantForceField = std::make_shared<ConstantForceField>(system->getParticles(), DynamicSystem::courant);
    system->addForceField( courantForceField );

    flag->setGlobalTransform(getTranslationMatrix(0.05,3.7,0.1)); 

    ship->addGlobalTransformKeyframe(getTranslationMatrix(0,0,0.0)*getScaleMatrix(5)*getRotationMatrix(M_PI,0,1,0)*getRotationMatrix(-M_PI/8,0,0,1.0),0);
    ship->addGlobalTransformKeyframe(getTranslationMatrix(0,0,0.0)*getScaleMatrix(5)*getRotationMatrix(M_PI,0,1,0)*getRotationMatrix(-M_PI/8,0,0,1.0),10);


    HierarchicalRenderable::addChild(ship,flag);
     //Initialize a force field that apply to all the particles of the system to simulate vicosity (air friction)
    float dampingCoefficient = 1.0;
    DampingForceFieldPtr dampingForceField = std::make_shared<DampingForceField>(system->getParticles(), dampingCoefficient);
    system->addForceField(dampingForceField);

    viewer.addRenderable(ship);

    viewer.stopAnimation();
}

