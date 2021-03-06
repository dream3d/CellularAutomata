RecrystalizeVolume {#recrystalizevolume}
=====

## Group (Subgroup) ##
CellularAutomata (CellularAutomata)


## Description ##
This filter simulates recrystallization using cellular automata. Initially all voxels are unrecrystallized. Cells are evolved with the following rules until all cells are recrystallized:
	
1. If at least one neighbor is recrystallized, pick a random recrystallized neighbor to join
2. If no neighbors are recrystallized a new grain may spontaneously nucleate (if the extended Moore neighborhood is also unrecrystallized and according to the nucleation rate).

Six types of neighborhoods are available:

1. Von Neumann: 6 nearest neighbors (face connected)
2. 8 Cell: 8 nearest neighbors (6 face connected, 2 randomly selected opposing edge connected)
3. 14 Cell: 14 neighbors (6 face connected, 2 randomly selected opposing corner connected + 6 adjacent edge connected)
4. 18 Cell: 18 nearest neighbors (6 face connected, 12 edge connected)
5. 20 Cell: 20 nearest neighbors (6 face connected, 12 edge connected, 2 randomly selected opposing corner connected)
6. Moore: 26 nearest neighbors (6 face connected, 12 edge connected, 8 corner connected)

The fraction of volume recrytsallized at each time step is saved and fit to the Avrami equation: f(t) = 1 - exp( -K * t ^ n ).


## Parameters ##
| Name             | Type |
|------------------|------|
| Nucleation Rate | Float |
| Neighborhood Type | Choice |
| Dimensions | Integer |
| Resolution | Float |
| Origin | Float |

## Required Arrays ##
None


## Created Arrays ##

| Type | Default Array Name | Description | Comment |
|------|--------------------|-------------|---------|
| Int  | FeatureIds           | Unique ID's for recrystallized grains | numbered in order of generation   |
| Int  | RecrystallizationTime           | Time step of assignment to current feature |  |
| Bool | Active	| Active flag for grains | true for all features except feature 0 |
| Int  | RecrystallizationHistory	| Percent volume recrystallized at each time step |  |
| Int  | AvramiParameters	| Avrami parameters fit to RecrystallizationHistory | K, n |



## Authors: ##










