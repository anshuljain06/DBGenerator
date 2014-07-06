#ifndef TABLEATTRIBUTE_H
#define TABLEATTRIBUTE_H
#include <QString>

using namespace std;

class TableAttribute
{
public:
    TableAttribute();
    void setName(QString name);
    void setClassAttributeName(QString classAttributeName);
    void setAttributeNameWithFirstUpper(QString attributeNameWithFirstUpper);
    void setDataType(QString dataType);
    void setPrimaryKey(bool primaryKey);
    void setNotNull(bool notNull);
    void setAutoInc(bool autoInc);
    QString getName();
    QString getClassAttributeName();
    QString getAttributeNameWithFirstUpper();
    QString getDataType();
    bool isPrimaryKey();
    bool isNotNull();
    bool isAutoInc();

private:
    QString name;
    QString classAttributeName;
    QString attributeNameWithFirstUpper;
    QString dataType;
    bool primaryKey;
    bool notNull;
    bool autoInc;
};

#endif // TABLEATTRIBUTE_H
