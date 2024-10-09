/*
 * ControlCommands.h
 *
 *  Created on: 4 Nov 2020
 *      Author: manuel
 *
 * This file is a modified version of the ControlCommands.h source file from PanelDue
 * The original can be found at https://github.com/Duet3D/PanelDueFirmware/blob/master/src/ControlCommands.h
 */

#ifndef SRC_CONTROLCOMMANDS_HPP_
#define SRC_CONTROLCOMMANDS_HPP_

enum class ControlCommand
{
	invalid,
	reset,
	eraseAndReset,
};


struct ControlCommandMapEntry
{
	const char* key;
	const ControlCommand val;
};

// This table has to be kept in alphabetical order of the keys
const ControlCommandMapEntry controlCommandMap[] =
{
	{ "eraseAndReset",	ControlCommand::eraseAndReset },
	{ "reset",			ControlCommand::reset },
};

#endif /* SRC_CONTROLCOMMANDS_HPP_ */
