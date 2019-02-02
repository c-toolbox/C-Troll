/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016                                                                    *
 * Alexander Bock, Erik Sunden, Emil Axelsson                                            *
 *                                                                                       *
 * All rights reserved.                                                                  *
 *                                                                                       *
 * Redistribution and use in source and binary forms, with or without modification, are  *
 * permitted provided that the following conditions are met:                             *
 *                                                                                       *
 * 1. Redistributions of source code must retain the above copyright notice, this list   *
 * of conditions and the following disclaimer.                                           *
 *                                                                                       *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this     *
 * list of conditions and the following disclaimer in the documentation and/or other     *
 * materials provided with the distribution.                                             *
 *                                                                                       *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be  *
 * used to endorse or promote products derived from this software without specific prior *
 * written permission.                                                                   *
 *                                                                                       *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY   *
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES  *
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT   *
 * SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,        *
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED  *
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR    *
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN      *
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN    *
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH   *
 * DAMAGE.                                                                               *
 *                                                                                       *
 ****************************************************************************************/

#ifndef __JSONSUPPORT_H__
#define __JSONSUPPORT_H__

#include <QJsonArray>
#include <QJsonObject>
#include <QStringList>

enum class Optional { Yes, No };

namespace common {

/**
 * Tests whether the \p key exists in the QJsonObject \p obj and if it is of the string
 * type. If the value is \p optional and does not exist, the \p defaultValue is returned.
 * If the value is not optional but does not exist or is of the wrong type, a
 * <code>std::runtime_error</code> is raised.
 * \param obj The QJsonObject from which the value is returned
 * \param key The key to inspect
 * \param optional Determines whether the \p key is optional in the object \obj
 * \param defaultValue The default value that is returned if the \p key is optional and
 * does not exist
 * \return The value if the \p key exists and is a string, if it is optional and does not
 * exist or has the wrong type, \p defaultValue is returned
 * \throw std::runtime_error If the \p key is not \p optional and does not exist or if it
 * is not a string
 */
QString testAndReturnString(const QJsonObject& obj, const QString& key,
    Optional optional = Optional::No, QString defaultValue = "");

/**
* Tests whether the \p index exists in the QJsonArray \p arr and if it is of the string
* type. If the value is \p optional and does not exist, the \p defaultValue is returned.
* If the value is not optional but does not exist or is of the wrong type, a
* <code>std::runtime_error</code> is raised.
* \param arr The QJsonArray from which the value is returned
* \param index The index to inspect
* \param optional Determines whether the \p index is optional in the array \arr
* \param defaultValue The default value that is returned if the \p index is optional and
* does not exist
* \return The value if the \p index exists and is a string, if it is optional and does not
* exist or has the wrong type, \p defaultValue is returned
* \throw std::runtime_error If the \p index is not \p optional and does not exist or if it
* is not a string
*/
QString testAndReturnString(const QJsonArray& arr, int index,
    Optional optional = Optional::No, QString defaultValue = "");

/**
 * Tests whether the \p key exists in the QJsonObject \p obj and if it is of the integer
 * type. If the value is \p optional and does not exist, the \p defaultValue is returned.
 * If the value is not optional but does not exist or is of the wrong type, a
 * <code>std::runtime_error</code> is raised.
 * \param obj The QJsonObject from which the value is returned
 * \param key The key to inspect
 * \param optional Determines whether the \p key is optional in the object \obj
 * \param defaultValue The default value that is returned if the \p key is optional and
 * does not exist
 * \return The value if the \p key exists and is a integer, if it is optional and does not
 * exist or has the wrong type, \p defaultValue is returned
 * \throw std::runtime_error If the \p key is not \p optional and does not exist or if it
 * is not an integer
 */
int testAndReturnInt(const QJsonObject& obj, const QString& key,
    Optional optional = Optional::No, int defaultValue = 0);

/**
 * Tests whether the \p index exists in the QJsonArray \p arr and if it is of the integer
 * type. If the value is \p optional and does not exist, the \p defaultValue is returned.
 * If the value is not optional but does not exist or is of the wrong type, a
 * <code>std::runtime_error</code> is raised.
 * \param arr The QJsonArray from which the value is returned
 * \param index The index to inspect
 *  \param optional Determines whether the \p index is optional in the array \arr
 * \param defaultValue The default value that is returned if the \p index is optional and
 * does not exist
 * \return The value if the \p index exists and is an integer, if it is optional and does
 * not exist or has the wrong type, \p defaultValue is returned
 * \throw std::runtime_error If the \p index is not \p optional and does not exist or if
 * it is not an integer
 */
int testAndReturnInt(const QJsonArray& arr, int index, Optional optional = Optional::No,
    int defaultValue = 0);

/**
 * Tests whether the \p key exists in the QJsonObject \p obj and if it is of the double
 * type. If the value is \p optional and does not exist, the \p defaultValue is returned.
 * If the value is not optional but does not exist or is of the wrong type, a
 * <code>std::runtime_error</code> is raised.
 * \param obj The QJsonObject from which the value is returned
 * \param key The key to inspect
 * \param optional Determines whether the \p key is optional in the object \obj
 * \param defaultValue The default value that is returned if the \p key is optional and
 * does not exist
 * \return The value if the \p key exists and is a double, if it is optional and does not
 * exist or has the wrong type, \p defaultValue is returned
 * \throw std::runtime_error If the \p key is not \p optional and does not exist or if it
 * is not a double
 */
double testAndReturnDouble(const QJsonObject& obj, const QString& key,
    Optional optional = Optional::No, int defaultValue = 0.0);

/**
 * Tests whether the \p index exists in the QJsonArray \p arr and if it is of the double
 * type. If the value is \p optional and does not exist, the \p defaultValue is returned.
 * If the value is not optional but does not exist or is of the wrong type, a
 * <code>std::runtime_error</code> is raised.
 * \param arr The QJsonArray from which the value is returned
 * \param index The index to inspect
 * \param optional Determines whether the \p index is optional in the array \arr
 * \param defaultValue The default value that is returned if the \p index is optional and
 * does not exist
 * \return The value if the \p index exists and is a double, if it is optional and does
 * not exist or has the wrong type, \p defaultValue is returned
 * \throw std::runtime_error If the \p index is not \p optional and does not exist or if
 * it is not a double
 */
double testAndReturnDouble(const QJsonArray& arr, int index,
    Optional optional = Optional::No, int defaultValue = 0.0);

/**
 * Tests whether the \p key exists in the QJsonObject \p obj and if it is of the array
 * type. If the value is \p optional and does not exist, the \p defaultValue is returned.
 * If the value is not optional but does not exist or is of the wrong type, a
 * <code>std::runtime_error</code> is raised.
 * \param obj The QJsonObject from which the value is returned
 * \param key The key to inspect
 * \param optional Determines whether the \p key is optional in the object \obj
 * \param defaultValue The default value that is returned if the \p key is optional and
 * does not exist
 * \return The value if the \p key exists and is a string, if it is optional and does not
 * exist or has the wrong type, \p defaultValue is returned
 * \throw std::runtime_error If the \p key is not \p optional and does not exist or if it
 * is not an array
 */
QJsonArray testAndReturnArray(const QJsonObject& obj, const QString& key,
    Optional optional = Optional::No, QJsonArray defaultValue = QJsonArray());

/**
* Tests whether the \p index exists in the QJsonArray \p arr and if it is of the array
* type. If the value is \p optional and does not exist, the \p defaultValue is returned.
* If the value is not optional but does not exist or is of the wrong type, a
* <code>std::runtime_error</code> is raised.
* \param arr The QJsonArray from which the value is returned
* \param index The index to inspect
* \param optional Determines whether the \p index is optional in the array \arr
* \param defaultValue The default value that is returned if the \p index is optional and
* does not exist
* \return The value if the \p index exists and is an array, if it is optional and does not
* exist or has the wrong type, \p defaultValue is returned
* \throw std::runtime_error If the \p index is not \p optional and does not exist or if it
* is not an array
*/
QJsonArray testAndReturnArray(const QJsonArray& arr, int index,
    Optional optional = Optional::No, QJsonArray defaultValue = QJsonArray());

/**
 * Tests whether the \p key exists in the QJsonObject \p obj and if it is of the
 * StringList type. If the value is \p optional and does not exist, the \p defaultValue is
 * returned. If the value is not optional but does not exist or is of the wrong type, a
 * <code>std::runtime_error</code> is raised.
 * \param obj The QJsonObject from which the value is returned
 * \param key The key to inspect
 * \param optional Determines whether the \p key is optional in the object \obj
 * \param defaultValue The default value that is returned if the \p key is optional and
 * does not exist
 * \return The value if the \p key exists and is a string, if it is optional and does not
 * exist or has the wrong type, \p defaultValue is returned
 * \throw std::runtime_error If the \p key is not \p optional and does not exist or if it
 * is not a string list
 */
QStringList testAndReturnStringList(const QJsonObject& obj, const QString& key,
    Optional optional = Optional::No, QStringList defaultValue = QStringList());

/**
 * Tests whether the \p index exists in the QJsonArray \p arr and if it is of the
 * StringList type. If the value is \p optional and does not exist, the \p defaultValue is
 * returned. If the value is not optional but does not exist or is of the wrong type, a
 * <code>std::runtime_error</code> is raised.
 * \param arr The QJsonArray from which the value is returned
 * \param index The index to inspect
 * \param optional Determines whether the \p index is optional in the array \arr
 * \param defaultValue The default value that is returned if the \p index is optional and
 * does not exist
 * \return The value if the \p index exists and is a StringList, if it is optional and
 * does not exist or has the wrong type, \p defaultValue is returned
 * \throw std::runtime_error If the \p index is not \p optional and does not exist or if
 * it is not an string list
 */
QStringList testAndReturnStringList(const QJsonArray& arr, int index,
    Optional optional = Optional::No, QStringList defaultValue = QStringList());

/**
 * Tests whether the \p key exists in the QJsonObject \p obj and if it is an object. If
 * the value is \p optional and does not exist, the \p defaultValue is returned. If the
 * value is not optional but does not exist or is of the wrong type, a
 * <code>std::runtime_error</code> is raised.
 * \param obj The QJsonObject from which the value is returned
 * \param key The key to inspect
 * \param optional Determines whether the \p key is optional in the object \obj
 * \param defaultValue The default value that is returned if the \p key is optional and
 * does not exist
 * \return The value if the \p key exists and is a string, if it is optional and does not
 * exist or has the wrong type, \p defaultValue is returned
 * \throw std::runtime_error If the \p key is not \p optional and does not exist or if it
 * is not an object
 */
QJsonObject testAndReturnObject(const QJsonObject& obj, const QString& key,
    Optional optional = Optional::No, QJsonObject defaultValue = QJsonObject());

/**
 * Tests whether the \p index exists in the QJsonArray \p arr and if it is of the object
 * type. If the value is \p optional and does not exist, the \p defaultValue is returned.
 * If the value is not optional but does not exist or is of the wrong type, a
 * <code>std::runtime_error</code> is raised.
 * \param arr The QJsonArray from which the value is returned
 * \param index The index to inspect
 * \param optional Determines whether the \p index is optional in the array \arr
 * \param defaultValue The default value that is returned if the \p index is optional and
 * does not exist
 * \return The value if the \p index exists and is an object, if it is optional and does
 * not exist or has the wrong type, \p defaultValue is returned
 * \throw std::runtime_error If the \p index is not \p optional and does not exist or if
 * it is not an object
 */
QJsonObject testAndReturnObject(const QJsonArray& arr, int index,
    Optional optional = Optional::No, QJsonObject defaultValue = QJsonObject());

/**
 * Tests whether the \p key exists in the QJsonObject \p obj and if it is a boolean. If
 * the value is \p optional and does not exist, the \p defaultValue is returned. If the
 * value is not optional but does not exist or is of the wrong type, a
 * <code>std::runtime_error</code> is raised.
 * \param obj The QJsonObject from which the value is returned
 * \param key The key to inspect
 * \param optional Determines whether the \p key is optional in the object \obj
 * \param defaultValue The default value that is returned if the \p key is optional and
 * does not exist
 * \return The value if the \p key exists and is a string, if it is optional and does not
 * exist or has the wrong type, \p defaultValue is returned
 * \throw std::runtime_error If the \p key is not \p optional and does not exist or if it
 * is not a boolean
 */
bool testAndReturnBool(const QJsonObject& obj, const QString& key,
    Optional optional = Optional::No, bool defaultValue = true);

/**
 * Tests whether the \p index exists in the QJsonArray \p arr and if it is of the bool
 * type. If the value is \p optional and does not exist, the \p defaultValue is returned.
 * If the value is not optional but does not exist or is of the wrong type, a
 * <code>std::runtime_error</code> is raised.
 * \param arr The QJsonArray from which the value is returned
 * \param index The index to inspect
 * \param optional Determines whether the \p index is optional in the array \arr
 * \param defaultValue The default value that is returned if the \p index is optional and
 * does not exist
 * \return The value if the \p index exists and is an object, if it is optional and does
 * not exist or has the wrong type, \p defaultValue is returned
 * \throw std::runtime_error If the \p index is not \p optional and does not exist or if
 * it is not a boolean
 */
bool testAndReturnBool(const QJsonArray& arr, int index,
    Optional optional = Optional::No, bool defaultValue = true);

} // namespace common

#endif // __JSONSUPPORT_H__
