#include "headers/Camera.h"

Camera::Camera(int width, int height, glm::vec3 position, GLFWwindow* window)
{
    Camera::width = width;
    Camera::height = height;
    Position = position;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetWindowUserPointer(window, this);
    glfwSetCursorPosCallback(window, MouseCallback);
}

void Camera::Matrix(float FOVdeg, float nearPlane, float farPlane, ShaderProgram& shader, const char* uniform)
{
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    view = glm::lookAt(Position, Position + front, Up);
    projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);

    glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(projection * view));
}

void Camera::Inputs(GLFWwindow* window)
{
    // Movement
    if (!cursorModeInitialized)
    {
        glfwSetWindowUserPointer(window, this);
        glfwSetCursorPosCallback(window, MouseCallback);

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        mouseCaptured = true;
        firstMouse = true;
        cursorModeInitialized = true;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        Position += speed * front;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        Position -= speed * front;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        Position -= speed * glm::normalize(glm::cross(front, Up));

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        Position += speed * glm::normalize(glm::cross(front, Up));

    // Initialize pointer lock once.
    if (!cursorModeInitialized)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        mouseCaptured = true;
        firstMouse = true;
        cursorModeInitialized = true;
    }

    // Escape releases the mouse.
    bool escapeDown =
        glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;

    if (escapeDown && !escapeWasDown)
    {
        mouseCaptured = !mouseCaptured;

        glfwSetInputMode(
            window,
            GLFW_CURSOR,
            mouseCaptured
                ? GLFW_CURSOR_DISABLED
                : GLFW_CURSOR_NORMAL
        );

        firstMouse = true;
    }

    escapeWasDown = escapeDown;

    // F1 also toggles pointer lock.
    bool toggleDown =
        glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS;

    if (toggleDown && !toggleWasDown)
    {
        mouseCaptured = !mouseCaptured;

        glfwSetInputMode(
            window,
            GLFW_CURSOR,
            mouseCaptured
                ? GLFW_CURSOR_DISABLED
                : GLFW_CURSOR_NORMAL
        );

        firstMouse = true;
    }

    toggleWasDown = toggleDown;

    // Mouse-look only while the cursor is captured.
    if (mouseCaptured)
    {
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        if (firstMouse)
        {
            lastX = mouseX;
            lastY = mouseY;
            firstMouse = false;
        }

        float offsetX = static_cast<float>(mouseX - lastX);
        float offsetY = static_cast<float>(lastY - mouseY);

        lastX = mouseX;
        lastY = mouseY;

        float rotx = sensitivity * offsetY;
        float roty = sensitivity * offsetX;

        glm::vec3 right =
            glm::normalize(glm::cross(front, Up));

        glm::vec3 newFront = glm::normalize(
            glm::rotate(
                front,
                glm::radians(rotx),
                right
            )
        );

        // Prevent looking directly past the vertical limits.
        float verticalAngle =
            glm::degrees(glm::angle(newFront, Up));

        if (verticalAngle > 5.0f &&
            verticalAngle < 175.0f)
        {
            front = newFront;
        }

        front = glm::normalize(
            glm::rotate(
                front,
                glm::radians(-roty),
                Up
            )
        );
    }
}

float Camera::getPositionX() {
    return Position.x;
}

float Camera::getPositionZ() {
    return Position.z;
}

glm::vec3 Camera::getCameraPos(){
    return Position;
}

glm::vec3 Camera::getCameraDirection(){
    return front;
}

void Camera::MouseCallback(
    GLFWwindow* window,
    double xpos,
    double ypos)
{
    auto* camera = static_cast<Camera*>(
        glfwGetWindowUserPointer(window)
    );

    if (!camera || !camera->mouseCaptured)
        return;

    if (camera->firstMouse)
    {
        camera->lastX = xpos;
        camera->lastY = ypos;
        camera->firstMouse = false;
        return;
    }

    float offsetX = static_cast<float>(xpos - camera->lastX);
    float offsetY = static_cast<float>(camera->lastY - ypos);

    camera->lastX = xpos;
    camera->lastY = ypos;

    float yaw = camera->sensitivity * offsetX;
    float pitch = camera->sensitivity * offsetY;

    glm::vec3 right = glm::normalize(
        glm::cross(camera->front, camera->Up)
    );

    glm::vec3 newFront = glm::normalize(
        glm::rotate(
            camera->front,
            glm::radians(pitch),
            right
        )
    );

    float angle = glm::degrees(
        glm::angle(newFront, camera->Up)
    );

    if (angle > 5.0f && angle < 175.0f)
        camera->front = newFront;

    camera->front = glm::normalize(
        glm::rotate(
            camera->front,
            glm::radians(-yaw),
            camera->Up
        )
    );
}