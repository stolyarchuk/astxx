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
 * Don't include this file directly, include astxx/manager.h instead.
 * 
 * This file contains the astxx::manager exception classes.
 */

#ifndef ASTXX_MANAGER_ERROR_H
#define ASTXX_MANAGER_ERROR_H

#include <astxx/exception.h>
#include <string>

namespace astxx {
   namespace manager {
      
      /// A generic manager error.
      class error : public astxx::exception {
         public:
            error() throw() : astxx::exception("Manager Error") { }
            explicit error(const std::string& desc) throw() : astxx::exception(desc) { }
      };
      
      /// A parse error.
      class parse_error : public manager::error {
         public:
            parse_error() throw() : manager::error("parse error") { }
            explicit parse_error(const std::string& desc) throw() : manager::error(desc) { }
      };

      /// We tried to parse an empty header.
      class empty_header : public parse_error {
         public:
            empty_header() throw() : parse_error("empty header recieved") { }
      };

      /// An unknown message type was recieved.
      class unknown_message : public parse_error {
         public:
            explicit unknown_message(const std::string& desc) throw() : parse_error("unknown message type: " + desc), m_type(desc) { }
            ~unknown_message() throw() { }

            /// Get the type of unknown message.
            std::string type() const { return m_type; }
         private:
            std::string m_type;
      };
      
      /** The error string of the 'Message' header for a permission denied 
       * error from Asterisk.
       */
      extern const char* permission_error_string;

      /** The error strong of the 'Message' header for an authentication 
       * required error from asterisk.
       */
      extern const char* authentication_error_string;

      /// A permission error.
      class permission_denied : public manager::error {
         public:
            explicit permission_denied() throw() : manager::error(permission_error_string) { }
      };

      /// An authentication error.
      class authentication_required : public manager::error {
         public:
            explicit authentication_required() throw() : manager::error(authentication_error_string) { }
      };

   }
}

#endif
