
#include <Viewer.hpp>

# include <ShaderProgram.hpp>

#include <dynamics/DynamicSystemRenderable.hpp>
#include <dynamics/DampingForceField.hpp>
#include <dynamics/ConstantForceField.hpp>
#include <dynamics/SpringForceField.hpp>
#include <dynamics/EulerExplicitSolver.hpp>

#include <texturing/FlagRenderable.hpp>
#include <texturing/TexturedPlaneRenderable.hpp>

#include <texturing/TexturedMeshRenderable.hpp>
#include <texturing/CubeMapRenderable.hpp>
#include <lighting/DirectionalLightRenderable.hpp>
#include <lighting/LightedMeshRenderable.hpp>



#include <SFML/System/Clock.hpp>
#include <dirent.h>


//LAST_SCENE//


void movingShip( Viewer& viewer);
void movingCamera( Viewer& viewer, float time);
void createFlag(Viewer& viewer, DynamicSystemPtr& system, DynamicSystemRenderablePtr &systemRenderable);
void createIce(Viewer& viewer);
void createSky(Viewer& viewer);
void animateTexture(const TexturedPlaneRenderablePtr texPlane, const std::vector<std::string> textureFiles, sf::Clock &clock, int &currentTextureIndex);
std::vector<std::string> listFilesInDirectory(const std::string& directoryPath);

void initialize_scene( Viewer& viewer,DynamicSystemPtr system )
{

    EulerExplicitSolverPtr solver = std::make_shared<EulerExplicitSolver>();
    system->setSolver(solver);
    system->setDt(0.001);

    DynamicSystemRenderablePtr systemRenderable = std::make_shared<DynamicSystemRenderable>(system);
    viewer.addRenderable(systemRenderable);

    createFlag(viewer,system,systemRenderable);
    movingShip(viewer);

    viewer.startAnimation();
    system->setDt(5e-4);

    // Lumière directionnelle du jour pour l'ensemble de la scène
   ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/flatVertex.glsl",
                                                                       "../../sfmlGraphicsPipeline/shaders/flatFragment.glsl");
   viewer.addShaderProgram( flatShader );
   glm::vec3 d_direction = glm::normalize(glm::vec3(0.0,-1.0,-1.0));
   glm::vec3 d_ambient(0.9,0.85,0.8), d_diffuse(0.9,0.8,0.7), d_specular(0.8,0.8,0.8);
   glm::vec3 lightPosition(0.0,300,0.0);
   DirectionalLightPtr directionalLight = std::make_shared<DirectionalLight>(d_direction, d_ambient, d_diffuse, d_specular);
   DirectionalLightRenderablePtr directionalLightRenderable = std::make_shared<DirectionalLightRenderable>(flatShader, directionalLight);
   directionalLight->setGlobalTransform(getTranslationMatrix(lightPosition) * directionalLight->getGlobalTransform());
   viewer.addDirectionalLight(directionalLight);
   viewer.addRenderable(directionalLightRenderable);

}


