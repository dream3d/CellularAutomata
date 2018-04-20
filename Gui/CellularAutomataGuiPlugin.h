#pragma once

#include "CellularAutomata/CellularAutomataPlugin.h"

class CellularAutomataGuiPlugin : public CellularAutomataPlugin
{
  Q_OBJECT
  Q_INTERFACES(ISIMPLibPlugin)
  Q_PLUGIN_METADATA(IID "net.bluequartz.dream3d.CellularAutomataGuiPlugin")

public:
  CellularAutomataGuiPlugin();
  ~CellularAutomataGuiPlugin() override;

public:
  CellularAutomataGuiPlugin(const CellularAutomataGuiPlugin&) = delete;            // Copy Constructor Not Implemented
  CellularAutomataGuiPlugin(CellularAutomataGuiPlugin&&) = delete;                 // Move Constructor
  CellularAutomataGuiPlugin& operator=(const CellularAutomataGuiPlugin&) = delete; // Copy Assignment Not Implemented
  CellularAutomataGuiPlugin& operator=(CellularAutomataGuiPlugin&&) = delete;      // Move Assignment Not Implemented
};
