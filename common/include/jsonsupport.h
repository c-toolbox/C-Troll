
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
 * Tests whether the \p key exists in the QJsonObject \p obj and if it is of the integer
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
 * is not an integer
 */
int testAndReturnInt(const QJsonObject& obj, const QString& key,
    Optional optional = Optional::No, int defaultValue = 0);

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

} // namespace common

#endif // __JSONSUPPORT_H__
