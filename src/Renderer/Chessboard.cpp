#include "Renderer/Chessboard.hpp"

#include "Common.hpp"

namespace
{

// Convenience function to add a vertex to the vertex buffer
constexpr auto add_vertex(std::vector<float>& vs, glm::vec3 pos, glm::vec2 tex) -> void
{
    vs.push_back(pos.x);
    vs.push_back(pos.y);
    vs.push_back(pos.z);
    vs.push_back(tex.x);
    vs.push_back(tex.y);
}

// Creates a quad with the given size, at the given position, and with given texture position
auto add_cell(std::vector<float>& vs, glm::vec2 size, glm::vec3 pos_shift, glm::vec2 tex_shift) -> void
{
    constexpr float sq = square_size;
    constexpr float bo = border_size;
    
    constexpr float tex_w = 2 * sq + 2 * bo;
    constexpr float tex_h = sq + 2 * bo;

    const auto x = size.x;
    const auto y = size.y;

    tex_shift.x = tex_shift.x / tex_w;
    tex_shift.y = tex_shift.y / tex_h;

    add_vertex(
        vs,
        glm::vec3{0.f, 0.f, 0.f} + pos_shift,
        glm::vec2{0.f, 0.f} + tex_shift);
    add_vertex(
        vs,
        glm::vec3{x, 0.f, 0.f} + pos_shift,
        glm::vec2{x / tex_w, 0.f} + tex_shift);
    add_vertex(
        vs,
        glm::vec3{x, y, 0.f} + pos_shift,
        glm::vec2{x / tex_w, y / tex_h} + tex_shift);


    add_vertex(
        vs,
        glm::vec3{0.f, 0.f, 0.f} + pos_shift,
        glm::vec2{0.f, 0.f} + tex_shift);
    add_vertex(
        vs,
        glm::vec3{x, y, 0.f} + pos_shift,
        glm::vec2{x / tex_w, y / tex_h} + tex_shift);
    add_vertex(
        vs,
        glm::vec3{0.f, y, 0.f} + pos_shift,
        glm::vec2{0.f, y / tex_h} + tex_shift);
}

// Creates a side that is perpendicular to the x or y axis
auto add_side(std::vector<float>& vs, glm::vec2 size, bool xaxis, glm::vec3 pos_shift, glm::vec2 tex_shift)
{
    constexpr float sq = square_size;
    constexpr float bo = border_size;
    
    constexpr float tex_w = 2 * sq + 2 * bo;
    constexpr float tex_h = sq + 2 * bo;

    const auto x = size.x;
    const auto y = size.y;

    tex_shift.x = tex_shift.x / tex_w;
    tex_shift.y = tex_shift.y / tex_h;

    const float x1 = xaxis ? x : 0.f;
    const float y1 = xaxis ? 0.f : x;

    add_vertex(
        vs,
        glm::vec3{0.f, 0.f, -y} + pos_shift,
        glm::vec2{0.f, 0.f} + tex_shift);
    add_vertex(
        vs,
        glm::vec3{x1, y1, -y} + pos_shift,
        glm::vec2{x / tex_w, 0.f} + tex_shift);
    add_vertex(
        vs,
        glm::vec3{x1, y1, 0.f} + pos_shift,
        glm::vec2{x / tex_w, y / tex_h} + tex_shift);

    add_vertex(
        vs,
        glm::vec3{0.f, 0.f, -y} + pos_shift,
        glm::vec2{0.f, 0.f} + tex_shift);
    add_vertex(
        vs,
        glm::vec3{x1, y1, 0.f} + pos_shift,
        glm::vec2{x / tex_w, y / tex_h} + tex_shift);
    add_vertex(
        vs,
        glm::vec3{0.f, 0.f, 0.f} + pos_shift,
        glm::vec2{0.f, y / tex_h} + tex_shift);
}

} // namespace

