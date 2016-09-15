/*
 * Your License or Copyright Information can go here
 */

#include "RecrystalizeVolume.h"
#include "CellularAutomataHelpers.hpp"

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#include <tbb/atomic.h>
#endif

#include <QtCore/QString>

#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>

#include "CellularAutomata/CellularAutomataConstants.h"

class RecrystalizeVolumeImpl
{
  public:
    static const int VON_NEUMAN = 0;
    static const int EIGHT_CELL = 1;
    static const int FOURTEEN_CELL = 2;
    static const int EIGHTEEN_CELL = 3;
    static const int TWENTY_CELL = 4;
    static const int MOORE = 5;

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
    RecrystalizeVolumeImpl(CellularAutomata::Lattice* cellLattice, int32_t* currentGrainIDs, int32_t* workingGrainIDs, uint32_t* updateTime, int neighborhoodType, tbb::atomic<size_t>* counter, uint32_t* time, tbb::atomic<int32_t>* grainCount, float nucleationRate) :
#elif
    RecrystalizeVolumeImpl(CellularAutomata::Lattice cellLattice, int32_t* currentGrainIDs, int32_t* workingGrainIDs, uint32_t* updateTime, int neighborhoodType, size_t* counter, uint32_t* time, int32_t* grainCount, float nucleationRate) :
#endif
      m_lattice(cellLattice),
      m_currentIDs(currentGrainIDs),
      m_workingIDs(workingGrainIDs),
      m_updateTime(updateTime),
      m_neighborhood(neighborhoodType),
      m_unrecrystalizedCount(counter),
      m_time(time),
      m_grainCount(grainCount),
      m_nucleationRate(nucleationRate)
    {}

    virtual ~RecrystalizeVolumeImpl() {}

    inline void computeBase(size_t index, std::vector<size_t>::iterator begin, std::vector<size_t>::iterator end, boost::mt19937& generator) const
    {
      //check if any neighbors are recrystallized
      std::vector<size_t> goodNeighbors;
      for(std::vector<size_t>::iterator iter = begin; iter != end; ++iter)
      {
        if(0 != m_currentIDs[*iter])
        { goodNeighbors.push_back(*iter); }
      }

      if(0 == goodNeighbors.size())
      {
        //if no immediate neighbors are recrystalized, allow random chance to create nucluie
        boost::uniform_real<> distribution(0, 1);
        boost::variate_generator<boost::mt19937&, boost::uniform_real<> > seedGen(generator, distribution);
        if(seedGen() <= m_nucleationRate)
        {
          //if extended neighborhood is empty allow nucleation, otherwise supress
          std::vector<size_t> extendedNeighbors = m_lattice->ExtendedMoore(index);
          bool goodSeed = true;
          for(std::vector<size_t>::iterator iter = extendedNeighbors.begin(); iter != extendedNeighbors.end(); ++iter)
          {
            if(0 != m_currentIDs[*iter])
            {
              goodSeed = false;
              break;
            }
          }

          if(goodSeed)
          {
            m_workingIDs[index] = m_grainCount->fetch_and_increment() + 1;
            m_updateTime[index] = *m_time;
          }
          else
          {
            ++(*m_unrecrystalizedCount);
            m_workingIDs[index] = 0;
          }
        }
        else
        {
          ++(*m_unrecrystalizedCount);
          m_workingIDs[index] = 0;
        }
      }
      else
      {
        //if neighbors are recrystallized, choose one at random to join
        boost::uniform_int<> distribution(0, goodNeighbors.size() - 1);//range is inclusive
        boost::variate_generator<boost::mt19937&, boost::uniform_int<> > indexGen(generator, distribution);
        m_workingIDs[index] = m_currentIDs[goodNeighbors[indexGen()]];
        m_updateTime[index] = *m_time;
      }
    }

    void computeVonNeuman(size_t start, size_t end, boost::mt19937& generator) const
    {
      for (size_t i = start; i < end; i++)
      {
        //don't change cells that are already recrystallized
        if(0 != m_currentIDs[i])
        {
          m_workingIDs[i] = m_currentIDs[i];
          continue;
        }

        //otherwise get cell neighbors and determine next state
        std::vector<size_t> neighborList = m_lattice->VonNeumann(i);
        computeBase(i, neighborList.begin(), neighborList.end(), generator);
      }
    }

