#include "MGSpatialSelectionShaders.h"

IMPLEMENT_GLOBAL_SHADER(FMGSpatialSelectionVS, "/Plugin/MGSpatialSelection/MGSpatialSelection.usf", "MainVS", SF_Vertex);
IMPLEMENT_GLOBAL_SHADER(FMGSpatialSelectionPS, "/Plugin/MGSpatialSelection/MGSpatialSelection.usf", "MainPS", SF_Pixel);
