#pragma once

class INmiListener
{
public:
    virtual void onNMI() = 0;

    virtual ~INmiListener() {};
};
