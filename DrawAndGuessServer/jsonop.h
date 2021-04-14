#ifndef JSONOP_H
#define JSONOP_H

#include <QJsonDocument>
#include <QJsonObject>
class JsonOp
{
public:
    JsonOp();
    static QJsonObject qString2Json(QString jsonString);
    static QString JsonToQstring(QJsonObject jsonObject);
};

#endif // JSONOP_H
