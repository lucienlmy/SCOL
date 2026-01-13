#pragma once
#include "Vector3.hpp"

namespace rage
{
    class scrVector
    {
    public:
        alignas(8) float x{};
        alignas(8) float y{};
        alignas(8) float z{};

        constexpr scrVector()
            : x(),
              y(),
              z()
        {
        }

        constexpr scrVector(float x, float y, float z)
            : x(x),
              y(y),
              z(z)
        {
        }

        scrVector(rage::Vector3 vec)
            : x(vec.x),
              y(vec.y),
              z(vec.z)
        {
        }

        operator bool() const
        {
            return this->x != 0.0f || this->y != 0.0f || this->z != 0.0f;
        }

        operator rage::Vector3()
        {
            return {x, y, z};
        }

        bool operator==(const scrVector& other) const
        {
            return this->x == other.x && this->y == other.y && this->z == other.z;
        }

        bool operator!=(const scrVector& other) const
        {
            return this->x != other.x || this->y != other.y || this->z != other.z;
        }

        scrVector operator+(const scrVector& other) const
        {
            scrVector vec;
            vec.x = this->x + other.x;
            vec.y = this->y + other.y;
            vec.z = this->z + other.z;
            return vec;
        }

        scrVector operator-(const scrVector& other) const
        {
            scrVector vec;
            vec.x = this->x - other.x;
            vec.y = this->y - other.y;
            vec.z = this->z - other.z;
            return vec;
        }

        scrVector operator*(const scrVector& other) const
        {
            scrVector vec;
            vec.x = this->x * other.x;
            vec.y = this->y * other.y;
            vec.z = this->z * other.z;
            return vec;
        }

        scrVector operator/(const scrVector& other) const
        {
            scrVector vec;
            vec.x = this->x / other.x;
            vec.y = this->y / other.y;
            vec.z = this->z / other.z;
            return vec;
        }
    };
    static_assert(sizeof(scrVector) == 0x18);
}