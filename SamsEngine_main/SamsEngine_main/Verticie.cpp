#include "Verticie.h"


ConnectingVertex::~ConnectingVertex()
{
	for (unsigned int i = 0; i < ConnectingPoints.GetSize(); i++)
	{
		delete ConnectingPoints[i];
	}
}

void ConnectingVertex::DeepCopy(Array<ConnectingVertex*>& other, Array<ConnectingVertex*>& self)
{
	for (unsigned int i = 0; i < self.GetSize(); i++)
	{
		ConnectingVertex* vert = self[i];
		for (unsigned int j = 0; j < vert->ConnectingPoints.GetSize(); j++)
		{
			bool bInside = false;
			for (unsigned int p = 0; p < other.GetSize(); p++)
			{
				if (other[p] == vert->ConnectingPoints[j])
				{
					bInside = true;
					vert->ConnectingPoints[j] = other[p];
					break;
				}
			}
			if (!bInside)
			{
				ConnectingVertex* point = new ConnectingVertex(*vert->ConnectingPoints[j]);
				other.Add(point);
			}

		}
		other.Add(vert);
	}
}