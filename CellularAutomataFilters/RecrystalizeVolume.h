/*
 * Your License or Copyright Information can go here
 */

#ifndef _recrystalizevolume_h_
#define _recrystalizevolume_h_

#include <QtCore/QDateTime>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "CellularAutomata/CellularAutomataDLLExport.h"

/**
 * @class RecrystalizeVolume RecrystalizeVolume.h CellularAutomata/CellularAutomataFilters/RecrystalizeVolume.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class CellularAutomata_EXPORT RecrystalizeVolume : public AbstractFilter
{
    Q_OBJECT
//    PYB11_CREATE_BINDINGS(RecrystalizeVolume SUPERCLASS AbstractFilter)
//    PYB11_PROPERTY(QString DataContainerName READ getDataContainerName WRITE setDataContainerName)
//    PYB11_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)
//    PYB11_PROPERTY(QString CellFeatureAttributeMatrixName READ getCellFeatureAttributeMatrixName WRITE setCellFeatureAttributeMatrixName)
//    PYB11_PROPERTY(QString CellEnsembleAttributeMatrixName READ getCellEnsembleAttributeMatrixName WRITE setCellEnsembleAttributeMatrixName)
//    PYB11_PROPERTY(QString FeatureIdsArrayName READ getFeatureIdsArrayName WRITE setFeatureIdsArrayName)
//    PYB11_PROPERTY(QString RecrystallizationTimeArrayName READ getRecrystallizationTimeArrayName WRITE setRecrystallizationTimeArrayName)
//    PYB11_PROPERTY(QString RecrystallizationHistoryArrayName READ getRecrystallizationHistoryArrayName WRITE setRecrystallizationHistoryArrayName)
//    PYB11_PROPERTY(QString AvramiArrayName READ getAvramiArrayName WRITE setAvramiArrayName)
//    PYB11_PROPERTY(QString ActiveArrayName READ getActiveArrayName WRITE setActiveArrayName)
//    PYB11_PROPERTY(IntVec3_t Dimensions READ getDimensions WRITE setDimensions)
//    PYB11_PROPERTY(FloatVec3_t Resolution READ getResolution WRITE setResolution)
//    PYB11_PROPERTY(FloatVec3_t Origin READ getOrigin WRITE setOrigin)
//    PYB11_PROPERTY(float NucleationRate READ getNucleationRate WRITE setNucleationRate)
//    PYB11_PROPERTY(unsigned int Neighborhood READ getNeighborhood WRITE setNeighborhood)
//    PYB11_PROPERTY(QString ImagePrefix READ getImagePrefix WRITE setImagePrefix)
//    PYB11_PROPERTY(int ImageSize READ getImageSize WRITE setImageSize)

  public:
    SIMPL_SHARED_POINTERS(RecrystalizeVolume)
    SIMPL_FILTER_NEW_MACRO(RecrystalizeVolume)
    SIMPL_TYPE_MACRO_SUPER_OVERRIDE(RecrystalizeVolume, AbstractFilter)

    ~RecrystalizeVolume() override;

    SIMPL_FILTER_PARAMETER(QString, DataContainerName)
    Q_PROPERTY(QString DataContainerName READ getDataContainerName WRITE setDataContainerName)

    SIMPL_FILTER_PARAMETER(QString, CellAttributeMatrixName)
    Q_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)

    SIMPL_FILTER_PARAMETER(QString, CellFeatureAttributeMatrixName)
    Q_PROPERTY(QString CellFeatureAttributeMatrixName READ getCellFeatureAttributeMatrixName WRITE setCellFeatureAttributeMatrixName)

    SIMPL_FILTER_PARAMETER(QString, CellEnsembleAttributeMatrixName)
    Q_PROPERTY(QString CellEnsembleAttributeMatrixName READ getCellEnsembleAttributeMatrixName WRITE setCellEnsembleAttributeMatrixName)

    SIMPL_FILTER_PARAMETER(QString, FeatureIdsArrayName)
    Q_PROPERTY(QString FeatureIdsArrayName READ getFeatureIdsArrayName WRITE setFeatureIdsArrayName)

    SIMPL_FILTER_PARAMETER(QString, RecrystallizationTimeArrayName)
    Q_PROPERTY(QString RecrystallizationTimeArrayName READ getRecrystallizationTimeArrayName WRITE setRecrystallizationTimeArrayName)

    SIMPL_FILTER_PARAMETER(QString, RecrystallizationHistoryArrayName)
    Q_PROPERTY(QString RecrystallizationHistoryArrayName READ getRecrystallizationHistoryArrayName WRITE setRecrystallizationHistoryArrayName)

    SIMPL_FILTER_PARAMETER(QString, AvramiArrayName)
    Q_PROPERTY(QString AvramiArrayName READ getAvramiArrayName WRITE setAvramiArrayName)

    SIMPL_FILTER_PARAMETER(QString, ActiveArrayName)
    Q_PROPERTY(QString ActiveArrayName READ getActiveArrayName WRITE setActiveArrayName)

    SIMPL_FILTER_PARAMETER(IntVec3_t, Dimensions)
    Q_PROPERTY(IntVec3_t Dimensions READ getDimensions WRITE setDimensions)

    SIMPL_FILTER_PARAMETER(FloatVec3_t, Resolution)
    Q_PROPERTY(FloatVec3_t Resolution READ getResolution WRITE setResolution)

    SIMPL_FILTER_PARAMETER(FloatVec3_t, Origin)
    Q_PROPERTY(FloatVec3_t Origin READ getOrigin WRITE setOrigin)

    SIMPL_FILTER_PARAMETER(float, NucleationRate)
    Q_PROPERTY(float NucleationRate READ getNucleationRate WRITE setNucleationRate)

    SIMPL_FILTER_PARAMETER(unsigned int, Neighborhood)
    Q_PROPERTY(unsigned int Neighborhood READ getNeighborhood WRITE setNeighborhood)

    /* Place your input parameters here using the DREAM3D macros to declare the Filter Parameters
     * or other instance variables
     */
    //SIMPL_FILTER_PARAMETER(QString, ImagePrefix)
    /* If you declare a filter parameter above then you MUST create a Q_PROPERTY for that FilterParameter */
    //Q_PROPERTY(QString ImagePrefix READ getImagePrefix WRITE setImagePrefix)

    /* Here is another example of declaring an integer FilterParameter */
    // SIMPL_FILTER_PARAMETER(int, ImageSize)
    // Q_PROPERTY(int ImageSize READ getImageSize WRITE setImageSize)



    /**
     * @brief getCompiledLibraryName Returns the name of the Library that this filter is a part of
     * @return
     */
    const QString getCompiledLibraryName() const override;

    /**
    * @brief This returns a string that is displayed in the GUI. It should be readable
    * and understandable by humans.
    */
    const QString getHumanLabel() const override;

    /**
    * @brief This returns the group that the filter belonds to. You can select
    * a different group if you want. The string returned here will be displayed
    * in the GUI for the filter
    */
    const QString getGroupName() const override;

    /**
    * @brief This returns a string that is displayed in the GUI and helps to sort the filters into
    * a subgroup. It should be readable and understandable by humans.
    */
    const QString getSubGroupName() const override;

    /**
     * @brief getUuid Return the unique identifier for this filter.
     * @return A QUuid object.
     */
    const QUuid getUuid() override;

    /**
    * @brief This method will instantiate all the end user settable options/parameters
    * for this filter
    */
    void setupFilterParameters() override;

    /**
    * @brief This method will read the options from a file
    * @param reader The reader that is used to read the options from a file
    * @param index The index to read the information from
    */
    void readFilterParameters(AbstractFilterParametersReader* reader, int index) override;

    /**
     * @brief Reimplemented from @see AbstractFilter class
     */
    void execute() override;

    /**
    * @brief This function runs some sanity checks on the DataContainer and inputs
    * in an attempt to ensure the filter can process the inputs.
    */
    void preflight() override;

    /**
     * @brief newFilterInstance Returns a new instance of the filter optionally copying the filter parameters from the
     * current filter to the new instance.
     * @param copyFilterParameters
     * @return
     */
    AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;

  signals:
    /**
     * @brief updateFilterParameters This is emitted when the filter requests all the latest Filter Parameters need to be
     * pushed from a user facing control such as the FilterParameter Widget
     * @param filter The filter to push the values into
     */
    void updateFilterParameters(AbstractFilter* filter);

    /**
     * @brief parametersChanged This signal can be emitted when any of the filter parameters are changed internally.
     */
    void parametersChanged();

    /**
     * @brief preflightAboutToExecute Emitted just before the dataCheck() is called. This can change if needed.
     */
    void preflightAboutToExecute();

    /**
     * @brief preflightExecuted Emitted just after the dataCheck() is called. Typically. This can change if needed.
     */
    void preflightExecuted();

  protected:
    RecrystalizeVolume();

    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

    /**
     * @brief Initializes all the private instance variables.
     */
    void initialize();


  private:
    /* Your private class instance variables go here. You can use several preprocessor macros to help
     * make sure you have all the variables defined correctly. Those are "DEFINE_REQUIRED_DATAARRAY_VARIABLE()"
     * and  DEFINE_CREATED_DATAARRAY_VARIABLE() which are defined in DREAM3DGetSetMacros.h
     */
    DEFINE_CREATED_DATAARRAY_VARIABLE(int32_t, FeatureIds)
    DEFINE_CREATED_DATAARRAY_VARIABLE(uint32_t, RecrystallizationTime)
    DEFINE_CREATED_DATAARRAY_VARIABLE(bool, Active)
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, RecrystallizationHistory)
    DEFINE_CREATED_DATAARRAY_VARIABLE(float, Avrami)

  public:
    RecrystalizeVolume(const RecrystalizeVolume&) = delete; // Copy Constructor Not Implemented
    RecrystalizeVolume(RecrystalizeVolume&&) = delete;      // Move Constructor
    RecrystalizeVolume& operator=(const RecrystalizeVolume&) = delete; // Copy Assignment Not Implemented
    RecrystalizeVolume& operator=(RecrystalizeVolume&&) = delete;      // Move Assignment
};

#endif /* _RecrystalizeVolume_H_ */
