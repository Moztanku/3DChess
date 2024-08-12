#include "Chess/Board.hpp"

#include <fstream>

namespace
{
    auto parse_layout(const std::vector<std::string>& layout,
        const uint width,
        const uint height) -> std::unordered_map<Chess::Pos, Chess::Piece*, Chess::PosKeyFuncs>
    {
        std::unordered_map<Chess::Pos, Chess::Piece*, Chess::PosKeyFuncs> pieces{};

        for (uint y = 0; y < height; y++)
        {
            uint first = layout[y].find_first_not_of(' ');

            for (uint x = 0; x < width; x++)
            {
                if (layout[y][first] == '.')
                {
                    first = layout[y].find_first_not_of(' ', first + 1);
                    continue;
                }

                // color
                const char c = layout[y][first];
                // piece type
                const char p = layout[y][first + 1];

                first = layout[y].find_first_not_of(' ', first + 2);

                const Chess::Pos pos{
                    x, height - (y + 1)};
                const Chess::Player color =
                    c == 'W' ? Chess::Player::White : Chess::Player::Black;
                
                Chess::Piece* piece = nullptr;

                if (p == '0')
                    piece = new Chess::Pawn{pos, color};
                else if (p == '1')
                    piece = new Chess::Bishop{pos, color};
                else if (p == '2')
                    piece = new Chess::Knight{pos, color};
                else if (p == '3')
                    piece = new Chess::Rook{pos, color};
                else if (p == '4')
                    piece = new Chess::Queen{pos, color};
                else if (p == '5')
                    piece = new Chess::King{pos, color};
                else
                {
                    std::cerr << "Unknown piece type: " << p << std::endl;
                    std::exit(EXIT_FAILURE);
                }

                pieces[pos] = piece;
            }
        }

        return pieces;
    }
} // namespace

namespace Chess
{

Board::Board(const std::string_view layout_file)
{
    std::fstream file{layout_file.data(), std::ios::in};

    if (!file.is_open())
    {
        std::cerr << "Failed to open file " << layout_file << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::string line;

    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        // Split the line into words separated by spaces
        std::vector<std::string_view> words{};

        size_t start = 0;
        size_t end = 0;

        while (end != std::string::npos)
        {
            end = line.find(' ', start);

            words.push_back(
                std::string_view{line}.substr(start, end - start)
            );

            start = end + 1;
        }

        // Parse the words
        if (words[0] == "size")
        {
            m_width = std::stol(std::string{words[1]});
            m_height = std::stol(std::string{words[2]});

            m_chessboard = new Renderer::Chessboard{m_width, m_height};
        }
        else if (words[0] == "player")
        {
            // TODO
        }
        else if (words[0] == "layout")
        {
            std::vector<std::string> layout{};

            for (size_t i = 0; i < m_height; i++)
            {
                std::getline(file, line);

                layout.push_back(line);
            }

            m_pieces = parse_layout(layout, m_width, m_height);

            break;
        }
        else
        {
            std::cerr << "Unknown command: " << line << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    file.close();

    // Find the kings
    for (const auto& [pos, piece] : m_pieces)
    {
        if (piece->get_type() == "King")
        {
            if (piece->get_color() == Player::White)
            {
                if (m_KingWhite)
                {
                    std::cerr << "Multiple white kings found" << std::endl;
                    std::exit(EXIT_FAILURE);
                }
                else
                    m_KingWhite = dynamic_cast<King*>(piece);
            }
            else
            {
                if (m_KingBlack)
                {
                    std::cerr << "Multiple black kings found" << std::endl;
                    std::exit(EXIT_FAILURE);
                }
                else
                    m_KingBlack = dynamic_cast<King*>(piece);
            }
        }
    }

    if (!m_KingWhite || !m_KingBlack)
    {
        std::cerr << "No kings found" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    s_board = this;
}

auto Board::get_piece(Pos where) const -> const Piece*
{
    const auto it = m_pieces.find(where);

    if (it == m_pieces.end())
        return nullptr;

    return it->second;
}

auto Board::draw(const glm::mat4& projView) const -> void
{
    m_chessboard->render(projView);

    for (const auto& [pos, piece] : m_pieces)
    {
        piece->draw(projView);
    }
}

auto Board::get_board() -> const Board&
{
    if (!s_board)
    {
        std::cerr << "Board not initialized!" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    return *s_board;
}

Board* Board::s_board = nullptr;

} // namespace Chess