int main() 
{
    DynamicSystemPtr system = std::make_shared<DynamicSystem>();
    glm::vec4 background_color(0.7,0.7,0.7,1);
	Viewer viewer(1280,720, background_color);
	initialize_scene(viewer,system);

    createSky(viewer);
    createIce(viewer);


   // Pour le plan avec texture eau animée
   ShaderProgramPtr texShader = std::make_shared<ShaderProgram>(   "../../sfmlGraphicsPipeline/shaders/simpleTextureVertex.glsl",
                                                              "../../sfmlGraphicsPipeline/shaders/simpleTextureFragment.glsl");
   viewer.addShaderProgram( texShader );
   // Vecteur contenant toutes les frames de la texture animée
   std::string directoryPath = "./../../sfmlGraphicsPipeline/textures/waves";
   std::vector<std::string> textureFiles = listFilesInDirectory(directoryPath);
   // Initialise un TexturedPlaneRenderable avec la première texture du vecteur textureFiles
   TexturedPlaneRenderablePtr texPlane = std::make_shared<TexturedPlaneRenderable>(texShader, textureFiles[0]);
   texPlane->setGlobalTransform(getRotationMatrix(M_PI/2,1,0,0)*getScaleMatrix(500));
   texPlane->updateTextureOption(1);
   viewer.addRenderable(texPlane);


   sf::Clock clock; // Minuterie pour le changement de texture
   int currentTextureIndex = 0; // Indice de l'image de texture initial


	float animationTime = 0.0f;

	while( viewer.isRunning() )
	{
		viewer.handleEvent();
		movingCamera(viewer, animationTime);
        // Anime la texture du plan avec la mer
        animateTexture(texPlane, textureFiles, clock, currentTextureIndex);
		viewer.animate();
		viewer.draw();
		viewer.display();
		animationTime += 0.001f; // Ajustez le pas de temps en conséquence

	}	

	return EXIT_SUCCESS;
}

void movingShip( Viewer& viewer )
{
    
    //Default shader
    ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/flatVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/flatFragment.glsl");
    viewer.addShaderProgram( flatShader );


    //Textured shader
    //    ShaderProgramPtr texShader = std::make_shared<ShaderProgram>("../shaders/textureVertex.glsl","../shaders/textureFragment.glsl");
    ShaderProgramPtr texShader = std::make_shared<ShaderProgram>(   "../../sfmlGraphicsPipeline/shaders/simpleTextureVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/simpleTextureFragment.glsl");
    viewer.addShaderProgram( texShader );

	//Textured ship
    //viewer.getCamera().setViewMatrix( glm::lookAt( glm::vec3(110, 20, -100 ), glm::vec3(0, 0, 0), glm::vec3( 0, 10, 0 ) ) );
    std::string ship_mesh_path = "./../../sfmlGraphicsPipeline/meshes/ship_big_mat.obj";
    std::string ship_texture_path = "./../../sfmlGraphicsPipeline/textures/ship_finished_uv.png";
    auto ship = std::make_shared<TexturedMeshRenderable>(texShader, ship_mesh_path, ship_texture_path);

	//Animated ship

    ship->setGlobalTransform(getTranslationMatrix(100,0,-100.0)*getScaleMatrix(5)*getRotationMatrix(M_PI,0,1,0));
    ship->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0),glm::quat{},glm::vec3(1,1,1)),0.0);
    ship->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0),glm::quat{},glm::vec3(1,1,1)),10.0);
    ship->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,-50),glm::quat{},glm::vec3(1,1,1)),110.0);


    viewer.addRenderable(ship);

}




void createFlag(Viewer& viewer, DynamicSystemPtr& system, DynamicSystemRenderablePtr &systemRenderable)
{
    ShaderProgramPtr textureShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/simpleTextureVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/simpleTextureFragment.glsl");
    ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/flatVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/flatFragment.glsl");
    ShaderProgramPtr instancedShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/instancedVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/instancedFragment.glsl");
    viewer.addShaderProgram( flatShader );
    viewer.addShaderProgram( instancedShader );
    viewer.addShaderProgram( textureShader );


    std::string texture_path = "../../sfmlGraphicsPipeline/textures/drapeau-pirate.jpg";
    FlagRenderablePtr flag = std::make_shared<FlagRenderable>(textureShader,
        0.39f, 0.39, 15, 10, 100e03, 10, texture_path);


    for (const ParticlePtr & particle : flag->getParticles())
        system->addParticle(particle);
    for (const ForceFieldPtr & force_field : flag->getSprings())
        system->addForceField(force_field);

    ConstantForceFieldPtr windForceField = std::make_shared<ConstantForceField>(system->getParticles(), DynamicSystem::wind);
    system->addForceField( windForceField );

	//Animated flag
    flag->setGlobalTransform(getTranslationMatrix(99.8,18.5,-100.5)*getScaleMatrix(5)*getRotationMatrix(M_PI,0,1,0));
    flag->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0),glm::quat{},glm::vec3(1,1,1)),0.0);
    flag->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,0),glm::quat{},glm::vec3(1,1,1)),10.0);
    flag->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,-50),glm::quat{},glm::vec3(1,1,1)),110.0);

    float dampingCoefficient = 1.0;
    DampingForceFieldPtr dampingForceField = std::make_shared<DampingForceField>(system->getParticles(), dampingCoefficient);
    system->addForceField(dampingForceField);

    viewer.addRenderable(flag);
    viewer.stopAnimation();
}

