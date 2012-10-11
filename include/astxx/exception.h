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

/** @mainpage Astxx
 *
 * The goal of Astxx is to provide a fully functional and easy to use C++ 
 * wrapper for Asterisk enabling developers to write Asterisk related software 
 * using the full range of what C++ has to offer.  This includes AGI scripts, 
 * accessing the Manager API, writing ExternalIVR scripts, writing IAX clients, 
 * and writing Asterisk modules.
 */

/** @file
 *
 * This file contains exceptions know to Astxx.  All exceptions are derrived 
 * from astxx::exception which is derrived from std::exception.
 */

#ifndef ASTXX_EXCEPTION_H
#define ASTXX_EXCEPTION_H 

#include <stdexcept>
#include <string>

namespace astxx {
   /**
    * The general Astxx exception from which all other Astxx exceptions are 
    * derrived.  This is derrived from std::exception.
    */
   class exception : public std::exception {
      public:
         /// constructor
         explicit exception(const std::string& desc) throw() : std::exception(), description(desc) { }
         virtual ~exception() throw() { }
         /// Get a description of this exception.
         virtual const char* what() const throw() {
            return description.c_str();
         }
      private:
         std::string description;
   };
   
}

#endif
