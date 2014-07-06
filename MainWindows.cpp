#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "qfiledialog.h"
#include "qmessagebox.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowMinimizeButtonHint);
    this->setFixedSize(this->width(),this->height());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_fPlatform_activated()
{
    //Hide message when slection in combobox is changed
    this->ui->fMessageText->setText("");
}

void MainWindow::on_srcButton_clicked()
{
    //Get the file name and display it in textbox
    fileName = QFileDialog::getOpenFileName(this,tr("Open DB Generator File"), "C:\\CPP\\DBgenerator-Test-Folder\\", tr("DB Files (*.dbg)"));
    if(fileName.length() == 0)
    {
        fileName = this->ui->fFileName->text();
    }
    this->ui->fMessageText->setText("");
    this->ui->fFileName->setText(fileName);
}

void MainWindow::on_fGenerateButton_clicked()
{
    //Check if no file is selected
    if(fileName.length() == 0)
    {
        this->ui->fMessageText->setStyleSheet("QLabel {color : red;}");
        this->ui->fMessageText->setText("Error : No File Selected");
        return;
    }

    //Get table name from complete file name
    tableName = fileName;
    int lastIndexOf = fileName.lastIndexOf("/");
    tableName.remove(0,lastIndexOf + 1);
    tableName.chop(4);

    //Check if the file exists or not
    QFile file(fileName);
    if(!file.exists())
    {
        this->ui->fMessageText->setStyleSheet("QLabel {color : red;}");
        this->ui->fMessageText->setText("Error : Selected File Does Not Exists");
        return;
    }

    //Validate table name and extract class name out of it
    char charString[256];
    tableName.toStdString().copy(&charString[0],tableName.length());
    charString[tableName.length()] = '\0';

    if(charString[0] != 95 && !(charString[0] >= 65 && charString[0] <= 90) && !(charString[0] >= 97 && charString[0] <= 122))
    {
        this->ui->fMessageText->setStyleSheet("QLabel {color : red;}");
        this->ui->fMessageText->setText("Error : Invalid Table Name");
        return;
    }

    for(int x = 1; x < tableName.length(); x++)
    {
        if(charString[x] != 95 && !(charString[x] >= 65 && charString[x] <= 90) && !(charString[x] >= 97 && charString[x] <= 122) && !(charString[x] >= 48 && charString[x] <= 57))
        {
            this->ui->fMessageText->setStyleSheet("QLabel {color : red;}");
            this->ui->fMessageText->setText("Error : Invalid Table Name");
            return;
        }
    }

    for(int x = 0; x < tableName.length(); x++)
    {
        if(charString[x] == 95 && x != (tableName.length() - 1))
        {
            if(charString[x + 1] >= 97 && charString[x + 1] <= 122)
            {
                charString[x + 1] = charString[x + 1] - 32;
            }
        }
    }

    QString className = QString::fromAscii(charString);
    className.replace("_","");

    if(charString[0] >= 65 && charString[0] <= 90)
    {
        charString[0] = charString[0] + 32;
    }

    QString classNameWithFisrtLower = QString::fromAscii(charString);
    className.replace("_","");

    QString selectedPlatform = this->ui->fPlatform->currentText();

    //Make a new folder in which new files would be generated
    QString dirName = fileName.mid(0,lastIndexOf) + "/Generated Files - " + selectedPlatform;
    QDir dir(dirName);
    if(!dir.exists())
    {
        dir.mkdir(dirName);
    }

    //Read the source file
    QString fileContents = "";
    ifstream is;
    is.open(fileName.toStdString().c_str());
    while (is.good())
    {
      char c = is.get();
      if (is.good())
      {
          fileContents = fileContents + c;
      }
    }
    is.close();

    QList<QString> fileContentLines = fileContents.split("\n");

    QList<TableAttribute> tableAttributes;
    bool isPrimaryKeySet = false;

    for(int x = 0; x < fileContentLines.size() ; x++)
    {
        TableAttribute tableAttribute;

        QString lineContents = fileContentLines.at(x).trimmed();

        if(lineContents.length() == 0)
        {
            continue;
        }

        QList<QString> lineContentTokens = lineContents.split(" ");

        if(lineContentTokens.size() > 3 || lineContentTokens.size() < 2)
        {
            this->ui->fMessageText->setStyleSheet("QLabel {color : red;}");
            this->ui->fMessageText->setText("Error : File Contains Error At Line " + QString::number(x + 1));
            return;
        }

        QString attributeName = lineContentTokens.at(0).trimmed();

        if(attributeName.compare("int",Qt::CaseInsensitive) == 0 || attributeName.compare("integer",Qt::CaseInsensitive) == 0 || attributeName.compare("text",Qt::CaseInsensitive) == 0 || attributeName.compare("real",Qt::CaseInsensitive) == 0 || attributeName.compare("primary",Qt::CaseInsensitive) == 0 || attributeName.compare("key",Qt::CaseInsensitive) == 0 || attributeName.compare("auto",Qt::CaseInsensitive) == 0 || attributeName.compare("increment",Qt::CaseInsensitive) == 0 || attributeName.compare("not",Qt::CaseInsensitive) == 0 || attributeName.compare("null",Qt::CaseInsensitive) == 0)
        {
            this->ui->fMessageText->setStyleSheet("QLabel {color : red;}");
            this->ui->fMessageText->setText("Error : File Contains Error At Line " + QString::number(x + 1));
            return;
        }

        char attributeCharString[256];
        attributeName.toStdString().copy(&attributeCharString[0],attributeName.length());
        attributeCharString[attributeName.length()] = '\0';

        if(attributeCharString[0] != 95 && !(attributeCharString[0] >= 65 && attributeCharString[0] <= 90) && !(attributeCharString[0] >= 97 && attributeCharString[0] <= 122))
        {
            this->ui->fMessageText->setStyleSheet("QLabel {color : red;}");
            this->ui->fMessageText->setText("Error : File Contains Error At Line " + QString::number(x + 1));
            return;
        }

        for(int y = 1; y < attributeName.length(); y++)
        {
            if(attributeCharString[y] != 95 && !(attributeCharString[y] >= 65 && attributeCharString[y] <= 90) && !(attributeCharString[y] >= 97 && attributeCharString[y] <= 122) && !(attributeCharString[y] >= 48 && attributeCharString[y] <= 57))
            {
                this->ui->fMessageText->setStyleSheet("QLabel {color : red;}");
                this->ui->fMessageText->setText("Error : File Contains Error At Line " + QString::number(x + 1));
                return;
            }
        }

        for(int y = 0; y < attributeName.length(); y++)
        {
            if(attributeCharString[y] == 95 && y != (attributeName.length() - 1))
            {
                if(attributeCharString[y + 1] >= 97 && attributeCharString[y + 1] <= 122)
                {
                    attributeCharString[y + 1] = attributeCharString[y + 1] - 32;
                }
            }
        }

        QString classAttributeName = QString::fromAscii(attributeCharString);
        classAttributeName.replace("_","");

        if(attributeCharString[0] >= 97 && attributeCharString[0] <=122)
        {
            attributeCharString[0] = attributeCharString[0] - 32;
        }

        QString attributeNameWithFirstUpper = QString::fromAscii(attributeCharString);
        attributeNameWithFirstUpper.replace("_","");

        tableAttribute.setName(attributeName);
        tableAttribute.setClassAttributeName(classAttributeName);
        tableAttribute.setAttributeNameWithFirstUpper(attributeNameWithFirstUpper);

        QString attributeDataType = lineContentTokens.at(1).trimmed();

        if(attributeDataType.compare("int",Qt::CaseInsensitive) != 0 && attributeDataType.compare("text",Qt::CaseInsensitive) != 0 && attributeDataType.compare("real",Qt::CaseInsensitive) != 0)
        {
            this->ui->fMessageText->setStyleSheet("QLabel {color : red;}");
            this->ui->fMessageText->setText("Error : File Contains Error At Line " + QString::number(x + 1));
            return;
        }

        tableAttribute.setDataType(attributeDataType);

        if(lineContentTokens.size() == 3)
        {
            QList<QString> constraints = lineContentTokens.at(2).trimmed().split(",");
            for(int x = 0; x < constraints.size(); x++)
            {
                QString constraintName = constraints.at(x).trimmed();
                if(constraintName.compare("pk",Qt::CaseInsensitive) != 0 && constraintName.compare("ai",Qt::CaseInsensitive) != 0 && constraintName.compare("nn",Qt::CaseInsensitive) != 0)
                {
                    this->ui->fMessageText->setStyleSheet("QLabel {color : red;}");
                    this->ui->fMessageText->setText("Error : File Contains Error At Line " + QString::number(x + 1));
                    return;
                }

                if(constraintName.compare("pk",Qt::CaseInsensitive) == 0 && isPrimaryKeySet == false)
                {
                    tableAttribute.setPrimaryKey(true);
                    isPrimaryKeySet = true;
                }
                if(constraintName.compare("ai",Qt::CaseInsensitive) == 0 && attributeDataType.compare("int",Qt::CaseInsensitive) == 0)
                {
                    tableAttribute.setAutoInc(true);
                }
                if(constraintName.compare("nn",Qt::CaseInsensitive) == 0)
                {
                    tableAttribute.setNotNull(true);
                }
            }
        }

        tableAttributes.push_back(tableAttribute);
    }

    if(isPrimaryKeySet == false)
    {
        this->ui->fMessageText->setStyleSheet("QLabel {color : red;}");
        this->ui->fMessageText->setText("Error : Primary Key Not Set");
        return;
    }

    //Now we have a QList of TableAttribute in hand, next is to call appropriate method based on platform
    if(selectedPlatform.compare("iPhone") == 0)
    {
        writeiPhoneDB(tableAttributes, dirName, className, classNameWithFisrtLower, tableName);
    }
    else if(selectedPlatform.compare("Blackberry 10") == 0)
    {
        writeBlackberryDB(tableAttributes, dirName, className, classNameWithFisrtLower, tableName);
    }
    else
    {
        writeQtApplicationDB(tableAttributes,dirName,className, classNameWithFisrtLower, tableName);
    }

    //Successfully created database files
    this->ui->fMessageText->setStyleSheet("QLabel {color : green;}");
    this->ui->fMessageText->setText("Database Files Generated Successfully");
}


