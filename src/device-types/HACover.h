#ifndef AHA_HACOVER_H
#define AHA_HACOVER_H

#include "HABaseDeviceType.h"

#ifndef EX_ARDUINOHA_COVER

#define HACOVER_CALLBACK(name) void (*name)(CoverCommand cmd, HACover* sender)
#define HACOVER_SET_POSITION_CALLBACK(name) void (*name)(uint8_t position, HACover* sender)
#define HACOVER_TILT_CALLBACK(name) void (*name)(uint8_t tilt, HACover* sender)

/**
 * HACover allows to control a cover (such as blinds, a roller shutter or a garage door).
 *
 * @note
 * You can find more information about this entity in the Home Assistant documentation:
 * https://www.home-assistant.io/integrations/cover.mqtt/
 */
class HACover : public HABaseDeviceType {
public:
    static const int16_t DefaultPosition = -32768;
    static const int16_t DefaultTilt = -32768;

    enum CoverState {
        StateUnknown = 0,
        StateClosed,
        StateClosing,
        StateOpen,
        StateOpening,
        StateStopped
    };

    enum CoverCommand {
        CommandOpen,
        CommandClose,
        CommandStop
    };

    enum Features {
        DefaultFeatures = 0,
        PositionFeature = 1,
        SetPositionFeature = 2,
        TiltFeature = 4
    };

    /**
     * @param uniqueId The unique ID of the cover. It needs to be unique in a scope of your device.
     * @param features Features that should be enabled for the cover.
     *                 You can enable multiple features by using OR bitwise operator, for example:
     *                 `HACover::PositionFeature | HACover::TiltFeature`
     */
    HACover(const char *uniqueId, const uint8_t features = DefaultFeatures);

    /**
     * Changes state of the cover and publishes MQTT message.
     * Please note that if a new value is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param state New state of the cover.
     * @param force Forces to update state without comparing it to previous known state.
     * @returns Returns true if MQTT message has been published successfully.
     */
    bool setState(const CoverState state, const bool force = false);

    /**
     * Changes the position of the cover and publishes MQTT message.
     * Please note that if a new value is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param position The new position of the cover (0-100).
     * @param force Forces to update the state without comparing it to a previous known state.
     * @returns Returns `true` if MQTT message has been published successfully.
     */
    bool setPosition(const int16_t position, const bool force = false);

    /**
     * Changes the tilt of the cover and publishes MQTT message.
     * Please note that if a new value is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param tilt The new tilt value of the cover (0-100).
     * @param force Forces to update the state without comparing it to a previous known state.
     * @returns Returns `true` if MQTT message has been published successfully.
     */
    bool setTilt(const int16_t tilt, const bool force = false);

    /**
     * Sets the current state of the cover without publishing it to Home Assistant.
     * This method may be useful if you want to change the state before the connection
     * with the MQTT broker is acquired.
     *
     * @param state The new state of the cover.
     */
    inline void setCurrentState(const CoverState state) { _currentState = state; }

    /**
     * Returns last known state of the cover.
     * By default the state is set to CoverState::StateUnknown
     */
    inline CoverState getCurrentState() const { return _currentState; }

    /**
     * Sets the current position of the cover without pushing the value to Home Assistant.
     * This method may be useful if you want to change the position before the connection
     * with the MQTT broker is acquired.
     *
     * @param position The new position of the cover (0-100).
     */
    inline void setCurrentPosition(const int16_t position) { _currentPosition = position; }

    /**
     * Returns the last known position of the cover.
     * By default position is set to HACover::DefaultPosition
     */
    inline int16_t getCurrentPosition() const { return _currentPosition; }

    /**
     * Sets the current tilt of the cover without pushing the value to Home Assistant.
     * This method may be useful if you want to change the tilt before the connection
     * with the MQTT broker is acquired.
     *
     * @param position The new tilt value of the cover (0-100).
     */
    inline void setCurrentTilt(const int16_t tilt) { _currentTilt = tilt; }

    /**
     * Returns the last known tilt of the cover.
     * By default tilt is set to HACover::DefaultTilt
     */
    inline int16_t getCurrentTilt() const { return _currentTilt; }

    /**
     * Sets class of the device.
     * You can find list of available values here: https://www.home-assistant.io/integrations/cover/
     *
     * @param deviceClass The class name.
     */
    inline void setDeviceClass(const char *deviceClass) { _class = deviceClass; }

