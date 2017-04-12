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
 * Include this file to use the QueuePause action. You should include it by hands.
 *
 */

#ifndef QUEUE_PAUSE_H
#define QUEUE_PAUSE_H

#include "manager/basic_action.h"
#include "manager/message.h"
// #include "manager/error.h"

#include <vector>
#include <string>

namespace astxx
{
    namespace manager
    {                                                                                                                                                                                                                                        
        namespace action                                                                                                                                                                                                                     
        {                                                                                                                                                                                                                                    
            /** Set Queue Pause state                                                                                                                                                                                                        
            */                                                                                                                                                                                                                               
            class queue_pause : public basic_action                                                                                                                                                                                          
            {                                                                                                                                                                                                                                
            public:                                                                                                                                                                                                                          
                class error : public manager::error                                                                                                                                                                                          
                {                                                                                                                                                                                                                            
                public:                                                                                                                                                                                                                      
                    explicit error(const std::string& desc) throw() : manager::error(desc) { }                                                                                                                                               
                };                                                                                                                                                                                                                           
                                                                                                                                                                                                                                             
            public:                                                                                                                                                                                                                          
                /** Construct a QueuePause action.
                * @param interface      - member
                * @param is_on          - pause state
                * @param queue          - queue for process, if omited - all queues is used
                */
                queue_pause(const std::string &interface, const bool is_on = false, const std::string &queue = "") :
                    _interface(interface),
                    _queue(queue),
                    _is_on(is_on) {}

               /** Format this action as a message::action.
                * @return this message as a message::action
                */
                message::action action() const
                {
                    message::action action("QueuePause");

                    if (!_queue.empty())
                    {
                        action["Queue"] = _queue;
                    }

                    action["Interface"] = _interface;

                    std::string on_off_str = "false";
                    if (_is_on)
                    {
                        on_off_str = "true";
                    }

                    action["Paused"] = on_off_str;

                    return action;
                }

                message::response handle_response(message::response response)
                {
                    basic_action::handle_response(response);
                    if ("Success" != response)
                    {
                        throw queue_pause::error(response["Message"]);
                    }

                    return response;
                }

            private:
                std::string _interface;
                std::string _queue;
                bool        _is_on;
            };

        }
    }
}

#endif // QUEUE_PAUSE_H