#pragma once
#ifndef NODE_H
#define NODE_H

class node {
    public:
        //  0 <= x <= world_size-1 & 0 <= y <= wolrd_size-1   !!
        int parent_x = 0, parent_y = 0;
        // f = g + h
        double f = 0, g = 0, h = 0;
    private:
};

#endif // !NODE_H