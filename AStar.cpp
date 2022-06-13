#include <queue>
#include <vector>
#include <unordered_map>
#include <functional>
#include <cmath>

#include "eAttribute.h"
#include "Node.h"
#include "Candidate.h"
#include "AStar.h"

AStar::AStar()
	:	mStartPoint(nullptr),
		mEndPoint(nullptr),
		mbIsFindPath(false)
{
	for (int row = 0; row < HEIGHT; ++row)
	{
		for (int col = 0; col < WIDTH; ++col)
		{
			Node* node = &mNodes[row][col];

			node->SetX(col);
			node->SetY(row);
			node->SetAttribute(eAttribute::Road);
			node->AddRoad(GetNodeOrNull(col - 1, row));
			node->AddRoad(GetNodeOrNull(col, row - 1));
			node->AddRoad(GetNodeOrNull(col - 1, row - 1));
			node->AddRoad(GetNodeOrNull(col + 1, row));
			node->AddRoad(GetNodeOrNull(col, row + 1));
			node->AddRoad(GetNodeOrNull(col + 1, row + 1));
			node->AddRoad(GetNodeOrNull(col + 1, row - 1));
			node->AddRoad(GetNodeOrNull(col - 1, row + 1));
		}
	}
}

AStar::~AStar()
{
}

Node* AStar::GetNodeOrNull(int x, int y)
{
	if (x < 0 || WIDTH <= x || y < 0 || HEIGHT <= y)
	{
		return nullptr;
	}
	return &mNodes[y][x];
}

Node* AStar::GetStartPoint()
{
	return mStartPoint;
}

Node* AStar::GetEndPoint()
{
	return mEndPoint;
}

std::unordered_map<Node*, Node*>& AStar::GetPath()
{
	return mPath;
}

void AStar::SetAttribute(int x, int y, eAttribute attribute)
{
	Node* node = &mNodes[y][x];

	if (attribute == eAttribute::StartPoint)
	{
		if (mStartPoint != nullptr)
		{
			mStartPoint->SetAttribute(eAttribute::Road);
		}
		mStartPoint = node;
	}
	else if (attribute == eAttribute::EndPoint)
	{
		if (mEndPoint != nullptr)
		{
			mEndPoint->SetAttribute(eAttribute::Road);
		}
		mEndPoint = node;
	}
	node->SetAttribute(attribute);
}

bool AStar::Ready()
{
	if (mStartPoint == nullptr || mEndPoint == nullptr)
	{
		return false;
	}

	mMinDists.clear();
	mPath.clear();
	mOpen = std::priority_queue<Candidate, std::vector<Candidate>, std::greater<Candidate>>();

	for (int row = 0; row < HEIGHT; ++row)
	{
		for (int col = 0; col < WIDTH; ++col)
		{
			Node* node = &mNodes[row][col];
			eAttribute attribute = node->GetAttribute();

			mMinDists.insert({ node, FLT_MAX });

			if (attribute == eAttribute::Open || attribute == eAttribute::Close)
			{
				node->SetAttribute(eAttribute::Road);
			}
		}
	}
	mbIsFindPath = false;
	mMinDists[mStartPoint] = 0;
	mPath[mStartPoint] = nullptr;
	mOpen.push(Candidate(mStartPoint, 0, GetDistance(mStartPoint, mEndPoint)));

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

	float minDist = mMinDists[openNode];
	float dist = candidate.GetDistance();

	while (minDist < dist)
	{
		candidate = mOpen.top();
		mOpen.pop();

		openNode = candidate.GetNode();

		minDist = mMinDists[openNode];
		dist = candidate.GetDistance();
	}

	if (openNode != mStartPoint)
	{
		openNode->SetAttribute(eAttribute::Close);
	}

	std::vector<Node*>& roads = openNode->GetRoads();

	for (Node* next : roads)
	{
		eAttribute nextAtrribute = next->GetAttribute();

		if (nextAtrribute == eAttribute::Wall || nextAtrribute == eAttribute::Close)
		{
			continue;
		}
		float newDist = dist + next->GetDistance(*openNode);
		float nextMinDist = mMinDists.at(next);

		if (nextMinDist <= newDist)
		{
			continue;
		}
		mMinDists[next] = newDist;
		mPath[next] = openNode;
		if (next == mEndPoint)
		{
			mbIsFindPath = true;
			return;
		}
		next->SetAttribute(eAttribute::Open);
		mOpen.push(Candidate(next, newDist, GetDistance(next, mEndPoint)));
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

bool AStar::IsFindPath()
{
	return mbIsFindPath;
}

float AStar::GetDistance(Node* from, Node* to)
{
	int x = abs(from->GetX() - to->GetX());
	int y = abs(from->GetY() - to->GetY());

	return x + y;
}
