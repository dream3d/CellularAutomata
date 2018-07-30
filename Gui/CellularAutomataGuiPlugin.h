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
  
  /**
   * @brief Register all the filters with the FilterWidgetFactory
   */
  void registerFilterWidgets(FilterWidgetManager* fwm) override;
  

public:
  CellularAutomataGuiPlugin(const CellularAutomataGuiPlugin&) = delete;            // Copy Constructor Not Implemented
  CellularAutomataGuiPlugin(CellularAutomataGuiPlugin&&) = delete;                 // Move Constructor Not Implemented
  CellularAutomataGuiPlugin& operator=(const CellularAutomataGuiPlugin&) = delete; // Copy Assignment Not Implemented
  CellularAutomataGuiPlugin& operator=(CellularAutomataGuiPlugin&&) = delete;      // Move Assignment Not Implemented
};
