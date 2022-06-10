#pragma once

class Candidate final
{
public:
	Candidate(Node* node, float g, float h);
	~Candidate() = default;

	Node* GetNode();
	float GetDistance() const;

	bool operator<(const Candidate& other) const;
private:
	Node* mNode;
	float mG;
	float mH;
};