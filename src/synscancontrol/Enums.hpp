/*
 * Project Name: synscancontrol
 * File: Enums.hpp
 *
 * Copyright (C) 2024 Jon Dalrymple
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Jon Dalrymple
 * Created: 13 November 2024
 * Description: Useful enums for readability
 */
#ifndef ENUMS_H
#define ENUMS_H

namespace SynScanControl
{

    enum class AxisEnum
    {
        AXIS_RA = 1,
        AXIS_DEC = 2,
        AXIS_BOTH = 3,
        AXIS_NONE = 0
    };

    enum class SlewTypeEnum
    {
        GOTO,
        TRACKING,
        NONE
    };

    enum class SlewSpeedEnum
    {
        FAST,
        SLOW,
        NONE
    };

    enum class SlewDirectionEnum
    {
        CCW,
        CW,
        NONE
    };

    enum class CommandEnum
    {
        SET_POSITION_CMD = 'E',
        INITIALIZATION_DONE_CMD = 'F',
        SET_MOTION_MODE_CMD = 'G',
        SET_GOTO_TARGET_CMD = 'S',
        SET_GOTO_TARGET_INCREMENT_CMD = 'H',
        SET_BREAKPOINT_INCREMENT_CMD = 'M',
        SET_STEP_PERIOD_CMD = 'I',
        START_MOTION_CMD = 'J',
        STOP_MOTION_CMD = 'K',
        INSTANT_STOP_CMD = 'L',
        SET_SWITCH_CMD = 'O',
        SET_AUTOGUIDE_SPEED_CMD = 'P',
        SET_POLAR_LED_BRIGHTNESS_CMD = 'V',
        GET_COUNTS_PER_REV_CMD = 'a',
        GET_TIMER_FREQ_CMD = 'b',
        GET_GOTO_TARGET_CMD = 'h',
        GET_STEP_PERIOD_CMD = 'i',
        GET_POSITION_CMD = 'j',
        GET_STATUS_CMD = 'f',
        GET_HIGH_SPEED_RATIO_CMD = 'g',
        GET_SIDEREAL_PERIOD_CMD = 'D',
        GET_AXIS_POSITION_CMD = 'd',
        GET_VERSION_CMD = 'e',
        GET_PEC_PERIOD_CMD = 's',
        GET_EXTENDED_STATUS_CMD = 'q',
        UNKNOWN_CMD = '\0'
    };

    enum class ErrorEnum
    {
        UNKNOWN_CMD_ERROR = 0,
        COMMAND_LENGTH_ERROR = 1,
        MOTOR_NOT_STOPPED_ERROR = 2,
        INVALID_CHARACTER_ERROR = 3,
        NOT_INITIALIZED_ERROR = 4,
        DRIVER_SLEEPING_ERROR = 5,
        PEC_TRAINING_IS_RUNNING_ERROR = 7,
        NO_VALID_PEC_DATA_ERROR = 8
    };

} // namespace SynScanControl

#endif /* ENUMS_H */