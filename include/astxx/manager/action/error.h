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
 * This file contains manager::action related errors.
 */

#ifndef ASTXX_MANAGER_ACTION_ERROR_H
#define ASTXX_MANAGER_ACTION_ERROR_H

#include <astxx/manager/message.h>
#include <astxx/manager/error.h>

namespace astxx {
   namespace manager {
      namespace action {

         /// There was an error executing an action.
         class error : public manager::error {
            public:
               explicit error(std::string error, message::action action) : manager::error(error), m_action(action) { }

               virtual ~error() throw() { }
               /** Get the action that generated this error.
                * @return the action that generated this error.
                */
               virtual message::action action() const {
                  return m_action;
               }
            public:
               message::action m_action;
         };

         /// Asterisk reported required data was missing
         class missing_data : public action::error {
            public:
               missing_data(std::string error, message::action action) : action::error(error, action) { }
         };

         /// Asterisk reported the given data was bad
         class bad_data : public action::error {
            public:
               bad_data(std::string error, message::action action) : action::error(error, action) { }
         };

         /// Asterisk reported that the specified channel was not found.
         class channel_not_found : public action::error {
            public:
               channel_not_found(std::string error, message::action action) : action::error(error, action) { }
         };

      }
   }
}

#endif
