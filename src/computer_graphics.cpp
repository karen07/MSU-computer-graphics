#include "../glslprogram/glslprogram.h"
#include "../mesh/drawable.h"
#include "../mesh/vboplane.h"
#include "../mesh/vbosphere.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <lodepng.h>
#include <stdio.h>
#include <stdlib.h>

mat4 projection;
GLSLProgram prog, prog1, prog2, prog3;
vec3 pos = vec3(0.f, 0.f, 0.f);
int set_post = 0, set_dof = 0, set_fog = 0, set_fxaa = 0;
GLuint colorTexture = 0, depthTexture = 0;
GLuint tex1, tex2;
int index_normal_map = 0;

GLuint Set_Image(char* str)
{
    std::vector<unsigned char> image;
    unsigned width_img, height_img;
    lodepng::decode(image, width_img, height_img, str);
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width_img, height_img);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width_img, height_img, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    return texID;
}

static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key == GLFW_KEY_W)
        pos += vec3(0.f, 0.f, -0.05f);
    if (key == GLFW_KEY_S)
        pos += vec3(0.f, 0.f, 0.05f);
    if (key == GLFW_KEY_D)
        pos += vec3(0.05f, 0.f, 0.f);
    if (key == GLFW_KEY_A)
        pos += vec3(-0.05f, 0.f, 0.f);
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
        set_post = !set_post;
    if (key == GLFW_KEY_E && action == GLFW_PRESS)
        set_dof = !set_dof;
    if (key == GLFW_KEY_F && action == GLFW_PRESS)
        set_fog = !set_fog;
    if (key == GLFW_KEY_X && action == GLFW_PRESS)
        set_fxaa = !set_fxaa;
    if (key == GLFW_KEY_EQUAL && action == GLFW_PRESS) {
        index_normal_map = ++index_normal_map % 6;
        char normal_name[80];
        sprintf(normal_name, "../texture/%d_norm.png", index_normal_map);
        glActiveTexture(GL_TEXTURE0);
        tex1 = Set_Image(normal_name);

        char texture_name[80];
        sprintf(texture_name, "../texture/%d.png", index_normal_map);
        glActiveTexture(GL_TEXTURE1);
        tex2 = Set_Image(texture_name);
    }

    if (key == GLFW_KEY_MINUS && action == GLFW_PRESS) {
        index_normal_map--;
        if (index_normal_map < 0) {
            index_normal_map = 5;
        }
        char normal_name[80];
        sprintf(normal_name, "../texture/%d_norm.png", index_normal_map);
        glActiveTexture(GL_TEXTURE0);
        tex1 = Set_Image(normal_name);

        char texture_name[80];
        sprintf(texture_name, "../texture/%d.png", index_normal_map);
        glActiveTexture(GL_TEXTURE1);
        tex2 = Set_Image(texture_name);
    }
}

static void window_size_callback(GLFWwindow* window, int width, int height)
{
    float ratio;
    ratio = width / (float)height;
    glViewport(0, 0, width, height);
    projection = glm::perspective(glm::quarter_pi<float>(), ratio, 0.3f, 50.0f);
    prog.use();
    prog.setUniform("Projection", projection);
    prog.setUniform("Resolution", vec2(width, height));

    prog1.use();
    prog1.setUniform("Projection", projection);
    prog1.setUniform("Resolution", vec2(width, height));

    prog2.use();
    prog2.setUniform("Projection", projection);
    prog2.setUniform("Resolution", vec2(width, height));

    prog3.use();
    prog3.setUniform("Resolution", vec2(width, height));

    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
}

