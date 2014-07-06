#include "TableAttribute.h"

TableAttribute::TableAttribute()
{
    name = "";
    dataType = "";
    primaryKey = false;
    notNull = false;
    autoInc = false;
}

void TableAttribute::setName(QString name)
{
    this->name = name;
}

void  TableAttribute::setClassAttributeName(QString classAttributeName)
{
    this->classAttributeName = classAttributeName;
}

void TableAttribute::setAttributeNameWithFirstUpper(QString attributeNameWithFirstUpper)
{
    this->attributeNameWithFirstUpper = attributeNameWithFirstUpper;
}

void TableAttribute::setDataType(QString dataType)
{
    this->dataType = dataType;
}

void TableAttribute::setPrimaryKey(bool primaryKey)
{
    this->primaryKey = primaryKey;
}

void TableAttribute::setNotNull(bool notNull)
{
    this->notNull = notNull;
}

void TableAttribute::setAutoInc(bool autoInc)
{
    this->autoInc = autoInc;
}

QString TableAttribute::getName()
{
    return this->name;
}

QString TableAttribute::getClassAttributeName()
{
    return this->classAttributeName;
}

QString TableAttribute::getAttributeNameWithFirstUpper()
{
    return this->attributeNameWithFirstUpper;
}

QString TableAttribute::getDataType()
{
    return this->dataType;
}

bool TableAttribute::isPrimaryKey()
{
    return this->primaryKey;
}

bool TableAttribute::isNotNull()
{
    return this->notNull;
}

bool TableAttribute::isAutoInc()
{
    return this->autoInc;
}