void MainWindow::writeiPhoneDB(QList<TableAttribute> tableAttributes, QString dirName, QString className, QString classNameWithFirstLower, QString tableName)
{
    QString pkName;
    QString pkClassAttributeName;
    QString pkAttributeNameWithFirstUpper;
    QString pkDataType;

    for(int x = 0; x< tableAttributes.size(); x++)
    {
        TableAttribute tableAttribute = tableAttributes.at(x);

        if(tableAttribute.isPrimaryKey())
        {
            pkName = tableAttribute.getName();
            pkClassAttributeName = tableAttribute.getClassAttributeName();
            pkAttributeNameWithFirstUpper = tableAttribute.getAttributeNameWithFirstUpper();
            pkDataType = tableAttribute.getDataType();
        }
    }

    QString pkActualDataType = pkDataType;
    if(pkActualDataType.compare("text",Qt::CaseInsensitive) == 0)
    {
        pkActualDataType = "NSString *";
    }
    else if(pkActualDataType.compare("real",Qt::CaseInsensitive) == 0)
    {
        pkActualDataType = "double";
    }

    //Writing DTO Header File
    QFile dtoHeaderFile(dirName + "/" + className + ".h");
    if(!dtoHeaderFile.open(QIODevice::WriteOnly))
    {
        this->ui->fMessageText->setStyleSheet("QLabel {color : red;}");
        this->ui->fMessageText->setText("Error : Unable To Write Header File");
        return;
    }
    dtoHeaderFile.write("//Generated Using DBGenerator By DevAJ \n\n");
    QString lineToBeWritten = "#import<Foundation/Foundation.h> \n@interface " + className + ":NSObject \n";
    dtoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "{ \n";
    dtoHeaderFile.write(lineToBeWritten.toStdString().c_str());

    for(int x = 0; x < tableAttributes.size(); x++)
    {
        TableAttribute tableAttribute = tableAttributes.at(x);

        if(tableAttribute.getDataType().compare("int") == 0)
        {
            lineToBeWritten = "int " + tableAttribute.getClassAttributeName() + "; \n";
        }
        else if(tableAttribute.getDataType().compare("text") == 0)
        {
            lineToBeWritten = "NSString *" + tableAttribute.getClassAttributeName() + "; \n";
        }
        else
        {
            lineToBeWritten = "double " + tableAttribute.getClassAttributeName() + "; \n";
        }
        dtoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    }

    lineToBeWritten = "} \n";
    dtoHeaderFile.write(lineToBeWritten.toStdString().c_str());

    for(int x = 0; x < tableAttributes.size(); x++)
    {
        TableAttribute tableAttribute = tableAttributes.at(x);

        if(tableAttribute.getDataType().compare("int") == 0)
        {
            lineToBeWritten = "-(void)set" + tableAttribute.getAttributeNameWithFirstUpper() + ":(int)param; \n";
        }
        else if(tableAttribute.getDataType().compare("text") == 0)
        {
            lineToBeWritten = "-(void)set" + tableAttribute.getAttributeNameWithFirstUpper() + ":(NSString *)param; \n";
        }
        else
        {
            lineToBeWritten = "-(void)set" + tableAttribute.getAttributeNameWithFirstUpper() + ":(double)param; \n";
        }
        dtoHeaderFile.write(lineToBeWritten.toStdString().c_str());

        if(tableAttribute.getDataType().compare("int") == 0)
        {
            lineToBeWritten = "-(int)get" + tableAttribute.getAttributeNameWithFirstUpper() + "; \n";
        }
        else if(tableAttribute.getDataType().compare("text") == 0)
        {
            lineToBeWritten = "-(NSString *)get" + tableAttribute.getAttributeNameWithFirstUpper() + "; \n";
        }
        else
        {
            lineToBeWritten = "-(double)get" + tableAttribute.getAttributeNameWithFirstUpper() + "; \n";
        }
        dtoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    }

    lineToBeWritten = "@end";
    dtoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    dtoHeaderFile.close();

    //Writing DTO Class File
    QFile dtoClassFile(dirName + "/" + className + ".m");
    if(!dtoClassFile.open(QIODevice::WriteOnly))
    {
        this->ui->fMessageText->setStyleSheet("QLabel {color : red;}");
        this->ui->fMessageText->setText("Error : Unable To Write Class File");
        return;
    }
    dtoClassFile.write("//Generated Using DBGenerator By DevAJ \n\n");
    lineToBeWritten = "#import \"" + className + ".h\" \n";
    dtoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "@implementation " + className + " \n";
    dtoClassFile.write(lineToBeWritten.toStdString().c_str());

    for(int x = 0; x < tableAttributes.size(); x++)
    {
        TableAttribute tableAttribute = tableAttributes.at(x);

        if(tableAttribute.getDataType().compare("int") == 0)
        {
            lineToBeWritten = "-(void)set" + tableAttribute.getAttributeNameWithFirstUpper() + ":(int)param \n";
        }
        else if(tableAttribute.getDataType().compare("text") == 0)
        {
            lineToBeWritten = "-(void)set" + tableAttribute.getAttributeNameWithFirstUpper() + ":(NString *)param \n";
        }
        else
        {
            lineToBeWritten = "-(void)set" + tableAttribute.getAttributeNameWithFirstUpper() + ":(double)param \n";
        }
        dtoClassFile.write(lineToBeWritten.toStdString().c_str());

        lineToBeWritten = "{ \n" + tableAttribute.getClassAttributeName() + " = param; \n} \n";
        dtoClassFile.write(lineToBeWritten.toStdString().c_str());

        if(tableAttribute.getDataType().compare("int") == 0)
        {
            lineToBeWritten = "-(int)get" + tableAttribute.getAttributeNameWithFirstUpper() + " \n";
        }
        else if(tableAttribute.getDataType().compare("text") == 0)
        {
            lineToBeWritten = "-(NSString *)get" + tableAttribute.getAttributeNameWithFirstUpper() + " \n";
        }
        else
        {
            lineToBeWritten = "-(double)get" + tableAttribute.getAttributeNameWithFirstUpper() + " \n";
        }
        dtoClassFile.write(lineToBeWritten.toStdString().c_str());

        lineToBeWritten = "{ \nreturn " + tableAttribute.getClassAttributeName() + "; \n} \n";
        dtoClassFile.write(lineToBeWritten.toStdString().c_str());
    }
    lineToBeWritten = "@end";
    dtoClassFile.write(lineToBeWritten.toStdString().c_str());
    dtoClassFile.close();

    //Write DAO Header File
    QFile daoHeaderFile(dirName + "/" + className + "DAO.h");
    if(!daoHeaderFile.open(QIODevice::WriteOnly))
    {
        this->ui->fMessageText->setStyleSheet("QLabel {color : red;}");
        this->ui->fMessageText->setText("Error : Unable To Write Header DAO File");
        return;
    }
    daoHeaderFile.write("//Generated Using DBGenerator By DevAJ \n\n");
    lineToBeWritten = "#import \"" + className + ".h\" \n";
    daoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "@interface " + className + "DAO:NSObject \n";
    daoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "-(id)initWithDB; \n";
    daoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "-(void)insert:(" + className + " *)" + classNameWithFirstLower + ":(BOOL)inserted:(NString *)errorMessage; \n";
    daoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "-(void)update:(" + className + " *)" + classNameWithFirstLower + ":(BOOL)updated:(NSString *)errorMessage; \n";
    daoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "-(void)remove:(" + pkActualDataType + ")" + pkClassAttributeName + ":(BOOL)removed:(NSString *)errorMessage; \n";
    daoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "-(NSMutableArray *)get:(BOOL)found:(NSString *)errorMessage; \n";
    daoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "-(BOOL)exists:(" + pkActualDataType + ")" + pkClassAttributeName + "; \n";
    daoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "@end";
    daoHeaderFile.write(lineToBeWritten.toStdString().c_str());

    daoHeaderFile.close();

    //Writing DAO Class File
    QFile daoClassFile(dirName + "/" + className + "DAO.m");
    if(!daoClassFile.open(QIODevice::WriteOnly))
    {
        this->ui->fMessageText->setStyleSheet("QLabel {color : red;}");
        this->ui->fMessageText->setText("Error : Unable To Write Class DAO File");
        return;
    }
    daoClassFile.write("//Generated Using DBGenerator By DevAJ \n\n");
    lineToBeWritten = "#import \"" + className + "DAO.h\" \n#import<sqlite3.h> \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "@implementation " + className + "DAO \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());

    lineToBeWritten = "-(id)initWithDB \n{ \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    sqlite3 *db; \n    int returnCode; \n    char *error; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    returnCode = sqlite3_open(\"" + className + ".db\",&db); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    if(returnCode==SQLITE_OK) \n    { \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        const char *sql = \"create table IF NOT EXISTS " + tableName + "(";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());

    for(int x = 0; x < tableAttributes.size(); x++)
    {
        TableAttribute tableAttribute = tableAttributes.at(x);

        lineToBeWritten = tableAttribute.getName() + " " + tableAttribute.getDataType().toUpper();
        daoClassFile.write(lineToBeWritten.toStdString().c_str());

        if(tableAttribute.isPrimaryKey())
        {
            lineToBeWritten = " PRIMARY KEY";
            daoClassFile.write(lineToBeWritten.toStdString().c_str());
        }
        if(tableAttribute.isAutoInc())
        {
            lineToBeWritten = " AUTO INCREMENT";
            daoClassFile.write(lineToBeWritten.toStdString().c_str());
        }
        if(tableAttribute.isNotNull())
        {
            lineToBeWritten = " NOT NULL";
            daoClassFile.write(lineToBeWritten.toStdString().c_str());
        }

        if(x != tableAttributes.size() - 1)
        {
            lineToBeWritten = ",";
            daoClassFile.write(lineToBeWritten.toStdString().c_str());
        }
    }

    lineToBeWritten = ")\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        sqlite3_exec(db, sql, NULL, NULL, &error); \n        sqlite3_close(db); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "} \n\n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());

    lineToBeWritten = "void " + className + "DAO::insert(" + className + " *" + classNameWithFirstLower + ", bool *inserted, QString &errorMessage) \n{ \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    if(exists(" + classNameWithFirstLower + "->get" + pkAttributeNameWithFirstUpper + "())) \n    { \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        *inserted = false; \n        errorMessage = \"" + className + " With This " + pkAttributeNameWithFirstUpper + " Already Exists\"; \n        return; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    sqlite3 *db; \n    int returnCode; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    returnCode = sqlite3_open(\"" + className + ".db\",&db); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    if(returnCode==SQLITE_OK) \n    { \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        sqlite3_stmt *statement; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        const char *sql = \"insert into " + tableName + "(";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());

    lineToBeWritten = "";
    for(int x = 0; x < tableAttributes.size(); x++)
    {
        TableAttribute tableAttribute = tableAttributes.at(x);

        if(!tableAttribute.isAutoInc())
        {
            lineToBeWritten = lineToBeWritten + tableAttribute.getName() + ",";
        }
    }

    lineToBeWritten.chop(1);
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = ") values(";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());

    lineToBeWritten = "";
    for(int x = 0; x < tableAttributes.size(); x++)
    {
        TableAttribute tableAttribute = tableAttributes.at(x);

        if(!tableAttribute.isAutoInc())
        {
            lineToBeWritten = lineToBeWritten + "?,";
        }
    }

    lineToBeWritten.chop(1);
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = ")\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        sqlite3_prepare(db, sql, strlen(sql), &statement, NULL); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());

    int count = 0;
    for(int x = 0; x < tableAttributes.size(); x++)
    {
        TableAttribute tableAttribute = tableAttributes.at(x);

        if(!tableAttribute.isAutoInc())
        {
            count++;
            if(tableAttribute.getDataType().compare("int",Qt::CaseInsensitive) == 0)
            {
                lineToBeWritten = "        sqlite3_bind_int(statement," + QString::number(count) + "," + classNameWithFirstLower + "->get" + tableAttribute.getAttributeNameWithFirstUpper() + "()); \n";
                daoClassFile.write(lineToBeWritten.toStdString().c_str());
            }
            else if(tableAttribute.getDataType().compare("text",Qt::CaseInsensitive) == 0)
            {
                lineToBeWritten = "        sqlite3_bind_text(statement," + QString::number(count) + "," + classNameWithFirstLower + "->get" + tableAttribute.getAttributeNameWithFirstUpper() + "().toStdString().c_str()," + classNameWithFirstLower + "->get" + tableAttribute.getAttributeNameWithFirstUpper() + "().length(), SQLITE_TRANSIENT); \n";
                daoClassFile.write(lineToBeWritten.toStdString().c_str());
            }
            else
            {
                lineToBeWritten = "        sqlite3_bind_double(statement," + QString::number(count) + "," + classNameWithFirstLower + "->get" + tableAttribute.getAttributeNameWithFirstUpper() + "()); \n";
                daoClassFile.write(lineToBeWritten.toStdString().c_str());
            }
        }
    }

    lineToBeWritten = "        sqlite3_step(statement); \n        sqlite3_finalize(statement); \n        sqlite3_close(db); \n        *inserted = true; \n        errorMessage = \"\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    else \n    { \n        *inserted = false; \n        errorMessage = \"Unable To Insert " + className + "\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "} \n\n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());

    lineToBeWritten = "void " + className + "DAO::update(" + className + " *" + classNameWithFirstLower + ", bool *updated, QString &errorMessage) \n{ \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    if(!exists(" + classNameWithFirstLower + "->get" + pkAttributeNameWithFirstUpper + "())) \n    { \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        *updated = false; \n        errorMessage = \"" + className + " With This " + pkAttributeNameWithFirstUpper + " Does Not Exists\"; \n        return; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    sqlite3 *db; \n    int returnCode; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    returnCode = sqlite3_open(\"" + className + ".db\",&db); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    if(returnCode==SQLITE_OK) \n    { \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        sqlite3_stmt *statement; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        const char *sql = \"update " + tableName + " set ";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());

    lineToBeWritten = "";
    for(int x = 0; x < tableAttributes.size(); x++)
    {
        TableAttribute tableAttribute = tableAttributes.at(x);

        if(!tableAttribute.isAutoInc() && !tableAttribute.isPrimaryKey())
        {
            lineToBeWritten = lineToBeWritten + tableAttribute.getName() + "=?,";
        }
    }

    lineToBeWritten.chop(1);
    lineToBeWritten = lineToBeWritten + " where " + pkName + "=?\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        sqlite3_prepare(db, sql, strlen(sql), &statement, NULL); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());

    count = 0;
    for(int x = 0; x < tableAttributes.size(); x++)
    {
        TableAttribute tableAttribute = tableAttributes.at(x);

        if(!tableAttribute.isAutoInc() && !tableAttribute.isPrimaryKey())
        {
            count++;
            if(tableAttribute.getDataType().compare("int",Qt::CaseInsensitive) == 0)
            {
                lineToBeWritten = "        sqlite3_bind_int(statement," + QString::number(count) + "," + classNameWithFirstLower + "->get" + tableAttribute.getAttributeNameWithFirstUpper() + "()); \n";
                daoClassFile.write(lineToBeWritten.toStdString().c_str());
            }
            else if(tableAttribute.getDataType().compare("text",Qt::CaseInsensitive) == 0)
            {
                lineToBeWritten = "        sqlite3_bind_text(statement," + QString::number(count) + "," + classNameWithFirstLower + "->get" + tableAttribute.getAttributeNameWithFirstUpper() + "().toStdString().c_str()," + classNameWithFirstLower + "->get" + tableAttribute.getAttributeNameWithFirstUpper() + "().length(), SQLITE_TRANSIENT); \n";
                daoClassFile.write(lineToBeWritten.toStdString().c_str());
            }
            else
            {
                lineToBeWritten = "        sqlite3_bind_double(statement," + QString::number(count) + "," + classNameWithFirstLower + "->get" + tableAttribute.getAttributeNameWithFirstUpper() + "()); \n";
                daoClassFile.write(lineToBeWritten.toStdString().c_str());
            }
        }
    }

    count++;
    if(pkDataType.compare("int",Qt::CaseInsensitive) == 0)
    {
        lineToBeWritten = "        sqlite3_bind_int(statement," + QString::number(count) + "," + classNameWithFirstLower + "->get" + pkAttributeNameWithFirstUpper + "()); \n";
        daoClassFile.write(lineToBeWritten.toStdString().c_str());
    }
    else if(pkDataType.compare("text",Qt::CaseInsensitive) == 0)
    {
        lineToBeWritten = "        sqlite3_bind_text(statement," + QString::number(count) + "," + classNameWithFirstLower + "->get" + pkAttributeNameWithFirstUpper + "().toStdString().c_str()," + classNameWithFirstLower + "->get" + pkAttributeNameWithFirstUpper + "().length(), SQLITE_TRANSIENT); \n";
        daoClassFile.write(lineToBeWritten.toStdString().c_str());
    }
    else
    {
        lineToBeWritten = "        sqlite3_bind_double(statement," + QString::number(count) + "," + classNameWithFirstLower + "->get" + pkAttributeNameWithFirstUpper + "()); \n";
        daoClassFile.write(lineToBeWritten.toStdString().c_str());
    }

    lineToBeWritten = "        sqlite3_step(statement); \n        sqlite3_finalize(statement); \n        sqlite3_close(db); \n        *updated = true; \n        errorMessage = \"\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    else \n    { \n        *updated = false; \n        errorMessage = \"Unable To Update " + className + "\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "} \n\n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());

    lineToBeWritten = "void " + className + "DAO::remove(" + pkActualDataType + " " + pkClassAttributeName + ", bool *removed, QString &errorMessage) \n{ \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    if(!exists(" + pkClassAttributeName + ")) \n    { \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        *removed = false; \n        errorMessage = \"" + className + " With This " + pkAttributeNameWithFirstUpper + " Does Not Exists\"; \n        return; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    sqlite3 *db; \n    int returnCode; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    returnCode = sqlite3_open(\"" + className + ".db\",&db); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    if(returnCode==SQLITE_OK) \n    { \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        sqlite3_stmt *statement; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        const char *sql = \"delete from " + tableName + " where " + pkName + "=?\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        sqlite3_prepare(db, sql, strlen(sql), &statement, NULL); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    if(pkDataType.compare("int",Qt::CaseInsensitive) == 0)
    {
        lineToBeWritten = "        sqlite3_bind_int(statement,1," + pkClassAttributeName + "); \n";
        daoClassFile.write(lineToBeWritten.toStdString().c_str());
    }
    else if(pkDataType.compare("text",Qt::CaseInsensitive) == 0)
    {
        lineToBeWritten = "        sqlite3_bind_text(statement,1," + pkClassAttributeName + ".toStdString().c_str()," + pkClassAttributeName + ".length(), SQLITE_TRANSIENT); \n";
        daoClassFile.write(lineToBeWritten.toStdString().c_str());
    }
    else
    {
        lineToBeWritten = "        sqlite3_bind_double(statement,1," + pkClassAttributeName + "); \n";
        daoClassFile.write(lineToBeWritten.toStdString().c_str());
    }
    lineToBeWritten = "        sqlite3_step(statement); \n        sqlite3_finalize(statement); \n        sqlite3_close(db); \n        *removed = true; \n        errorMessage = \"\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    else \n    { \n        *removed = false; \n        errorMessage = \"Unable To Remove " + className + "\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "} \n\n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());

    lineToBeWritten = "QList<" + className + ">* " + className + "DAO::get(bool *found, QString &errorMessage) \n{ \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    QList<" + className + ">* " + classNameWithFirstLower + "s; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    sqlite3 *db; \n    int returnCode; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    returnCode = sqlite3_open(\"" + className + ".db\",&db); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    if(returnCode==SQLITE_OK) \n    { \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        " + classNameWithFirstLower + "s = new QList<" + className + ">; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        sqlite3_stmt *statement; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        const char *sql = \"select * from " + tableName + "\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        sqlite3_prepare(db, sql, strlen(sql), &statement, NULL); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        returnCode = sqlite3_step(statement); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        while(returnCode == SQLITE_ROW) \n        { \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "            " + className + " " + classNameWithFirstLower + "; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());

    count = 0;
    for(int x = 0; x < tableAttributes.size(); x++)
    {
        TableAttribute tableAttribute = tableAttributes.at(x);

        lineToBeWritten = "            " + classNameWithFirstLower + ".set" + tableAttribute.getAttributeNameWithFirstUpper() + "(";
        daoClassFile.write(lineToBeWritten.toStdString().c_str());
        if(tableAttribute.getDataType().compare("int",Qt::CaseInsensitive) == 0)
        {
            lineToBeWritten = "sqlite3_column_int(statement,"+ QString::number(count) +")";
        }
        else if(tableAttribute.getDataType().compare("text",Qt::CaseInsensitive) == 0)
        {
            lineToBeWritten = "reinterpret_cast<const char *>(sqlite3_column_text(statement,"+ QString::number(count) +"))";
        }
        else
        {
            lineToBeWritten = "sqlite3_column_double(statement,"+ QString::number(count) +")";
        }
        lineToBeWritten = lineToBeWritten + "); \n";
        daoClassFile.write(lineToBeWritten.toStdString().c_str());
        count++;
    }

    lineToBeWritten = "            " + classNameWithFirstLower + "s->push_back(" + classNameWithFirstLower + "); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "            returnCode = sqlite3_step(statement); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        } \n        sqlite3_close(db); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        if(" + classNameWithFirstLower +"s->size()==0) \n        { \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "            *found = false; \n            errorMessage = \"No Records Found\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        else \n        { \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "            *found = true; \n            errorMessage = \"\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    else \n    { \n        *found = false; \n        errorMessage = \"Unable To Retrive All " + className + "s\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    return " + classNameWithFirstLower + "s; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "} \n\n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());

    lineToBeWritten = "bool " + className + "DAO::exists(" + pkActualDataType + " " + pkClassAttributeName + ") \n{ \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    bool exists = false; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    sqlite3 *db; \n    int returnCode; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    returnCode = sqlite3_open(\"" + className + ".db\",&db); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    if(returnCode==SQLITE_OK) \n    { \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        sqlite3_stmt *statement; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        const char *sql = \"select * " + tableName + " where " + pkName + "=?\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        sqlite3_prepare(db, sql, strlen(sql), &statement, NULL); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    if(pkDataType.compare("int",Qt::CaseInsensitive) == 0)
    {
        lineToBeWritten = "        sqlite3_bind_int(statement,1," + pkClassAttributeName + "); \n";
        daoClassFile.write(lineToBeWritten.toStdString().c_str());
    }
    else if(pkDataType.compare("text",Qt::CaseInsensitive) == 0)
    {
        lineToBeWritten = "        sqlite3_bind_text(statement,1," + pkClassAttributeName + ".toStdString().c_str()," + pkClassAttributeName + ".length(), SQLITE_TRANSIENT); \n";
        daoClassFile.write(lineToBeWritten.toStdString().c_str());
    }
    else
    {
        lineToBeWritten = "        sqlite3_bind_double(statement,1," + pkClassAttributeName + "); \n";
        daoClassFile.write(lineToBeWritten.toStdString().c_str());
    }
    lineToBeWritten = "        returnCode = sqlite3_step(statement); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        if(returnCode == SQLITE_ROW) \n        { \n";
    lineToBeWritten = lineToBeWritten + "            exists = true; \n        } \n";
    lineToBeWritten = lineToBeWritten + "        else \n        { \n";
    lineToBeWritten = lineToBeWritten + "            exists = false; \n        } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        sqlite3_close(db); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    return exists; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "} \n\n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "@end";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    daoClassFile.close();
}

