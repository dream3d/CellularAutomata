/*
 * Your License should go here
 */
#ifndef _cellularautomataconstants_h_
#define _cellularautomataconstants_h_

#include <QtCore/QString>

/**
* @brief This namespace is used to define some Constants for the plugin itself.
*/
namespace CellularAutomata
{
  const QString CellularAutomataPluginFile("CellularAutomataPlugin");
  const QString CellularAutomataPluginDisplayName("CellularAutomata");
  const QString CellularAutomataBaseName("CellularAutomata");
}

/**
* @brief Use this namespace to define any custom GUI widgets that collect FilterParameters
* for a filter. Do NOT define general reusable widgets here.
*/
namespace FilterParameterWidgetType
{

  /*  const QString SomeCustomWidget("SomeCustomWidget"); */

}


namespace BlueQuartz
{
  const QString VendorName("BlueQuartz Software, LLC");
  const QString URL("http://www.bluequartz.net");
  const QString Copyright("(C) 2016 BlueQuartz Software, LLC");
}


#endif
