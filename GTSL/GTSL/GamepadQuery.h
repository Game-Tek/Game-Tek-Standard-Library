#pragma once

#include "Core.h"

#include "Delegate.hpp"
#include "Input.h"
#include "Math/Vector2.h"

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
	protected:
#if (_WIN32)
		XINPUT_STATE input_state{};
		uint8 controllerId{ 0 };
		static constexpr ButtonState intToGamepadButtonState(const int a) noexcept { return static_cast<ButtonState>(!a); }
#endif

	public:
		enum class GamepadButtonPosition : uint8
		{
			TOP, RIGHT, BOTTOM, LEFT
		};

		enum class Side : uint8
		{
			RIGHT, LEFT
		};
		
		/**
		 * \brief This delegate is called when the right trigger of the gamepad changes it's state in respect to the last update to the gamepad query.
		 * The first float represents the current value and the second float represents the delta from the last update to the gamepad query.
		 */
		Delegate<void(Side, float, float)> OnTriggersChange;
		/**
		 * \brief This delegate is called when the right stick of the gamepad changes it's state in respect to the last update to the gamepad query.
		 * The first two floats represent the current X; Y of the stick and the two other floats represent the delta X;Y in respect to
		 * the last update to this gamepad query.
		 */
		Delegate<void(Side, Vector2, Vector2)> OnSticksMove;

		Delegate<void(Side, ButtonState)> OnMenuButtonsChange;
		
		Delegate<void(Side, ButtonState)> OnHatsChange;

		Delegate<void(Side, ButtonState)> OnSticksChange;
		
		Delegate<void(GamepadButtonPosition, ButtonState)> OnDPadChange;
		Delegate<void(GamepadButtonPosition, ButtonState)> OnRightButtonsChange;

		/**
		 * \brief Checks state of gamepad to see what changed. During this call all parameters that changed will produce an event call.
		 * \param connected Returns true if the gamepad is connected, false otherwise.
		 */
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