void movingCamera(Viewer& viewer, float time)
{
    static glm::vec3 cameraPosition;
    static glm::vec3 view;

    if (time < 1.5f) { 
        viewer.getCamera().setViewMatrix( glm::lookAt( glm::vec3(101, 19.5, -99.5 ), glm::vec3(0, 19.5, -99.5 ), glm::vec3( 0, 10, 0 ) ) );
    }else if(1.5f <= time && time < 4.0f){
         float t = (time - 1.5f) / (3.5f - 1.5f);

        float startX = 101.0f; 
        float endX = 110.0f;
        float startY = 19.5f; 
        float endY = 12.0f;
        float currentX = startX + t *t* (endX - startX); 
        float currentY = startY + t *t* (endY - startY); 
        view =  glm::vec3(0, currentY, -99.5);
        cameraPosition = glm::vec3(currentX,currentY,-99.5);
        viewer.getCamera().setViewMatrix(glm::lookAt(glm::vec3(currentX, currentY, -99.5), view, glm::vec3(0, 10, 0)));

 } else if (3.5f <= time && time < 6.0f) {
        float t = (time - 3.5f) / (6.0f - 3.5f);
        float startz = cameraPosition.z;
        float endz = -80;
        float startX = cameraPosition.x; 
        float endX = 120.0f;
        float startY = view.y; 
        float endY = 17.0f; 
        float currentX = startX + t*0.005*(endX - startX);
        float currentZ = startz+ t *0.005*(endz - startz);
        float currentY = startY+ t *0.005*(endY - startY);

        cameraPosition = glm::vec3(currentX, currentY, currentZ);
        viewer.getCamera().setViewMatrix(glm::lookAt(glm::vec3(currentX, currentY, currentZ), glm::vec3(0, currentY, currentZ), glm::vec3(0, 10, 0)));

    }else if (6.0f <= time && time < 10.0f) {
    float t = (time - 6.0f) / (10.0f - 6.0f);

    float startz = cameraPosition.z;
    float endz = 0;
    float currentZ = startz+ t *(endz - startz);
    view = glm::vec3(0, cameraPosition.y, currentZ);
    viewer.getCamera().setViewMatrix(glm::lookAt(glm::vec3(cameraPosition.x, cameraPosition.y, cameraPosition.z), view, glm::vec3(0, 10, 0)));
    }
}




