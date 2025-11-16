/**
 * @file AuxController.h
 * @brief Consolidates all logic for auxiliary functions, effects, and physical outputs.
 */
#ifndef XDRAILS_DCC_LIGHTS_AND_FUNCTIONS_H
#define XDRAILS_DCC_LIGHTS_AND_FUNCTIONS_H

#include <Arduino.h>
#include <vector>
#include <map>
#include <cstdint>
#include <memory>
#include "interfaces/ICVAccess.h"
#include "LightSources/LightSource.h"
#include "PhysicalOutput.h"
#include "LogicalFunction.h"
#include "FunctionMapping.h"

#define MAX_DCC_FUNCTIONS 29

namespace xDuinoRails {

/**
 * @enum DecoderDirection
 * @brief Defines the direction of the decoder.
 */
enum DecoderDirection {
    DECODER_DIRECTION_REVERSE = 0, ///< The decoder is moving in reverse.
    DECODER_DIRECTION_FORWARD = 1  ///< The decoder is moving forward.
};

/**
 * @class AuxController
 * @brief Central controller for all auxiliary functions, lighting, and other effects.
 *
 * This class manages all physical outputs, logical functions, effects, and the
 * complex mapping logic that ties them together based on decoder state and CV settings.
 */
class AuxController {
public:
    /** @brief Construct a new Aux Controller object. */
    AuxController();
    /** @brief Destructor. Cleans up dynamically allocated resources. */
    ~AuxController();

    /**
     * @brief Adds and initializes a physical output.
     * @param pin The microcontroller pin number.
     * @param type The type of the output (LIGHT_SOURCE or SERVO).
     */
    void addPhysicalOutput(uint8_t pin, OutputType type);

    /**
     * @brief Adds and initializes a light source physical output.
     * @param lightSource A unique_ptr to a LightSource object.
     */
    void addLightSource(std::unique_ptr<LightSource> lightSource);

    /**
     * @brief Updates the state of all logical functions and effects. Call every loop.
     * @param delta_ms Time elapsed since the last update in milliseconds.
     */
    void update(uint32_t delta_ms);
    /**
     * @brief Loads the entire function mapping configuration from CVs.
     * @param cvAccess A reference to an object that implements the ICVAccess interface.
     */
    void loadFromCVs(ICVAccess& cvAccess);

    // --- State Update Methods ---
    /**
     * @brief Sets the state of a DCC function key.
     * @param functionNumber The function number (0-28).
     * @param functionState True if the function is on, false if off.
     */
    void setFunctionState(uint8_t functionNumber, bool functionState);
    /**
     * @brief Sets the decoder's direction of travel.
     * @param direction The new direction.
     */
    void setDirection(DecoderDirection direction);
    /**
     * @brief Sets the decoder's current speed.
     * @param speed The new speed value.
     */
    void setSpeed(uint16_t speed);
    /**
     * @brief Sets the value of a binary state.
     * @param state_number The ID of the binary state.
     * @param value The new boolean value.
     */
    void setBinaryState(uint16_t state_number, bool value);

    // --- State Getter Methods (for evaluation) ---
    /**
     * @brief Gets the current state of a DCC function key.
     * @param functionNumber The function number to check.
     * @return True if the function is on, false otherwise.
     */
    bool getFunctionState(uint8_t functionNumber) const;
    /** @brief Gets the decoder's current direction. @return The current direction. */
    DecoderDirection getDirection() const;
    /** @brief Gets the decoder's current speed. @return The current speed. */
    uint16_t getSpeed() const;
    /**
     * @brief Gets the evaluated state of a ConditionVariable.
     * @param cv_id The ID of the ConditionVariable.
     * @return The cached boolean result of the variable's evaluation.
     */
    bool getConditionVariableState(uint8_t cv_id) const;
    /**
     * @brief Gets the current value of a binary state.
     * @param state_number The ID of the binary state to check.
     * @return The current boolean value of the state.
     */
    bool getBinaryState(uint16_t state_number) const;
    /**
     * @brief Gets a pointer to a logical function by its index.
     * @param index The index of the logical function.
     * @return A pointer to the LogicalFunction, or nullptr if not found.
     */
    LogicalFunction* getLogicalFunction(size_t index);

#ifdef UNIT_TEST
public:
#endif
private:
    void addLogicalFunction(LogicalFunction* function);
    void addConditionVariable(const ConditionVariable& cv);
    void addMappingRule(const MappingRule& rule);
    void reset();

    void evaluateMapping();
    PhysicalOutput* getOutputById(uint8_t id);

    // --- CV Loading ---
    void parseRcn225(ICVAccess& cvAccess);
    void parseRcn227PerFunction(ICVAccess& cvAccess);
    void parseRcn227PerOutputV1(ICVAccess& cvAccess);
    void parseRcn227PerOutputV2(ICVAccess& cvAccess);
    void parseRcn227PerOutputV3(ICVAccess& cvAccess);

    std::vector<PhysicalOutput> _outputs;
    std::vector<LogicalFunction*> _logical_functions;
    std::vector<ConditionVariable> _condition_variables;
    std::vector<MappingRule> _mapping_rules;

    // --- Decoder State ---
    bool _function_states[MAX_DCC_FUNCTIONS] = {false};
    DecoderDirection _direction = DECODER_DIRECTION_FORWARD;
    uint16_t _speed = 0;
    std::map<uint16_t, bool> m_binary_states;
    std::map<uint8_t, bool> _cv_states;
    bool _state_changed = true;
};

} // namespace xDuinoRails

#endif // XDRAILS_DCC_LIGHTS_AND_FUNCTIONS_H
