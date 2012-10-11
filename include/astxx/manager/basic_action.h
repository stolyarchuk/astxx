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
 * Include this file to derrive from the astxx::manager::basic_action class.
 * 
 * This file contains the astxx::manager::basic_action class which should be 
 * derrived from to create new manager actions.
 */

#ifndef ASTXX_MANAGER_ACTION_H
#define ASTXX_MANAGER_ACTION_H

#include <astxx/manager/message.h>
#include <astxx/manager/error.h>
#include <astxx/manager/action/error.h>

namespace astxx {
   namespace manager {
      class connection;

      /** Base class for manager commands.
       * This class should be derrived from to form actual manager commands.  
       * Derrived classes will be passed to the 
       * manager::connection::send_action() function which will then execute 
       * their basic_action::action() member function and send whatever it 
       * returns to Asterisk.
       */
      class basic_action {
         public:
            basic_action() { }
            virtual ~basic_action() { }

            /** Format this command as a header for sending to Asterisk.
             * This function is called by manager::connection::send_action() to 
             * get the data to send to Asterisk.
             * @return a message::action message to send to Asterisk
             */
            virtual message::action action() const = 0;

            /** Get the ActionID for this action.
             * @return the ActionID for this action.
             */
            virtual std::string action_id() const {
               return m_action_id;
            }

            /** Set the ActionID for this action.
             */
            virtual void action_id(const std::string& id) {
               m_action_id = id;
            }

            virtual message::response handle_response(message::response response);
            message::response operator()(connection& c);

         private:
            std::string m_action_id;
      };
   }
}

#endif
