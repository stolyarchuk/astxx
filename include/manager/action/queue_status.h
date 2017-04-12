/* 
 * Additonal actions for Astxx
 * Copyright (C) 2010  Alexander Drozdov <hatred@inbox.ru>
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
 * Include this file to use the QueueStatus action. You should include it by hands.
 *
 */

#ifndef ASTXX_MANAGER_ACTION_QUEUE_STATUS_H
#define ASTXX_MANAGER_ACTION_QUEUE_STATUS_H

#include "manager/basic_action.h"
#include "manager/message.h"
// #include "manager/error.h"

namespace astxx {
   namespace manager {
      namespace action {
         /** Ask Queue Status
          */
         class queue_status : public basic_action {
            public:
               /** Construct a QueueStatus action.
                * @param queue          - concretize queue [optional]
                * @param member         - select member [optional]
                * @param action_id      - add optional action Id to all responses [optional]
                */
               queue_status(const std::string& member    = "",
                            const std::string& queue     = "", 
                            const std::string& action_id = "") :
                                member(member),
                                queue(queue),
                                action_id(action_id) {}

               /** Format this action as a message::action.
                * @return this message as a message::action
                */
               message::action action() const {
                  message::action action("QueueStatus");

                  if (!queue.empty())
                  {
                    action["Queue"] = queue;
                  }

                  if (!member.empty())
                  {
                    action["Member"] = member;
                  }

                  if (!action_id.empty())
                  {
                    action["ActionID"] = action_id;
                  }

                  return action;
               }

            private:
               std::string queue;
               std::string member;
               std::string action_id;
         };
      }
   }
}

#endif // QUEUESTATUS_H