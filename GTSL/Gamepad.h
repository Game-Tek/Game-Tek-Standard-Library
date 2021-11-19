#pragma once

#include "Core.h"

#include "Delegate.hpp"
#include "Math/Vectors.hpp"

#if (_WIN64)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Xinput.h>
#endif

namespace GTSL
{
	/**
	 * \brief Represents a gamepad that might or might not be connected at the time, and allows querying for connection and setting it's vibration state.
	 */
	class Gamepad
	{
	protected:
		friend class Window;

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
	public:
		enum class SourceNames : uint8 {
			TRIGGER, SHOULDER, THUMB, MIDDLE_BUTTONS, DPAD, FACE_BUTTONS, THUMB_BUTTONS
		};

		enum class Side : uint8 {
			UP, RIGHT, DOWN, LEFT
		};

		template<typename F>
		static bool Update(Gamepad& gamepadQuery, F&& updateFunction, uint8 controllerId) noexcept {
			XINPUT_STATE xinput_state;

			if (XInputGetState(controllerId, &xinput_state) != ERROR_SUCCESS) { return false; }
			if (gamepadQuery.packetNumber == xinput_state.dwPacketNumber) { return true; }

			gamepadQuery.packetNumber = xinput_state.dwPacketNumber;

			if (xinput_state.Gamepad.bLeftTrigger != gamepadQuery.leftTrigger) {
				const auto value = static_cast<float>(xinput_state.Gamepad.bLeftTrigger) / 255.0f;
				updateFunction(SourceNames::TRIGGER, Side::LEFT, &value);
				gamepadQuery.leftTrigger = xinput_state.Gamepad.bLeftTrigger;
			}

			if (xinput_state.Gamepad.bRightTrigger != gamepadQuery.rightTrigger) {
				const auto value = static_cast<float>(xinput_state.Gamepad.bRightTrigger) / 255.0f;
				updateFunction(SourceNames::TRIGGER, Side::RIGHT, &value);
				gamepadQuery.rightTrigger = xinput_state.Gamepad.bRightTrigger;
			}

			if (xinput_state.Gamepad.sThumbLX != gamepadQuery.thumbLX || xinput_state.Gamepad.sThumbLY != gamepadQuery.thumbLY) {
				const auto value = Vector2(static_cast<float>(xinput_state.Gamepad.sThumbLX) / 32768.f, static_cast<float>(xinput_state.Gamepad.sThumbLY) / 32768.f);
				updateFunction(SourceNames::THUMB, Side::LEFT, &value);
				gamepadQuery.thumbLX = xinput_state.Gamepad.sThumbLX; gamepadQuery.thumbLY = xinput_state.Gamepad.sThumbLY;
			}

			if (xinput_state.Gamepad.sThumbRX != gamepadQuery.thumbRX || xinput_state.Gamepad.sThumbRY != gamepadQuery.thumbRY) {
				const auto value = Vector2(static_cast<float>(xinput_state.Gamepad.sThumbRX) / 32768.f, static_cast<float>(xinput_state.Gamepad.sThumbRY) / 32768.f);
				updateFunction(SourceNames::THUMB, Side::RIGHT, &value);
				gamepadQuery.thumbRX = xinput_state.Gamepad.sThumbRX; gamepadQuery.thumbRY = xinput_state.Gamepad.sThumbRY;
			}

			if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != gamepadQuery.dpadUp) {
				const auto value = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP);
				gamepadQuery.dpadUp = value;
				updateFunction(SourceNames::DPAD, Side::UP, &value);
			}

			if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != gamepadQuery.dpadDown) {
				const auto value = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
				gamepadQuery.dpadDown = value;
				updateFunction(SourceNames::DPAD, Side::DOWN, &value);
			}

			if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != gamepadQuery.dpadLeft) {
				const auto value = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
				gamepadQuery.dpadLeft = value;
				updateFunction(SourceNames::DPAD, Side::LEFT, &value);
			}

			if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != gamepadQuery.dpadRight) {
				const auto value = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
				gamepadQuery.dpadRight = value;
				updateFunction(SourceNames::DPAD, Side::RIGHT, &value);
			}

			if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_START) != gamepadQuery.start) {
				const auto value = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_START);
				gamepadQuery.start = value;
				updateFunction(SourceNames::MIDDLE_BUTTONS, Side::RIGHT, &value);
			}

			if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != gamepadQuery.back) {
				const auto value = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK);
				gamepadQuery.back = value;
				updateFunction(SourceNames::MIDDLE_BUTTONS, Side::LEFT, &value);
			}

			if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != gamepadQuery.leftThumb) {
				const auto value = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB);
				gamepadQuery.leftThumb = value;
				updateFunction(SourceNames::THUMB_BUTTONS, Side::LEFT, &value);
			}

			if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != gamepadQuery.rightThumb) {
				const auto value = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB);
				gamepadQuery.rightThumb = value;
				updateFunction(SourceNames::THUMB_BUTTONS, Side::RIGHT, &value);
			}

			if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != gamepadQuery.leftShoulder) {
				const auto value = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
				gamepadQuery.leftShoulder = value;
				updateFunction(SourceNames::SHOULDER, Side::LEFT, &value);
			}

			if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != gamepadQuery.rightShoulder) {
				const auto value = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
				gamepadQuery.rightShoulder = value;
				updateFunction(SourceNames::SHOULDER, Side::RIGHT, &value);
			}

			if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != gamepadQuery.a) {
				const auto value = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_A);
				gamepadQuery.a = value;
				updateFunction(SourceNames::FACE_BUTTONS, Side::DOWN, &value);
			}

			if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_B) != gamepadQuery.b) {
				const auto value = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_B);
				gamepadQuery.b = value;
				updateFunction(SourceNames::FACE_BUTTONS, Side::RIGHT, &value);
			}

			if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_X) != gamepadQuery.x) {
				const auto value = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_X);
				gamepadQuery.x = value;
				updateFunction(SourceNames::FACE_BUTTONS, Side::LEFT, &value);
			}

			if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != gamepadQuery.y) {
				const auto value = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_Y);
				gamepadQuery.y = value;
				updateFunction(SourceNames::FACE_BUTTONS, Side::UP, &value);
			}

			return true;
		}

		/**
		 * \brief Sets the level of vibration on the gamepad controllers.
		 * \param lowFrequency Amount of low frequency vibration, 0 is none, 1 is max.
		 * \param highFrequency Amount of high frequency vibration, 0 is none, 1 is max.
		 */
		void SetVibration(float32 lowFrequency, float32 highFrequency) const noexcept
		{
			XINPUT_VIBRATION xinput_vibration;
			xinput_vibration.wLeftMotorSpeed = static_cast<WORD>(lowFrequency * 65535);
			xinput_vibration.wRightMotorSpeed = static_cast<WORD>(highFrequency * 65535);
			XInputSetState(controllerId, &xinput_vibration);
		}
	};
}