    void compute18Cell(size_t start, size_t end, boost::mt19937& generator) const
    {
      for (size_t i = start; i < end; i++)
      {
        //don't change cells that are already recrystallized
        if(0 != m_currentIDs[i])
        {
          m_workingIDs[i] = m_currentIDs[i];
          continue;
        }

        //otherwise get cell neighbors and determine next state
        std::vector<size_t> neighborList = m_lattice->EighteenCell(i);
        computeBase(i, neighborList.begin(), neighborList.end(), generator);
      }
    }

    void computeMoore(size_t start, size_t end, boost::mt19937& generator) const
    {
      for (size_t i = start; i < end; i++)
      {
        //don't change cells that are already recrystallized
        if(0 != m_currentIDs[i])
        {
          m_workingIDs[i] = m_currentIDs[i];
          continue;
        }

        //otherwise get cell neighbors and determine next state
        std::vector<size_t> neighborList = m_lattice->Moore(i);
        computeBase(i, neighborList.begin(), neighborList.end(), generator);
      }
    }

    void compute8Cell(size_t start, size_t end, boost::mt19937& generator) const
    {
      //wrap generator in uniform interger distribution for selecting neighborhood variant
      boost::uniform_int<> distribution(0, 5);
      boost::variate_generator<boost::mt19937&, boost::uniform_int<> > indexGen(generator, distribution);
      for (size_t i = start; i < end; i++)
      {
        //don't change cells that are already recrystallized
        if(0 != m_currentIDs[i])
        {
          m_workingIDs[i] = m_currentIDs[i];
          continue;
        }

        //otherwise get cell neighbors and determine next state
        std::vector<size_t> neighborList = m_lattice->EightCell(i, indexGen());
        computeBase(i, neighborList.begin(), neighborList.end(), generator);
      }
    }

    void compute14Cell(size_t start, size_t end, boost::mt19937& generator) const
    {
      //wrap generator in uniform interger distribution for selecting neighborhood variant
      boost::uniform_int<> distribution(0, 3);
      boost::variate_generator<boost::mt19937&, boost::uniform_int<> > indexGen(generator, distribution);
      for (size_t i = start; i < end; i++)
      {
        //don't change cells that are already recrystallized
        if(0 != m_currentIDs[i])
        {
          m_workingIDs[i] = m_currentIDs[i];
          continue;
        }

        //otherwise get cell neighbors and determine next state
        std::vector<size_t> neighborList = m_lattice->FourteenCell(i, indexGen());
        computeBase(i, neighborList.begin(), neighborList.end(), generator);
      }
    }

    void compute20Cell(size_t start, size_t end, boost::mt19937& generator) const
    {
      //wrap generator in uniform interger distribution for selecting neighborhood variant
      boost::uniform_int<> distribution(0, 3);
      boost::variate_generator<boost::mt19937&, boost::uniform_int<> > indexGen(generator, distribution);
      for (size_t i = start; i < end; i++)
      {
        //don't change cells that are already recrystallized
        if(0 != m_currentIDs[i])
        {
          m_workingIDs[i] = m_currentIDs[i];
          continue;
        }

        //otherwise get cell neighbors and determine next state
        std::vector<size_t> neighborList = m_lattice->TwentyCell(i, indexGen());
        computeBase(i, neighborList.begin(), neighborList.end(), generator);
      }
    }

    void compute(size_t start, size_t end) const
    {
      //create random number generator + initialize (parallel threads may call simultaneously so rand() or time alone are insufficient for different seeds)
      boost::mt19937 generator( static_cast<size_t>( QDateTime::currentMSecsSinceEpoch() * end + start ) );

      switch(m_neighborhood)
      {
        case VON_NEUMAN:
          computeVonNeuman(start, end, generator);
          break;

        case EIGHT_CELL:
          compute8Cell(start, end, generator);
          break;

        case FOURTEEN_CELL:
          compute14Cell(start, end, generator);
          break;

        case EIGHTEEN_CELL:
          compute18Cell(start, end, generator);
          break;

        case TWENTY_CELL:
          compute20Cell(start, end, generator);
          break;

        case MOORE:
          computeMoore(start, end, generator);
          break;
      }
    }

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range<size_t>& r) const
    {
      compute(r.begin(), r.end());
    }
#endif
  private:
    CellularAutomata::Lattice* m_lattice;
    int32_t* m_currentIDs;
    int32_t* m_workingIDs;
    uint32_t* m_updateTime;
    int m_neighborhood;

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
    tbb::atomic<size_t>* m_unrecrystalizedCount;
    tbb::atomic<int32_t>* m_grainCount;
