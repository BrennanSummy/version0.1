#include "TextBox.h"


TextBox::TextBox(const QString& label, const QString& default_txt, QWidget* parent):QWidget(parent)
{
    layout      = new QVBoxLayout(this);
    nameLabel   = new QLabel(label);
    box = new QLineEdit();
    //box->setPlaceholderText(default_txt);

    m_paramName = paramNames[m_paramIndex];

    box->setPlaceholderText(QString::fromStdString(m_paramName) + " parameters to probe");
    // Add to layout
    layout->setSpacing(5);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(nameLabel);
    layout->addWidget(box);

    connect(box,&QLineEdit::returnPressed,this,&TextBox::emitParamList);
}

ParameterList TextBox::getParamList(std::string paramName)
{
    std::string s = box->text().toStdString();
    return ParameterList(s,paramName);
}

void TextBox::emitParamList()
{
    emit paramListSignal(getParamList(m_paramName));
    box->clear();
    m_paramIndex = (m_paramIndex + 1)%paramNames.size();
    m_paramName = paramNames[m_paramIndex];
    box->setPlaceholderText(QString::fromStdString(m_paramName) + " parameters to probe");
}