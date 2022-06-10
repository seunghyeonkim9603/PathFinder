#include <vector>
#include <cmath>

#include "eAttribute.h"
#include "Node.h"


void Node::SetX(int x)
{
    mX = x;
}

void Node::SetY(int y)
{
    mY = y;
}

void Node::SetAttribute(eAttribute attribute)
{
    mAttribute = attribute;
}

int Node::GetX() const
{
    return mX;
}

int Node::GetY() const
{
    return mY;
}

eAttribute Node::GetAttribute() const
{
    return mAttribute;
}

float Node::GetDistance(const Node& to) const
{
    int xDist = abs(mX - to.mX);
    int yDist = abs(mY - to.mY);

    if (xDist < yDist)
    {
        return xDist * 1.5f + (yDist - xDist);
    }
    return yDist * 1.5f + (xDist - yDist);
}

void Node::AddRoad(Node* node)
{
    mRoads.push_back(node);
}

std::vector<Node*>& Node::GetRoads()
{
    return mRoads;
}
