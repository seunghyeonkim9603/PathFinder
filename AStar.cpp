#include <queue>
#include <unordered_map>
#include <functional>

#include "eAttribute.h"
#include "Node.h"
#include "Candidate.h"
#include "AStar.h"

AStar::AStar()
	:	mStartPoint(nullptr),
		mEndPoint(nullptr),
		mbIsFindPath(true)
{
	for (int row = 0; row < HEIGHT; ++row)
	{
		for (int col = 0; col < WIDTH; ++col)
		{
			Node* node = &mNodes[row][col];

			node->SetX(col);
			node->SetY(row);
			node->SetAttribute(eAttribute::Road);
		}
	}
}

AStar::~AStar()
{
}

void AStar::SetAttribute(int x, int y, eAttribute attribute)
{
	Node* node = &mNodes[y][x];

	if (attribute == eAttribute::StartPoint && mStartPoint != nullptr)
	{
		mStartPoint->SetAttribute(eAttribute::Road);
	}
	else if (attribute == eAttribute::EndPoint && mEndPoint != nullptr)
	{
		mEndPoint->SetAttribute(eAttribute::Road);
	}

	node->SetAttribute(attribute);
}

bool AStar::Ready()
{
	if (mStartPoint == nullptr || mEndPoint == nullptr)
	{
		return false;
	}

	for (int row = 0; row < HEIGHT; ++row)
	{
		for (int col = 0; col < WIDTH; ++col)
		{
			Node* node = &mNodes[row][col];

			mMinDists.insert({ node, FLT_MAX });
		}
	}
	mbIsFindPath = false;
	mMinDists.insert({ mStartPoint, 0 });
	mPath.insert({ mStartPoint, nullptr });
	mOpen.push(Candidate(mStartPoint, 0, mStartPoint->GetDistance(*mEndPoint)));

	return true;
}

void AStar::DoNextStep()
{
	if (IsEnd())
	{
		return;
	}
	Candidate candidate = mOpen.top();
	mOpen.pop();

	Node* openNode = candidate.GetNode();
	openNode->SetAttribute(eAttribute::Close);

	float minDist = mMinDists.at(openNode);
	float dist = candidate.GetDistance();
	
	if (minDist < dist)
	{
		return;
	}

	std::vector<Node*>& roads = openNode->GetRoads();

	for (Node* next : roads)
	{
		float newDist = dist + next->GetDistance(*openNode);
		float nextMinDist = mMinDists.at(next);

		if (nextMinDist <= newDist)
		{
			continue;
		}

		mMinDists.insert({ next, newDist });
		mPath.insert({ next, openNode });
		if (next == mEndPoint)
		{
			mbIsFindPath = true;
			return;
		}

		mOpen.push(Candidate(next, newDist, next->GetDistance(*mEndPoint)));
	}
}

void AStar::Render()
{
}

void AStar::Clear()
{
	mMinDists.clear();
	mPath.clear();
	mOpen = std::priority_queue<Candidate, std::vector<Candidate>, std::greater<Candidate>>();

	for (int row = 0; row < HEIGHT; ++row)
	{
		for (int col = 0; col < WIDTH; ++col)
		{
			Node* node = &mNodes[row][col];

			node->SetAttribute(eAttribute::Road);
		}
	}
}

bool AStar::IsEnd()
{
	if (mOpen.empty() || mbIsFindPath)
	{
		return true;
	}
	return false;
}
