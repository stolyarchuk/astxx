/* vim: set et sw=3 tw=0 fo=croqlaw cino=t0:
 *
 * Astxx, the Asterisk C++ API and Utility Library.
 * Copyright (C) 2005-2007  Matthew A. Nicholson
 * Copyright (C) 2005-2007  Digium, Inc.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/** @file
 *
 * Include this file to use the astxx Asterisk Manager API.
 * 
 * This file includes all other files needed to use the manager as well as all 
 * of the manager actions/commands.
 */

#ifndef ASTXX_MANAGER_H
#define ASTXX_MANAGER_H

#include <astxx/manager/connection.h>
#include <astxx/manager/error.h>
#include <astxx/manager/message.h>

#include <astxx/manager/action/absolute_timeout.h>
#include <astxx/manager/action/command.h>
#include <astxx/manager/action/extension_state.h>
#include <astxx/manager/action/getvar.h>
#include <astxx/manager/action/hangup.h>
#include <astxx/manager/action/list_commands.h>
#include <astxx/manager/action/login.h>
#include <astxx/manager/action/logoff.h>
#include <astxx/manager/action/mailbox_count.h>
#include <astxx/manager/action/mailbox_status.h>
#include <astxx/manager/action/originate.h>
#include <astxx/manager/action/ping.h>
#include <astxx/manager/action/redirect.h>
#include <astxx/manager/action/setvar.h>
#include <astxx/manager/action/status.h>
#include <astxx/manager/action/user_event.h>

#endif
