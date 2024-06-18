#pragma once

#include "DrawDebugHelpers.h"

#define DRAW_SPHERE(location) if(GetWorld()) DrawDebugSphere(GetWorld(), location, 25.f, 24, FColor::Green, true);
#define DRAW_LINE(StartLocation, EndLocation) if(GetWorld()) DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, true, -1.f, 0, 1.f);
#define DRAW_POINT(Location) if(GetWorld()) DrawDebugPoint(GetWorld(), Location, 15.f, FColor::Red, true);
#define DRAW_VECTOR(StartLocation, EndLocation) if(GetWorld()) \
{\
	DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, true, -1.f, 0, 1.f);\
	DrawDebugPoint(GetWorld(), EndLocation, 15.f, FColor::Red, true);\
}