    /**
     * Sets icon of the cover.
     * Any icon from MaterialDesignIcons.com (for example: `mdi:home`).
     *
     * @param icon The icon name.
     */
    inline void setIcon(const char *icon) { _icon = icon; }

    /**
     * Sets retain flag for the cover's command.
     * If set to `true` the command produced by Home Assistant will be retained.
     *
     * @param retain
     */
    inline void setRetain(const bool retain) { _retain = retain; }

    /**
     * Sets optimistic flag for the cover state.
     * In this mode the cover state doesn't need to be reported back to the HA panel when a command is received.
     * By default the optimistic mode is disabled.
     *
     * @param optimistic The optimistic mode (`true` - enabled, `false` - disabled).
     */
    inline void setOptimistic(const bool optimistic) { _optimistic = optimistic; }

    /**
     * Registers callback that will be called each time the command from HA is received.
     * Please note that it's not possible to register multiple callbacks for the same cover.
     *
     * @param callback
     */
    inline void onCommand(HACOVER_CALLBACK(callback)) { _commandCallback = callback; }

    /**
     * Registers callback that will be called each time the set position command from HA is received.
     * Please note that it's not possible to register multiple callbacks for the same cover.
     *
     * @param callback
     */
    inline void
    onSetPositionCommand(HACOVER_SET_POSITION_CALLBACK(callback)) { _setPositionCommandCallback = callback; }

    /**
     * Registers callback that will be called each time the tilt command from HA is received.
     * Please note that it's not possible to register multiple callbacks for the same cover.
     *
     * @param callback
     */
    inline void onTiltCommand(HACOVER_TILT_CALLBACK(callback)) { _tiltCommandCallback = callback; }

protected:
    virtual void buildSerializer() override;

    virtual void onMqttConnected() override;

    virtual void onMqttMessage(
            const char *topic,
            const uint8_t *payload,
            const uint16_t length
    ) override;

private:
    /**
     * Publishes the MQTT message with the given state.
     *
     * @param state The state to publish.
     * @returns Returns `true` if the MQTT message has been published successfully.
     */
    bool publishState(const CoverState state);

    /**
     * Publishes the MQTT message with the given position.
     *
     * @param position The position to publish.
     * @returns Returns `true` if the MQTT message has been published successfully.
     */
    bool publishPosition(const int16_t position);

    /**
     * Publishes the MQTT message with the given tilt.
     *
     * @param tilt The tilt to publish.
     * @returns Returns `true` if the MQTT message has been published successfully.
     */
    bool publishTilt(const int16_t tilt);

    /**
     * Parses the given command and executes the cover's callback with proper enum's property.
     *
     * @param cmd The data of the command.
     * @param length Length of the command.
     */
    void handleCommand(const uint8_t *cmd, const uint16_t length);

    /**
     * Parses the given set position command and executes the callback with the proper value.
     *
     * @param cmd The data of the command.
     * @param length Length of the command.
     */
    void handleSetPositionCommand(const uint8_t *cmd, const uint16_t length);

    /**
     * Parses the given tilt command and executes the callback with the proper value.
     *
     * @param cmd The data of the command.
     * @param length Length of the command.
     */
    void handleTiltCommand(const uint8_t *cmd, const uint16_t length);

    /// Features enabled for the cover.
    const uint8_t _features;

    /// The current state of the cover. By default it's `HACover::StateUnknown`.
    CoverState _currentState;

    /// The current position of the cover. By default it's `HACover::DefaultPosition`.
    int16_t _currentPosition;

    /// The current tilt of the cover. By default it's `HACover::DefaultTilt`.
    int16_t _currentTilt;

    /// The device class. It can be nullptr.
    const char *_class;

    /// The icon of the button. It can be nullptr.
    const char *_icon;

    /// The retain flag for the HA commands.
    bool _retain;

    /// The optimistic mode of the cover (`true` - enabled, `false` - disabled).
    bool _optimistic;

    /// The command callback that will be called when clicking the cover's button in the HA panel.
    HACOVER_CALLBACK(_commandCallback);

    /// The command callback that will be called when setting the cover's position.
    HACOVER_SET_POSITION_CALLBACK(_setPositionCommandCallback);

    /// The command callback that will be called when changing the cover's tilt level.
    HACOVER_TILT_CALLBACK(_tiltCommandCallback);
};

#endif
#endif
