#pragma once

class Entity{
    public:
    bool grabbable = true;
    bool collision = true;
    bool isGrabbed = false;

    public:
    bool grabEntity();
};