void MainWindow::writeBlackberryDB(QList<TableAttribute> tableAttributes, QString dirName, QString className, QString classNameWithFirstLower, QString tableName)
{
    QString pkName;
    QString pkClassAttributeName;
    QString pkAttributeNameWithFirstUpper;
    QString pkDataType;

    for(int x = 0; x< tableAttributes.size(); x++)
    {
        TableAttribute tableAttribute = tableAttributes.at(x);

        if(tableAttribute.isPrimaryKey())
        {
            pkName = tableAttribute.getName();
            pkClassAttributeName = tableAttribute.getClassAttributeName();
            pkAttributeNameWithFirstUpper = tableAttribute.getAttributeNameWithFirstUpper();
            pkDataType = tableAttribute.getDataType();
        }
    }

QString pkActualDataType = pkDataType;
    if(pkActualDataType.compare("text",Qt::CaseInsensitive) == 0)
    {
        pkActualDataType = "string";
    }
    else if(pkActualDataType.compare("real",Qt::CaseInsensitive) == 0)
    {
        pkActualDataType = "double";
    }

    //Writing DTO Header File
    QFile dtoHeaderFile(dirName + "/" + className + ".hpp");
    if(!dtoHeaderFile.open(QIODevice::WriteOnly))
    {
        this->ui->fMessageText->setStyleSheet("QLabel {color : red;}");
        this->ui->fMessageText->setText("Error : Unable To Write Header File");
        return;
    }
    dtoHeaderFile.write("//Generated Using DBGenerator By DevAJ \n\n");
    QString lineToBeWritten = "#ifndef " + className.toUpper() + "_HPP \n#define " + className.toUpper() + "_HPP \n#include<string> \n\nusing namespace std; \n\n";
    dtoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "class " + className + " \n{ \n";
    dtoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "private: \n";
    dtoHeaderFile.write(lineToBeWritten.toStdString().c_str());

    for(int x = 0; x < tableAttributes.size(); x++)
    {
        TableAttribute tableAttribute = tableAttributes.at(x);

        if(tableAttribute.getDataType().compare("int") == 0)
        {
            lineToBeWritten = "    int " + tableAttribute.getClassAttributeName() + "; \n";
        }
        else if(tableAttribute.getDataType().compare("text") == 0)
        {
            lineToBeWritten = "    string " + tableAttribute.getClassAttributeName() + "; \n";
        }
        else
        {
            lineToBeWritten = "    double " + tableAttribute.getClassAttributeName() + "; \n";
        }
        dtoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    }

    lineToBeWritten = "\npublic: \n";
    dtoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    " + className + "(); \n";
    dtoHeaderFile.write(lineToBeWritten.toStdString().c_str());

    for(int x = 0; x < tableAttributes.size(); x++)
    {
        TableAttribute tableAttribute = tableAttributes.at(x);

        if(tableAttribute.getDataType().compare("int") == 0)
        {
            lineToBeWritten = "    void set" + tableAttribute.getAttributeNameWithFirstUpper() + "(int " + tableAttribute.getClassAttributeName() +"); \n";
        }
        else if(tableAttribute.getDataType().compare("text") == 0)
        {
            lineToBeWritten = "    void set" + tableAttribute.getAttributeNameWithFirstUpper() + "(string " + tableAttribute.getClassAttributeName() +"); \n";
        }
        else
        {
            lineToBeWritten = "    void set" + tableAttribute.getAttributeNameWithFirstUpper() + "(double " + tableAttribute.getClassAttributeName() +"); \n";
        }
        dtoHeaderFile.write(lineToBeWritten.toStdString().c_str());

        if(tableAttribute.getDataType().compare("int") == 0)
        {
            lineToBeWritten = "    int get" + tableAttribute.getAttributeNameWithFirstUpper() + "(); \n";
        }
        else if(tableAttribute.getDataType().compare("text") == 0)
        {
            lineToBeWritten = "    string get" + tableAttribute.getAttributeNameWithFirstUpper() + "(); \n";
        }
        else
        {
            lineToBeWritten = "    double get" + tableAttribute.getAttributeNameWithFirstUpper() + "(); \n";
        }
        dtoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    }

    lineToBeWritten = "    virtual ~" + className + "(){} \n";
    dtoHeaderFile.write(lineToBeWritten.toStdString().c_str());

    lineToBeWritten = "}; \n#endif // "+ className.toUpper() + "_HPP";
    dtoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    dtoHeaderFile.close();

    //Writing DTO Class File
    QFile dtoClassFile(dirName + "/" + className + ".cpp");
    if(!dtoClassFile.open(QIODevice::WriteOnly))
    {
        this->ui->fMessageText->setStyleSheet("QLabel {color : red;}");
        this->ui->fMessageText->setText("Error : Unable To Write Class File");
        return;
    }
    dtoClassFile.write("//Generated Using DBGenerator By DevAJ \n\n");
    lineToBeWritten = "#include \"" + className + ".hpp\" \n\n";
    dtoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = className + "::" + className + "() \n{ \n} \n\n";
    dtoClassFile.write(lineToBeWritten.toStdString().c_str());

    for(int x = 0; x < tableAttributes.size(); x++)
    {
        TableAttribute tableAttribute = tableAttributes.at(x);

        if(tableAttribute.getDataType().compare("int") == 0)
        {
            lineToBeWritten = "void " + className + "::set" + tableAttribute.getAttributeNameWithFirstUpper() + "(int " + tableAttribute.getClassAttributeName() +") \n";
        }
        else if(tableAttribute.getDataType().compare("text") == 0)
        {
            lineToBeWritten = "void " + className + "::set" + tableAttribute.getAttributeNameWithFirstUpper() + "(string " + tableAttribute.getClassAttributeName() +") \n";
        }
        else
        {
            lineToBeWritten = "void " + className + "::set" + tableAttribute.getAttributeNameWithFirstUpper() + "(double " + tableAttribute.getClassAttributeName() +") \n";
        }
        dtoClassFile.write(lineToBeWritten.toStdString().c_str());

        lineToBeWritten = "{ \n    this->" + tableAttribute.getClassAttributeName() + " = " + tableAttribute.getClassAttributeName() + "; \n} \n\n";
        dtoClassFile.write(lineToBeWritten.toStdString().c_str());

        if(tableAttribute.getDataType().compare("int") == 0)
        {
            lineToBeWritten = "int " + className + "::get" + tableAttribute.getAttributeNameWithFirstUpper() + "() \n";
        }
        else if(tableAttribute.getDataType().compare("text") == 0)
        {
            lineToBeWritten = "string " + className + "::get" + tableAttribute.getAttributeNameWithFirstUpper() + "() \n";
        }
        else
        {
            lineToBeWritten = "double " + className + "::get" + tableAttribute.getAttributeNameWithFirstUpper() + "() \n";
        }
        dtoClassFile.write(lineToBeWritten.toStdString().c_str());

        lineToBeWritten = "{ \n    return this->" + tableAttribute.getClassAttributeName() + "; \n} \n\n";
        dtoClassFile.write(lineToBeWritten.toStdString().c_str());
    }
    dtoClassFile.close();

    //Write DAO Header File
    QFile daoHeaderFile(dirName + "/" + className + "DAO.hpp");
    if(!daoHeaderFile.open(QIODevice::WriteOnly))
    {
        this->ui->fMessageText->setStyleSheet("QLabel {color : red;}");
        this->ui->fMessageText->setText("Error : Unable To Write Header DAO File");
        return;
    }
    daoHeaderFile.write("//Generated Using DBGenerator By DevAJ \n\n");
    lineToBeWritten = "#ifndef " + className.toUpper() + "DAO_HPP \n#define " + className.toUpper() + "DAO_HPP \n#include \"" + className + ".hpp\" \n#include<string> \n#include<vector> \n\nusing namespace std; \n\n";
    daoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "class " + className + "DAO \n{ \n";
    daoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "public: \n";
    daoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    " + className + "DAO(); \n";
    daoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    void insert(" + className + " *" + classNameWithFirstLower + ", bool *inserted, string &errorMessage); \n";
    daoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    void update(" + className + " *" + classNameWithFirstLower + ", bool *updated, string &errorMessage); \n";
    daoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    void remove(" + pkActualDataType + " " + pkClassAttributeName + ", bool *removed, string &errorMessage); \n";
    daoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    vector<" + className + ">* get(bool *found, string &errorMessage); \n";
    daoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    bool exists(" + pkActualDataType + " " + pkClassAttributeName + "); \n";
    daoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "}; \n#endif // "+ className.toUpper() + "DAO_HPP";
    daoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    daoHeaderFile.close();

    //Writing DAO Class File
    QFile daoClassFile(dirName + "/" + className + "DAO.cpp");
    if(!daoClassFile.open(QIODevice::WriteOnly))
    {
        this->ui->fMessageText->setStyleSheet("QLabel {color : red;}");
        this->ui->fMessageText->setText("Error : Unable To Write Class DAO File");
        return;
    }
    daoClassFile.write("//Generated Using DBGenerator By DevAJ \n\n");
    lineToBeWritten = "#include \"" + className + "DAO.hpp\" \n#include<sqlite3.h> \n#include<string.h> \n\n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = className + "DAO::" + className + "DAO() \n{ \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    sqlite3 *db; \n    int returnCode; \n    char *error; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    returnCode = sqlite3_open(\"" + className + ".db\",&db); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    if(returnCode==SQLITE_OK) \n    { \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        const char *sql = \"create table IF NOT EXISTS " + tableName + "(";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());

    for(int x = 0; x < tableAttributes.size(); x++)
    {
        TableAttribute tableAttribute = tableAttributes.at(x);

        lineToBeWritten = tableAttribute.getName() + " " + tableAttribute.getDataType().toUpper();
        daoClassFile.write(lineToBeWritten.toStdString().c_str());

        if(tableAttribute.isPrimaryKey())
        {
            lineToBeWritten = " PRIMARY KEY";
            daoClassFile.write(lineToBeWritten.toStdString().c_str());
        }
        if(tableAttribute.isAutoInc())
        {
            lineToBeWritten = " AUTO INCREMENT";
            daoClassFile.write(lineToBeWritten.toStdString().c_str());
        }
        if(tableAttribute.isNotNull())
        {
            lineToBeWritten = " NOT NULL";
            daoClassFile.write(lineToBeWritten.toStdString().c_str());
        }

        if(x != tableAttributes.size() - 1)
        {
            lineToBeWritten = ",";
            daoClassFile.write(lineToBeWritten.toStdString().c_str());
        }
    }

    lineToBeWritten = ")\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        sqlite3_exec(db, sql, NULL, NULL, &error); \n        sqlite3_close(db); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "} \n\n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());

    lineToBeWritten = "void " + className + "DAO::insert(" + className + " *" + classNameWithFirstLower + ", bool *inserted, string &errorMessage) \n{ \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    if(exists(" + classNameWithFirstLower + "->get" + pkAttributeNameWithFirstUpper + "())) \n    { \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        *inserted = false; \n        errorMessage = \"" + className + " With This " + pkAttributeNameWithFirstUpper + " Already Exists\"; \n        return; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    sqlite3 *db; \n    int returnCode; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    returnCode = sqlite3_open(\"" + className + ".db\",&db); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    if(returnCode==SQLITE_OK) \n    { \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        sqlite3_stmt *statement; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        const char *sql = \"insert into " + tableName + "(";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());

    lineToBeWritten = "";
    for(int x = 0; x < tableAttributes.size(); x++)
    {
        TableAttribute tableAttribute = tableAttributes.at(x);

        if(!tableAttribute.isAutoInc())
        {
            lineToBeWritten = lineToBeWritten + tableAttribute.getName() + ",";
        }
    }

    lineToBeWritten.chop(1);
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = ") values(";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());

    lineToBeWritten = "";
    for(int x = 0; x < tableAttributes.size(); x++)
    {
        TableAttribute tableAttribute = tableAttributes.at(x);

        if(!tableAttribute.isAutoInc())
        {
            lineToBeWritten = lineToBeWritten + "?,";
        }
    }

    lineToBeWritten.chop(1);
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = ")\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        sqlite3_prepare(db, sql, strlen(sql), &statement, NULL); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());

    int count = 0;
    for(int x = 0; x < tableAttributes.size(); x++)
    {
        TableAttribute tableAttribute = tableAttributes.at(x);

        if(!tableAttribute.isAutoInc())
        {
            count++;
            if(tableAttribute.getDataType().compare("int",Qt::CaseInsensitive) == 0)
            {
                lineToBeWritten = "        sqlite3_bind_int(statement," + QString::number(count) + "," + classNameWithFirstLower + "->get" + tableAttribute.getAttributeNameWithFirstUpper() + "()); \n";
                daoClassFile.write(lineToBeWritten.toStdString().c_str());
            }
            else if(tableAttribute.getDataType().compare("text",Qt::CaseInsensitive) == 0)
            {
                lineToBeWritten = "        sqlite3_bind_text(statement," + QString::number(count) + "," + classNameWithFirstLower + "->get" + tableAttribute.getAttributeNameWithFirstUpper() + "().c_str()," + classNameWithFirstLower + "->get" + tableAttribute.getAttributeNameWithFirstUpper() + "().length(), SQLITE_TRANSIENT); \n";
                daoClassFile.write(lineToBeWritten.toStdString().c_str());
            }
            else
            {
                lineToBeWritten = "        sqlite3_bind_double(statement," + QString::number(count) + "," + classNameWithFirstLower + "->get" + tableAttribute.getAttributeNameWithFirstUpper() + "()); \n";
                daoClassFile.write(lineToBeWritten.toStdString().c_str());
            }
        }
    }

    lineToBeWritten = "        sqlite3_step(statement); \n        sqlite3_finalize(statement); \n        sqlite3_close(db); \n        *inserted = true; \n        errorMessage = \"\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    else \n    { \n        *inserted = false; \n        errorMessage = \"Unable To Insert " + className + "\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "} \n\n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());

    lineToBeWritten = "void " + className + "DAO::update(" + className + " *" + classNameWithFirstLower + ", bool *updated, string &errorMessage) \n{ \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    if(!exists(" + classNameWithFirstLower + "->get" + pkAttributeNameWithFirstUpper + "())) \n    { \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        *updated = false; \n        errorMessage = \"" + className + " With This " + pkAttributeNameWithFirstUpper + " Does Not Exists\"; \n        return; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    sqlite3 *db; \n    int returnCode; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    returnCode = sqlite3_open(\"" + className + ".db\",&db); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    if(returnCode==SQLITE_OK) \n    { \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        sqlite3_stmt *statement; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        const char *sql = \"update " + tableName + " set ";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());

    lineToBeWritten = "";
    for(int x = 0; x < tableAttributes.size(); x++)
    {
        TableAttribute tableAttribute = tableAttributes.at(x);

        if(!tableAttribute.isAutoInc() && !tableAttribute.isPrimaryKey())
        {
            lineToBeWritten = lineToBeWritten + tableAttribute.getName() + "=?,";
        }
    }

    lineToBeWritten.chop(1);
    lineToBeWritten = lineToBeWritten + " where " + pkName + "=?\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        sqlite3_prepare(db, sql, strlen(sql), &statement, NULL); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());

    count = 0;
    for(int x = 0; x < tableAttributes.size(); x++)
    {
        TableAttribute tableAttribute = tableAttributes.at(x);

        if(!tableAttribute.isAutoInc() && !tableAttribute.isPrimaryKey())
        {
            count++;
            if(tableAttribute.getDataType().compare("int",Qt::CaseInsensitive) == 0)
            {
                lineToBeWritten = "        sqlite3_bind_int(statement," + QString::number(count) + "," + classNameWithFirstLower + "->get" + tableAttribute.getAttributeNameWithFirstUpper() + "()); \n";
                daoClassFile.write(lineToBeWritten.toStdString().c_str());
            }
            else if(tableAttribute.getDataType().compare("text",Qt::CaseInsensitive) == 0)
            {
                lineToBeWritten = "        sqlite3_bind_text(statement," + QString::number(count) + "," + classNameWithFirstLower + "->get" + tableAttribute.getAttributeNameWithFirstUpper() + "().c_str()," + classNameWithFirstLower + "->get" + tableAttribute.getAttributeNameWithFirstUpper() + "().length(), SQLITE_TRANSIENT); \n";
                daoClassFile.write(lineToBeWritten.toStdString().c_str());
            }
            else
            {
                lineToBeWritten = "        sqlite3_bind_double(statement," + QString::number(count) + "," + classNameWithFirstLower + "->get" + tableAttribute.getAttributeNameWithFirstUpper() + "()); \n";
                daoClassFile.write(lineToBeWritten.toStdString().c_str());
            }
        }
    }

    count++;
    if(pkDataType.compare("int",Qt::CaseInsensitive) == 0)
    {
        lineToBeWritten = "        sqlite3_bind_int(statement," + QString::number(count) + "," + classNameWithFirstLower + "->get" + pkAttributeNameWithFirstUpper + "()); \n";
        daoClassFile.write(lineToBeWritten.toStdString().c_str());
    }
    else if(pkDataType.compare("text",Qt::CaseInsensitive) == 0)
    {
        lineToBeWritten = "        sqlite3_bind_text(statement," + QString::number(count) + "," + classNameWithFirstLower + "->get" + pkAttributeNameWithFirstUpper + "().c_str()," + classNameWithFirstLower + "->get" + pkAttributeNameWithFirstUpper + "().length(), SQLITE_TRANSIENT); \n";
        daoClassFile.write(lineToBeWritten.toStdString().c_str());
    }
    else
    {
        lineToBeWritten = "        sqlite3_bind_double(statement," + QString::number(count) + "," + classNameWithFirstLower + "->get" + pkAttributeNameWithFirstUpper + "()); \n";
        daoClassFile.write(lineToBeWritten.toStdString().c_str());
    }

    lineToBeWritten = "        sqlite3_step(statement); \n        sqlite3_finalize(statement); \n        sqlite3_close(db); \n        *updated = true; \n        errorMessage = \"\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    else \n    { \n        *updated = false; \n        errorMessage = \"Unable To Update " + className + "\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "} \n\n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());

    lineToBeWritten = "void " + className + "DAO::remove(" + pkActualDataType + " " + pkClassAttributeName + ", bool *removed, string &errorMessage) \n{ \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    if(!exists(" + pkClassAttributeName + ")) \n    { \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        *removed = false; \n        errorMessage = \"" + className + " With This " + pkAttributeNameWithFirstUpper + " Does Not Exists\"; \n        return; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    sqlite3 *db; \n    int returnCode; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    returnCode = sqlite3_open(\"" + className + ".db\",&db); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    if(returnCode==SQLITE_OK) \n    { \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        sqlite3_stmt *statement; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        const char *sql = \"delete from " + tableName + " where " + pkName + "=?\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        sqlite3_prepare(db, sql, strlen(sql), &statement, NULL); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    if(pkDataType.compare("int",Qt::CaseInsensitive) == 0)
    {
        lineToBeWritten = "        sqlite3_bind_int(statement,1," + pkClassAttributeName + "); \n";
        daoClassFile.write(lineToBeWritten.toStdString().c_str());
    }
    else if(pkDataType.compare("text",Qt::CaseInsensitive) == 0)
    {
        lineToBeWritten = "        sqlite3_bind_text(statement,1," + pkClassAttributeName + ".c_str()," + pkClassAttributeName + ".length(), SQLITE_TRANSIENT); \n";
        daoClassFile.write(lineToBeWritten.toStdString().c_str());
    }
    else
    {
        lineToBeWritten = "        sqlite3_bind_double(statement,1," + pkClassAttributeName + "); \n";
        daoClassFile.write(lineToBeWritten.toStdString().c_str());
    }
    lineToBeWritten = "        sqlite3_step(statement); \n        sqlite3_finalize(statement); \n        sqlite3_close(db); \n        *removed = true; \n        errorMessage = \"\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    else \n    { \n        *removed = false; \n        errorMessage = \"Unable To Remove " + className + "\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "} \n\n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());

    lineToBeWritten = "vector<" + className + ">* " + className + "DAO::get(bool *found, string &errorMessage) \n{ \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    vector<" + className + ">* " + classNameWithFirstLower + "s; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    sqlite3 *db; \n    int returnCode; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    returnCode = sqlite3_open(\"" + className + ".db\",&db); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    if(returnCode==SQLITE_OK) \n    { \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        " + classNameWithFirstLower + "s = new vector<" + className + ">; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        sqlite3_stmt *statement; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        const char *sql = \"select * from " + tableName + "\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        sqlite3_prepare(db, sql, strlen(sql), &statement, NULL); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        returnCode = sqlite3_step(statement); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        while(returnCode == SQLITE_ROW) \n        { \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "            " + className + " " + classNameWithFirstLower + "; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());

    count = 0;
    for(int x = 0; x < tableAttributes.size(); x++)
    {
        TableAttribute tableAttribute = tableAttributes.at(x);

        lineToBeWritten = "            " + classNameWithFirstLower + ".set" + tableAttribute.getAttributeNameWithFirstUpper() + "(";
        daoClassFile.write(lineToBeWritten.toStdString().c_str());
        if(tableAttribute.getDataType().compare("int",Qt::CaseInsensitive) == 0)
        {
            lineToBeWritten = "sqlite3_column_int(statement,"+ QString::number(count) +")";
        }
        else if(tableAttribute.getDataType().compare("text",Qt::CaseInsensitive) == 0)
        {
            lineToBeWritten = "std::string(reinterpret_cast<const char *>(sqlite3_column_text(statement,"+ QString::number(count) +")))";
        }
        else
        {
            lineToBeWritten = "sqlite3_column_double(statement,"+ QString::number(count) +")";
        }
        lineToBeWritten = lineToBeWritten + "); \n";
        daoClassFile.write(lineToBeWritten.toStdString().c_str());
        count++;
    }

    lineToBeWritten = "            " + classNameWithFirstLower + "s->push_back(" + classNameWithFirstLower + "); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "            returnCode = sqlite3_step(statement); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        } \n        sqlite3_close(db); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        if(" + classNameWithFirstLower +"s->size()==0) \n        { \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "            *found = false; \n            errorMessage = \"No Records Found\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        else \n        { \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "            *found = true; \n            errorMessage = \"\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    else \n    { \n        *found = false; \n        errorMessage = \"Unable To Retrive All " + className + "s\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    return " + classNameWithFirstLower + "s; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "} \n\n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());

    lineToBeWritten = "bool " + className + "DAO::exists(" + pkActualDataType + " " + pkClassAttributeName + ") \n{ \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    bool exists = false; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    sqlite3 *db; \n    int returnCode; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    returnCode = sqlite3_open(\"" + className + ".db\",&db); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    if(returnCode==SQLITE_OK) \n    { \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        sqlite3_stmt *statement; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        const char *sql = \"select * " + tableName + " where " + pkName + "=?\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        sqlite3_prepare(db, sql, strlen(sql), &statement, NULL); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    if(pkDataType.compare("int",Qt::CaseInsensitive) == 0)
    {
        lineToBeWritten = "        sqlite3_bind_int(statement,1," + pkClassAttributeName + "); \n";
        daoClassFile.write(lineToBeWritten.toStdString().c_str());
    }
    else if(pkDataType.compare("text",Qt::CaseInsensitive) == 0)
    {
        lineToBeWritten = "        sqlite3_bind_text(statement,1," + pkClassAttributeName + ".c_str()," + pkClassAttributeName + ".length(), SQLITE_TRANSIENT); \n";
        daoClassFile.write(lineToBeWritten.toStdString().c_str());
    }
    else
    {
        lineToBeWritten = "        sqlite3_bind_double(statement,1," + pkClassAttributeName + "); \n";
        daoClassFile.write(lineToBeWritten.toStdString().c_str());
    }
    lineToBeWritten = "        returnCode = sqlite3_step(statement); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        if(returnCode == SQLITE_ROW) \n        { \n";
    lineToBeWritten = lineToBeWritten + "            exists = true; \n        } \n";
    lineToBeWritten = lineToBeWritten + "        else \n        { \n";
    lineToBeWritten = lineToBeWritten + "            exists = false; \n        } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        sqlite3_close(db); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    return exists; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "} \n\n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    daoClassFile.close();
}

