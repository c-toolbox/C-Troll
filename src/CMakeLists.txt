##########################################################################################
#                                                                                        #
# Copyright (c) 2016-2025                                                                #
# Alexander Bock                                                                         #
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

target_compile_options(project_options INTERFACE
  "/MP"                 # Multi-threading support
  "/permissive-"        # Enable conformance mode
  "/EHsc"               # Exception handling
  "/Zc:__cplusplus"     # Correctly set the __cplusplus macro
  "/Zc:strictStrings-"  # Windows header don't adhere to this   @CHECK
)

target_compile_options(project_options INTERFACE
  "/W4"       # Highest warning level
  "/w44062"   # missing case label
  "/w44165"   # 'HRESULT' is being converted to 'bool'
  "/w44242"   # conversion from 'type1' to 'type2', possible loss of data
  "/w44254"   # conversion from 'type1' to 'type2', possible loss of data
  "/w44263"   # member function does not override any base class virtual member function
  "/w44265"   # class has virtual functions, but destructor is not virtual
  "/w44287"   # unsigned/negative constant mismatch
  "/w44289"   # using for-loop variable outside of loop
  "/w44296"   # expression is always true/false
  "/w44437"   # dynamic_cast could fail in some contexts
  "/w44545"   # expression before comma evaluates to a function missing an argument list
  "/w44546"   # function call before comma missing argument list
  "/w44547"   # operator before comma has no effect
  "/w44548"   # operator before comma has no effect
  "/w44549"   # operator before comma has no effect
  "/w44555"   # expression has no effect; expected expression with side-effect
  "/w44574"   # 'identifier' is defined to be '0': did you mean to use '#if identifier'?
  "/w44619"   # #pragma warning: there is no warning number 'number'
  "/w44640"   # 'instance' : construction of local static object is not thread-safe  @CHECK
  "/w44643"   # Forward declaring 'identifier' in namespace std is not permitted
  "/w44800"   # Implicit conversion from 'type' to bool. Possible information loss
  "/w44822"   # local class member function does not have a body
  "/w44841"   # non-standard extension used: compound member designator used in offsetof
  "/w44842"   # the result of 'offsetof' applied to a type using multiple inheritance is
              # not guaranteed to be consistent between compiler releases
  "/w44905"   # wide string literal cast to 'LPSTR'
  "/w44906"   # string literal cast to 'LPWSTR'
  "/w44907"   # multiple calling conventions cannot be specified; last given will be
              # used
  "/w44928"   # illegal copy-initialization; more than one user-defined conversion has
              # been implicitly applied
  "/w44946"   # reinterpret_cast used between related classes: 'class1' and 'class2'
  "/w44986"   # exception specification does not match previous declaration
  "/w44987"   # nonstandard extension used: 'throw (...)'
  "/w45022"   # multiple move constructors specified
  "/w45023"   # multiple move assignment operators specified
  "/w45031"   # #pragma warning(pop): likely mismatch, popping warning state pushed in
              # different file
  "/w45032"   # detected #pragma warning(push) with no #pragma warning(pop)
  "/w45038"   # data member 'member1' will be initialized after data member 'member2'
  "/w45041"   # out-of-line definition for constexpr data is deprecated
  "/w45042"   # function declarations at block scope cannot be specified 'inline'
  "/w45204"   # virtual class has non-virtual trivial destructor
  "/w45233"   # explicit lambda capture 'identifier' is not used
  "/w45340"   # attribute is ignored in this syntactic position
  "/w45243"   # using incomplete class 'class-name' can cause potential one definition
              # rule violation due to ABI limitation
  "/w45245"   # unreferenced function with internal linkage has been removed
  "/w45249"   # 'bitfield' of type 'enumeration_name' has named enumerators with values
              # that cannot be represented in the given bit field width of
              # 'bitfield_width'
  "/w45258"   # explicit capture of 'symbol' is not required for this use
  "/w45259"   # explicit specialization requires 'template <>'
  "/w45262"   # implicit fall-through occurs here
  "/w45263"   # calling 'std::move' on a temporary object prevents copy elision
  "/w45264"   # 'const' variable is not used
  "/w45266"   # 'const' qualifier on return type has no effect
  "/w45270"   # 'value' is not allowed for option 'switch name'; allowed values are:
              # value list
  "/w45272"   # throwing an object of non-copyable type 'type' is non-standard. If a
              # copy is needed at runtime it will be made as if by memcpy
  "/w45277"   # 	type trait optimization for 'class name' is disabled
  "/w45304"   # a declaration designated by the using-declaration 'name1' exported from
              # this module has internal linkage and using such a name outside the
              # module is ill-formed; consider declaring 'name2' 'inline' to use it
              # outside of this module
  "/w45305"   # 'name': an explicit instantiation declaration that follows an explicit
              # instantiation definition is ignored
  "/w45307"   #  	'function': argument (argument number) converted from 'type 1' to
              # 'type 2'. Missing 'L' encoding-prefix for character literal?
  "/w45308"   # Modifying reserved macro name 'macro name' may cause undefined behavior
  "/w45309"   #	literal suffix 'name' requires at least 'language version'
)

target_compile_options(project_options INTERFACE
  "/wd4068"   # unknown pragma
  "/wd5030"   # attribute 'attribute' is not recognized  [raised by: codegen]
)

target_compile_definitions(project_options INTERFACE
  "_CRT_SECURE_NO_WARNINGS"
  "NOMINMAX"
  "VC_EXTRALEAN"
  "WIN32_LEAN_AND_MEAN"
)

target_precompile_headers(project_options INTERFACE <vector> <string> <map> <utility>)


add_subdirectory(common)
add_subdirectory(ctroll)
add_subdirectory(editor)
add_subdirectory(starter)
add_subdirectory(tray)
