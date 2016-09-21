#ifndef __GENERICMESSAGE_H__
#define __GENERICMESSAGE_H__

#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

namespace common {

struct GenericMessage {
    /// Default constructor
    GenericMessage() = default;
    
    /**
     * Creates a GenericMessage from the passed \p document. The \p document must
     * contain a \c type field of type \c string and a \c payload field that contains
     * a JSON object.
     * \param document The QJsonDocument that contains the information about this
     * GenericMessage
     * \throws std::runtime_error If one of the required keys were not present or of 
     * the wrong type
     */
    GenericMessage(const QJsonDocument& document);
    
    /**
     * Converts the GenericMessage into a valid QJsonDocument object and returns it.
     * The JSON object will contain a \c type field of type \c string and a \c payload
     * field that contains a JSON object.
     * \return The QJsonDocument representing this GenericMessage
     */
    QJsonDocument toJson() const;
    
    /// A string representing the type of payload contained in this GenericMessage
    QString type;
    /// The payload of the message
    QJsonObject payload;
};
    
} // namespace common
    
#endif // __GENERICMESSAGE_H__
