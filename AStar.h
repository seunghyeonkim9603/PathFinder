#pragma once

class AStar final
{
public:
	AStar();
	~AStar();

	Node* GetEndPoint();
	Node* GetStartPoint();
	Node* GetNodeOrNull(int x, int y);
	float GetDistance(Node* from, Node* to);

	void SetAttribute(int x, int y, eAttribute attribute);

	bool Ready();
	void DoNextStep();

	bool IsEnd();
	bool IsFindPath();

	std::unordered_map<Node*, Node*>& GetPath();
private:
	enum { WIDTH = 100, HEIGHT = 50 };

	Node mNodes[HEIGHT][WIDTH];
	std::priority_queue<Candidate, std::vector<Candidate>, std::greater<Candidate>> mOpen;
	std::unordered_map<Node*, Node*> mPath;
	std::unordered_map<Node*, float> mMinDists;

	Node* mStartPoint;
	Node* mEndPoint;
	bool mbIsFindPath;
};