int main()
{
    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);

    window = glfwCreateWindow(1280, 720, "Opengl Project", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);

    glewExperimental = GL_TRUE;
    if (GLEW_OK != glewInit()) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    prog.compileShader("../shader/0.vert");
    prog.compileShader("../shader/0.frag");
    prog.link();

    prog1.compileShader("../shader/1.vert");
    prog1.compileShader("../shader/1.frag");
    prog1.link();

    prog2.compileShader("../shader/2.vert");
    prog2.compileShader("../shader/2.frag");
    prog2.link();

    prog3.compileShader("../shader/3.vert");
    prog3.compileShader("../shader/3.frag");
    prog3.link();

    glEnable(GL_DEPTH_TEST);

    VBOSphere* sphere = new VBOSphere(0.3f, 100, 100);
    VBOSphere* sphere_toon = new VBOSphere(0.3f, 100, 100);
    VBOSphere* sphere_fog = new VBOSphere(0.3f, 100, 100);
    VBOSphere* sphere_fog1 = new VBOSphere(0.3f, 100, 100);
    VBOSphere* sphere_fog2 = new VBOSphere(0.3f, 100, 100);
    VBOSphere* sphere_fog3 = new VBOSphere(4.f, 100, 100);
    VBOPlane* plane = new VBOPlane(10.f, 10.f, 100, 100);
    VBOPlane* plane_norm = new VBOPlane(1.f, 1.f, 10, 10);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    projection = glm::perspective(glm::quarter_pi<float>(), width / (float)height, 0.3f, 50.0f);
    vec4 light_pos;
    mat4 model;
    mat4 view;

    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);

    GLuint renderFBO = 0;
    glGenFramebuffers(1, &renderFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, renderFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

    prog.use();
    prog.setUniform("Projection", projection);
    prog.setUniform("LightIntensity", vec3(1.0f, 1.0f, 1.0f));
    prog.setUniform("Kd", 0.9f, 0.5f, 0.3f);
    prog.setUniform("Ks", 0.8f, 0.8f, 0.8f);
    prog.setUniform("Ka", 0.9f, 0.5f, 0.3f);
    prog.setUniform("Shininess", 100.0f);
    prog.setUniform("Resolution", vec2(width, height));

    prog2.use();
    model = glm::translate(mat4(1.f), vec3(-0.2f, 0.5f, -4.f));
    prog2.setUniform("Model", model);
    prog2.setUniform("Projection", projection);
    prog2.setUniform("LightIntensity", vec3(1.0f, 1.0f, 1.0f));
    prog2.setUniform("Kd", 0.9f, 0.5f, 0.3f);
    prog2.setUniform("Ka", 0.9f, 0.5f, 0.3f);
    prog2.setUniform("Resolution", vec2(width, height));

    prog1.use();
    model = glm::translate(mat4(1.f), vec3(1.f, 1.f, -1.f)) * glm::rotate(glm::half_pi<float>(), vec3(1.f, 0.f, 0.f));
    prog1.setUniform("Projection", projection);
    prog1.setUniform("Model", model);
    prog1.setUniform("LightIntensity", vec3(0.9f, 0.9f, 0.9f));
    prog1.setUniform("Kd", 0.9f, 0.9f, 0.9f);
    prog1.setUniform("Ks", 0.1f, 0.1f, 0.1f);
    prog1.setUniform("Ka", 0.1f, 0.1f, 0.1f);
    prog1.setUniform("Shininess", 1.0f);
    prog1.setUniform("Resolution", vec2(width, height));

    prog3.use();
    prog3.setUniform("Resolution", vec2(width, height));

    char normal_name[80];
    sprintf(normal_name, "../texture/%d_norm.png", index_normal_map);
    glActiveTexture(GL_TEXTURE0);
    tex1 = Set_Image(normal_name);

    char texture_name[80];
    sprintf(texture_name, "../texture/%d.png", index_normal_map);
    glActiveTexture(GL_TEXTURE1);
    tex2 = Set_Image(texture_name);

    GLuint VAO = 0, VBO_vert = 0, VBO_tex = 0;
    vec3 vert[6] = {
        { -1.0f, -1.0f, 0.0f },
        { 1.0f, -1.0f, 0.0f },
        { -1.0f, 1.0f, 0.0f },
        { 1.0f, -1.0f, 0.0f },
        { 1.0f, 1.0f, 0.0f },
        { -1.0f, 1.0f, 0.0f },
    };
    vec2 tex[6] = {
        { 0.0f, 0.0f },
        { 1.0f, 0.0f },
        { 0.0f, 1.0f },
        { 1.0f, 0.0f },
        { 1.0f, 1.0f },
        { 0.0f, 1.0f }
    };
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO_vert);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_vert);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(vec3), vert, GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)));
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &VBO_tex);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_tex);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(vec2), tex, GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)1, 2, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)));
    glEnableVertexAttribArray(1);

    float time = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        glBindFramebuffer(GL_FRAMEBUFFER, renderFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        prog.use();
        light_pos = vec4(0.2f, 1.5f, -3.5f, 1.0f);
        view = glm::lookAt(vec3(0.f, 1.f, 1.f) + pos, vec3(0.f, 1.f, 0.f) + pos, vec3(0.f, 1.0f, 0.f));
        prog.setUniform("Set_fog", set_fog);
        prog.setUniform("LightPosition", view * light_pos);
        prog.setUniform("Veiw", view);
        prog.setUniform("Ks", 0.8f, 0.8f, 0.8f);
        prog.setUniform("Kd", 0.3f, 0.3f, 0.3f);
        prog.setUniform("Time", pos.x + pos.y + pos.z);

        model = glm::translate(mat4(1.f), vec3(0.6f, 0.5f, -4.f));
        prog.setUniform("Model", model);
        prog.setUniform("Ka", 0.9f, 0.5f, 0.3f);
        sphere->render();

        model = glm::translate(mat4(1.f), vec3(0.f, 0.2f, -5.f));
        prog.setUniform("Model", model);
        prog.setUniform("Ka", 0.f, 0.f, 1.f);
        plane->render();

        light_pos = vec4(-1.5f, 1.f, -0.5f, 1.0f);
        prog.setUniform("LightPosition", view * light_pos);
        prog.setUniform("Ka", 0.f, 0.3f, 0.f);

        model = glm::translate(mat4(1.f), vec3(-1.f, 0.5f, -1.f));
        prog.setUniform("Model", model);
        sphere_fog->render();

        model = glm::translate(mat4(1.f), vec3(-2.f, 0.5f, -3.5f));
        prog.setUniform("Model", model);
        sphere_fog1->render();

        model = glm::translate(mat4(1.f), vec3(-1.5f, 0.5f, -2.2f));
        prog.setUniform("Model", model);
        sphere_fog2->render();

        model = glm::translate(mat4(1.f), vec3(0.f, 0.5f, -3.1f));
        prog.setUniform("Model", model);
        prog.setUniform("Ks", 0.f, 0.f, 0.f);
        prog.setUniform("Ka", 0.f, 0.2f, 0.4f);
        sphere_fog3->render();

        prog2.use();
        light_pos = vec4(0.2f, 1.5f, -3.5f, 1.0f);
        prog2.setUniform("LightPosition", view * light_pos);
        prog2.setUniform("Set_fog", set_fog);
        prog2.setUniform("Veiw", view);
        prog2.setUniform("Time", pos.x + pos.y + pos.z);
        sphere_toon->render();

        prog1.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, tex2);
        light_pos = vec4((cos((float)glfwGetTime() - time) / 1.7) + 1, 1.f, -0.7f, 1.0f);
        prog1.setUniform("LightPosition", view * light_pos);
        prog1.setUniform("Set_fog", set_fog);
        prog1.setUniform("Veiw", view);
        prog1.setUniform("Time", pos.x + pos.y + pos.z);
        plane_norm->render();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        prog3.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthTexture);
        glBindVertexArray(VAO);
        prog3.setUniform("Set_post", set_post);
        prog3.setUniform("Set_dof", set_dof);
        prog3.setUniform("Set_fxaa", set_fxaa);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
