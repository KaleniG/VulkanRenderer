#include <vkrenpch.h>

#include <glm/gtc/matrix_transform.hpp>

#include "VulkanRenderer/Core/Input.h"
#include "VulkanRenderer/Core/Application.h"
#include "VulkanRenderer/GameComponents/Camera.h"

namespace vkren
{

  Camera::Camera(glm::vec3 position, glm::vec3 world_up, float yaw, float pitch, float start_speed, float start_zoom, float mouse_sensetivity)
    : m_Position(position), m_WorldUp(world_up), m_Yaw(yaw), m_Pitch(pitch), m_MovementSpeed(start_speed), m_Zoom(start_zoom), m_MouseSensetivity(mouse_sensetivity)
  {
    Camera::UpdateCameraVectors();
  }

  void Camera::OnUpdate(Timestep timestep)
  {
    // KEYBOARD LOGIC
    {
      const float velocity = m_MovementSpeed * timestep;

      if (Input::IsKeyPressed(Key::W))
        m_Position += m_Front * velocity;
      if (Input::IsKeyPressed(Key::A))
        m_Position -= m_Right * velocity;
      if (Input::IsKeyPressed(Key::S))
        m_Position -= m_Front * velocity;
      if (Input::IsKeyPressed(Key::D))
        m_Position += m_Right * velocity;

      // TEMP
      if (Input::IsKeyPressed(Key::Space))
        m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    // MOUSE KEY LOGIC
    {
      if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
      {
        if (!m_MouseMode)
        {
          Application::GetWindow().SetCursorEnabled(false);
          m_MouseMode = true;
        }
      }
      else if(m_MouseMode)
      {
        Application::GetWindow().SetCursorEnabled(true);
        m_MouseMode = false;
        m_FirstMouse = true;
      }
    }

    if (m_MouseMode)
    {
      // MOUSE POSITION
      {
        glm::vec2 mousePos = Input::GetMousePos();

        if (m_FirstMouse)
        {
          m_LastMousePos = mousePos;
          m_FirstMouse = false;
        }

        if (mousePos != m_LastMousePos)
        {
          float xoffset = mousePos.x - m_LastMousePos.x;
          float yoffset = m_LastMousePos.y - mousePos.y;

          m_LastMousePos = mousePos;

          xoffset *= m_MouseSensetivity;
          yoffset *= m_MouseSensetivity;

          m_Yaw += xoffset;
          m_Pitch += yoffset;

          m_Pitch = std::clamp(m_Pitch, -89.9999999999f, 89.9999999999f);

          Camera::UpdateCameraVectors();
        }
      }
    }
  }

  glm::mat4 Camera::GetView() const
  {
    return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
  }

  void Camera::UpdateCameraVectors()
  {
    glm::vec3 front;
    front.x = std::cos(glm::radians(m_Yaw)) * std::cos(glm::radians(m_Pitch));
    front.y = std::sin(glm::radians(m_Pitch));
    front.z = std::sin(glm::radians(m_Yaw)) * std::cos(glm::radians(m_Pitch));
    m_Front = glm::normalize(front);
    m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
    m_Up    = glm::normalize(glm::cross(m_Right, m_Front));
  }

}