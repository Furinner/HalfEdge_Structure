#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include <memory>
#include <filesystem>
#include <string>
#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include "global.h"
#include "camera.h"
#include "shaderprogram.h"
#include "model.h"
#include "display.h"

//可调
//屏幕设置
const unsigned int SCR_WIDTH = 1980;
const unsigned int SCR_HEIGHT = 1080;

//不可调
float deltaTime = 0.f;
float lastTime = 0.f;
//鼠标变量
bool useMouse = false; //是否使用鼠标
bool recordFirstMousePos = true; //鼠标的行为辅助变量
float lastX = SCR_WIDTH / 2.0f; //记录鼠标X轴地点
float lastY = SCR_HEIGHT / 2.0f; //记录鼠标Y轴地点
//Euler操作变量
vec3 mvfsPos(0.f);
vec3 mevPos(0.f);
//元素选择变量
int currSolidIdx = -1;
int currFaceIdx = -1;
int currLoopIdx = -1;
int currEdgeIdx = -1;
int currVertIdx = -1;
int mefV2Idx = -1;
//Debug Log
std::string debugLog = "No warning or errors!";
//创建摄像机
uPtr<Camera> cam = mkU<Camera>();

// 处理窗口大小改变的callback
void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// 处理键鼠输入的callback，检查某个按键的状态是按下还是没按下
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cam->keyboardMovement(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cam->keyboardMovement(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cam->keyboardMovement(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cam->keyboardMovement(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        cam->keyboardMovement(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        cam->keyboardMovement(DOWN, deltaTime);
}

// 处理键鼠event的callback。
// 用于检测键位是否被按了一下。与上面processInput不同，如果按住某个键不放，这里也只会被触发一次，而processInput会一直触发。
void keyEventCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_M && action == GLFW_PRESS) {
        useMouse = !useMouse;
        recordFirstMousePos = true;
    }
}

// 处理鼠标位移的callback。
void mousePosCallback(GLFWwindow* window, double xposIn, double yposIn) {
    if (useMouse) {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (recordFirstMousePos)
        {
            lastX = xpos;
            lastY = ypos;
            recordFirstMousePos = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        cam->mouseMovement(xoffset, yoffset);
    }
}

// imgui的辅助方法，将所有元素选择相关变量归0
void cleanGeoEleIdx() {
    currSolidIdx = -1;
    currFaceIdx = -1;
    currLoopIdx = -1;
    currEdgeIdx = -1;
    currVertIdx = -1;
    mefV2Idx = -1;
}


// ImGui的draw call。这里可以设置ImGui的整体格式
void drawGui(int windowWidth, int windowHeight, uPtr<Model>& model, uPtr<VertDisplay>& vertDisplay, uPtr<LoopDisplay>& loopDisplay, uPtr<EdgeDisplay>& edgeDisplay) {
    // Dear imgui new frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Dear imgui define
    ImVec2 minSize(300.f, 220.f);
    ImVec2 maxSize((float)windowWidth * 0.3, (float)windowHeight * 0.5);
    ImGui::SetNextWindowSizeConstraints(minSize, maxSize);

    ImGui::Begin("Control Panel");

    ImGui::TextWrapped("Press M to open and close mouse mode!");
    ImGui::Checkbox("Use Mouse", &useMouse);
    ImGui::Separator();
    ImGui::Text("Camera Settings");
    ImGui::InputFloat3("Camera Position", &(cam->position.x));
    ImGui::Separator();
    ImGui::Text("Euler Operations");
    ImGui::InputFloat3("##mvfsVec3", &(mvfsPos.x));
    ImGui::SameLine();
    if (ImGui::Button("mvfs")) {
        model->mvfs(mvfsPos);
    };
    ImGui::SameLine();
    ImGui::Text("(vec3)");
    ImGui::InputFloat3("##mevVec3", &(mevPos.x));
    ImGui::SameLine();
    if (ImGui::Button("mev")) {
        if (currVertIdx == -1) {
            debugLog = "Select a vertex before performing mev operation!";
        }
        else if (currLoopIdx == -1) {
            debugLog = "Select a loop before performing mev operation!";
        }
        else {
            Vertex* currVert = model->vertices[currVertIdx].get();
            Loop* currLoop = model->loops[currLoopIdx].get();
            model->mev(currVert, currLoop, mevPos);
            cleanGeoEleIdx();
        }
    };
    ImGui::SameLine();
    ImGui::Text("(v1, lp, vec3)");
    std::string mefV2Value = "";
    if (mefV2Idx != -1) {
        mefV2Value = "v" + std::to_string(mefV2Idx);
    }
    ImGui::PushItemWidth(ImGui::CalcTextSize("v2").x * 4);
    if (ImGui::BeginCombo("v2", mefV2Value.c_str(), 0))
    {
        for (int i = 0; i < model->vertices.size(); ++i)
        {
            if (ImGui::Selectable(("v" + std::to_string(i)).c_str(), (mefV2Idx == i))) {
                if (mefV2Idx == i) {
                    mefV2Idx = -1;
                }
                else {
                    mefV2Idx = i;
                }
                vertDisplay->update(currVertIdx, mefV2Idx, model);
            }
        }
        ImGui::EndCombo();
    }
    ImGui::PopItemWidth();
    ImGui::SameLine();
    if (ImGui::Button("mef")) {
        if (currVertIdx == -1) {
            debugLog = "Select a vertex1 before performing mev operation!";
        }
        else if (mefV2Idx == -1) {
            debugLog = "Select a vertex2 before performing mev operation!";
        }
        else if (currVertIdx == mefV2Idx) {
            debugLog = "Select different v1 and v2 to perform mev operation!";
        }
        else {
            Vertex* v1 = model->vertices[currVertIdx].get();
            Vertex* v2 = model->vertices[mefV2Idx].get();
            model->mef(v1, v2);
            cleanGeoEleIdx();
        }
    }
    ImGui::SameLine();
    ImGui::Text("(v1, v2)");
    if (ImGui::Button("kemr")) {
        
    }
    ImGui::Separator();
    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
    {
        if (ImGui::BeginTabItem("Solids"))
        {
            
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Loops"))
        {
            if (ImGui::BeginListBox("##LoopsListBox"))
            {
                for (int i = 0; i < model->loops.size(); ++i)
                {
                    if (ImGui::Selectable(("loop" + std::to_string(i)).c_str(), (currLoopIdx == i))) {
                        if (currLoopIdx == i) {
                            currLoopIdx = -1;
                        }
                        else {
                            currLoopIdx = i;
                        }
                        loopDisplay->update(currLoopIdx, model);
                    }
                }
                ImGui::EndListBox();
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Edge"))
        {
            if (ImGui::BeginListBox("##EdgeListBox"))
            {
                for (int i = 0; i < model->edges.size(); ++i)
                {
                    if (ImGui::Selectable(("edge" + std::to_string(i)).c_str(), (currEdgeIdx == i))) {
                        if (currEdgeIdx == i) {
                            currEdgeIdx = -1;
                        }
                        else {
                            currEdgeIdx = i;
                        }
                        edgeDisplay->update(currEdgeIdx, model);
                    }
                }
                ImGui::EndListBox();
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Vertices"))
        {
            if (ImGui::BeginListBox("##LoopsVerBox"))
            {
                for (int i = 0; i < model->vertices.size(); ++i)
                {
                    if (ImGui::Selectable(("v" + std::to_string(i)).c_str(), (currVertIdx == i))) {
                        if (currVertIdx == i) {
                            currVertIdx = -1;
                        }
                        else {
                            currVertIdx = i;
                        }
                        vertDisplay->update(currVertIdx, mefV2Idx, model);
                    }
                }
                ImGui::EndListBox();
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::Separator();
    ImGui::Text("Debug Log:");
    if (ImGui::BeginChild("", ImVec2(-FLT_MIN, ImGui::GetTextLineHeightWithSpacing() * 2), ImGuiInputTextFlags_AllowTabInput)) {
        ImGui::Text(debugLog.c_str());
    }
    ImGui::EndChild();
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void setProjViewModelMat(uPtr<ShaderProgram>& shaderProgram) {
    shaderProgram->use();
    mat4 proj = glm::perspective(glm::radians(cam->fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
    shaderProgram->setMat4("proj", proj);

    mat4 view = cam->getViewMatrix();
    shaderProgram->setMat4("view", view);

    mat4 model = mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
    shaderProgram->setMat4("model", model);
}

int main()
{
    // glfw: 初始化
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "render scene", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback); //窗口大小改变时调用函数
    glfwSetKeyCallback(window, keyEventCallback); //设置键鼠event的callback
    glfwSetCursorPosCallback(window, mousePosCallback);
    // glad: 初始化
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //接下来可以使用gl方法了
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f); //窗口清理所用颜色

    //创建shaderProgram
    uPtr<ShaderProgram> shaderProgram = mkU<ShaderProgram>("glsl/lambert.vert.glsl", "glsl/lambert.frag.glsl");
    uPtr<ShaderProgram> shaderProgramDis = mkU<ShaderProgram>("glsl/display.vert.glsl", "glsl/display.frag.glsl");

    uPtr<Model> model = mkU<Model>();
    model->setupModel();

    uPtr<VertDisplay> vertDisplay = mkU<VertDisplay>();
    uPtr<EdgeDisplay> edgeDisplay = mkU<EdgeDisplay>();
    uPtr<LoopDisplay> loopDisplay = mkU<LoopDisplay>();

    
    //创建ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");


    // glfw: render loop
    while (!glfwWindowShouldClose(window))
    {
        //per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastTime;
        lastTime = currentFrame;

        //检查event和input
        glfwPollEvents(); //检查触发事件，更新窗口状态，调用相应回调函数
        processInput(window); //处理各种键盘输入

        //清理color buffer的所有信息
        //一定要在imgui渲染前调用，不然imgui会有残影问题
        glClear(GL_COLOR_BUFFER_BIT);
        
        //draw
        setProjViewModelMat(shaderProgram); //更新proj，view，model矩阵
        setProjViewModelMat(shaderProgramDis);
        shaderProgram->use();
        model->draw();
        //model->drawPoint();
        shaderProgramDis->use();
        loopDisplay->draw();
        edgeDisplay->draw();
        vertDisplay->draw();

        //ImGui
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        drawGui(display_w, display_h, model, vertDisplay, loopDisplay, edgeDisplay);

        glfwSwapBuffers(window); //双缓冲机制，交换前缓冲和后缓冲
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

