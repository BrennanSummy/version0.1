#pragma once

#include "structs.h"
#include "constants.h"
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QLineEdit>

class TextBox : public QWidget
{
    Q_OBJECT
    public:
        TextBox(const QString& label,const QString& default_txt, QWidget* parent = nullptr);

        ParameterList getParamList(std::string paramName);
        QLabel* nameLabel;
        QLineEdit* box;
        std::string m_paramName;
        void emitParamList();
    signals:
        void paramListSignal(ParameterList paramList);

    private:
        QVBoxLayout* layout;
        int m_paramIndex = 0;
};