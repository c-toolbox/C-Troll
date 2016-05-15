#ifndef __JSONSUPPORT_H__
#define __JSONSUPPORT_H__

#include <QJsonObject>

namespace json {

/**
 * Tests whether the \p key exists in the QJsonObject \p obj and if it is of the string
 * type. If it is, the value is returned, otherwise a <code>std::runtime_error</code> is
 * raised.
 * \param obj The QJsonObject from which the value is returned
 * \param key The key to inspect
 * \return The value if the \p key exists and is a string
 * \throw std::runtime_error If the \p key does not exist or if it is not a string
 */
QString testAndReturnString(const QJsonObject& obj, const QString& key);

/**
 * Tests whether the \p key exists in the QJsonObject \p obj and if it is of the integer
 * type. If it is, the value is returned, otherwise a <code>std::runtime_error</code> is
 * raised.
 * \param obj The QJsonObject from which the value is returned
 * \param key The key to inspect
 * \return The value if the \p key exists and is a integer
 * \throw std::runtime_error If the \p key does not exist or if it is not a integer
 */
int testAndReturnInt(const QJsonObject& obj, const QString& key);

/**
 * Tests whether the \p key exists in the QJsonObject \p obj and if it is of the array
 * type. If it is, the value is returned, otherwise a <code>std::runtime_error</code> is
 * raised.
 * \param obj The QJsonObject from which the value is returned
 * \param key The key to inspect
 * \return The value if the \p key exists and is a array
 * \throw std::runtime_error If the \p key does not exist or if it is not a array
 */
QJsonArray testAndReturnArray(const QJsonObject& obj, const QString& key);

/**
 * Tests whether the \p key exists in the QJsonObject \p obj and if it is of the boolean
 * type. If it is, the value is returned, otherwise a <code>std::runtime_error</code> is
 * raised.
 * \param obj The QJsonObject from which the value is returned
 * \param key The key to inspect
 * \return The value if the \p key exists and is a boolean
 * \throw std::runtime_error If the \p key does not exist or if it is not a boolean
 */
bool testAndReturnBool(const QJsonObject& obj, const QString& key);

} // namespace json

#endif // __JSONSUPPORT_H__
