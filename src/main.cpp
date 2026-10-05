#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "error.h"
#include "shader.h"
#include "shadow.h"

#include "Bezier.h"
#include "DayNightCycle.h"
#include "Light.h"
#include "LightManager.h"
#include "Model.h"
#include "ParticleEmitter.h"
#include "Player.h"
#include "Procedual.h"
#include "Scene.h"
#include "Skybox.h"

#define WIDTH 800
#define HEIGHT 600

#define SH_MAP_WIDTH 2048
#define SH_MAP_HEIGHT 2048

GLFWwindow *window;
Player *player;

Scene scene;

Shader *shader;
Shader *shadowShader;
Shader *particleShader;
Shader *skyboxShader;

ParticleEmitter *particleEmitter;

LightManager lightManager;

DayNightCycle cycle;
Bezier *curve;

SceneNode *ball;

float near_plane = 0.1f;
float far_plane = 500.f;

// Variables for mouse_callback
bool firstMouse = true;
float lastX = (float)WIDTH / 2.f;
float lastY = (float)HEIGHT / 2.f;

glm::vec3 clearColour;

// Capture mouse movement
static void MouseCallback(GLFWwindow *window, double xpos, double ypos) {
  if (firstMouse) {
    lastX = (float)xpos;
    lastY = (float)ypos;
    firstMouse = false;
    return;
  }

  float xoffset = (float)xpos - lastX;
  float yoffset = (float)ypos - lastY;

  lastX = (float)xpos;
  lastY = (float)ypos;

  float normalisedX = xoffset / (float)WIDTH;
  float normalisedY = yoffset / (float)HEIGHT;

  player->updateDirection(normalisedX, normalisedY);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

// GLFW initialise
void initWindow() {
  glfwInit();
  // enable multisampling
  glfwWindowHint(GLFW_SAMPLES, 32);

  // Get primary monitor to fullscreen
  GLFWmonitor *monitor = glfwGetPrimaryMonitor();
  window = glfwCreateWindow(WIDTH, HEIGHT, "psyjt11 CW", NULL,
                            NULL); // change monitor to NULL for windowed
  glfwMakeContextCurrent(window);
  // Enable v-sync to stop tearing
  glfwSwapInterval(1);
  glfwSetCursorPosCallback(window, MouseCallback);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  gl3wInit();

  glfwSetCursorPos(window, (float)WIDTH / 2.f, (float)HEIGHT / 2.f);
}

// OpenGL Initialise
void initGL() {
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(DebugCallback, 0);
  glEnable(GL_DEPTH_TEST);
  glViewport(0, 0, WIDTH, HEIGHT);
}

// Initialise the scene
void initScene() {
  player = new Player();

  int terrainW = 500, terrainD = 500;
  // Setup shaders
  shader = new Shader("pbr.vert", "pbr.frag");
  shadowShader = new Shader("shadow.vert", "shadow.frag");
  particleShader = new Shader("particle.vert", "particle.frag");

  std::vector<point> ctrl_points = {
      point(2.6f, 0.0f, 1.6f), point(2.f, 0.0f, -4.2f), point(12.f, 0.0f, 10.f),
      point(-1.8f, 0.0f, 0.4f)};
  curve = new Bezier(ctrl_points, 0.2f);

  // Setup Models
  Model *campfireModel = new Model("Objects/campfire/campfire.obj");
  Model *arrowModel = new Model("Objects/arrow/Arrow5.obj");
  Model *benchModel = new Model("Objects/Bench/Bench.obj");
  Model *slabModel = new Model("Objects/ConcreteSlab/slab.obj");

  Model *ballModel = new Model("Objects/RustyBall/rustyBall.obj");

  // Walls
  Model *wallAModel = new Model("Objects/walls/CaveWalls1_C.obj");
  Model *wallBModel = new Model("Objects/walls/CaveWalls1_D.obj");

  Model *redPlaneModel =
      new Model("Objects/TransparentPlanes/RedPlane.obj", 0.5f);
  Model *OrangePlaneModel =
      new Model("Objects/TransparentPlanes/OrangePlane.obj", 0.5f);

  Procedual *terrainMesh = new Procedual(terrainW, terrainD, 1);
  Model *terrainModel = new Model(terrainMesh->generateMesh());

  // Particle Emitters
  particleEmitter = new ParticleEmitter(5000, "Particles/kennyFireTextures.png",
                                        "Particles/kennySmokeTextures2.png");

  // Setup Nodes
  SceneNode *root = new SceneNode();
  SceneNode *campfire = new SceneNode(campfireModel);
  SceneNode *arrow = new SceneNode(arrowModel);
  SceneNode *fireScene = new SceneNode();

  SceneNode *transPlaneScene = new SceneNode();
  SceneNode *redPlane = new SceneNode(redPlaneModel);
  redPlane->setIsTransparent(true);
  SceneNode *orangePlane = new SceneNode(OrangePlaneModel);
  orangePlane->setIsTransparent(true);

  SceneNode *benchScene = new SceneNode();
  SceneNode *bench = new SceneNode(benchModel);
  SceneNode *slab = new SceneNode(slabModel);

  SceneNode *wallsScene = new SceneNode();
  SceneNode *wallA = new SceneNode(wallAModel);
  SceneNode *wallB = new SceneNode(wallBModel);

  ball = new SceneNode(ballModel);

  SceneNode *emitter = new SceneNode(particleEmitter);
  emitter->setIsTransparent(true);

  SceneNode *terrain = new SceneNode(terrainModel);

  SceneNode *transRoot = new SceneNode();

  // Transforms
  emitter->setPosition(glm::vec3(-250, 45, -80));

  campfire->setScale(glm::vec3(0.01f));
  campfire->setPosition(glm::vec3(0, 0, 0));

  arrow->setPosition(glm::vec3(-20, 4, 0));
  arrow->setScale(glm::vec3(0.5));

  benchScene->setPosition(glm::vec3(-15, 0, -15));
  slab->setScale(glm::vec3(4.f));
  slab->setPosition(glm::vec3(0, 0.1, 0));

  ball->setScale(glm::vec3(0.01));

  wallA->setScale(glm::vec3(0.4));
  wallB->setScale(glm::vec3(0.4));

  wallA->setPosition(glm::vec3(10, 0, 40));

  wallsScene->setPosition(glm::vec3(-35, 0, 20));
  wallsScene->setRotation(90, glm::vec3(0, 1, 0));

  // Planes
  redPlane->setRotation(90, glm::vec3(1, 0, 0));
  orangePlane->setRotation(90, glm::vec3(1, 0, 0));

  redPlane->setPosition(glm::vec3(0, 0, -10));

  redPlane->setScale(glm::vec3(5.f));
  orangePlane->setScale(glm::vec3(4.f));

  transPlaneScene->setPosition(glm::vec3(20, 0, 35));

  terrain->setPosition(
      glm::vec3(-float(terrainW) / 2, 0.f, -float(terrainD) / 2));

  // Add Children
  fireScene->addChild(arrow);
  fireScene->addChild(campfire);
  campfire->addChild(emitter);

  benchScene->addChild(bench);
  benchScene->addChild(slab);

  transPlaneScene->addChild(redPlane);
  transPlaneScene->addChild(orangePlane);

  wallsScene->addChild(wallA);
  wallsScene->addChild(wallB);

  // Roots
  root->addChild(fireScene);

  root->addChild(terrain);
  root->addChild(benchScene);
  root->addChild(ball);
  root->addChild(wallsScene);

  transRoot->addChild(transPlaneScene);

  // Add root
  scene.addRoot(root);
  scene.addRoot(transRoot);
}

void initLights() {
  Light dirLight(LightType::DIRECTIONAL, glm::vec3(0.942f, 0.936f, 0.771f),
                 near_plane, far_plane, WIDTH, HEIGHT);
  dirLight.setPosition(glm::vec3(-24, 0, 0));
  dirLight.setDirection(glm::vec3(0.5f, 0, -1));
  dirLight.enableShadowMapping(SH_MAP_WIDTH, SH_MAP_HEIGHT);

  Light spotLight(LightType::SPOT, glm::vec3(1.f), near_plane, far_plane, WIDTH,
                  HEIGHT);
  spotLight.enableShadowMapping(SH_MAP_WIDTH, SH_MAP_HEIGHT);

  Light pointLight(LightType::POINT, glm::vec3(1.0, 0.5, 0.1), near_plane,
                   far_plane, WIDTH, HEIGHT);
  pointLight.setPosition(glm::vec3(-1, 1, 0));

  lightManager.addLight(spotLight);
  lightManager.addLight(dirLight);
  lightManager.addLight(pointLight);
}

// Handle keyboard presses
void processInput(float dt) {
  // Esc close window
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  // Player movement
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    player->move(SCamera::FORWARD, dt);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    player->move(SCamera::BACKWARD, dt);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    player->move(SCamera::LEFT, dt);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    player->move(SCamera::RIGHT, dt);
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    player->move(SCamera::UP, dt);
  if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    player->move(SCamera::DOWN, dt);

  if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
    glm::vec3 spotPos = player->getPosition();
    lightManager.updateLight(0, spotPos, player->camera.Front);
  }
}

