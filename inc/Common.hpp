#pragma once

#include <glm/glm.hpp>

// Constants used in the project, they relate to the size of board and board textures
constexpr float square_size = 64.0f;
constexpr float border_size = square_size / 2.0f;

// Ray structure
struct Ray
{
    // Origin
    glm::vec3 O;
    // Direction (normalized)
    glm::vec3 D;
};

// Plane structure, with the equation ax + by + cz + d = 0
struct Plane
{
    float a, b, c, d;
};

// Function to find the intersection between a ray and a plane
constexpr auto find_intersection(const Ray& ray, const Plane& plane) -> glm::vec3
{
    const float t =
        -(plane.a * ray.O.x + plane.b * ray.O.y + plane.c * ray.O.z + plane.d) /
        (plane.a * ray.D.x + plane.b * ray.D.y + plane.c * ray.D.z);

    return ray.O + t * ray.D;
};
