#include "jsonop.h"
#include "QDebug"
JsonOp::JsonOp()
{

}

QJsonObject JsonOp::qString2Json(QString jsonString)
{
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonString.toUtf8().data());
    if(jsonDocument.isNull())
    {
        qDebug()<< "String NULL"<< jsonString.toUtf8().data();
    }
    QJsonObject jsonObject = jsonDocument.object();
    return jsonObject;
}

QString JsonOp::JsonToQstring(QJsonObject jsonObject)
{
     return QString(QJsonDocument(jsonObject).toJson());
}
