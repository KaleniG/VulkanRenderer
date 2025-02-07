#pragma once

#include <glm/glm.hpp>

#include "VulkanRenderer/Core/Timestep.h"

namespace vkren
{

  class Camera
  {
  public:
    Camera() = default;
    Camera(glm::vec3 position, glm::vec3 world_up, float yaw, float pitch, float start_speed = 2.5f, float start_zoom = 45.0f, float mouse_sensetivity = 0.1f);

    void OnUpdate(Timestep timestep);

    glm::mat4 GetView() const;
    float GetZoom() const { return m_Zoom; }
    glm::vec3 GetPosition() const { return m_Position; }

  private:
    void UpdateCameraVectors();

  private:
    glm::vec3 m_Position;
    glm::vec3 m_Front;
    glm::vec3 m_Up;
    glm::vec3 m_Right;
    glm::vec3 m_WorldUp;

    float m_Yaw;
    float m_Pitch;
    float m_MovementSpeed;
    float m_MouseSensetivity;
    float m_Zoom;

    bool m_MouseMode = false;
    glm::vec2 m_LastMousePos;
    bool m_FirstMouse = true;
  };

}