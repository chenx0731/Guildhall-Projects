#pragma once
#include <vector>

template< typename T>
void ClearAndDeleteEverything(std::vector<T*>& myVector)
{
	for (int index = 0; index < (int)myVector.size(); index++) {
		delete myVector[index];
	}
	myVector.clear();
}

template< typename T>
void NullAnyReferencesToThisObject(std::vector<T*>& myVector, T* objectToStopPointingThing)
{
	for (int index = 0; index < (int)myVector.size(); index++) {
		if (myVector[index] == objectToStopPointingThing)
		{
			myVector[index] = nullptr;
		}
	}

}