void createIce(Viewer& viewer){
   ShaderProgramPtr texShader = std::make_shared<ShaderProgram>(   "../../sfmlGraphicsPipeline/shaders/simpleTextureVertex.glsl",
                                                                   "../../sfmlGraphicsPipeline/shaders/simpleTextureFragment.glsl");
   viewer.addShaderProgram( texShader );
   ShaderProgramPtr phongShader = std::make_shared<ShaderProgram>( "../../sfmlGraphicsPipeline/shaders/phongVertex.glsl",
                                                                       "../../sfmlGraphicsPipeline/shaders/phongFragment.glsl");
   viewer.addShaderProgram( phongShader );


   // Partie banquise en 3D avec matériau neige illuminable
   std::string banquise_mesh = "./../../sfmlGraphicsPipeline/meshes/banquise.obj";
   MaterialPtr ice = Material::Snow();
   LightedMeshRenderablePtr banquise = std::make_shared<LightedMeshRenderable>(phongShader, banquise_mesh, ice);
   banquise->setGlobalTransform(getTranslationMatrix(-40,0,0));
   banquise->setGlobalTransform(getScaleMatrix(0.5,0.5,0.5));
   viewer.addRenderable(banquise);
   // Partie banquise plane avec texture image neige
   std::string banquise_text = "./../../sfmlGraphicsPipeline/textures/banq_sunset.png";
   TexturedPlaneRenderablePtr texPlane1 = std::make_shared<TexturedPlaneRenderable>(texShader, banquise_text);
   texPlane1->setGlobalTransform(getTranslationMatrix(-500,1,0)*getRotationMatrix(M_PI/2,1,0,0)*getScaleMatrix(950,700,0));
   viewer.addRenderable(texPlane1);
   TexturedPlaneRenderablePtr texPlane2 = std::make_shared<TexturedPlaneRenderable>(texShader, banquise_text);
   texPlane2->setGlobalTransform(getTranslationMatrix(-100,1,350)*getRotationMatrix(M_PI/2,1,0.0,0.0)*getRotationMatrix(M_PI,0,1.0,0.0)
                                   *getRotationMatrix(-2.3*M_PI/3,0,0.0,1.0)*getScaleMatrix(450,600,0));
   viewer.addRenderable(texPlane2);
   TexturedPlaneRenderablePtr texPlane3 = std::make_shared<TexturedPlaneRenderable>(texShader, banquise_text);
   texPlane3->setGlobalTransform(getTranslationMatrix(0,1,-630)*getRotationMatrix(M_PI/2,1,0.0,0.0)*getRotationMatrix(M_PI,0,1.0,0.0)
                                      *getRotationMatrix(2.2*M_PI/3,0,0.0,1.0)*getScaleMatrix(800,800,0));
   viewer.addRenderable(texPlane3);

}


void createSky(Viewer& viewer){ // Skybox du décor panoramique

   ShaderProgramPtr cubeMapShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/cubeMapVertex.glsl",
                                                                   "../../sfmlGraphicsPipeline/shaders/cubeMapFragment.glsl");
   viewer.addShaderProgram(cubeMapShader);
   std::string cubemap_dir = "../../sfmlGraphicsPipeline/textures/CubeMap_Sunset";
   auto cubemap = std::make_shared<CubeMapRenderable>(cubeMapShader, cubemap_dir);
   viewer.addRenderable(cubemap);
}


void animateTexture(const TexturedPlaneRenderablePtr texPlane, const std::vector<std::string> textureFiles, sf::Clock &clock, int &currentTextureIndex){
    // Mesure le temps écoulé depuis le dernier changement de texture
    float elapsedTime = clock.getElapsedTime().asSeconds();
    const float textureChangeInterval = 0.1f;
    // Vérifie si le temps écoulé est supérieur à l'intervalle de changement de texture
    if (elapsedTime >= textureChangeInterval) {
       // Change l'indice de la texture à charger avec l'indice suivant
       currentTextureIndex = (currentTextureIndex + 1) % textureFiles.size();
       // Met à jour la texture avec le nouveau filename du vecteur
       texPlane->setTexture(textureFiles[currentTextureIndex]);
       // Réinitialise la minuterie
       clock.restart();
    }
}


std::vector<std::string> listFilesInDirectory(const std::string& directoryPath) {
   std::vector<std::string> textureFiles;
   DIR* dir;
   struct dirent* ent;
   if ((dir = opendir(directoryPath.c_str())) != NULL) {
       while ((ent = readdir(dir)) != NULL) {
           if (ent->d_type == DT_REG) {  // Vérifier si c'est un fichier régulier
               std::string fileName = ent->d_name;
                   textureFiles.push_back(directoryPath + "/" + fileName);
           }
       }
       closedir(dir);
   } else {
       std::cerr << "Erreur lors de l'ouverture du répertoire." << std::endl;
   }
   return textureFiles;
}

