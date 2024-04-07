#include <glad/glad.h>

#include "GLFW/glfw3.h"
#include <glm/glm.hpp>

#include <imgui.h>
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

class guiDialog
{
public:
    guiDialog(GLFWwindow *w);
    void CreateGuiDialog();
    void ShowGui();
    auto GetParam() const
    {
        return objPar;
    }
    auto GetEyeLookAT() const
    {
        return objEye;
    }

    auto GetPointLookAT() const
    {
        return objPoint;
    }

    virtual ~guiDialog();
private:
    GLFWwindow* window;

    struct PropertiesObject
    {
        PropertiesObject(float a, float b, float c):
            valueRed(a), valueGreen(b), valueBlue(c)
        {}
        float valueRed;
        float valueGreen;
        float valueBlue;
    } objPar;

    struct eyeLookAt{
        int tX;
        int tY;
        int tZ;
    } objEye = {0, 0, 3};

    struct pointLookAt{
        float tX;
        float tY;
        float tZ;
    } objPoint = {0, 0, 0};

};

guiDialog::guiDialog(GLFWwindow *w):window(w), objPar{0.1,0.5,0.0}
{
    //Инициализация интерфейса ImGui
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void guiDialog::CreateGuiDialog()
{
    //Запуск нового окна ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    //Элементы интерфейса в окне
    if (ImGui::Begin("Control propeties")) {
        ImGui::Text("Create a point's array");
        ImGui::SliderFloat("Red", &objPar.valueRed, 0.0f, 1.0f);
        ImGui::SliderFloat("Green", &objPar.valueGreen, 0.0f, 1.0f);
        ImGui::SliderFloat("Blue", &objPar.valueBlue, 0.0f, 1.0f);

        ImGui::SliderInt("Eye_X", &objEye.tX, -50, 50);
        ImGui::SliderInt("Eye_Y", &objEye.tY, -50, 50);
        ImGui::SliderInt("Eye_Z", &objEye.tZ, 0, 30);

        ImGui::SliderFloat("Object_X", &objPoint.tX, -5, 5);
        ImGui::SliderFloat("Object_Y", &objPoint.tY, -5, 5);
        ImGui::SliderFloat("Object_Z", &objPoint.tZ, -5, 5);

    }
    ImGui::End();
}

void guiDialog::ShowGui()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

guiDialog::~guiDialog()
{
    //Освобождение ресурсов ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void reshapeWindows (GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);


class Lines {
    int shaderProgram;
    unsigned int VBO, VAO;
    std::vector<float> vertices;
    glm::vec3 startPoint;
    glm::vec3 endPoint;
    glm::mat4 MVP;
    glm::vec3 lineColor;
    //count of lines
    size_t numberOfLines;
public:
    Lines() {
    //vertex Shader
    const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    uniform mat4 MVP;
    void main()
    {
        gl_Position = MVP * vec4(aPos.x, aPos.y, aPos.z, 1.0)  ;
    }
)";
    //fragment Shader
    const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    uniform vec3 color;
    void main()
    {
        FragColor = vec4(color, 1.0f);
    }
)";
        // vertex shader
        int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);
        // check for shader compile errors

        // fragment shader
        int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);
        // check for shader compile errors

        // link shaders
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        // check for linking errors

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        vertices.clear();
    }
    //Add Line in data of vector
    void AddLine(glm::vec3 start, glm::vec3 end)
    {
        startPoint = start;
        endPoint = end;
        lineColor = glm::vec3(1,1,1);
        MVP = glm::mat4(1.0f);

        //add coord start (end) point to vector
        vertices.push_back(start.x);
        vertices.push_back(start.y);
        vertices.push_back(start.z);

        vertices.push_back(end.x);
        vertices.push_back(end.y);
        vertices.push_back(end.z);

        numberOfLines = vertices.size()/6;

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, numberOfLines * sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    bool eraseValues()
    {
        vertices.clear();
        return true;
    }

    int setMVP(glm::mat4 mvp) {
        MVP = mvp;
        return 1;
    }

    int setColor(glm::vec3 color) {
        lineColor = color;
        return 1;
    }

    int draw() {
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "MVP"), 1, GL_FALSE, &MVP[0][0]);
        glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, &lineColor[0]);

        glBindVertexArray(VAO);
        glDrawArrays(GL_LINES, 0, numberOfLines * 2);
        return 1;
    }

    ~Lines() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteProgram(shaderProgram);
        vertices.clear();
    }
};

