#include <vector>

#include "eAttribute.h"
#include "Node.h"
#include "Candidate.h"

Candidate::Candidate(Node* node, float g, float h)
    :   mNode(node),
        mG(g),
        mH(h)
{
}

Node* Candidate::GetNode()
{
    return mNode;
}

float Candidate::GetDistance() const
{
    return mG;
}

bool Candidate::operator>(const Candidate& other) const
{
    return mG + mH > other.mG + other.mH;
}
