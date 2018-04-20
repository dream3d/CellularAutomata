

#include "CellularAutomataGuiPlugin.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CellularAutomataGuiPlugin::CellularAutomataGuiPlugin()
: CellularAutomataPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CellularAutomataGuiPlugin::~CellularAutomataGuiPlugin() = default;

#include "CellularAutomata/Gui/FilterParameterWidgets/RegisterKnownFilterParameterWidgets.cpp"