namespace Renderer
{

// Creates a chessboard 3D model with the given width and height (in squares)
Chessboard::Chessboard(Chess::Pos size)
{
    constexpr float sq = square_size;
    constexpr float bo = border_size;

    const int width = size.x;
    const int height = size.y;

    std::vector<float> vertices;

    // width + 2 and height + 2 because we need to add the border vertices
    // * 6 because for each square we have 2 triangles with 3 vertices each (we can't use index buffer)
    // * 5 because each vertex has 5 components (x, y, z, u, v)
    vertices.reserve((width + 2) * (height + 2) * 6 * 5);

    // Corners
    add_cell(vertices,
        glm::vec2{bo, bo},
        glm::vec3{0.f, 0.f, 0.f},
        glm::vec2{0.f, 0.f}); // Bottom left
    add_cell(vertices, 
        glm::vec2{bo, bo},
        glm::vec3{bo + width * sq, 0.f, 0.f},
        glm::vec2{sq + bo, 0.f}); // Bottom right
    add_cell(vertices, 
        glm::vec2{bo, bo},
        glm::vec3{0.f, bo + height * sq, 0.f},
        glm::vec2{0.f, sq + bo}); // Top left
    add_cell(vertices, 
        glm::vec2{bo, bo},
        glm::vec3{bo + width * sq, bo + height * sq, 0.f},
        glm::vec2{sq + bo, sq + bo}); // Top right

    // Bottom border
    for (int x = 0; x < width; x++)
        add_cell(vertices,
            glm::vec2{sq, bo},
            glm::vec3{bo + x * sq, 0.f, 0.f},
            glm::vec2{bo, 0.f});
    // Top border
    for (int x = 0; x < width; x++)
        add_cell(vertices,
            glm::vec2{sq, bo},
            glm::vec3{bo + x * sq, bo + height * sq, 0.f},
            glm::vec2{bo, sq + bo});
    // Left border
    for (int y = 0; y < height; y++)
        add_cell(vertices,
            glm::vec2{bo, sq},
            glm::vec3{0.f, bo + y * sq, 0.f},
            glm::vec2{0.f, bo});
    // Right border
    for (int y = 0; y < height; y++)
        add_cell(vertices,
            glm::vec2{bo, sq},
            glm::vec3{bo + width * sq, bo + y * sq, 0.f},
            glm::vec2{sq + bo, bo});

    // Middle
    for (int y = 0; y < height; y++)
    for (int x = 0; x < width; x++)
    {
        const bool is_white = (x + y) % 2 == 1;

        add_cell(vertices,
            glm::vec2{sq, sq},
            glm::vec3{bo + x * sq, bo + y * sq, 0.f},
            is_white ? glm::vec2{sq + 2*bo, sq} : glm::vec2{sq + 2*bo, 0.f});
    }

    //// Sides
    // Corners
    add_side(vertices, glm::vec2{bo, bo}, true, {0.f, 0.f, 0.f}, glm::vec2{bo, bo});
    add_side(vertices, glm::vec2{bo, bo}, true, {bo + width * sq, 0.f, 0.f}, glm::vec2{bo, bo});
    add_side(vertices, glm::vec2{bo, bo}, true, {0.f, 2 * bo + height * sq, 0.f}, glm::vec2{bo, bo});
    add_side(vertices, glm::vec2{bo, bo}, true, {bo + width * sq, 2 * bo + height * sq, 0.f}, glm::vec2{bo, bo});

    add_side(vertices, glm::vec2{bo, bo}, false, {0.f, 0.f, 0.f}, glm::vec2{bo, bo});
    add_side(vertices, glm::vec2{bo, bo}, false, {2 * bo + width * sq, 0.f, 0.f}, glm::vec2{bo, bo});
    add_side(vertices, glm::vec2{bo, bo}, false, {0.f, bo + height * sq, 0.f}, glm::vec2{bo, bo});
    add_side(vertices, glm::vec2{bo, bo}, false, {2 * bo + width * sq, bo + height * sq, 0.f}, glm::vec2{bo, bo});

    // Walls
    for (int x = 0; x < width; x++)
    {
        add_side(vertices, glm::vec2{sq, bo}, true, glm::vec3{bo + x * sq, 0.f, 0.f}, glm::vec2{bo, sq});
        add_side(vertices, glm::vec2{sq, bo}, true, glm::vec3{bo + x * sq, 2 * bo + height * sq, 0.f}, glm::vec2{bo, sq});
    }

    for (int y = 0; y < height; y++)
    {
        add_side(vertices, glm::vec2{sq, bo}, false, glm::vec3{0.f, bo + y * sq, 0.f}, glm::vec2{bo, sq});
        add_side(vertices, glm::vec2{sq, bo}, false, glm::vec3{2 * bo + width * sq, bo + y * sq, 0.f}, glm::vec2{bo, sq});
    }

    m_VBO = std::make_unique<GPU::VertexBuffer>(vertices.data(), vertices.size() * sizeof(float));
    m_VAO = std::make_unique<GPU::VertexArray>();

    GPU::VertexBufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(2);

    m_VAO->AddBuffer(*m_VBO, layout);
}

// Renders the chessboard with the given MVP matrix
auto Chessboard::render(const glm::mat4& MVP) -> void
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_Shader.Bind();
    m_Shader.SetUniformM("uMVP", MVP);

    m_Texture.Bind();
    m_Shader.SetUniform("uTexture", 0);

    m_VAO->Bind();
    glDrawArrays(GL_TRIANGLES, 0, m_VBO->GetCount() / 5);

    m_VAO->Unbind();
    m_Texture.Unbind();
    m_Shader.Unbind();
}

} // namespace Renderer