#elif
    size_t* m_unrecrystalizedCount;
    int32_t* m_grainCount;
#endif
    uint32_t* m_time;
    float m_nucleationRate;
};

#define INIT_SYNTH_VOLUME_CHECK(var, errCond) \
  if (m_##var <= 0) { QString ss = QObject::tr(":%1 must be a value > 0\n").arg( #var); notifyErrorMessage(getHumanLabel(), ss, errCond);}

// Include the MOC generated file for this class
#include "moc_RecrystalizeVolume.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RecrystalizeVolume::RecrystalizeVolume() :
  AbstractFilter(),
  m_DataContainerName(SIMPL::Defaults::SyntheticVolumeDataContainerName),
  m_CellAttributeMatrixName(SIMPL::Defaults::CellAttributeMatrixName),
  m_CellFeatureAttributeMatrixName(SIMPL::Defaults::CellFeatureAttributeMatrixName),
  m_CellEnsembleAttributeMatrixName(SIMPL::Defaults::CellEnsembleAttributeMatrixName),
  m_NucleationRate(0.0001f),
  m_Neighborhood(0),
  m_FeatureIds(nullptr),
  m_FeatureIdsArrayName(SIMPL::CellData::FeatureIds),
  m_RecrystallizationTime(nullptr),
  m_RecrystallizationTimeArrayName("RecrystallizationTime"),
  m_Active(nullptr),
  m_ActiveArrayName(SIMPL::FeatureData::Active),
  m_RecrystallizationHistory(nullptr),
  m_RecrystallizationHistoryArrayName("RecrystallizationHistory"),
  m_Avrami(nullptr),
  m_AvramiArrayName("AvaramiParameters")
{
  m_Dimensions.x = 128;
  m_Dimensions.y = 128;
  m_Dimensions.z = 128;

  m_Resolution.x = 0.25f;
  m_Resolution.y = 0.25f;
  m_Resolution.z = 0.25f;

  m_Origin.x = 0.0f;
  m_Origin.y = 0.0f;
  m_Origin.z = 0.0f;
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RecrystalizeVolume::~RecrystalizeVolume()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RecrystalizeVolume::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SeparatorFilterParameter::New(Required Information, FilterParameter::Uncategorized));
  parameters.push_back(;
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Neighborhood Type");
    parameter->setPropertyName("Neighborhood");
    
    QVector<QString> choices;
    choices.push_back("Von Neumann (6 cell) [octohedra]");
    choices.push_back("8 cell [sphere]");
    choices.push_back("14 cell [~superellipsoid]");
    choices.push_back("18 cell [cubeoctahedron]");
    choices.push_back("20 cell [~truncated cube]");
    choices.push_back("Moore (26 cell) [cube]");
    parameter->setChoices(choices);
    parameter->setAdvanced(false);
    parameters.push_back(parameter);
  }
  parameters.push_back(SIMPL_NEW_STRING_FP("New DataContainer Name", DataContainerName, FilterParameter::Uncategorized, RecrystalizeVolume));
  parameters.push_back(SIMPL_NEW_STRING_FP("New Cell Attribute Matrix Name", CellAttributeMatrixName, FilterParameter::Uncategorized, RecrystalizeVolume));
  parameters.push_back(SIMPL_NEW_STRING_FP("New Cell Feature Attribute Matrix Name", CellFeatureAttributeMatrixName, FilterParameter::Uncategorized, RecrystalizeVolume));
  parameters.push_back(SIMPL_NEW_STRING_FP("New Cell Ensemble Attribute Matrix Name", CellEnsembleAttributeMatrixName, FilterParameter::Uncategorized, RecrystalizeVolume));
  parameters.push_back(SIMPL_NEW_STRING_FP("Feature Ids Array Name", FeatureIdsArrayName, FilterParameter::Uncategorized, RecrystalizeVolume));
  parameters.push_back(SIMPL_NEW_STRING_FP("Recrystallization Time Array Name", RecrystallizationTimeArrayName, FilterParameter::Uncategorized, RecrystalizeVolume));
  parameters.push_back(SIMPL_NEW_STRING_FP("Recrystallization History Array Name", RecrystallizationHistoryArrayName, FilterParameter::Uncategorized, RecrystalizeVolume));
  parameters.push_back(SIMPL_NEW_STRING_FP("Active Array Name", ActiveArrayName, FilterParameter::Uncategorized, RecrystalizeVolume));
  parameters.push_back(SIMPL_NEW_STRING_FP("Avrami Parameter Array Name", AvramiArrayName, FilterParameter::Uncategorized, RecrystalizeVolume));
  parameters.push_back(;
                       parameters.push_back(;
                                            parameters.push_back(;
                                                setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
                                          void RecrystalizeVolume::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setNucleationRate(reader->readValue("NucleationRate", getNucleationRate() ) );
  setNeighborhood(reader->readValue("Neighborhood", getNeighborhood() ) );
  setDataContainerName(reader->readString("DataContainerName", getDataContainerName() ) );
  setCellAttributeMatrixName(reader->readString("CellAttributeMatrixName", getCellAttributeMatrixName() ) );
  setCellFeatureAttributeMatrixName(reader->readString("CellFeatureAttributeMatrixName", getCellFeatureAttributeMatrixName() ) );
  setCellEnsembleAttributeMatrixName(reader->readString("CellEnsembleAttributeMatrixName", getCellEnsembleAttributeMatrixName() ) );
  setFeatureIdsArrayName(reader->readString("FeatureIdsArrayName", getFeatureIdsArrayName() ) );
  setRecrystallizationTimeArrayName(reader->readString("RecrystallizationTimeArrayName", getRecrystallizationTimeArrayName() ) );
  setRecrystallizationHistoryArrayName(reader->readString("RecrystallizationHistoryArrayName", getRecrystallizationHistoryArrayName() ) );
  setActiveArrayName(reader->readString("ActiveArrayName", getActiveArrayName() ) );
  setAvramiArrayName(reader->readString("AvramiArrayName", getAvramiArrayName() ) );
  setDimensions( reader->readIntVec3("Dimensions", getDimensions() ) );
  setResolution( reader->readFloatVec3("Resolution", getResolution() ) );
  setOrigin( reader->readFloatVec3("Origin", getOrigin() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RecrystalizeVolume::initialize()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RecrystalizeVolume::dataCheck()
{
  setErrorCondition(0);

  // Create the output Data Container
  DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getDataContainerName());
  if(getErrorCondition() < 0) { return; }

  // Sanity Check the Dimensions and Resolution
  INIT_SYNTH_VOLUME_CHECK(Dimensions.x, -5000);
  INIT_SYNTH_VOLUME_CHECK(Dimensions.y, -5001);
  INIT_SYNTH_VOLUME_CHECK(Dimensions.z, -5002);
  INIT_SYNTH_VOLUME_CHECK(Resolution.x, -5003);
  INIT_SYNTH_VOLUME_CHECK(Resolution.y, -5004);
  INIT_SYNTH_VOLUME_CHECK(Resolution.z, -5005);

  // Set teh Dimensions, Resolution and Origin of the output data container
  /* FIXME: ImageGeom */ m->getGeometryAs<ImageGeom>()->setDimensions(m_Dimensions.x, m_Dimensions.y, m_Dimensions.z);
  /* FIXME: ImageGeom */ m->getGeometryAs<ImageGeom>()->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);
  /* FIXME: ImageGeom */ m->getGeometryAs<ImageGeom>()->setOrigin(m_Origin.x, m_Origin.y, m_Origin.z);

  // Create our output Attribute Matrix objects
  QVector<size_t> tDims(3, 0);
  AttributeMatrix::Pointer cellAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), tDims, SIMPL::AttributeMatrixType::Cell);
  if(getErrorCondition() < 0) { return; }

  QVector<size_t> cDims(1, 1);
  AttributeMatrix::Pointer cellFeatureAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellFeatureAttributeMatrixName(), cDims, SIMPL::AttributeMatrixType::CellFeature);
  if(getErrorCondition() < 0) { return; }

  AttributeMatrix::Pointer CellEnsembleAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellEnsembleAttributeMatrixName(), cDims, SIMPL::AttributeMatrixType::CellEnsemble);
  if(getErrorCondition() < 0) { return; }

  //create arrays
  QVector<size_t> dims(1, 1);
  DataArrayPath tempPath;
  tempPath.update(getDataContainerName(), getCellAttributeMatrixName(), getFeatureIdsArrayName() );
  m_FeatureIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0, dims);
  if( nullptr != m_FeatureIdsPtr.lock().get() )
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); }

  tempPath.update(getDataContainerName(), getCellAttributeMatrixName(), getRecrystallizationTimeArrayName() );
  m_RecrystallizationTimePtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter, uint32_t>(this, tempPath, 0, dims);
  if( nullptr != m_RecrystallizationTimePtr.lock().get() )
  { m_RecrystallizationTime = m_RecrystallizationTimePtr.lock()->getPointer(0); }

  tempPath.update(getDataContainerName(), getCellFeatureAttributeMatrixName(), getActiveArrayName() );
  m_ActivePtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, tempPath, 0, dims);
  if( nullptr != m_ActivePtr.lock().get() )
  { m_Active = m_ActivePtr.lock()->getPointer(0); }

  tempPath.update(getDataContainerName(), getCellEnsembleAttributeMatrixName(), getRecrystallizationHistoryArrayName() );
  m_RecrystallizationHistoryPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, dims);
  if( nullptr != m_RecrystallizationHistoryPtr.lock().get() )
  { m_RecrystallizationHistory = m_RecrystallizationHistoryPtr.lock()->getPointer(0); }

  dims[0] = 2;
  tempPath.update(getDataContainerName(), getCellEnsembleAttributeMatrixName(), getAvramiArrayName() );
  m_AvramiPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, dims);
  if( nullptr != m_AvramiPtr.lock().get() )
  { m_Avrami = m_AvramiPtr.lock()->getPointer(0); }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RecrystalizeVolume::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true); // Set the fact that we are preflighting.
  emit preflightAboutToExecute(); // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck(); // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted(); // We are done preflighting this filter
  setInPreflight(false); // Inform the system this filter is NOT in preflight mode anymore.

  /* *** THIS FILTER NEEDS TO BE CHECKED *** */
  setErrorCondition(0xABABABAB);
  QString ss = QObject::tr("Filter is NOT updated for IGeometry Redesign. A Programmer needs to check this filter. Please report this to the DREAM3D developers.");
  notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  /* *** THIS FILTER NEEDS TO BE CHECKED *** */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RecrystalizeVolume::getCompiledLibraryName()
{
  return CellularAutomata::CellularAutomataBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RecrystalizeVolume::getGroupName()
{
  return "CellularAutomata";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RecrystalizeVolume::getHumanLabel()
{
  return "RecrystalizeVolume";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RecrystalizeVolume::getSubGroupName()
{
  return "Misc";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RecrystalizeVolume::execute()
{
  int err = 0;
  // typically run your dataCheck function to make sure you can get that far and all your variables are initialized
  dataCheck();
  // Check to make sure you made it through the data check. Errors would have been reported already so if something
  // happens to fail in the dataCheck() then we simply return
  if(getErrorCondition() < 0) { return; }
  setErrorCondition(0);

  //get cretaed data container
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_DataContainerName);

  //get created cell attribute matricies
  AttributeMatrix::Pointer cellAttrMat = m->getAttributeMatrix(m_CellAttributeMatrixName);
  AttributeMatrix::Pointer cellFeatureAttrMat = m->getAttributeMatrix(m_CellFeatureAttributeMatrixName);
  AttributeMatrix::Pointer cellEnsembleAttrMat = m->getAttributeMatrix(m_CellEnsembleAttributeMatrixName);

  // Resize the cell attribute matric
  QVector<size_t> cellDims(3, 0);
  cellDims[0] = /* FIXME: ImageGeom */ m->getGeometryAs<ImageGeom>()->getXPoints();
  cellDims[1] = /* FIXME: ImageGeom */ m->getGeometryAs<ImageGeom>()->getYPoints();
  cellDims[2] = /* FIXME: ImageGeom */ m->getGeometryAs<ImageGeom>()->getZPoints();
  cellAttrMat->resizeAttributeArrays(cellDims);

  //convert nucleation rate to probabilty / voxel / timestep
  float pNuc = m_NucleationRate * m_Resolution.x * m_Resolution.y * m_Resolution.z;

  //determine number of cells and create helper object for indicies
  size_t numCells = m_Dimensions.x * m_Dimensions.y * m_Dimensions.z;
  CellularAutomata::Lattice lattice(m_Dimensions.x, m_Dimensions.y, m_Dimensions.z);

  //create arrays to hold gradin ids + recrystallization time
  QVector<size_t> cDims(1, 1);
  Int32ArrayType::Pointer currentIDs = m_FeatureIdsPtr.lock();
  Int32ArrayType::Pointer workingIDs = Int32ArrayType::CreateArray(numCells, cDims, getFeatureIdsArrayName());
  UInt32ArrayType::Pointer recrstTime = m_RecrystallizationTimePtr.lock();

  //make sure allocation was sucessful
  if(Int32ArrayType::NullPointer() == workingIDs)
  {
    QString ss = QObject::tr("Unable to allocate memory for working array");
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  //initialize arrays
  currentIDs->initializeWithValue(0);
  workingIDs->initializeWithValue(0);

  //initialize variables to track recrystallizatino progress
#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  tbb::atomic<size_t> unrecrstallizedCount;
  unrecrstallizedCount = 1;
  tbb::atomic<int32_t> grainCount;
  grainCount = 0;
#elif
  size_t unrecrstallizedCount = 1;
  int32_t grainCount = 1;
#endif

  uint32_t timeStep = 1;
  std::vector<float> recrystallizationHistory;
  recrystallizationHistory.push_back(0);

  //continue time stepping until all cells are recrystallized
  while(0 != unrecrstallizedCount)
  {
    //reset count of remaining cells to recrystallize
    unrecrstallizedCount = 0;

    //perform time step
#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
    bool doParallel = true;
    if (doParallel == true)
    {
      tbb::parallel_for(tbb::blocked_range<size_t>(0, numCells),
                        RecrystalizeVolumeImpl(&lattice, currentIDs->getPointer(0), workingIDs->getPointer(0), recrstTime->getPointer(0), m_Neighborhood, &unrecrstallizedCount, &timeStep, &grainCount, pNuc), tbb::auto_partitioner());
    }
    else
#endif
    {
      RecrystalizeVolumeImpl serial(&lattice, currentIDs->getPointer(0), workingIDs->getPointer(0), recrstTime->getPointer(0), m_Neighborhood, &unrecrstallizedCount, &timeStep, &grainCount, pNuc);
      serial.compute(0, numCells);
    }

    // swap working + current arrays
    currentIDs.swap(workingIDs);

    //compute recrstallized percent + update progress
    float percent = 1 - (static_cast<float>(unrecrstallizedCount) / lattice.size());
    QString ss = QObject::tr("%1% recrystallized").arg(100 * percent);
    notifyStatusMessage(getHumanLabel(), ss);

    //only add to history/consider as time step if there is at least some recrystallization (low nucleations rates may require multiple timesteps for the first nuclei to form)
    if(percent > 0)
    {
      timeStep++;
      recrystallizationHistory.push_back(percent);
    }
  }

  //clean up working copy
  workingIDs = Int32ArrayType::NullPointer();

  //resize cell feature attribute matrix
  QVector<size_t> featureDims(1, grainCount + 1);
  cellFeatureAttrMat->resizeAttributeArrays(featureDims);

  //fill active array with true (except for grain 0)
  m_ActivePtr.lock()->initializeWithValue(true);
  m_ActivePtr.lock()->getPointer(0)[0] = false;

  //fill recrystalization history
  cDims[0] = recrystallizationHistory.size();
  FloatArrayType::Pointer history = FloatArrayType::CreateArray(numCells, cDims, getRecrystallizationHistoryArrayName());
  float* pHistory = history->getPointer(0);
  for(size_t i = 0; i < recrystallizationHistory.size(); i++)
  { pHistory[i] = recrystallizationHistory[i]; }
  cellEnsembleAttrMat->addAttributeArray(getRecrystallizationHistoryArrayName(), history);

  //assemble linear pairs to fit avrami equation parameters
  recrystallizationHistory.pop_back();//last step is 100% recrystallized
  std::vector<float> x;
  std::vector<float> y;
  for(size_t i = 1; i < recrystallizationHistory.size(); i++)//first step is 0% recrystallized
  {
    x.push_back(logf(i));
    y.push_back( logf( -logf(1.0 - recrystallizationHistory[i]) ) );
  }

  //perform regression
  double slope, intercept;
  if(SIMPLibMath::linearRegression<float>(slope, intercept, x, y))
  {
    m_Avrami[0] = exp(intercept);//k
    m_Avrami[1] = slope;//n
  }
  else
  {
    QString ss = QObject::tr("Unable to fit Avrami Parameters");
    notifyWarningMessage(getHumanLabel(), ss, 1);
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer RecrystalizeVolume::newFilterInstance(bool copyFilterParameters)
{
  RecrystalizeVolume::Pointer filter = RecrystalizeVolume::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

