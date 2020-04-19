#pragma once

#include "Core.h"

#include "Delegate.hpp"

#if (_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Xinput.h>
#undef WIN32_LEAN_AND_MEAN
#endif


namespace GTSL
{
	/**
	 * \brief Represents a gamepad that might or might not be connected at the time, and allows querying for connection and setting it's vibration state.
	 */
	class GamepadQuery
	{
	public:
		enum class GamepadButtonState : uint8
		{
			PRESSED, RELEASED
		};

	protected:
#if (_WIN32)
		XINPUT_STATE input_state{};
		uint8 controllerId{ 0 };
		static constexpr GamepadButtonState intToGamepadButtonState(const int a) noexcept
		{
			return static_cast<GamepadButtonState>(!a);
		}
#endif

	public:
		/**
		 * \brief This delegate is called when the right trigger of the gamepad changes it's state in respect to the last update to the gamepad query.
		 * The first float represents the current value and the second float represents the delta from the last update to the gamepad query.
		 */
		Delegate<void(float, float)> OnRightTriggerChanged;
		/**
		 * \brief This delegate is called when the left trigger of the gamepad changes it's state in respect to the last update to the gamepad query.
		 * The first float represents the current value and the second float represents the delta from the last update to the gamepad query.
		 */
		Delegate<void(float, float)> OnLeftTriggerChanged;
		/**
		 * \brief This delegate is called when the right stick of the gamepad changes it's state in respect to the last update to the gamepad query.
		 * The first two floats represent the current X; Y of the stick and the two other floats represent the delta X;Y in respect to
		 * the last update to this gamepad query.
		 */
		Delegate<void(float, float, float, float)> OnRightStickMove;
		/**
		 * \brief This delegate is called when the left stick of the gamepad changes it's state in respect to the last update to the gamepad query.
		 * The first two floats represent the current X; Y of the stick and the two other floats represent the delta X;Y in respect to
		 * the last update to this gamepad query.
		 */
		Delegate<void(float, float, float, float)> OnLeftStickMove;
		Delegate<void(GamepadButtonState)> OnRightHatChanged;
		Delegate<void(GamepadButtonState)> OnLeftHatChanged;
		Delegate<void(GamepadButtonState)> OnRightStickButtonChanged;
		Delegate<void(GamepadButtonState)> OnLeftStickButtonChanged;
		Delegate<void(GamepadButtonState)> OnTopFaceButtonChanged;
		Delegate<void(GamepadButtonState)> OnRightFaceButtonChanged;
		Delegate<void(GamepadButtonState)> OnBottomFaceButtonChanged;
		Delegate<void(GamepadButtonState)> OnLeftFaceButtonChanged;
		Delegate<void(GamepadButtonState)> OnTopDPadButtonChanged;
		Delegate<void(GamepadButtonState)> OnRightDPadButtonChanged;
		Delegate<void(GamepadButtonState)> OnBottomDPadButtonChanged;
		Delegate<void(GamepadButtonState)> OnLeftDPadButtonChanged;
		Delegate<void(GamepadButtonState)> OnStartButtonChanged;
		Delegate<void(GamepadButtonState)> OnBackButtonChanged;

		void Update(bool& connected) noexcept;

		struct GamepadVibration
		{
			/**
			 * \brief Amount of low frequency vibration, 0 is none, 1 is max.
			 */
			float LowFrequency{ 0 };
			/**
			 * \brief Amount of high frequency vibration, 0 is none, 1 is max.
			 */
			float HighFrequency{ 0 };
		};
		void SetVibration(const GamepadVibration& gamepadVibration) const noexcept;
	};
}