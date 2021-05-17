#pragma once

#include "Core.h"

#include "Delegate.hpp"
#include "Math/Vectors.h"

namespace GTSL
{
	/**
	 * \brief Represents a gamepad that might or might not be connected at the time, and allows querying for connection and setting it's vibration state.
	 */
	class Gamepad
	{
	protected:
#if (_WIN64)
		uint8 controllerId{ 0 };

		uint32 packetNumber = 0;
		
		byte leftTrigger = 0, rightTrigger = 0;
		int16 thumbLX = 0, thumbLY = 0, thumbRX = 0, thumbRY = 0;
		bool dpadUp : 1 = false, dpadRight : 1 = false, dpadDown : 1 = false, dpadLeft : 1 = false;
		bool start  : 1 = false, back : 1 = false;
		bool leftThumb : 1 = false, rightThumb : 1 = false;
		bool leftShoulder : 1 = false, rightShoulder : 1 = false;
		bool a : 1 = false, b : 1 = false, x : 1 = false, y : 1 = false;
#endif
		template<typename BF, typename FF, typename VF>
		friend bool Update(Gamepad& gamepadQuery, BF&& buttonFunction, FF&& floatFunction, VF&& vectorFunction, uint8 controllerId) noexcept;
	public:
		enum class GamepadButtonPosition : uint8
		{
			TOP, RIGHT, BOTTOM, LEFT, BACK, HOME, DPAD_UP, DPAD_RIGHT, DPAD_DOWN, DPAD_LEFT, LEFT_SHOULDER, RIGHT_SHOULDER,
			LEFT_STICK, RIGHT_STICK
		};

		enum class Side : uint8
		{
			RIGHT, LEFT
		};

		/**
		 * \brief Sets the level of vibration on the gamepad controllers.
		 * \param lowFrequency Amount of low frequency vibration, 0 is none, 1 is max.
		 * \param highFrequency Amount of high frequency vibration, 0 is none, 1 is max.
		 */
		void SetVibration(float32 lowFrequency, float32 highFrequency) const noexcept;
	};
}