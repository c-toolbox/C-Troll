##########################################################################################
#                                                                                        #
# Copyright (c) 2016 - 2022                                                              #
# Alexander Bock, Erik Sundén, Emil Axelsson                                             #
#                                                                                        #
# All rights reserved.                                                                   #
#                                                                                        #
# Redistribution and use in source and binary forms, with or without modification, are   #
# permitted provided that the following conditions are met:                              #
#                                                                                        #
# 1. Redistributions of source code must retain the above copyright notice, this list    #
#    of conditions and the following disclaimer.                                         #
#                                                                                        #
# 2. Redistributions in binary form must reproduce the above copyright notice, this      #
#    list of conditions and the following disclaimer in the documentation and/or other   #
#    materials provided with the distribution.                                           #
#                                                                                        #
# 3. Neither the name of the copyright holder nor the names of its contributors may be   #
#    used to endorse or promote products derived from this software without specific     #
#    prior written permission.                                                           #
#                                                                                        #
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY    #
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES   #
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT    #
# SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,         #
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED   #
# TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR     #
# BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN       #
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     #
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH    #
# DAMAGE.                                                                                #
#                                                                                        #
##########################################################################################

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)
option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

set(MSVC_WARNINGS
  /MP
  /ZI
  /std:c++17
  /permissive-
  /W4 # Baseline reasonable warnings
  /w14242 # 'identfier': conversion from 'type1' to 'type1', possible loss of data
  /w14254 # 'operator': conversion from 'type1:field_bits' to
          # 'type2:field_bits', possible loss of data
  /w14263 # 'function': member function does not override any base class
          # virtual member function
  /w14265 # 'classname': class has virtual functions, but destructor is not
          # virtual instances of this class may not be destructed correctly
  /w14287 # 'operator': unsigned/negative constant mismatch
  /we4289 # nonstandard extension used: 'variable': loop control variable
          # declared in the for-loop is used outside the for-loop scope
  /w14296 # 'operator': expression is always 'boolean_value'
  /w14311 # 'variable': pointer truncation from 'type1' to 'type2'
  /w14545 # expression before comma evaluates to a function which is missing
          # an argument list
  /w14546 # function call before comma missing argument list
  /w14547 # 'operator': operator before comma has no effect; expected
          # operator with side-effect
  /w14549 # 'operator': operator before comma has no effect; did you intend
          # 'operator'?
  /w14555 # expression has no effect; expected expression with side- effect
  /w14619 # pragma warning: there is no warning number 'number'
  /w14640 # Enable warning on thread un-safe static member initialization
  /w14826 # Conversion from 'type1' to 'type_2' is sign-extended. This may
          # cause unexpected runtime behavior.
  /w14905 # wide string literal cast to 'LPSTR'
  /w14906 # string literal cast to 'LPWSTR'
  /w14928 # illegal copy-initialization; more than one user-defined
          # conversion has been implicitly applied
  /w44061 # enumerator 'identifier' in switch of enum 'enumeration' is not explicitly handled by a case label
  /wd4619 # #pragma warning: there is no warning number '4345'
)

set(CLANG_WARNINGS
  -Wall
  -Wextra # reasonable and standard
  -Wshadow # warn the user if a variable declaration shadows one from a
           # parent context
  -Wnon-virtual-dtor # warn the user if a class with virtual functions has a
                     # non-virtual destructor. This helps catch hard to
                     # track down memory errors
  -Wold-style-cast # warn for c-style casts
  -Wcast-align # warn for potential performance problem casts
  -Wunused # warn on anything being unused
  -Woverloaded-virtual # warn if you overload (not override) a virtual
                       # function
  -Wpedantic # warn if non-standard C++ is used
  -Wconversion # warn on type conversions that may lose data
  -Wsign-conversion # warn on sign conversions
  -Wnull-dereference # warn if a null dereference is detected
  -Wdouble-promotion # warn if float is implicit promoted to double
  -Wformat=2 # warn on security issues around functions that format output
             # (ie printf)
)

if (WARNINGS_AS_ERRORS)
  set(CLANG_WARNINGS ${CLANG_WARNINGS} -Werror)
  set(MSVC_WARNINGS ${MSVC_WARNINGS} /WX)
endif ()

set(GCC_WARNINGS
  ${CLANG_WARNINGS}
  -Wmisleading-indentation # warn if identation implies blocks where blocks
                           # do not exist
  -Wduplicated-cond # warn if if / else chain has duplicated conditions
  -Wduplicated-branches # warn if if / else branches have duplicated code
  -Wlogical-op # warn about logical operations being used where bitwise were
               # probably wanted
  -Wuseless-cast # warn if you perform a cast to the same type
)

if (MSVC)
  target_compile_options(project_options INTERFACE ${MSVC_WARNINGS})
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  target_compile_options(project_options INTERFACE ${CLANG_WARNINGS})
else ()
  target_compile_options(project_options INTERFACE ${GCC_WARNINGS})
endif ()

option(ENABLE_PCH "Enable Precompiled Headers" OFF)
if (ENABLE_PCH)
  target_precompile_headers(project_options INTERFACE <vector> <string> <map> <utility>)
endif ()




add_subdirectory(common)
add_subdirectory(ctroll)
add_subdirectory(editor)
add_subdirectory(tray)