//Create a figure of butterfly
void  butterflyCreate (Lines &obj)
{
    obj.eraseValues();
    typedef struct {
        float x,y,z;
    } XYZ;
    constexpr size_t N = 2000;
    int i;
    float u;
    XYZ p,plast ={0,0,0};
    float scale = 0.2;
    for (i=0;i<N;i++) {
        u = i * 24.0 * M_PI / N;
        p.x = sin(u) * (exp(cos(u)) - 2 * cos(4 * u) - pow(sin(u / 12),5.0));
        p.y = cos(u) * (exp(cos(u)) - 2 * cos(4 * u) - pow(sin(u / 12),5.0));
        p.z = fabs(p.y) / 2;
        p.x *= scale;
        p.y *= scale;
        p.z *= scale;
        obj.AddLine({plast.x, plast.y, plast.z},{p.x,p.y,p.z});
        //colour = GetColour(u,0.0,24*PI,4);
        // if (i > 0) {
        //     Do something with the line from plast to p
        // }
        plast = p;
    }
}

//Create a figure of Monkey Saddle
void  monkeySaddleCreate (Lines &obj)
{
    obj.eraseValues();
    typedef struct {
        float x,y,z;
    } data;
    float u = -1.1;
    float v = -1.1;
    float step = 0.05;
    data after_p,before_p;
    bool flag  = false;
    for (u = -1.1; u < 1.1; u += step)
    {
        for (v = -1.1; v < 1.1; v += step)
        {
            after_p.x = u;
            after_p.y = v;
            after_p.z = u*u*u -3*u*v*v;

            after_p.x *= 0.5;
            after_p.y *= 0.5;
            after_p.z *= 0.5;
            if (flag == false) {
                flag = true;
            }
            else
            {
                obj.AddLine({before_p.x, before_p.y, before_p.z},
                             {after_p.x, after_p.y, after_p.z});
            }
            before_p = after_p;
        }
    }
}

int main()
{
    // settings screen
    const unsigned int SCR_WIDTH = 1024;
    const unsigned int SCR_HEIGHT = 768;

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL-Lession#11", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Lines obj;
    //butterflyCreate(obj);
    monkeySaddleCreate(obj);

    guiDialog myGUI(window);


    // render loop
    // -----------
    float koef = static_cast<float> (SCR_WIDTH)/static_cast<float> (SCR_HEIGHT);
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        obj.draw();
        myGUI.CreateGuiDialog();
        myGUI.ShowGui();
        myGUI.GetParam();
        glm::vec3 Color{myGUI.GetParam().valueRed, myGUI.GetParam().valueGreen,myGUI.GetParam().valueBlue};
        obj.setColor(Color);

        glm::mat4 model = glm::mat4(1.0f); // Identity matrix for the model
        glm::mat4 view = glm::lookAt(glm::vec3(myGUI.GetEyeLookAT().tX, myGUI.GetEyeLookAT().tY, myGUI.GetEyeLookAT().tZ),
                                     glm::vec3(myGUI.GetPointLookAT().tX, myGUI.GetPointLookAT().tY, myGUI.GetPointLookAT().tZ), glm::vec3(0, 1, 0)); // View matrix
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), koef, 0.1f, 100.0f); // Projection matrix
        glm::mat4 mvp = projection * view * model; // Combined MVP matrix

        obj.setMVP(mvp);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}



