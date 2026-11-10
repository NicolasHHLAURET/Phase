/*
 * Direction.cpp
 *
 *  Created on: 1 févr. 2012
 *      Author: Tiangang Yin
 */

#include "DirectionRoot.hpp"

ostream & operator<<(	ostream & stream,
						const DirectionRoot & direction) {
	double endphi_sub = direction.getEndPhi();
	if (endphi_sub < direction.getBeginPhi()) {
		endphi_sub += 2 * M_PI;
	}
	
	stream.precision(30);
	
	stream << "Direction {";
	stream << "Theta (Begin, Center, End) : " << direction.getBeginTheta();
	stream << ", " << direction.getCenterTheta();
	stream << ", " << direction.getEndTheta();
	stream << " - Phi (Begin, Center, End) " << direction.getBeginPhi();
	stream << ", " << direction.getCenterPhi();
	stream << ", " << endphi_sub;
	stream << " - Solid Angle : " << direction.getSolidAngle();
	stream << " - Angular Sector : " << direction.getAngularSector();
	stream << " - Property : " << DirectionRoot::stringOfProperty(direction.getProperty());
	stream << "}";
	
	return stream;
}