void MainWindow::writeQtApplicationDB(QList<TableAttribute> tableAttributes, QString dirName, QString className, QString classNameWithFirstLower, QString tableName)
{
    QString pkName;
    QString pkClassAttributeName;
    QString pkAttributeNameWithFirstUpper;
    QString pkDataType;

    for(int x = 0; x< tableAttributes.size(); x++)
    {
        TableAttribute tableAttribute = tableAttributes.at(x);

        if(tableAttribute.isPrimaryKey())
        {
            pkName = tableAttribute.getName();
            pkClassAttributeName = tableAttribute.getClassAttributeName();
            pkAttributeNameWithFirstUpper = tableAttribute.getAttributeNameWithFirstUpper();
            pkDataType = tableAttribute.getDataType();
        }
    }

    QString pkActualDataType = pkDataType;
    if(pkActualDataType.compare("text",Qt::CaseInsensitive) == 0)
    {
        pkActualDataType = "QString";
    }
    else if(pkActualDataType.compare("real",Qt::CaseInsensitive) == 0)
    {
        pkActualDataType = "double";
    }

    //Writing DTO Header File
    QFile dtoHeaderFile(dirName + "/" + className + ".h");
    if(!dtoHeaderFile.open(QIODevice::WriteOnly))
    {
        this->ui->fMessageText->setStyleSheet("QLabel {color : red;}");
        this->ui->fMessageText->setText("Error : Unable To Write Header File");
        return;
    }
    dtoHeaderFile.write("//Generated Using DBGenerator By DevAJ \n\n");
    QString lineToBeWritten = "#ifndef " + className.toUpper() + "_H \n#define " + className.toUpper() + "_H \n#include<QString> \n\nusing namespace std; \n\n";
    dtoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "class " + className + " \n{ \n";
    dtoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "private: \n";
    dtoHeaderFile.write(lineToBeWritten.toStdString().c_str());

    for(int x = 0; x < tableAttributes.size(); x++)
    {
        TableAttribute tableAttribute = tableAttributes.at(x);

        if(tableAttribute.getDataType().compare("int") == 0)
        {
            lineToBeWritten = "    int " + tableAttribute.getClassAttributeName() + "; \n";
        }
        else if(tableAttribute.getDataType().compare("text") == 0)
        {
            lineToBeWritten = "    QString " + tableAttribute.getClassAttributeName() + "; \n";
        }
        else
        {
            lineToBeWritten = "    double " + tableAttribute.getClassAttributeName() + "; \n";
        }
        dtoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    }

    lineToBeWritten = "\npublic: \n";
    dtoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    " + className + "(); \n";
    dtoHeaderFile.write(lineToBeWritten.toStdString().c_str());

    for(int x = 0; x < tableAttributes.size(); x++)
    {
        TableAttribute tableAttribute = tableAttributes.at(x);

        if(tableAttribute.getDataType().compare("int") == 0)
        {
            lineToBeWritten = "    void set" + tableAttribute.getAttributeNameWithFirstUpper() + "(int " + tableAttribute.getClassAttributeName() +"); \n";
        }
        else if(tableAttribute.getDataType().compare("text") == 0)
        {
            lineToBeWritten = "    void set" + tableAttribute.getAttributeNameWithFirstUpper() + "(QString " + tableAttribute.getClassAttributeName() +"); \n";
        }
        else
        {
            lineToBeWritten = "    void set" + tableAttribute.getAttributeNameWithFirstUpper() + "(double " + tableAttribute.getClassAttributeName() +"); \n";
        }
        dtoHeaderFile.write(lineToBeWritten.toStdString().c_str());

        if(tableAttribute.getDataType().compare("int") == 0)
        {
            lineToBeWritten = "    int get" + tableAttribute.getAttributeNameWithFirstUpper() + "(); \n";
        }
        else if(tableAttribute.getDataType().compare("text") == 0)
        {
            lineToBeWritten = "    QString get" + tableAttribute.getAttributeNameWithFirstUpper() + "(); \n";
        }
        else
        {
            lineToBeWritten = "    double get" + tableAttribute.getAttributeNameWithFirstUpper() + "(); \n";
        }
        dtoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    }

    lineToBeWritten = "    virtual ~" + className + "(){} \n";
    dtoHeaderFile.write(lineToBeWritten.toStdString().c_str());

    lineToBeWritten = "}; \n#endif // "+ className.toUpper() + "_H";
    dtoHeaderFile.write(lineToBeWritten.toStdString().c_str());

    dtoHeaderFile.close();

    //Writing DTO Class File
    QFile dtoClassFile(dirName + "/" + className + ".cpp");
    if(!dtoClassFile.open(QIODevice::WriteOnly))
    {
        this->ui->fMessageText->setStyleSheet("QLabel {color : red;}");
        this->ui->fMessageText->setText("Error : Unable To Write Class File");
        return;
    }
    dtoClassFile.write("//Generated Using DBGenerator By DevAJ \n\n");
    lineToBeWritten = "#include \"" + className + ".h\" \n\n";
    dtoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = className + "::" + className + "() \n{ \n} \n\n";
    dtoClassFile.write(lineToBeWritten.toStdString().c_str());

    for(int x = 0; x < tableAttributes.size(); x++)
    {
        TableAttribute tableAttribute = tableAttributes.at(x);

        if(tableAttribute.getDataType().compare("int") == 0)
        {
            lineToBeWritten = "void " + className + "::set" + tableAttribute.getAttributeNameWithFirstUpper() + "(int " + tableAttribute.getClassAttributeName() +") \n";
        }
        else if(tableAttribute.getDataType().compare("text") == 0)
        {
            lineToBeWritten = "void " + className + "::set" + tableAttribute.getAttributeNameWithFirstUpper() + "(QString " + tableAttribute.getClassAttributeName() +") \n";
        }
        else
        {
            lineToBeWritten = "void " + className + "::set" + tableAttribute.getAttributeNameWithFirstUpper() + "(double " + tableAttribute.getClassAttributeName() +") \n";
        }
        dtoClassFile.write(lineToBeWritten.toStdString().c_str());

        lineToBeWritten = "{ \n    this->" + tableAttribute.getClassAttributeName() + " = " + tableAttribute.getClassAttributeName() + "; \n} \n\n";
        dtoClassFile.write(lineToBeWritten.toStdString().c_str());

        if(tableAttribute.getDataType().compare("int") == 0)
        {
            lineToBeWritten = "int " + className + "::get" + tableAttribute.getAttributeNameWithFirstUpper() + "() \n";
        }
        else if(tableAttribute.getDataType().compare("text") == 0)
        {
            lineToBeWritten = "QString " + className + "::get" + tableAttribute.getAttributeNameWithFirstUpper() + "() \n";
        }
        else
        {
            lineToBeWritten = "double " + className + "::get" + tableAttribute.getAttributeNameWithFirstUpper() + "() \n";
        }
        dtoClassFile.write(lineToBeWritten.toStdString().c_str());

        lineToBeWritten = "{ \n    return this->" + tableAttribute.getClassAttributeName() + "; \n} \n\n";
        dtoClassFile.write(lineToBeWritten.toStdString().c_str());
    }

    dtoClassFile.close();

    //Write DAO Header File
    QFile daoHeaderFile(dirName + "/" + className + "DAO.h");
    if(!daoHeaderFile.open(QIODevice::WriteOnly))
    {
        this->ui->fMessageText->setStyleSheet("QLabel {color : red;}");
        this->ui->fMessageText->setText("Error : Unable To Write Header DAO File");
        return;
    }    
    daoHeaderFile.write("//Generated Using DBGenerator By DevAJ \n\n");
    lineToBeWritten = "#ifndef " + className.toUpper() + "DAO_H \n#define " + className.toUpper() + "DAO_H \n#include \"" + className + ".h\" \n#include<QString> \n#include<QList> \n\nusing namespace std; \n\n";
    daoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "class " + className + "DAO \n{ \n";
    daoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "public: \n";
    daoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    " + className + "DAO(); \n";
    daoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    void insert(" + className + " *" + classNameWithFirstLower + ", bool *inserted, QString &errorMessage); \n";
    daoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    void update(" + className + " *" + classNameWithFirstLower + ", bool *updated, QString &errorMessage); \n";
    daoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    void remove(" + pkActualDataType + " " + pkClassAttributeName + ", bool *removed, QString &errorMessage); \n";
    daoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    QList<" + className + ">* get(bool *found, QString &errorMessage); \n";
    daoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    bool exists(" + pkActualDataType + " " + pkClassAttributeName + "); \n";
    daoHeaderFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "}; \n#endif // "+ className.toUpper() + "DAO_H";
    daoHeaderFile.write(lineToBeWritten.toStdString().c_str());

    daoHeaderFile.close();

    //Writing DAO Class File
    QFile daoClassFile(dirName + "/" + className + "DAO.cpp");
    if(!daoClassFile.open(QIODevice::WriteOnly))
    {
        this->ui->fMessageText->setStyleSheet("QLabel {color : red;}");
        this->ui->fMessageText->setText("Error : Unable To Write Class DAO File");
        return;
    }
    daoClassFile.write("//Generated Using DBGenerator By DevAJ \n\n");
    lineToBeWritten = "#include \"" + className + "DAO.h\" \n#include<sqlite3.h> \n\n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = className + "DAO::" + className + "DAO() \n{ \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    sqlite3 *db; \n    int returnCode; \n    char *error; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    returnCode = sqlite3_open(\"" + className + ".db\",&db); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    if(returnCode==SQLITE_OK) \n    { \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        const char *sql = \"create table IF NOT EXISTS " + tableName + "(";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());

    for(int x = 0; x < tableAttributes.size(); x++)
    {
        TableAttribute tableAttribute = tableAttributes.at(x);

        lineToBeWritten = tableAttribute.getName() + " " + tableAttribute.getDataType().toUpper();
        daoClassFile.write(lineToBeWritten.toStdString().c_str());

        if(tableAttribute.isPrimaryKey())
        {
            lineToBeWritten = " PRIMARY KEY";
            daoClassFile.write(lineToBeWritten.toStdString().c_str());
        }
        if(tableAttribute.isAutoInc())
        {
            lineToBeWritten = " AUTO INCREMENT";
            daoClassFile.write(lineToBeWritten.toStdString().c_str());
        }
        if(tableAttribute.isNotNull())
        {
            lineToBeWritten = " NOT NULL";
            daoClassFile.write(lineToBeWritten.toStdString().c_str());
        }

        if(x != tableAttributes.size() - 1)
        {
            lineToBeWritten = ",";
            daoClassFile.write(lineToBeWritten.toStdString().c_str());
        }
    }

    lineToBeWritten = ")\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        sqlite3_exec(db, sql, NULL, NULL, &error); \n        sqlite3_close(db); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "} \n\n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());

    lineToBeWritten = "void " + className + "DAO::insert(" + className + " *" + classNameWithFirstLower + ", bool *inserted, QString &errorMessage) \n{ \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    if(exists(" + classNameWithFirstLower + "->get" + pkAttributeNameWithFirstUpper + "())) \n    { \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        *inserted = false; \n        errorMessage = \"" + className + " With This " + pkAttributeNameWithFirstUpper + " Already Exists\"; \n        return; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    sqlite3 *db; \n    int returnCode; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    returnCode = sqlite3_open(\"" + className + ".db\",&db); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    if(returnCode==SQLITE_OK) \n    { \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        sqlite3_stmt *statement; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        const char *sql = \"insert into " + tableName + "(";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());

    lineToBeWritten = "";
    for(int x = 0; x < tableAttributes.size(); x++)
    {
        TableAttribute tableAttribute = tableAttributes.at(x);

        if(!tableAttribute.isAutoInc())
        {
            lineToBeWritten = lineToBeWritten + tableAttribute.getName() + ",";
        }
    }

    lineToBeWritten.chop(1);
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = ") values(";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());

    lineToBeWritten = "";
    for(int x = 0; x < tableAttributes.size(); x++)
    {
        TableAttribute tableAttribute = tableAttributes.at(x);

        if(!tableAttribute.isAutoInc())
        {
            lineToBeWritten = lineToBeWritten + "?,";
        }
    }

    lineToBeWritten.chop(1);
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = ")\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        sqlite3_prepare(db, sql, strlen(sql), &statement, NULL); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());

    int count = 0;
    for(int x = 0; x < tableAttributes.size(); x++)
    {
        TableAttribute tableAttribute = tableAttributes.at(x);

        if(!tableAttribute.isAutoInc())
        {
            count++;
            if(tableAttribute.getDataType().compare("int",Qt::CaseInsensitive) == 0)
            {
                lineToBeWritten = "        sqlite3_bind_int(statement," + QString::number(count) + "," + classNameWithFirstLower + "->get" + tableAttribute.getAttributeNameWithFirstUpper() + "()); \n";
                daoClassFile.write(lineToBeWritten.toStdString().c_str());
            }
            else if(tableAttribute.getDataType().compare("text",Qt::CaseInsensitive) == 0)
            {
                lineToBeWritten = "        sqlite3_bind_text(statement," + QString::number(count) + "," + classNameWithFirstLower + "->get" + tableAttribute.getAttributeNameWithFirstUpper() + "().toStdString().c_str()," + classNameWithFirstLower + "->get" + tableAttribute.getAttributeNameWithFirstUpper() + "().length(), SQLITE_TRANSIENT); \n";
                daoClassFile.write(lineToBeWritten.toStdString().c_str());
            }
            else
            {
                lineToBeWritten = "        sqlite3_bind_double(statement," + QString::number(count) + "," + classNameWithFirstLower + "->get" + tableAttribute.getAttributeNameWithFirstUpper() + "()); \n";
                daoClassFile.write(lineToBeWritten.toStdString().c_str());
            }
        }
    }

    lineToBeWritten = "        sqlite3_step(statement); \n        sqlite3_finalize(statement); \n        sqlite3_close(db); \n        *inserted = true; \n        errorMessage = \"\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    else \n    { \n        *inserted = false; \n        errorMessage = \"Unable To Insert " + className + "\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "} \n\n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());

    lineToBeWritten = "void " + className + "DAO::update(" + className + " *" + classNameWithFirstLower + ", bool *updated, QString &errorMessage) \n{ \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    if(!exists(" + classNameWithFirstLower + "->get" + pkAttributeNameWithFirstUpper + "())) \n    { \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        *updated = false; \n        errorMessage = \"" + className + " With This " + pkAttributeNameWithFirstUpper + " Does Not Exists\"; \n        return; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    sqlite3 *db; \n    int returnCode; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    returnCode = sqlite3_open(\"" + className + ".db\",&db); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    if(returnCode==SQLITE_OK) \n    { \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        sqlite3_stmt *statement; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        const char *sql = \"update " + tableName + " set ";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());

    lineToBeWritten = "";
    for(int x = 0; x < tableAttributes.size(); x++)
    {
        TableAttribute tableAttribute = tableAttributes.at(x);

        if(!tableAttribute.isAutoInc() && !tableAttribute.isPrimaryKey())
        {
            lineToBeWritten = lineToBeWritten + tableAttribute.getName() + "=?,";
        }
    }

    lineToBeWritten.chop(1);
    lineToBeWritten = lineToBeWritten + " where " + pkName + "=?\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        sqlite3_prepare(db, sql, strlen(sql), &statement, NULL); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());

    count = 0;
    for(int x = 0; x < tableAttributes.size(); x++)
    {
        TableAttribute tableAttribute = tableAttributes.at(x);

        if(!tableAttribute.isAutoInc() && !tableAttribute.isPrimaryKey())
        {
            count++;
            if(tableAttribute.getDataType().compare("int",Qt::CaseInsensitive) == 0)
            {
                lineToBeWritten = "        sqlite3_bind_int(statement," + QString::number(count) + "," + classNameWithFirstLower + "->get" + tableAttribute.getAttributeNameWithFirstUpper() + "()); \n";
                daoClassFile.write(lineToBeWritten.toStdString().c_str());
            }
            else if(tableAttribute.getDataType().compare("text",Qt::CaseInsensitive) == 0)
            {
                lineToBeWritten = "        sqlite3_bind_text(statement," + QString::number(count) + "," + classNameWithFirstLower + "->get" + tableAttribute.getAttributeNameWithFirstUpper() + "().toStdString().c_str()," + classNameWithFirstLower + "->get" + tableAttribute.getAttributeNameWithFirstUpper() + "().length(), SQLITE_TRANSIENT); \n";
                daoClassFile.write(lineToBeWritten.toStdString().c_str());
            }
            else
            {
                lineToBeWritten = "        sqlite3_bind_double(statement," + QString::number(count) + "," + classNameWithFirstLower + "->get" + tableAttribute.getAttributeNameWithFirstUpper() + "()); \n";
                daoClassFile.write(lineToBeWritten.toStdString().c_str());
            }
        }
    }

    count++;
    if(pkDataType.compare("int",Qt::CaseInsensitive) == 0)
    {
        lineToBeWritten = "        sqlite3_bind_int(statement," + QString::number(count) + "," + classNameWithFirstLower + "->get" + pkAttributeNameWithFirstUpper + "()); \n";
        daoClassFile.write(lineToBeWritten.toStdString().c_str());
    }
    else if(pkDataType.compare("text",Qt::CaseInsensitive) == 0)
    {
        lineToBeWritten = "        sqlite3_bind_text(statement," + QString::number(count) + "," + classNameWithFirstLower + "->get" + pkAttributeNameWithFirstUpper + "().toStdString().c_str()," + classNameWithFirstLower + "->get" + pkAttributeNameWithFirstUpper + "().length(), SQLITE_TRANSIENT); \n";
        daoClassFile.write(lineToBeWritten.toStdString().c_str());
    }
    else
    {
        lineToBeWritten = "        sqlite3_bind_double(statement," + QString::number(count) + "," + classNameWithFirstLower + "->get" + pkAttributeNameWithFirstUpper + "()); \n";
        daoClassFile.write(lineToBeWritten.toStdString().c_str());
    }

    lineToBeWritten = "        sqlite3_step(statement); \n        sqlite3_finalize(statement); \n        sqlite3_close(db); \n        *updated = true; \n        errorMessage = \"\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    else \n    { \n        *updated = false; \n        errorMessage = \"Unable To Update " + className + "\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "} \n\n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());

    lineToBeWritten = "void " + className + "DAO::remove(" + pkActualDataType + " " + pkClassAttributeName + ", bool *removed, QString &errorMessage) \n{ \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    if(!exists(" + pkClassAttributeName + ")) \n    { \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        *removed = false; \n        errorMessage = \"" + className + " With This " + pkAttributeNameWithFirstUpper + " Does Not Exists\"; \n        return; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    sqlite3 *db; \n    int returnCode; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    returnCode = sqlite3_open(\"" + className + ".db\",&db); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    if(returnCode==SQLITE_OK) \n    { \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        sqlite3_stmt *statement; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        const char *sql = \"delete from " + tableName + " where " + pkName + "=?\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        sqlite3_prepare(db, sql, strlen(sql), &statement, NULL); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    if(pkDataType.compare("int",Qt::CaseInsensitive) == 0)
    {
        lineToBeWritten = "        sqlite3_bind_int(statement,1," + pkClassAttributeName + "); \n";
        daoClassFile.write(lineToBeWritten.toStdString().c_str());
    }
    else if(pkDataType.compare("text",Qt::CaseInsensitive) == 0)
    {
        lineToBeWritten = "        sqlite3_bind_text(statement,1," + pkClassAttributeName + ".toStdString().c_str()," + pkClassAttributeName + ".length(), SQLITE_TRANSIENT); \n";
        daoClassFile.write(lineToBeWritten.toStdString().c_str());
    }
    else
    {
        lineToBeWritten = "        sqlite3_bind_double(statement,1," + pkClassAttributeName + "); \n";
        daoClassFile.write(lineToBeWritten.toStdString().c_str());
    }
    lineToBeWritten = "        sqlite3_step(statement); \n        sqlite3_finalize(statement); \n        sqlite3_close(db); \n        *removed = true; \n        errorMessage = \"\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    else \n    { \n        *removed = false; \n        errorMessage = \"Unable To Remove " + className + "\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "} \n\n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());

    lineToBeWritten = "QList<" + className + ">* " + className + "DAO::get(bool *found, QString &errorMessage) \n{ \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    QList<" + className + ">* " + classNameWithFirstLower + "s; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    sqlite3 *db; \n    int returnCode; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    returnCode = sqlite3_open(\"" + className + ".db\",&db); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    if(returnCode==SQLITE_OK) \n    { \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        " + classNameWithFirstLower + "s = new QList<" + className + ">; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        sqlite3_stmt *statement; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        const char *sql = \"select * from " + tableName + "\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        sqlite3_prepare(db, sql, strlen(sql), &statement, NULL); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        returnCode = sqlite3_step(statement); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        while(returnCode == SQLITE_ROW) \n        { \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "            " + className + " " + classNameWithFirstLower + "; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());

    count = 0;
    for(int x = 0; x < tableAttributes.size(); x++)
    {
        TableAttribute tableAttribute = tableAttributes.at(x);

        lineToBeWritten = "            " + classNameWithFirstLower + ".set" + tableAttribute.getAttributeNameWithFirstUpper() + "(";
        daoClassFile.write(lineToBeWritten.toStdString().c_str());
        if(tableAttribute.getDataType().compare("int",Qt::CaseInsensitive) == 0)
        {
            lineToBeWritten = "sqlite3_column_int(statement,"+ QString::number(count) +")";
        }
        else if(tableAttribute.getDataType().compare("text",Qt::CaseInsensitive) == 0)
        {
            lineToBeWritten = "reinterpret_cast<const char *>(sqlite3_column_text(statement,"+ QString::number(count) +"))";
        }
        else
        {
            lineToBeWritten = "sqlite3_column_double(statement,"+ QString::number(count) +")";
        }
        lineToBeWritten = lineToBeWritten + "); \n";
        daoClassFile.write(lineToBeWritten.toStdString().c_str());
        count++;
    }

    lineToBeWritten = "            " + classNameWithFirstLower + "s->push_back(" + classNameWithFirstLower + "); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "            returnCode = sqlite3_step(statement); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        } \n        sqlite3_close(db); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        if(" + classNameWithFirstLower +"s->size()==0) \n        { \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "            *found = false; \n            errorMessage = \"No Records Found\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        else \n        { \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "            *found = true; \n            errorMessage = \"\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    else \n    { \n        *found = false; \n        errorMessage = \"Unable To Retrive All " + className + "s\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    return " + classNameWithFirstLower + "s; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "} \n\n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());

    lineToBeWritten = "bool " + className + "DAO::exists(" + pkActualDataType + " " + pkClassAttributeName + ") \n{ \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    bool exists = false; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    sqlite3 *db; \n    int returnCode; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    returnCode = sqlite3_open(\"" + className + ".db\",&db); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    if(returnCode==SQLITE_OK) \n    { \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        sqlite3_stmt *statement; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        const char *sql = \"select * " + tableName + " where " + pkName + "=?\"; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        sqlite3_prepare(db, sql, strlen(sql), &statement, NULL); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    if(pkDataType.compare("int",Qt::CaseInsensitive) == 0)
    {
        lineToBeWritten = "        sqlite3_bind_int(statement,1," + pkClassAttributeName + "); \n";
        daoClassFile.write(lineToBeWritten.toStdString().c_str());
    }
    else if(pkDataType.compare("text",Qt::CaseInsensitive) == 0)
    {
        lineToBeWritten = "        sqlite3_bind_text(statement,1," + pkClassAttributeName + ".toStdString().c_str()," + pkClassAttributeName + ".length(), SQLITE_TRANSIENT); \n";
        daoClassFile.write(lineToBeWritten.toStdString().c_str());
    }
    else
    {
        lineToBeWritten = "        sqlite3_bind_double(statement,1," + pkClassAttributeName + "); \n";
        daoClassFile.write(lineToBeWritten.toStdString().c_str());
    }
    lineToBeWritten = "        returnCode = sqlite3_step(statement); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        if(returnCode == SQLITE_ROW) \n        { \n";
    lineToBeWritten = lineToBeWritten + "            exists = true; \n        } \n";
    lineToBeWritten = lineToBeWritten + "        else \n        { \n";
    lineToBeWritten = lineToBeWritten + "            exists = false; \n        } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "        sqlite3_close(db); \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    } \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "    return exists; \n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    lineToBeWritten = "} \n\n";
    daoClassFile.write(lineToBeWritten.toStdString().c_str());
    daoClassFile.close();

}
