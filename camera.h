#pragma once

#include "geometry.h"

class Camera
{
public:
    Camera(Vec4f pos, Vec4f target, Vec4f up) :
        _pos(pos), _posTemp(pos), _target(target), _up(up) {
    }
    virtual ~Camera() noexcept {}

    void setPos(const Vec4f& pos)
    {
        _pos = pos;
        calcMatrix();
    }

    void setPerspectiveForLH(float fov, float aspect, float nearZ, float farZ)
    {
        _fov = fov; _aspect = aspect; _nearZ = nearZ; _farZ = farZ;
        calcMatrix();
    }

public:
    // 世界坐标系转到投影平面
    Mat4x4f _worldToProjection;

private:
    void calcMatrix()
    {
        Vec4f dir = { _target.x - _pos.x, _target.y - _pos.y, _target.z - _pos.z, 0.0f };
        Vec4f w = normallize(dir);
        Vec4f u = normallize(cross(_up, w));
        Vec4f v = cross(w, u);

        _worldToView = {
            u.x, v.x, w.x, 0.0f,
            u.y, v.y, w.y, 0.0f,
            u.z, v.z, w.z, 0.0f,
            -dot(_pos, u), -dot(_pos, v), -dot(_pos, w), 1.0
        };

        float f = 1.0f / (float)tan(_fov * 0.5f);
        _viewToProjection = {
            f / _aspect, 0.0f, 0.0f, 0.0f,
            0.0f, f, 0.0f, 0.0f,
            0.0f, 0.0f, _farZ / (_farZ - _nearZ), 1.0f,
            0.0f, 0.0f, -_nearZ * _farZ / (_farZ - _nearZ), 0.0f
        };

        _worldToProjection = mul(_worldToView, _viewToProjection);
    }

private:
    Vec4f _pos;
    Vec4f _posTemp;
    Vec4f _target;
    Vec4f _up;
    float _fov;
    float _aspect;
    float _nearZ;
    float _farZ;

    // 世界坐标系转观察坐标系
    Mat4x4f _worldToView;
    // 观察坐标系转投影坐标系
    Mat4x4f _viewToProjection;
};
