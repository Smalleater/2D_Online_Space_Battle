#ifndef VECTOR_HPP
#define VECTOR_HPP

struct Vector2f
{
    float x;
    float y;

    Vector2f(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}

    Vector2f operator+(const Vector2f& other) const
    {
        return Vector2f(x + other.x, y + other.y);
    }

    Vector2f& operator+=(const Vector2f& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vector2f operator*(float scalar) const
    {
        return Vector2f(x * scalar, y * scalar);
    }
};

#endif