void update(float dt) {
  particleEmitter->update(dt);
  cycle.update(dt);
  glm::vec3 sunCol = cycle.getColour();
  glm::vec3 sunPos = cycle.getPosition();
  glm::vec3 sunDir = cycle.getDirection();
  lightManager.updateLight(1, sunPos, sunDir);
  lightManager.updateLightColour(1, sunCol);

  clearColour = cycle.getBackgroundColour();

  ball->setPosition((curve->getPosition(dt) * 3.f) + glm::vec3(0, 3, -20));
}

void render() {

  lightManager.generateShadowMaps(scene, *shadowShader, SH_MAP_WIDTH,
                                  SH_MAP_HEIGHT);
  lightManager.uploadLightsToShader(*shader);
  lightManager.bindShadowMaps(*shader);

  int frameWidth, frameHeight;
  glfwGetFramebufferSize(window, &frameWidth, &frameHeight);
  glViewport(0, 0, frameWidth, frameHeight);

  GLfloat bgd[] = {clearColour.r, clearColour.g, clearColour.b, 1.f};
  glClearBufferfv(GL_COLOR, 0, bgd);
  glClear(GL_DEPTH_BUFFER_BIT);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  glm::mat4 view = player->getViewMatrix();

  glm::mat4 projection = glm::mat4(1.f);
  projection = glm::perspective(glm::radians(25.f),
                                (float)frameWidth / (float)frameHeight,
                                near_plane, far_plane);

  scene.drawAll(*shader, *particleShader, view, projection,
                player->getPosition());
}

void cleanup() {
  glfwDestroyWindow(window);
  glfwTerminate();
}

// Main loop
void run() {
  float lastFrame = (float)glfwGetTime();
  while (!glfwWindowShouldClose(window)) {
    float currentFrame = (float)glfwGetTime();
    float dt = currentFrame - lastFrame;
    lastFrame = currentFrame;

    update(dt);

    render();

    glfwSwapBuffers(window);

    glfwPollEvents();
    processInput(dt);
  }
}

int main(int argc, char **argv) {
  // Initialisation
  initWindow();
  initGL();
  initScene();
  initLights();

  run(); // Main loop

  cleanup();

  return 0;
}
