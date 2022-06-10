#pragma once

class Node final
{
public:
	Node() = default;
	~Node() = default;

	int GetX() const;
	void SetX(int x);

	int GetY() const;
	void SetY(int y);

	eAttribute GetAttribute() const;
	void SetAttribute(eAttribute attribute);

	float GetDistance(const Node& to) const;

	void AddRoad(Node* node);
	std::vector<Node*>& GetRoads();

private:
	std::vector<Node*> mRoads;
	eAttribute mAttribute;
	int mX;
	int mY;
};