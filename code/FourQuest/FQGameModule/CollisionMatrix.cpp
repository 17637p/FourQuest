#include "CollisionMatrix.h"

#include <cassert>
#include <ostream>
#include <fstream>

void fq::physics::CollisionMatrix::Load(const Path& path)
{
    std::ifstream input(path);

    assert(input.is_open());

    std::string line{};

    size_t index = -1;

    while (std::getline(input, line))
    {
        ++index;
        assert(index <= 31);
        data[index] = std::bitset<32>(line);
    }

    input.close();
}

void fq::physics::CollisionMatrix::Save(const Path& directory)
{
    auto filePath = directory / "collision_matrix.txt";
    
    std::ofstream output(filePath);
    assert(output.is_open());

    for (int i = 0; i < static_cast<int>(Tag::End); ++i)
    {
        output << data[i].to_string() << '\n';
    }

    output.close();
}

void fq::physics::CollisionMatrix::Reset()
{
	for (int i = 0; i < static_cast<int>(Tag::End); ++i)
	{
        data[i].reset();
        data[i].flip();
	}
}

